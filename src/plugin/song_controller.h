// What the instrument's UI talks to: the song session, the player and the studio behind one call interface, and the
// view of the song the UI draws (takes, the working score on the project's timeline, sung lines, jobs). Main thread.
#pragma once

#include "core/session.h"
#include "engine/player.h"
#include "ui/backend.h"

#include <atomic>
#include <memory>
#include <mutex>
#include <string>

namespace plenio::plugin
{

class SongController : public ui::Backend, public core::SongSession::Listener
{
public:
    explicit SongController (engine::Player& player);
    ~SongController() override;

    void setHostName (std::string name) { hostName = std::move (name); }
    /// The host's sample rate (setupProcessing): the take is loaded at it.
    void setHostRate (double rate);

    /// The project's state, from any thread (a copy kept up to date on the main thread).
    Json getState() const;
    /// Main thread.
    void setState (const Json& state);

    // ui::Backend
    Json call (const std::string& method, const Json& params) override;
    void setEventSink (EventSink sink) override;
    std::pair<int, int> editorSize() const override;
    void setEditorSize (int width, int height) override;

    // core::SongSession::Listener
    void songChanged() override;
    void jobsChanged() override;
    void activeTakeChanged() override;
    void tell (const std::string& text, bool problem) override;

private:
    void tick();
    void reloadAudio();
    void refreshState();
    void emit (const std::string& name, const Json& payload) const;
    Json songView() const;
    Json jobsView() const;
    Json connectionView() const;
    Json appState() const;
    void peaksOf (int index, int count);

    engine::Player& player;
    std::shared_ptr<core::Studio> studio;
    std::unique_ptr<core::SongSession> session;
    EventSink sink;
    std::string hostName;
    double hostRate = 0.0;
    std::uint64_t loadGeneration = 0;
    int loadedTake = -1;
    bool loading = false;
    std::string loadProblem;
    int timer = 0;
    int connectionListener = 0;
    std::int64_t lastSentPosition = -1;
    bool lastSentMoving = false;
    int editorWidth = 0, editorHeight = 0;
    mutable std::mutex stateLock;
    Json stateCopy = Json::object();
    std::shared_ptr<bool> alive = std::make_shared<bool> (true);
};

} // namespace plenio::plugin
