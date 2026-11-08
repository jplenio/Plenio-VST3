#include "core/client.h"
#include "core/log.h"

#include <httplib.h>

#include <fstream>

namespace plenio::core
{
namespace
{
constexpr std::size_t maxAnswerBytes = 64u * 1024u * 1024u;

ApiError fromResult (const httplib::Result& r)
{
    ApiError e;
    if (! r)
    {
        e.code = "network";
        auto why = httplib::to_string (r.error());
        e.message = "ComfyUI with Plenio does not answer (" + why + ")";
        return e;
    }
    e.status = r->status;
    auto parsed = json::parse (r->body);
    if (parsed && parsed->is_object())
    {
        const auto& error = json::obj (*parsed, "error");
        e.code = json::str (error, "code", {}, 64);
        e.message = json::str (error, "message", {}, 2000);
        e.hint = json::str (error, "hint", {}, 2000);
        e.field = json::str (error, "field", {}, 200);
    }
    if (e.message.empty())
        e.message = "Plenio answered " + std::to_string (r->status) + (r->body.empty() ? "" : ": " + r->body.substr (0, 300));
    if (e.code.empty())
        e.code = "http_" + std::to_string (r->status);
    return e;
}

Result<Json> answer (const httplib::Result& r)
{
    if (! r || r->status < 200 || r->status >= 300)
        return fromResult (r);
    if (r->body.size() > maxAnswerBytes)
        return ApiError { r->status, "too_large", "Plenio's answer is too large", {}, {} };
    if (r->body.empty())
        return Json (nullptr);
    auto parsed = json::parse (r->body);
    if (! parsed)
        return ApiError { r->status, "bad_answer", "Plenio's answer is not JSON", {}, {} };
    return *parsed;
}

httplib::Client make (const std::string& host, int port, int timeoutSeconds)
{
    httplib::Client c (host, port);
    c.set_connection_timeout (3, 0);
    c.set_read_timeout (timeoutSeconds, 0);
    c.set_write_timeout (timeoutSeconds, 0);
    c.set_keep_alive (false);
    return c;
}
} // namespace

std::string encodeSegment (std::string_view text)
{
    static const char* hex = "0123456789ABCDEF";
    std::string out;
    for (unsigned char c : text)
    {
        if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '-' || c == '_' || c == '.' || c == '~')
            out += static_cast<char> (c);
        else
        {
            out += '%';
            out += hex[c >> 4];
            out += hex[c & 15];
        }
    }
    return out;
}

PlenioClient::PlenioClient (std::string host, int port) : hostName (std::move (host)), portNumber (port) {}

std::string PlenioClient::baseUrl() const
{
    return "http://" + hostName + ":" + std::to_string (portNumber);
}

Result<Json> PlenioClient::get (const std::string& path, int timeoutSeconds) const
{
    auto c = make (hostName, portNumber, timeoutSeconds);
    return answer (c.Get (path));
}

Result<Json> PlenioClient::post (const std::string& path, const Json& body, const std::string& idempotencyKey, int timeoutSeconds) const
{
    auto c = make (hostName, portNumber, timeoutSeconds);
    httplib::Headers headers;
    if (! idempotencyKey.empty())
        headers.emplace ("Idempotency-Key", idempotencyKey.substr (0, 200));
    return answer (c.Post (path, headers, body.dump (-1, ' ', false, Json::error_handler_t::replace), "application/json"));
}

Result<Json> PlenioClient::del (const std::string& path, int timeoutSeconds) const
{
    auto c = make (hostName, portNumber, timeoutSeconds);
    return answer (c.Delete (path));
}

std::optional<ApiError> PlenioClient::download (const std::string& path, const std::filesystem::path& target,
                                                const std::function<bool (std::uint64_t, std::uint64_t)>& progress) const
{
    std::error_code ec;
    std::filesystem::create_directories (target.parent_path(), ec);
    auto temp = target;
    temp += ".part";
    std::ofstream out (temp, std::ios::binary | std::ios::trunc);
    if (! out)
        return ApiError { 0, "disk", "The file could not be written: " + temp.string(), {}, {} };

    auto c = make (hostName, portNumber, 300);
    std::uint64_t done = 0, total = 0;
    bool stopped = false;
    auto r = c.Get (
        path,
        [&] (const httplib::Response& response)
        {
            if (response.status < 200 || response.status >= 300)
                return false;
            if (response.has_header ("Content-Length"))
                total = std::strtoull (response.get_header_value ("Content-Length").c_str(), nullptr, 10);
            return true;
        },
        [&] (const char* data, std::size_t length)
        {
            out.write (data, static_cast<std::streamsize> (length));
            done += length;
            if (progress && ! progress (done, total))
            {
                stopped = true;
                return false;
            }
            return static_cast<bool> (out);
        });
    out.close();
    if (stopped)
    {
        std::filesystem::remove (temp, ec);
        return ApiError { 0, "cancelled", "The download was stopped", {}, {} };
    }
    if (! r || r->status < 200 || r->status >= 300)
    {
        std::filesystem::remove (temp, ec);
        return fromResult (r);
    }
    std::filesystem::rename (temp, target, ec);
    if (ec)
        return ApiError { 0, "disk", "The file could not be put in place: " + target.string(), {}, {} };
    return std::nullopt;
}

Result<Json> PlenioClient::upload (const std::string& path, const std::filesystem::path& file, const std::string& contentType) const
{
    std::error_code ec;
    auto size = std::filesystem::file_size (file, ec);
    if (ec)
        return ApiError { 0, "disk", "The recording could not be read: " + file.string(), {}, {} };
    auto in = std::make_shared<std::ifstream> (file, std::ios::binary);
    if (! *in)
        return ApiError { 0, "disk", "The recording could not be opened: " + file.string(), {}, {} };
    auto c = make (hostName, portNumber, 600);
    auto r = c.Post (
        path, static_cast<std::size_t> (size),
        [in] (std::size_t offset, std::size_t length, httplib::DataSink& sink)
        {
            std::vector<char> buffer (std::min<std::size_t> (length, 256 * 1024));
            in->seekg (static_cast<std::streamoff> (offset));
            in->read (buffer.data(), static_cast<std::streamsize> (buffer.size()));
            auto got = static_cast<std::size_t> (in->gcount());
            return got > 0 && sink.write (buffer.data(), got);
        },
        contentType);
    return answer (r);
}

bool PlenioClient::reachable (int timeoutMilliseconds) const
{
    httplib::Client c (hostName, portNumber);
    c.set_connection_timeout (0, timeoutMilliseconds * 1000);
    c.set_read_timeout (2, 0);
    auto r = c.Get ("/plenio/api/v1");
    return r && r->status == 200;
}

} // namespace plenio::core
