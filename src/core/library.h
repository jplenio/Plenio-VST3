// The song library on disk (Documents\Plenio\Songs by default): one folder per song, "<title> [<id>]", with its
// song.json and every take's files. Projects keep the song without its audio in their state; the audio stays here,
// so a saved project plays and edits without ComfyUI and Plenio.
#pragma once

#include "core/song.h"

#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace plenio::core
{

/// Writes a file through a temporary one and a rename: a crash never leaves half a file.
bool writeFileAtomically (const std::filesystem::path& file, std::string_view content);
std::optional<std::string> readFile (const std::filesystem::path& file, std::size_t limit = 64u * 1024u * 1024u);

struct SongSummary
{
    std::string id;
    std::string title;
    std::string kind;
    int takes = 0;
    std::int64_t modified = 0;
    std::optional<double> bestStars;
    std::filesystem::path folder;
};

class Library
{
public:
    explicit Library (std::filesystem::path root);

    const std::filesystem::path& root() const { return rootFolder; }

    /// The folder of the song with this id, if the library has it.
    std::optional<std::filesystem::path> find (std::string_view id) const;

    /// The song's folder, made if it is not there yet ("<title> [<id>]").
    std::filesystem::path folderFor (const Song& song) const;

    /// The folder of a take inside the song's folder (takes/<number>).
    static std::filesystem::path takeFolder (const std::filesystem::path& songFolder, int number);

    bool save (const Song& song) const;
    std::optional<Song> load (std::string_view id) const;
    static std::optional<Song> loadFrom (const std::filesystem::path& folder);

    /// Every song of the library, newest first.
    std::vector<SongSummary> list() const;

    /// A file of a song given relative to its folder, made absolute (or found again when it moved).
    std::filesystem::path resolve (const Song& song, std::string_view relative) const;

private:
    std::filesystem::path rootFolder;
};

} // namespace plenio::core
