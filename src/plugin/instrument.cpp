#include "plugin/instrument.h"
#include "plugin/ids.h"
#include "plugin/state_stream.h"
#include "platform/main_thread.h"
#include "ui/editor_view.h"
#include "core/log.h"
#include "plenio_version.h"

#include "pluginterfaces/vst/ivsthostapplication.h"
#include "pluginterfaces/vst/ivstprocesscontext.h"
#include "pluginterfaces/base/funknownimpl.h"
#include "public.sdk/source/vst/utility/stringconvert.h"

#include <algorithm>
#include <cstring>

using namespace Steinberg;

namespace plenio::plugin
{

Instrument::Instrument()
{
    processContextRequirements.needTransportState()
        .needTempo()
        .needTimeSignature()
        .needProjectTimeMusic()
        .needBarPositionMusic()
        .needCycleMusic();
}

Instrument::~Instrument() = default;

FUnknown* Instrument::createInstance (void*)
{
    return static_cast<Vst::IAudioProcessor*> (new Instrument());
}

tresult PLUGIN_API Instrument::initialize (FUnknown* context)
{
    auto result = SingleComponentEffect::initialize (context);
    if (result != kResultOk)
        return result;

    platform::attachMainThread();

    if (auto app = U::cast<Vst::IHostApplication> (context))
    {
        Vst::String128 name {};
        if (app->getName (name) == kResultOk)
            hostName = Vst::StringConvert::convert (name);
    }
    log::info ("Plenio {} instrument created in {}", PLENIO_VERSION_STRING, hostName.empty() ? "an unknown host" : hostName);

    addEventInput (STR16 ("MIDI In"), 16);
    addEventOutput (STR16 ("MIDI Out"), 16);
    addAudioOutput (STR16 ("Song"), Vst::SpeakerArr::kStereo, Vst::kMain, Vst::BusInfo::kDefaultActive);
    addAudioOutput (STR16 ("Vocals"), Vst::SpeakerArr::kStereo, Vst::kAux, 0);
    addAudioOutput (STR16 ("Drums"), Vst::SpeakerArr::kStereo, Vst::kAux, 0);
    addAudioOutput (STR16 ("Bass"), Vst::SpeakerArr::kStereo, Vst::kAux, 0);
    addAudioOutput (STR16 ("Other"), Vst::SpeakerArr::kStereo, Vst::kAux, 0);
    addAudioOutput (STR16 ("Rest"), Vst::SpeakerArr::kStereo, Vst::kAux, 0);
    return kResultOk;
}

tresult PLUGIN_API Instrument::terminate()
{
    sink = {};
    platform::detachMainThread();
    return SingleComponentEffect::terminate();
}

tresult PLUGIN_API Instrument::setBusArrangements (Vst::SpeakerArrangement* inputs, int32 numIns,
                                                   Vst::SpeakerArrangement* outputs, int32 numOuts)
{
    // no audio input; every output stereo
    if (numIns != 0 || numOuts != static_cast<int32> (audioOutputs.size()))
        return kResultFalse;
    for (int32 i = 0; i < numOuts; ++i)
        if (outputs[i] != Vst::SpeakerArr::kStereo)
            return kResultFalse;
    return SingleComponentEffect::setBusArrangements (inputs, numIns, outputs, numOuts);
}

tresult PLUGIN_API Instrument::canProcessSampleSize (int32 symbolicSampleSize)
{
    return symbolicSampleSize == Vst::kSample32 ? kResultTrue : kResultFalse;
}

tresult PLUGIN_API Instrument::setupProcessing (Vst::ProcessSetup& setup)
{
    return SingleComponentEffect::setupProcessing (setup);
}

tresult PLUGIN_API Instrument::setProcessing (TBool)
{
    return kResultOk;
}

tresult PLUGIN_API Instrument::setActive (TBool state)
{
    return SingleComponentEffect::setActive (state);
}

tresult PLUGIN_API Instrument::process (Vst::ProcessData& data)
{
    if (data.numSamples <= 0 || data.outputs == nullptr)
        return kResultOk;

    for (int32 b = 0; b < data.numOutputs; ++b)
    {
        auto& bus = data.outputs[b];
        if (bus.channelBuffers32 == nullptr)
            continue;
        for (int32 c = 0; c < bus.numChannels; ++c)
            if (bus.channelBuffers32[c] != nullptr)
                std::memset (bus.channelBuffers32[c], 0, sizeof (float) * static_cast<size_t> (data.numSamples));
        bus.silenceFlags = bus.numChannels >= 64 ? ~0ull : ((1ull << bus.numChannels) - 1ull);
    }
    return kResultOk;
}

tresult PLUGIN_API Instrument::setState (IBStream* state)
{
    auto read = readState (state);
    if (! read)
        return kResultFalse;
    restored = *read;
    const auto& editor = json::obj (restored, "editor");
    editorWidth = static_cast<int> (json::integer (editor, "width", 0));
    editorHeight = static_cast<int> (json::integer (editor, "height", 0));
    return kResultOk;
}

tresult PLUGIN_API Instrument::getState (IBStream* state)
{
    Json out = restored;
    out["schema"] = "plenio-vst3.instrument/1";
    out["version"] = PLENIO_VERSION_STRING;
    if (editorWidth > 0 && editorHeight > 0)
        out["editor"] = { { "width", editorWidth }, { "height", editorHeight } };
    return writeState (state, out) ? kResultOk : kResultFalse;
}

IPlugView* PLUGIN_API Instrument::createView (FIDString name)
{
    if (FIDStringsEqual (name, Vst::ViewType::kEditor))
        return new ui::EditorView (this, *this);
    return nullptr;
}

Json Instrument::call (const std::string& method, const Json& params)
{
    if (method == "ui.hello")
        return { { "kind", "instrument" },
                 { "version", PLENIO_VERSION_STRING },
                 { "host", hostName },
                 { "platform", "windows" } };

    if (method == "ui.log")
    {
        log::info ("UI: {}", json::str (params, "message", {}, 2000));
        return nullptr;
    }

    throw ui::CallError ("unknown_method", "Plenio does not know the call '" + method + "'.");
}

void Instrument::setEventSink (EventSink s)
{
    sink = std::move (s);
}

std::pair<int, int> Instrument::editorSize() const
{
    if (editorWidth > 0 && editorHeight > 0)
        return { editorWidth, editorHeight };
    return { ui::EditorView::defaultWidth, ui::EditorView::defaultHeight };
}

void Instrument::setEditorSize (int width, int height)
{
    editorWidth = std::clamp (width, ui::EditorView::minWidth, 6000);
    editorHeight = std::clamp (height, ui::EditorView::minHeight, 4000);
}

} // namespace plenio::plugin
