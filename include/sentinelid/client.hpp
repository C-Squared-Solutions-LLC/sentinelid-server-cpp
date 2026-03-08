#pragma once
#include "models.hpp"
#include <memory>
#include <string>
#include <optional>

namespace sentinelid {

class ApiException : public std::runtime_error {
public:
    int status_code;
    std::string error_code;
    ApiException(int status_code, std::string error_code)
        : std::runtime_error("SentinelID API error " + std::to_string(status_code) + ": " + error_code),
          status_code(status_code), error_code(std::move(error_code)) {}
};

class ServerClient {
public:
    ServerClient(std::string api_key, std::string game_id,
                 std::string base_url = "https://api.sentineltrustplay.io");
    ~ServerClient();

    // Non-copyable, movable
    ServerClient(const ServerClient&) = delete;
    ServerClient& operator=(const ServerClient&) = delete;
    ServerClient(ServerClient&&) noexcept;
    ServerClient& operator=(ServerClient&&) noexcept;

    DeviceCheckResponse device_check(const DeviceCheckRequest& req);
    CreateBanResponse create_ban(const CreateBanRequest& req);
    RevokeBanResponse revoke_ban(const std::string& ban_id);
    BanListResponse list_cheat_bans(const std::string& device_id,
        std::optional<std::string> status = std::nullopt,
        std::optional<int> limit = std::nullopt,
        std::optional<int64_t> cursor = std::nullopt);
    BanListResponse list_social_bans(const std::string& device_id,
        std::optional<std::string> status = std::nullopt,
        std::optional<int> limit = std::nullopt,
        std::optional<int64_t> cursor = std::nullopt);
    PublisherPolicy get_policy();
    PublisherPolicy update_policy(const UpdatePolicyRequest& req);

private:
    struct Impl;
    std::unique_ptr<Impl> pimpl_;
};

} // namespace sentinelid
