#include "core/settings.h"
#include "core/library.h"
#include "core/paths.h"

#include <algorithm>

namespace plenio::core
{

Json Settings::toJson() const
{
    return { { "schema", "plenio-vst3.settings/1" },
             { "host", host },
             { "port", port },
             { "library", library },
             { "start_comfyui", startComfyUI },
             { "licence_accepted", licenceAccepted },
             { "takes_per_run", takesPerRun },
             { "stems", stems },
             { "flat_keys", flatKeys } };
}

Settings Settings::fromJson (const Json& j)
{
    Settings s;
    s.host = json::str (j, "host", "127.0.0.1", 200);
    if (s.host.empty())
        s.host = "127.0.0.1";
    s.port = static_cast<int> (std::clamp<std::int64_t> (json::integer (j, "port", 0), 0, 65535));
    s.library = json::str (j, "library", {}, 2000);
    s.startComfyUI = json::boolean (j, "start_comfyui", true);
    s.licenceAccepted = json::boolean (j, "licence_accepted", false);
    s.takesPerRun = static_cast<int> (std::clamp<std::int64_t> (json::integer (j, "takes_per_run", 1), 1, 4));
    s.stems = json::boolean (j, "stems", true);
    s.flatKeys = json::boolean (j, "flat_keys", false);
    return s;
}

Settings loadSettings()
{
    auto text = readFile (paths::appData() / "settings.json", 1024 * 1024);
    if (! text)
        return {};
    auto parsed = json::parse (*text);
    return parsed ? Settings::fromJson (*parsed) : Settings {};
}

bool saveSettings (const Settings& settings)
{
    return writeFileAtomically (paths::appData() / "settings.json", settings.toJson().dump (1));
}

} // namespace plenio::core
