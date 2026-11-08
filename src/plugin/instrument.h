// Plenio, the instrument: one song with its takes and stems, played in sync with the host's timeline.
//
// Buses: MIDI in (reserved), MIDI out (melody, instrument line, chords), the song (main stereo output) and five
// stereo stem outputs (vocals, drums, bass, other, rest) that are off until the host activates them.
#pragma once

#include "ui/backend.h"

#include "public.sdk/source/vst/vstsinglecomponenteffect.h"

#include <string>

namespace plenio::plugin
{

class Instrument : public Steinberg::Vst::SingleComponentEffect, public ui::Backend
{
public:
    Instrument();
    ~Instrument() override;

    static Steinberg::FUnknown* createInstance (void*);

    // IPluginBase
    Steinberg::tresult PLUGIN_API initialize (Steinberg::FUnknown* context) override;
    Steinberg::tresult PLUGIN_API terminate() override;

    // IComponent
    Steinberg::tresult PLUGIN_API setState (Steinberg::IBStream* state) override;
    Steinberg::tresult PLUGIN_API getState (Steinberg::IBStream* state) override;
    Steinberg::tresult PLUGIN_API setActive (Steinberg::TBool state) override;

    // IAudioProcessor
    Steinberg::tresult PLUGIN_API setBusArrangements (Steinberg::Vst::SpeakerArrangement* inputs, Steinberg::int32 numIns,
                                                      Steinberg::Vst::SpeakerArrangement* outputs, Steinberg::int32 numOuts) override;
    Steinberg::tresult PLUGIN_API canProcessSampleSize (Steinberg::int32 symbolicSampleSize) override;
    Steinberg::tresult PLUGIN_API setupProcessing (Steinberg::Vst::ProcessSetup& setup) override;
    Steinberg::tresult PLUGIN_API setProcessing (Steinberg::TBool state) override;
    Steinberg::tresult PLUGIN_API process (Steinberg::Vst::ProcessData& data) override;

    // IEditController
    Steinberg::IPlugView* PLUGIN_API createView (Steinberg::FIDString name) override;

    // ui::Backend
    Json call (const std::string& method, const Json& params) override;
    void setEventSink (EventSink sink) override;
    std::pair<int, int> editorSize() const override;
    void setEditorSize (int width, int height) override;

    /// The stem outputs in their bus order (after the song's main output).
    static constexpr const char* stemNames[] = { "vocals", "drums", "bass", "other", "rest" };

private:
    std::string hostName;
    EventSink sink;
    int editorWidth = 0;
    int editorHeight = 0;
    Json restored = Json::object();
};

} // namespace plenio::plugin
