// The plug-in's settings (%APPDATA%\Plenio\VST3\settings.json): where ComfyUI is, where songs go, what the owner
// chose last. Shared by every instance and both plug-ins.
#pragma once

#include "core/json.h"

#include <string>

namespace plenio::core
{

struct Settings
{
    std::string host = "127.0.0.1";
    int port = 0;                  // 0: find ComfyUI (8188, 8000, the port Plenio last started it on)
    std::string library;           // empty: Documents\Plenio\Songs
    bool startComfyUI = true;      // start the owner's ComfyUI when a song is to be made and none answers
    bool licenceAccepted = false;  // the CC BY-NC 4.0 notice of YuE2 was confirmed
    int takesPerRun = 1;           // the last choice, for the next new song
    bool stems = true;
    bool flatKeys = false;         // chord names with flats (Bb) instead of sharps (A#)

    Json toJson() const;
    static Settings fromJson (const Json& j);
};

Settings loadSettings();
bool saveSettings (const Settings& settings);

} // namespace plenio::core
