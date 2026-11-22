#pragma once

// Bambu Cloud HTTP failure classification and retry pacing, ported from
// upstream PrintSphere (cptkirki/PrintSphere, main/include/printsphere/
// connection_policy.hpp). Used by BambuCloudClient so a Cloudflare block,
// CSRF failure or rate limit is recognised, reported in Web Config, and
// backed off from instead of being retried every few seconds.

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <string_view>

namespace printsphere::cloud_http {

enum class HttpFailure : uint8_t {
  kNone,
  kTransport,
  kUnauthorized,
  kForbidden,
  kWaf,
  kCsrf,
  kRateLimited,
  kServer,
  kRejected,
};

inline const char* failure_name(HttpFailure failure) {
  switch (failure) {
    case HttpFailure::kNone: return "none";
    case HttpFailure::kTransport: return "transport";
    case HttpFailure::kUnauthorized: return "unauthorized";
    case HttpFailure::kForbidden: return "forbidden";
    case HttpFailure::kWaf: return "cloudflare";
    case HttpFailure::kCsrf: return "csrf";
    case HttpFailure::kRateLimited: return "rate_limited";
    case HttpFailure::kServer: return "server";
    case HttpFailure::kRejected: return "rejected";
  }
  return "unknown";
}

inline HttpFailure classify_http(int status, std::string_view body) {
  if (status == 0) return HttpFailure::kTransport;
  if (status == 429) return HttpFailure::kRateLimited;
  if (status == 403 && (body.find("missing_cookie") != body.npos ||
                        body.find("CSRF") != body.npos || body.find("csrf") != body.npos)) {
    return HttpFailure::kCsrf;
  }
  if (status == 403 && (body.find("cloudflare") != body.npos ||
                        body.find("Cloudflare") != body.npos ||
                        body.find("<!DOCTYPE html") != body.npos)) {
    return HttpFailure::kWaf;
  }
  if (status == 401) return HttpFailure::kUnauthorized;
  if (status == 403) return HttpFailure::kForbidden;
  if (status >= 500) return HttpFailure::kServer;
  if (status < 200 || status >= 300) return HttpFailure::kRejected;
  return HttpFailure::kNone;
}

// Parses a Retry-After header: either delta-seconds or an IMF-fixdate
// ("Sun, 06 Nov 1994 08:49:37 GMT"). Falls back to 300 s when unparseable.
inline uint32_t retry_after_seconds(const std::string& value, int64_t now_utc) {
  if (value.empty()) return 300;
  char* end = nullptr;
  const unsigned long long numeric = std::strtoull(value.c_str(), &end, 10);
  if (end != value.c_str() && *end == 0 && value.front() != '-') {
    return static_cast<uint32_t>(std::min<unsigned long long>(numeric, UINT32_MAX));
  }
  int day = 0, year = 0, hour = 0, minute = 0, second = 0;
  char month_name[4] = {};
  if (std::sscanf(value.c_str(), "%*3s, %d %3s %d %d:%d:%d GMT", &day, month_name, &year, &hour,
                  &minute, &second) != 6) {
    return 300;
  }
  const std::string_view months = "JanFebMarAprMayJunJulAugSepOctNovDec";
  const auto position = months.find(month_name);
  if (position == months.npos || position % 3 || year < 1970 || day < 1 || day > 31 ||
      hour < 0 || hour > 23 || minute < 0 || minute > 59 || second < 0 || second > 59) {
    return 300;
  }
  const unsigned month = static_cast<unsigned>(position / 3 + 1);
  year -= month <= 2;
  const int era = year / 400;
  const unsigned yoe = static_cast<unsigned>(year - era * 400);
  const unsigned doy = (153 * (month > 2 ? month - 3 : month + 9) + 2) / 5 + day - 1;
  const unsigned doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
  const int64_t epoch = (static_cast<int64_t>(era) * 146097 + doe - 719468) * 86400 +
                        hour * 3600 + minute * 60 + second;
  return static_cast<uint32_t>(std::clamp<int64_t>(epoch - now_utc, 0, UINT32_MAX));
}

// "Not before" gate for network attempts: an early task wake-up (notify)
// must never permit an early retry.
class RetryBudget {
 public:
  bool due(int64_t now_us) const { return now_us >= not_before_us_; }
  void defer(int64_t now_us, uint32_t seconds) {
    not_before_us_ = now_us + static_cast<int64_t>(seconds) * 1000000;
  }
  void reset() { not_before_us_ = 0; }
  int64_t not_before_us() const { return not_before_us_; }

 private:
  int64_t not_before_us_ = 0;
};

}  // namespace printsphere::cloud_http
