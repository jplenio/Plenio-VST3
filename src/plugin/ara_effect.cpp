#include "plugin/ara_effect.h"
#include "plugin/ids.h"
#include "plugin/state_stream.h"
#include "ara/factory.h"
#include "ara/playback_renderer.h"
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

AraEffect::AraEffect()
{
    processContextRequirements.needTransportState();
}

AraEffect::~AraEffect() = default;

FUnknown* AraEffect::createInstance (void*)
{
    return static_cast<Vst::IAudioProcessor*> (new AraEffect());
}

tresult PLUGIN_API AraEffect::initialize (FUnknown* context)
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
    log::info ("Plenio ARA {} created in {}", PLENIO_VERSION_STRING, hostName.empty() ? "an unknown host" : hostName);

    addAudioInput (STR16 ("Stereo In"), Vst::SpeakerArr::kStereo);
    addAudioOutput (STR16 ("Stereo Out"), Vst::SpeakerArr::kStereo);
    return kResultOk;
}

tresult PLUGIN_API AraEffect::terminate()
{
    sink = {};
    platform::detachMainThread();
    return SingleComponentEffect::terminate();
}

tresult PLUGIN_API AraEffect::setBusArrangements (Vst::SpeakerArrangement* inputs, int32 numIns,
                                                  Vst::SpeakerArrangement* outputs, int32 numOuts)
{
    // one input and one output with the same channels (mono or stereo, like the clip's track)
    if (numIns == 1 && numOuts == 1 && inputs[0] == outputs[0]
        && (outputs[0] == Vst::SpeakerArr::kMono || outputs[0] == Vst::SpeakerArr::kStereo))
        return SingleComponentEffect::setBusArrangements (inputs, numIns, outputs, numOuts);
    return kResultFalse;
}

tresult PLUGIN_API AraEffect::canProcessSampleSize (int32 symbolicSampleSize)
{
    return symbolicSampleSize == Vst::kSample32 ? kResultTrue : kResultFalse;
}

tresult PLUGIN_API AraEffect::setProcessing (TBool)
{
    return kResultOk;
}

tresult PLUGIN_API AraEffect::setActive (TBool state)
{
    if (auto* renderer = araExtension.getPlaybackRenderer<ara::PlaybackRenderer>())
    {
        int32 channels = 2;
        if (! audioOutputs.empty())
            if (auto* bus = FCast<Vst::AudioBus> (audioOutputs[0].get()))
                channels = Vst::SpeakerArr::getChannelCount (bus->getArrangement());
        if (state)
            renderer->enableRendering (processSetup.sampleRate, channels, processSetup.maxSamplesPerBlock);
        else
            renderer->disableRendering();
    }
    return SingleComponentEffect::setActive (state);
}

tresult PLUGIN_API AraEffect::process (Vst::ProcessData& data)
{
    if (data.numSamples <= 0 || data.numOutputs < 1 || data.outputs[0].numChannels <= 0)
        return kResultOk;

    auto& out = data.outputs[0];
    if (auto* renderer = araExtension.getPlaybackRenderer<ara::PlaybackRenderer>())
    {
        const bool playing = data.processContext != nullptr && (data.processContext->state & Vst::ProcessContext::kPlaying) != 0;
        const auto position = data.processContext != nullptr ? data.processContext->projectTimeSamples : 0;
        renderer->render (out.channelBuffers32, out.numChannels, position, data.numSamples, playing);
        return kResultOk;
    }

    // not a playback renderer (editor only, or no ARA): the input passes unchanged
    for (int32 c = 0; c < out.numChannels; ++c)
    {
        float* dst = out.channelBuffers32[c];
        const float* src = (data.numInputs > 0 && c < data.inputs[0].numChannels) ? data.inputs[0].channelBuffers32[c] : nullptr;
        if (dst == nullptr)
            continue;
        if (src == nullptr)
            std::memset (dst, 0, sizeof (float) * static_cast<size_t> (data.numSamples));
        else if (src != dst)
            std::memcpy (dst, src, sizeof (float) * static_cast<size_t> (data.numSamples));
    }
    return kResultOk;
}

tresult PLUGIN_API AraEffect::setState (IBStream* state)
{
    auto read = readState (state);
    if (! read)
        return kResultFalse;
    const auto& editor = json::obj (*read, "editor");
    editorWidth = static_cast<int> (json::integer (editor, "width", 0));
    editorHeight = static_cast<int> (json::integer (editor, "height", 0));
    return kResultOk;
}

tresult PLUGIN_API AraEffect::getState (IBStream* state)
{
    Json out = { { "schema", "plenio-vst3.ara/1" }, { "version", PLENIO_VERSION_STRING } };
    if (editorWidth > 0 && editorHeight > 0)
        out["editor"] = { { "width", editorWidth }, { "height", editorHeight } };
    return writeState (state, out) ? kResultOk : kResultFalse;
}

IPlugView* PLUGIN_API AraEffect::createView (FIDString name)
{
    if (FIDStringsEqual (name, Vst::ViewType::kEditor))
        return new ui::EditorView (this, *this);
    return nullptr;
}

const ARA::ARAFactory* PLUGIN_API AraEffect::getFactory()
{
    return ara::factory();
}

const ARA::ARAPlugInExtensionInstance* PLUGIN_API AraEffect::bindToDocumentController (ARA::ARADocumentControllerRef)
{
    // ARA 1 only; this plug-in requires ARA 2
    return nullptr;
}

const ARA::ARAPlugInExtensionInstance* PLUGIN_API AraEffect::bindToDocumentControllerWithRoles (ARA::ARADocumentControllerRef controllerRef,
                                                                                                ARA::ARAPlugInInstanceRoleFlags knownRoles,
                                                                                                ARA::ARAPlugInInstanceRoleFlags assignedRoles)
{
    return araExtension.bindToARA (controllerRef, knownRoles, assignedRoles);
}

Json AraEffect::call (const std::string& method, const Json& params)
{
    if (method == "ui.hello")
        return { { "kind", "ara" },
                 { "version", PLENIO_VERSION_STRING },
                 { "host", hostName },
                 { "platform", "windows" },
                 { "bound", araExtension.isBoundToARA() } };

    if (method == "ui.log")
    {
        log::info ("UI (ARA): {}", json::str (params, "message", {}, 2000));
        return nullptr;
    }

    throw ui::CallError ("unknown_method", "Plenio ARA does not know the call '" + method + "'.");
}

void AraEffect::setEventSink (EventSink s)
{
    sink = std::move (s);
}

std::pair<int, int> AraEffect::editorSize() const
{
    if (editorWidth > 0 && editorHeight > 0)
        return { editorWidth, editorHeight };
    return { ui::EditorView::defaultWidth, 560 };
}

void AraEffect::setEditorSize (int width, int height)
{
    editorWidth = std::clamp (width, ui::EditorView::minWidth, 6000);
    editorHeight = std::clamp (height, ui::EditorView::minHeight, 4000);
}

} // namespace plenio::plugin
