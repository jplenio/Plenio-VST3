#include "ara/document_controller.h"
#include "ara/audio_source.h"
#include "ara/playback_renderer.h"

#include <thread>

namespace plenio::ara
{

bool DocumentController::rendererWillAccessModelGraph() noexcept
{
    ++renderersAccessing;
    return renderersMayAccess.load();
}

void DocumentController::rendererDidAccessModelGraph() noexcept
{
    --renderersAccessing;
}

void DocumentController::blockRenderers() noexcept
{
    static_assert (decltype (renderersMayAccess)::is_always_lock_free);
    renderersMayAccess = false;
    // renderers hold the model for one block at most
    while (renderersAccessing.load() > 0)
        std::this_thread::yield();
}

void DocumentController::unblockRenderers() noexcept
{
    renderersMayAccess = true;
}

void DocumentController::willBeginEditing() noexcept
{
    blockRenderers();
}

void DocumentController::didEndEditing() noexcept
{
    unblockRenderers();
}

void DocumentController::doUpdateMusicalContextContent (ARA::PlugIn::MusicalContext*, const ARA::ARAContentTimeRange*, ARA::ContentUpdateScopes) noexcept
{
    // the host's tempo, bars and chords are read when needed (phase 7)
}

ARA::PlugIn::AudioSource* DocumentController::doCreateAudioSource (ARA::PlugIn::Document* document, ARA::ARAAudioSourceHostRef hostRef) noexcept
{
    return new AudioSource (document, hostRef);
}

void DocumentController::willEnableAudioSourceSamplesAccess (ARA::PlugIn::AudioSource*, bool) noexcept
{
    if (! isHostEditingDocument())
        blockRenderers();
}

void DocumentController::didEnableAudioSourceSamplesAccess (ARA::PlugIn::AudioSource* audioSource, bool enable) noexcept
{
    auto* source = static_cast<AudioSource*> (audioSource);
    if (enable)
        source->updateCache();
    else
        source->clearCache();

    if (! isHostEditingDocument())
        unblockRenderers();
}

void DocumentController::doUpdateAudioSourceContent (ARA::PlugIn::AudioSource* audioSource, const ARA::ARAContentTimeRange*, ARA::ContentUpdateScopes scopeFlags) noexcept
{
    // the host changed the samples: read them again (only ever called while the host edits the document)
    if (scopeFlags.affectSamples() && audioSource->isSampleAccessEnabled())
        static_cast<AudioSource*> (audioSource)->updateCache();
}

void DocumentController::didDeactivateAudioSourceForUndoHistory (ARA::PlugIn::AudioSource* audioSource, bool deactivate) noexcept
{
    auto* source = static_cast<AudioSource*> (audioSource);
    if (deactivate)
        source->clearCache();
    else if (source->isSampleAccessEnabled())
        source->updateCache();
}

bool DocumentController::doRestoreObjectsFromArchive (ARA::PlugIn::HostArchiveReader*, const ARA::PlugIn::RestoreObjectsFilter*) noexcept
{
    return true;
}

bool DocumentController::doStoreObjectsToArchive (ARA::PlugIn::HostArchiveWriter*, const ARA::PlugIn::StoreObjectsFilter*) noexcept
{
    return true;
}

ARA::PlugIn::PlaybackRenderer* DocumentController::doCreatePlaybackRenderer() noexcept
{
    return new PlaybackRenderer (this);
}

ARA::PlugIn::EditorView* DocumentController::doCreateEditorView() noexcept
{
    return new ARA::PlugIn::EditorView (this);
}

} // namespace plenio::ara
