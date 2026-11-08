#include "ara/audio_source.h"
#include "core/log.h"

namespace plenio::ara
{
namespace
{
/// Longer sources are not cached (about 24 minutes of stereo at 48 kHz): the renderer passes silence for them.
constexpr ARA::ARASampleCount maxCachedSamples = 70'000'000;
} // namespace

void AudioSource::updateCache()
{
    cache.clear();
    const auto channels = getChannelCount();
    const auto samples = getSampleCount();
    if (channels <= 0 || samples <= 0 || channels > 32 || samples * channels > maxCachedSamples)
    {
        if (samples * channels > maxCachedSamples)
            log::warn ("ARA: an audio source of {} samples is too long to cache", samples);
        return;
    }

    try
    {
        cache.resize (static_cast<size_t> (channels) * static_cast<size_t> (samples));
        std::vector<void*> pointers (static_cast<size_t> (channels));
        for (ARA::ARAChannelCount c = 0; c < channels; ++c)
            pointers[static_cast<size_t> (c)] = cache.data() + static_cast<size_t> (c) * static_cast<size_t> (samples);

        // a failed read leaves silence: the host reports the error to the user
        ARA::PlugIn::HostAudioReader reader { this };
        reader.readAudioSamples (0, samples, pointers.data());
    }
    catch (const std::bad_alloc&)
    {
        log::error ("ARA: not enough memory to cache an audio source");
        cache.clear();
    }
}

void AudioSource::clearCache()
{
    cache.clear();
    cache.shrink_to_fit();
}

const float* AudioSource::channel (ARA::ARAChannelCount c) const noexcept
{
    if (c < 0 || c >= getChannelCount() || cache.empty())
        return nullptr;
    return cache.data() + static_cast<size_t> (c) * static_cast<size_t> (getSampleCount());
}

} // namespace plenio::ara
