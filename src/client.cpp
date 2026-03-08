#include "sentinelid/client.hpp"

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>
#include <nlohmann/json.hpp>

#include <sstream>
#include <stdexcept>
#include <utility>

namespace sentinelid {

// ---------------------------------------------------------------------------
// Impl (pimpl)
// ---------------------------------------------------------------------------
struct ServerClient::Impl {
    httplib::Client http;
    std::string api_key;
    std::string game_id;

    Impl(const std::string& base_url, std::string api_key, std::string game_id)
        : http(base_url), api_key(std::move(api_key)), game_id(std::move(game_id))
    {
        http.set_follow_location(true);
        http.set_connection_timeout(10);
        http.set_read_timeout(30);
        http.set_write_timeout(10);
    }

    httplib::Headers make_headers() const {
        return {
            {"Authorization", "Bearer " + api_key},
            {"X-Game-Id", game_id},
            {"Accept", "application/json"}
        };
    }

    // Validate a response: throw ApiException on non-2xx or connection error.
    void check_response(const httplib::Result& result, const std::string& context) const {
        if (!result) {
            throw ApiException(0, "Connection error during " + context + ": " +
                               httplib::to_string(result.error()));
        }
        if (result->status < 200 || result->status >= 300) {
            std::string error_msg;
            try {
                auto err = nlohmann::json::parse(result->body).get<ErrorResponse>();
                error_msg = err.error;
            } catch (...) {
                error_msg = result->body.empty() ? "Unknown error" : result->body;
            }
            throw ApiException(result->status, error_msg);
        }
    }

    // POST helper
    nlohmann::json post(const std::string& path, const nlohmann::json& body) {
        auto result = http.Post(path, make_headers(), body.dump(), "application/json");
        check_response(result, "POST " + path);
        return nlohmann::json::parse(result->body);
    }

    // GET helper
    nlohmann::json get(const std::string& path) {
        auto result = http.Get(path, make_headers());
        check_response(result, "GET " + path);
        return nlohmann::json::parse(result->body);
    }

    // PUT helper
    nlohmann::json put(const std::string& path, const nlohmann::json& body) {
        auto result = http.Put(path, make_headers(), body.dump(), "application/json");
        check_response(result, "PUT " + path);
        return nlohmann::json::parse(result->body);
    }

    // Build a query string from optional params, prepended with '?'
    static std::string build_query_string(
        const std::optional<std::string>& status,
        const std::optional<int>& limit,
        const std::optional<int64_t>& cursor)
    {
        std::ostringstream qs;
        bool first = true;
        auto append = [&](const std::string& kv) {
            qs << (first ? '?' : '&') << kv;
            first = false;
        };
        if (status.has_value())  append("status=" + status.value());
        if (limit.has_value())   append("limit=" + std::to_string(limit.value()));
        if (cursor.has_value())  append("cursor=" + std::to_string(cursor.value()));
        return qs.str();
    }
};

// ---------------------------------------------------------------------------
// ServerClient lifetime
// ---------------------------------------------------------------------------
ServerClient::ServerClient(std::string api_key, std::string game_id, std::string base_url)
    : pimpl_(std::make_unique<Impl>(base_url, std::move(api_key), std::move(game_id)))
{
}

ServerClient::~ServerClient() = default;

ServerClient::ServerClient(ServerClient&&) noexcept = default;
ServerClient& ServerClient::operator=(ServerClient&&) noexcept = default;

// ---------------------------------------------------------------------------
// API methods
// ---------------------------------------------------------------------------

DeviceCheckResponse ServerClient::device_check(const DeviceCheckRequest& req) {
    nlohmann::json body = req;
    auto resp_json = pimpl_->post("/v1/device/check", body);
    return resp_json.get<DeviceCheckResponse>();
}

CreateBanResponse ServerClient::create_ban(const CreateBanRequest& req) {
    nlohmann::json body = req;
    auto resp_json = pimpl_->post("/v1/bans", body);
    return resp_json.get<CreateBanResponse>();
}

RevokeBanResponse ServerClient::revoke_ban(int64_t ban_id) {
    std::string path = "/v1/bans/" + std::to_string(ban_id) + "/revoke";
    auto resp_json = pimpl_->post(path, nlohmann::json::object());
    return resp_json.get<RevokeBanResponse>();
}

BanListResponse ServerClient::list_cheat_bans(
    const std::string& device_id,
    std::optional<std::string> status,
    std::optional<int> limit,
    std::optional<int64_t> cursor)
{
    std::string path = "/v1/device/" + device_id + "/bans/cheat" + Impl::build_query_string(status, limit, cursor);
    auto resp_json = pimpl_->get(path);
    return resp_json.get<BanListResponse>();
}

BanListResponse ServerClient::list_social_bans(
    const std::string& device_id,
    std::optional<std::string> status,
    std::optional<int> limit,
    std::optional<int64_t> cursor)
{
    std::string path = "/v1/device/" + device_id + "/bans/social" + Impl::build_query_string(status, limit, cursor);
    auto resp_json = pimpl_->get(path);
    return resp_json.get<BanListResponse>();
}

PublisherPolicy ServerClient::get_policy() {
    auto resp_json = pimpl_->get("/v1/policy");
    return resp_json.get<PublisherPolicy>();
}

PublisherPolicy ServerClient::update_policy(const UpdatePolicyRequest& req) {
    nlohmann::json body = req;
    auto resp_json = pimpl_->put("/v1/policy", body);
    return resp_json.get<PublisherPolicy>();
}

} // namespace sentinelid
