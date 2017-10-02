#pragma once

#include "lift/Request.h"
#include "lift/RequestHandle.h"

#include <deque>
#include <memory>

namespace lift
{

class CurlPool;

class RequestPool
{
    friend class Request;

public:
    RequestPool();
    ~RequestPool();

    RequestPool(const RequestPool&) = delete;                       ///< No copying
    RequestPool(RequestPool&&) = default;                           ///< Can move
    auto operator = (const RequestPool&) -> RequestPool& = delete;  ///< No copy assign
    auto operator = (RequestPool&&) -> RequestPool& = default;      ///< Can move assign

    /**
     * Produces a new Request.
     *
     * This function is thread safe.
     *
     * @param url The url of the Request.
     * @param timeout_ms The timeout of the request in milliseconds.
     * @return A Request object setup for the URL + Timeout.
     */
    auto Produce(
        const std::string& url,
        uint64_t timeout_ms = 0
    ) -> Request;

private:
    std::mutex m_lock;                                      ///< Used for thread safe calls.
    std::deque<std::unique_ptr<RequestHandle>> m_requests;  ///< Pool of un-used Request handles.
    std::unique_ptr<CurlPool> m_curl_pool;                  ///< Pool of CURL* handles.

    /**
     * Returns a Request object to the pool to be re-used.
     *
     * This function is thread safe.
     *
     * @param request The request to return to the pool to be re-used.
     */
    auto returnRequest(
        std::unique_ptr<RequestHandle> request
    ) -> void;
};

} // lift