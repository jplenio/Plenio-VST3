#include "core/library.h"
#include "core/log.h"
#include "core/paths.h"

#include <algorithm>
#include <fstream>
#include <sstream>

namespace plenio::core
{
namespace fs = std::filesystem;

bool writeFileAtomically (const fs::path& file, std::string_view content)
{
    std::error_code ec;
    fs::create_directories (file.parent_path(), ec);
    auto temp = file;
    temp += ".tmp";
    {
        std::ofstream out (temp, std::ios::binary | std::ios::trunc);
        if (! out)
            return false;
        out.write (content.data(), static_cast<std::streamsize> (content.size()));
        if (! out)
            return false;
    }
    fs::rename (temp, file, ec);
    if (ec)
    {
        // a reader holding the old file open on Windows: replace by copy
        fs::copy_file (temp, file, fs::copy_options::overwrite_existing, ec);
        fs::remove (temp);
        return ! ec;
    }
    return true;
}

std::optional<std::string> readFile (const fs::path& file, std::size_t limit)
{
    std::error_code ec;
    auto size = fs::file_size (file, ec);
    if (ec || size > limit)
        return std::nullopt;
    std::ifstream in (file, std::ios::binary);
    if (! in)
        return std::nullopt;
    std::string content (static_cast<std::size_t> (size), '\0');
    in.read (content.data(), static_cast<std::streamsize> (size));
    if (! in && ! in.eof())
        return std::nullopt;
    return content;
}

Library::Library (fs::path root) : rootFolder (std::move (root)) {}

std::optional<fs::path> Library::find (std::string_view id) const
{
    if (id.empty())
        return std::nullopt;
    auto suffix = "[" + std::string (id) + "]";
    std::error_code ec;
    for (const auto& entry : fs::directory_iterator (rootFolder, ec))
    {
        if (! entry.is_directory (ec))
            continue;
        auto name = paths::toUtf8 (entry.path().filename());
        if (name.size() >= suffix.size() && name.compare (name.size() - suffix.size(), suffix.size(), suffix) == 0)
            return entry.path();
    }
    return std::nullopt;
}

fs::path Library::folderFor (const Song& song) const
{
    if (auto found = find (song.id))
        return *found;
    auto title = song.title.empty() ? std::string ("Untitled") : song.title;
    auto folder = rootFolder / paths::fromUtf8 (paths::safeName (title, 60) + " [" + song.id + "]");
    std::error_code ec;
    fs::create_directories (folder, ec);
    if (ec)
        log::error ("library: {} could not be made: {}", paths::toUtf8 (folder), ec.message());
    return folder;
}

fs::path Library::takeFolder (const fs::path& songFolder, int number)
{
    return songFolder / "takes" / std::to_string (number);
}

bool Library::save (const Song& song) const
{
    auto folder = folderFor (song);
    auto text = toJson (song).dump (1, ' ', false, Json::error_handler_t::replace);
    if (! writeFileAtomically (folder / "song.json", text))
    {
        log::error ("library: song.json of {} not written", song.id);
        return false;
    }
    return true;
}

std::optional<Song> Library::loadFrom (const fs::path& folder)
{
    auto text = readFile (folder / "song.json");
    if (! text)
        return std::nullopt;
    auto parsed = json::parse (*text);
    if (! parsed || ! parsed->is_object())
    {
        log::warn ("library: {} holds no readable song", paths::toUtf8 (folder));
        return std::nullopt;
    }
    return songFromJson (*parsed);
}

std::optional<Song> Library::load (std::string_view id) const
{
    auto folder = find (id);
    return folder ? loadFrom (*folder) : std::nullopt;
}

std::vector<SongSummary> Library::list() const
{
    std::vector<SongSummary> out;
    std::error_code ec;
    for (const auto& entry : fs::directory_iterator (rootFolder, ec))
    {
        if (! entry.is_directory (ec) || out.size() >= 5000)
            continue;
        auto text = readFile (entry.path() / "song.json", 64u * 1024u * 1024u);
        if (! text)
            continue;
        auto parsed = json::parse (*text);
        if (! parsed || ! parsed->is_object())
            continue;
        SongSummary s;
        s.id = json::str (*parsed, "id", {}, 64);
        s.title = json::str (*parsed, "title", {}, 400);
        s.kind = json::str (*parsed, "kind", "song", 16);
        s.modified = json::integer (*parsed, "modified", 0);
        for (const auto& t : json::arr (*parsed, "takes"))
        {
            ++s.takes;
            if (auto stars = json::num (t, "stars"); stars && (! s.bestStars || *stars > *s.bestStars))
                s.bestStars = *stars;
        }
        s.folder = entry.path();
        if (! s.id.empty())
            out.push_back (std::move (s));
    }
    std::sort (out.begin(), out.end(), [] (const SongSummary& a, const SongSummary& b) { return a.modified > b.modified; });
    return out;
}

fs::path Library::resolve (const Song& song, std::string_view relative) const
{
    auto rel = paths::fromUtf8 (relative);
    if (rel.is_absolute())
        return rel;
    if (auto folder = find (song.id))
        return *folder / rel;
    return folderFor (song) / rel;
}

} // namespace plenio::core
