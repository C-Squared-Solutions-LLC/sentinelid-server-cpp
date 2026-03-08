#pragma once
#include <nlohmann/json.hpp>
#include <string>
#include <optional>
#include <vector>
#include <cstdint>

namespace sentinelid {

// Helper: read a JSON field that may be either a string or an integer
inline std::string json_str_or_num(const nlohmann::json& j, const char* key) {
    const auto& v = j.at(key);
    if (v.is_string()) return v.get<std::string>();
    if (v.is_number_integer()) return std::to_string(v.get<int64_t>());
    if (v.is_number()) return std::to_string(v.get<double>());
    throw nlohmann::json::type_error::create(302, "ban_id must be string or number", &v);
}

// Helper: write optional field to JSON only if it has a value
template <typename T>
void optional_to_json(nlohmann::json& j, const char* key, const std::optional<T>& opt) {
    if (opt.has_value()) {
        j[key] = opt.value();
    }
}

// Helper: read optional field from JSON if present
template <typename T>
void optional_from_json(const nlohmann::json& j, const char* key, std::optional<T>& opt) {
    if (j.contains(key) && !j.at(key).is_null()) {
        opt = j.at(key).get<T>();
    } else {
        opt = std::nullopt;
    }
}

// ---------------------------------------------------------------------------
// DeviceCheckRequest
// ---------------------------------------------------------------------------
struct DeviceCheckRequest {
    std::string device_id;
    std::string publisher_id;
    std::string game_id;
    std::string alg;
    std::string payload_b64;
    std::string sig_b64;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(DeviceCheckRequest, device_id, publisher_id, game_id, alg, payload_b64, sig_b64)

// ---------------------------------------------------------------------------
// BanCategoryCounts
// ---------------------------------------------------------------------------
struct BanCategoryCounts {
    int64_t active;
    int64_t inactive;
    int64_t total;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(BanCategoryCounts, active, inactive, total)

// ---------------------------------------------------------------------------
// BanSummary
// ---------------------------------------------------------------------------
struct BanSummary {
    BanCategoryCounts cheat;
    BanCategoryCounts social;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(BanSummary, cheat, social)

// ---------------------------------------------------------------------------
// Reputation
// ---------------------------------------------------------------------------
struct Reputation {
    double cheat_score;
    double social_score;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Reputation, cheat_score, social_score)

// ---------------------------------------------------------------------------
// DeviceCheckResponse
// ---------------------------------------------------------------------------
struct DeviceCheckResponse {
    std::string device_id;
    std::string publisher_id;
    std::string game_id;
    bool banned;
    BanSummary bans;
    Reputation reputation;
    std::string status;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(DeviceCheckResponse, device_id, publisher_id, game_id, banned, bans, reputation, status)

// ---------------------------------------------------------------------------
// CreateBanRequest
// ---------------------------------------------------------------------------
struct CreateBanRequest {
    std::string device_id;
    std::string ban_type;
    std::string scope;
    std::string reason_code;
    std::optional<std::string> expires_at;
    std::optional<nlohmann::json> details;
    std::optional<std::string> idempotency_key;
};

inline void to_json(nlohmann::json& j, const CreateBanRequest& v) {
    j = nlohmann::json{
        {"device_id", v.device_id},
        {"ban_type", v.ban_type},
        {"scope", v.scope},
        {"reason_code", v.reason_code}
    };
    optional_to_json(j, "expires_at", v.expires_at);
    optional_to_json(j, "details", v.details);
    optional_to_json(j, "idempotency_key", v.idempotency_key);
}

inline void from_json(const nlohmann::json& j, CreateBanRequest& v) {
    j.at("device_id").get_to(v.device_id);
    j.at("ban_type").get_to(v.ban_type);
    j.at("scope").get_to(v.scope);
    j.at("reason_code").get_to(v.reason_code);
    optional_from_json(j, "expires_at", v.expires_at);
    optional_from_json(j, "details", v.details);
    optional_from_json(j, "idempotency_key", v.idempotency_key);
}

// ---------------------------------------------------------------------------
// CreateBanResponse
// ---------------------------------------------------------------------------
struct CreateBanResponse {
    std::string ban_id;
    std::string device_id;
    std::string ban_type;
    std::string scope;
    std::string publisher_id;
    std::optional<std::string> game_id;
    std::string status;
};

inline void to_json(nlohmann::json& j, const CreateBanResponse& v) {
    j = nlohmann::json{
        {"ban_id", v.ban_id},
        {"device_id", v.device_id},
        {"ban_type", v.ban_type},
        {"scope", v.scope},
        {"publisher_id", v.publisher_id},
        {"status", v.status}
    };
    optional_to_json(j, "game_id", v.game_id);
}

inline void from_json(const nlohmann::json& j, CreateBanResponse& v) {
    v.ban_id = json_str_or_num(j, "ban_id");
    j.at("device_id").get_to(v.device_id);
    j.at("ban_type").get_to(v.ban_type);
    j.at("scope").get_to(v.scope);
    j.at("publisher_id").get_to(v.publisher_id);
    optional_from_json(j, "game_id", v.game_id);
    j.at("status").get_to(v.status);
}

// ---------------------------------------------------------------------------
// RevokeBanResponse
// ---------------------------------------------------------------------------
struct RevokeBanResponse {
    std::string ban_id;
    std::string device_id;
    std::string status;
};

inline void to_json(nlohmann::json& j, const RevokeBanResponse& v) {
    j = nlohmann::json{{"ban_id", v.ban_id}, {"device_id", v.device_id}, {"status", v.status}};
}

inline void from_json(const nlohmann::json& j, RevokeBanResponse& v) {
    v.ban_id = json_str_or_num(j, "ban_id");
    j.at("device_id").get_to(v.device_id);
    j.at("status").get_to(v.status);
}

// ---------------------------------------------------------------------------
// BanItem
// ---------------------------------------------------------------------------
struct BanItem {
    std::string ban_id;
    std::string ban_type;
    std::string scope;
    std::string publisher_id;
    std::optional<std::string> game_id;
    std::string reason_code;
    std::optional<std::string> details_json;
    std::string created_at;
    std::optional<std::string> expires_at;
    std::optional<std::string> revoked_at;
};

inline void to_json(nlohmann::json& j, const BanItem& v) {
    j = nlohmann::json{
        {"ban_id", v.ban_id},
        {"ban_type", v.ban_type},
        {"scope", v.scope},
        {"publisher_id", v.publisher_id},
        {"reason_code", v.reason_code},
        {"created_at", v.created_at}
    };
    optional_to_json(j, "game_id", v.game_id);
    optional_to_json(j, "details_json", v.details_json);
    optional_to_json(j, "expires_at", v.expires_at);
    optional_to_json(j, "revoked_at", v.revoked_at);
}

inline void from_json(const nlohmann::json& j, BanItem& v) {
    v.ban_id = json_str_or_num(j, "ban_id");
    j.at("ban_type").get_to(v.ban_type);
    j.at("scope").get_to(v.scope);
    j.at("publisher_id").get_to(v.publisher_id);
    optional_from_json(j, "game_id", v.game_id);
    j.at("reason_code").get_to(v.reason_code);
    optional_from_json(j, "details_json", v.details_json);
    j.at("created_at").get_to(v.created_at);
    optional_from_json(j, "expires_at", v.expires_at);
    optional_from_json(j, "revoked_at", v.revoked_at);
}

// ---------------------------------------------------------------------------
// BanListResponse
// ---------------------------------------------------------------------------
struct BanListResponse {
    std::string device_id;
    std::string status;
    BanCategoryCounts counts;
    std::vector<BanItem> items;
};

inline void to_json(nlohmann::json& j, const BanListResponse& v) {
    j = nlohmann::json{
        {"device_id", v.device_id},
        {"status", v.status},
        {"counts", v.counts},
        {"items", v.items}
    };
}

inline void from_json(const nlohmann::json& j, BanListResponse& v) {
    j.at("device_id").get_to(v.device_id);
    j.at("status").get_to(v.status);
    j.at("counts").get_to(v.counts);
    j.at("items").get_to(v.items);
}

// ---------------------------------------------------------------------------
// PublisherPolicy
// ---------------------------------------------------------------------------
struct PublisherPolicy {
    std::string publisher_id;
    bool enforce_game;
    bool enforce_publisher;
    bool enforce_global;
    bool enforce_cheat_global;
    bool enforce_social_global;
    bool rep_include_game;
    bool rep_include_publisher;
    bool rep_include_global;
    std::string updated_at;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PublisherPolicy, publisher_id, enforce_game, enforce_publisher, enforce_global, enforce_cheat_global, enforce_social_global, rep_include_game, rep_include_publisher, rep_include_global, updated_at)

// ---------------------------------------------------------------------------
// UpdatePolicyRequest
// ---------------------------------------------------------------------------
struct UpdatePolicyRequest {
    std::optional<bool> enforce_game;
    std::optional<bool> enforce_publisher;
    std::optional<bool> enforce_global;
    std::optional<bool> enforce_cheat_global;
    std::optional<bool> enforce_social_global;
    std::optional<bool> rep_include_game;
    std::optional<bool> rep_include_publisher;
    std::optional<bool> rep_include_global;
};

inline void to_json(nlohmann::json& j, const UpdatePolicyRequest& v) {
    j = nlohmann::json::object();
    optional_to_json(j, "enforce_game", v.enforce_game);
    optional_to_json(j, "enforce_publisher", v.enforce_publisher);
    optional_to_json(j, "enforce_global", v.enforce_global);
    optional_to_json(j, "enforce_cheat_global", v.enforce_cheat_global);
    optional_to_json(j, "enforce_social_global", v.enforce_social_global);
    optional_to_json(j, "rep_include_game", v.rep_include_game);
    optional_to_json(j, "rep_include_publisher", v.rep_include_publisher);
    optional_to_json(j, "rep_include_global", v.rep_include_global);
}

inline void from_json(const nlohmann::json& j, UpdatePolicyRequest& v) {
    optional_from_json(j, "enforce_game", v.enforce_game);
    optional_from_json(j, "enforce_publisher", v.enforce_publisher);
    optional_from_json(j, "enforce_global", v.enforce_global);
    optional_from_json(j, "enforce_cheat_global", v.enforce_cheat_global);
    optional_from_json(j, "enforce_social_global", v.enforce_social_global);
    optional_from_json(j, "rep_include_game", v.rep_include_game);
    optional_from_json(j, "rep_include_publisher", v.rep_include_publisher);
    optional_from_json(j, "rep_include_global", v.rep_include_global);
}

// ---------------------------------------------------------------------------
// ErrorResponse
// ---------------------------------------------------------------------------
struct ErrorResponse {
    std::string error;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ErrorResponse, error)

} // namespace sentinelid
