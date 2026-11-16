#pragma once
// Reconstructed from sub_180095270 / sub_1800966D0 / sub_180096360
// HTTP client with WinHTTP, max 8 threads.

#include <windows.h>
#include <string>
#include <vector>
#include <cstdint>

namespace net {

// ─── HTTP request (from sub_180095270 structure) ──────────────────────────
struct HttpRequest {
    std::string url;           // +0 (string)
    std::string method;        // +16
    std::string body;          // +32
    std::string content_type;  // +48
    int         timeout_ms;    // +136
    bool        use_https;     // from scheme check
};

// ─── HTTP response ─────────────────────────────────────────────────────────
struct HttpResponse {
    int         status_code;    // +16
    std::string body;           // +24
    std::string headers;        // +56
    bool        success;
};

// ─── Thread result (from sub_180096360, 112 bytes per entry) ──────────────
struct HttpResult {
    int         status_code;
    bool        success;
    std::string body;
    std::string headers;
};

// ─── Client ────────────────────────────────────────────────────────────────
class HttpClient {
public:
    static HttpClient& Instance() {
        static HttpClient inst;
        return inst;
    }

    static constexpr int MAX_CONCURRENT = 8;   // dword_180CD0250
    static constexpr const char* USER_AGENT = "newerwin";
    static constexpr const char* BASE_URL = "https://lua.neverwin.one";

    // ─── Execute (async, creates thread) ──────────────────────────────
    bool GetAsync(const std::string& url, void(*callback)(const HttpResponse&)) {
        if (active_threads_ >= MAX_CONCURRENT)
            return false;

        active_threads_++;
        // CreateThread → sub_180096360 → sub_180095270
        return true;
    }

    bool PostAsync(const std::string& url, const std::string& body,
                   void(*callback)(const HttpResponse&))
    {
        if (active_threads_ >= MAX_CONCURRENT)
            return false;

        active_threads_++;
        return true;
    }

    // ─── Execute (sync, from sub_180095270) ───────────────────────────
    HttpResponse Execute(const HttpRequest& request) {
        HttpResponse response{};

        // 1. Crack URL (WinHttpCrackUrl)
        //    - scheme must be http or https
        //    - split host, port, path

        // 2. Open session (WinHttpOpen, UA = "newerwin")
        //    WinHttpOpen(L"newerwin", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, ...)

        // 3. Set timeouts
        //    WinHttpSetTimeouts(session, timeout, timeout, timeout, timeout)

        // 4. Connect
        //    WinHttpConnect(session, host, port, 0)

        // 5. Open request
        //    WinHttpOpenRequest(connection, method, path, ...)

        // 6. Send
        //    WinHttpSendRequest(request, headers, -1, body, body_len, body_len, 0)

        // 7. Receive
        //    WinHttpReceiveResponse(request, 0)

        // 8. Query status
        //    WinHttpQueryHeaders(request, WINHTTP_QUERY_STATUS_CODE, ...)

        // 9. Read body
        //    WinHttpQueryDataAvailable + WinHttpReadData (loop)

        return response;
    }

private:
    int active_threads_ = 0;
    std::vector<HttpResult> results_;
};

// ─── URLs found in binary ──────────────────────────────────────────────────
namespace urls {
    constexpr const char* LUA_API       = "https://lua.neverwin.one";
    constexpr const char* TICKETS       = "https://neverwin.one/tickets/";
    constexpr const char* TERMS         = "https://neverwin.one/help/terms/";
}

} // namespace net

