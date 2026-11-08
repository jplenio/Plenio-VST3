#include "core/log.h"
#include "core/paths.h"

#include <chrono>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <mutex>

namespace plenio::log
{
namespace
{
constexpr std::uintmax_t rotateAt = 4u * 1024u * 1024u;

std::mutex& lock()
{
    static std::mutex m;
    return m;
}

std::filesystem::path logPath()
{
    return paths::localData() / "plenio.log";
}

const char* name (Level level)
{
    switch (level)
    {
        case Level::debug:   return "debug";
        case Level::info:    return "info ";
        case Level::warning: return "warn ";
        case Level::error:   return "ERROR";
    }
    return "?";
}
} // namespace

std::string filePath()
{
    return paths::toUtf8 (logPath());
}

void write (Level level, std::string_view message) noexcept
{
    try
    {
        std::scoped_lock guard (lock());
        auto path = logPath();
        std::error_code ec;
        std::filesystem::create_directories (path.parent_path(), ec);

        if (std::filesystem::exists (path, ec) && std::filesystem::file_size (path, ec) > rotateAt)
        {
            auto old = path;
            old += ".1";
            std::filesystem::remove (old, ec);
            std::filesystem::rename (path, old, ec);
        }

        std::ofstream out (path, std::ios::app | std::ios::binary);
        if (! out)
            return;

        auto now = std::chrono::floor<std::chrono::milliseconds> (std::chrono::system_clock::now());
        out << std::format ("{:%Y-%m-%d %H:%M:%S} {} ", now, name (level)) << message << "\n";
    }
    catch (...)
    {
    }
}

} // namespace plenio::log
