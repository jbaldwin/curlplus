#include "lift/Request.hpp"
#include "lift/Const.hpp"
#include "lift/Executor.hpp"

namespace lift {

Request::Request(
    std::string url,
    std::optional<std::chrono::milliseconds> timeout,
    OnCompleteHandlerType on_complete_handler)
    : m_url(std::move(url))
    , m_timeout(std::move(timeout))
    , m_on_complete_handler(std::move(on_complete_handler))
{
}

auto Request::Perform(
    SharePtr share_ptr) -> Response
{
    Executor executor { this, share_ptr.get() };
    return executor.perform();
}

auto Request::OnCompleteHandler(
    OnCompleteHandlerType on_complete_handler) -> void
{
    m_on_complete_handler = std::move(on_complete_handler);
}

auto Request::TransferProgressHandler(
    std::optional<TransferProgressHandlerType> transfer_progress_handler) -> void
{
    if (transfer_progress_handler.has_value() && transfer_progress_handler.value()) {
        m_on_transfer_progress_handler = std::move(transfer_progress_handler.value());
    } else {
        m_on_transfer_progress_handler = nullptr;
    }
}

auto Request::FollowRedirects(
    bool follow_redirects,
    int64_t max_redirects) -> void
{
    if (follow_redirects) {
        m_follow_redirects = true;
        if (max_redirects >= -1) {
            m_max_redirects = max_redirects;
        } else {
            // treat any negative number as -1 (infinite).
            m_max_redirects = -1;
        }
    } else {
        m_follow_redirects = false;
    }
}

auto Request::Header(
    std::string_view name,
    std::string_view value) -> void
{
    m_request_headers.emplace_back(name, value);
}

auto Request::Data(
    std::string data) -> void
{
    if (m_mime_fields_set) {
        throw std::logic_error("Cannot set POST request data on Request after using adding Mime Fields.");
    }

    m_request_data_set = true;
    m_request_data = std::move(data);
    m_method = http::Method::POST;
}

auto Request::MimeField(
    lift::MimeField mime_field) -> void
{
    if (m_request_data_set) {
        throw std::logic_error("Cannot add Mime Fields on Request after using POST request data.");
    }

    m_mime_fields_set = true;
    m_mime_fields.emplace_back(std::move(mime_field));
}

} // namespace lift
