// An audio source of the host (a file under clips) as Plenio ARA keeps it: its samples, cached for the renderer
// once the host allows access.
#pragma once

#include "ARA_Library/PlugIn/ARAPlug.h"

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

private:
    std::vector<float> cache; // channel after channel
};

} // namespace plenio::ara
