// An audio source of the host (a file under clips) as Plenio ARA keeps it: its samples, cached for the renderer
// once the host allows access.
#pragma once

#include "ARA_Library/PlugIn/ARAPlug.h"
#include "core/clip_content.h"

#include <optional>
#include <vector>

namespace plenio::ara
{

class AudioSource : public ARA::PlugIn::AudioSource
{
public:
    using ARA::PlugIn::AudioSource::AudioSource;

    /// Reads all samples from the host (main thread, while sample access is enabled).
    void updateCache();
    void clearCache();

    bool hasCache() const noexcept { return ! cache.empty(); }
    const float* channel (ARA::ARAChannelCount c) const noexcept;

    /// The Plenio take this clip is, by its name and length (looked up in the library once), and what it holds;
    /// nothing for a clip that is no Plenio take. Main thread.
    const core::ClipContent* content() const;
    /// The name or the length changed: look again.
    void forgetTake() noexcept;

private:
    std::vector<float> cache; // channel after channel
    mutable bool looked = false;
    mutable std::optional<core::ClipContent> clipContent;
};

} // namespace plenio::ara
