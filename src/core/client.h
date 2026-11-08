// HTTP to the Plenio Song API on the ComfyUI of this computer (127.0.0.1). Blocking calls for worker threads, never
// for the UI or audio thread. Errors come back in words: Plenio's own error object, or what the network said.
#pragma once

#include "core/json.h"

#include <cstdint>
#include <filesystem>
#include <functional>
#include <optional>
#include <string>
#include <variant>

namespace plenio::core
{

struct ApiError
{
    int status = 0;          // HTTP status; 0: no answer
    std::string code;        // Plenio's code (invalid_value, not_found, …) or "network"
    std::string message;     // for people
    std::string hint;
    std::string field;

    bool network() const { return status == 0; }
    std::string text() const { return hint.empty() ? message : message + " (" + hint + ")"; }
};

template <typename T>
using Result = std::variant<T, ApiError>;

template <typename T>
bool ok (const Result<T>& r) { return std::holds_alternative<T> (r); }

class PlenioClient
{
public:
    PlenioClient (std::string host, int port);

    const std::string& host() const { return hostName; }
    int port() const { return portNumber; }
    std::string baseUrl() const;

    Result<Json> get (const std::string& path, int timeoutSeconds = 30) const;
    Result<Json> post (const std::string& path, const Json& body, const std::string& idempotencyKey = {}, int timeoutSeconds = 120) const;
    Result<Json> del (const std::string& path, int timeoutSeconds = 30) const;

    /// Downloads to `target` through a temporary file; `progress(done, total)` may stop it by returning false.
    std::optional<ApiError> download (const std::string& path, const std::filesystem::path& target,
                                      const std::function<bool (std::uint64_t, std::uint64_t)>& progress = {}) const;

    /// Uploads a file as the request body (POST), e.g. a cover's recording.
    Result<Json> upload (const std::string& path, const std::filesystem::path& file, const std::string& contentType) const;

    /// Whether Plenio's Song API answers at all (short timeout).
    bool reachable (int timeoutMilliseconds = 1500) const;

private:
    std::string hostName;
    int portNumber;
};

/// Percent-encoding of a path segment (UTF-8).
std::string encodeSegment (std::string_view text);

} // namespace plenio::core
