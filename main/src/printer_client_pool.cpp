#include "printsphere/printer_client_pool.hpp"

#include "esp_log.h"

namespace printsphere {

namespace {
constexpr char kTag[] = "printsphere.printer_pool";
// LWIP_MAX_SOCKETS=16 (sdkconfig.defaults) is shared with cloud MQTT, the
// setup-portal HTTP server, OTA and improv-serial — warn well before a
// profile count that would exhaust it, without hard-capping.
constexpr size_t kSocketBudgetWarnThreshold = 8;

bool connections_equal(const PrinterConnection& a, const PrinterConnection& b) {
  return a.host == b.host && a.serial == b.serial && a.access_code == b.access_code &&
         a.mqtt_username == b.mqtt_username && a.mqtt_port == b.mqtt_port;
}
}  // namespace

PrinterClientPool::Slot& PrinterClientPool::slot_for(uint8_t index) {
  if (index >= kMaxPrinterProfiles) {
    ESP_LOGW(kTag, "printer index %u out of range, clamping to 0", index);
    index = 0;
  }
  return slots_[index];
}

PrinterClient& PrinterClientPool::client_for(uint8_t index) {
  Slot& slot = slot_for(index);
  if (slot.client == nullptr) {
    slot.client = std::make_unique<PrinterClient>();
  }
  return *slot.client;
}

PrinterClient& PrinterClientPool::ensure_started(const PrinterProfile& profile) {
  Slot& slot = slot_for(profile.index);
  if (slot.client == nullptr) {
    slot.client = std::make_unique<PrinterClient>();
  }

  const PrinterConnection desired = profile.to_connection();
  if (!slot.started || !connections_equal(slot.active_connection, desired)) {
    slot.client->configure(desired);
    slot.active_connection = desired;
  }
  if (!slot.started) {
    slot.client->start();
    slot.started = true;
  }
  return *slot.client;
}

void PrinterClientPool::clear(uint8_t index) {
  Slot& slot = slot_for(index);
  if (slot.client == nullptr) {
    return;
  }
  const PrinterConnection empty{};
  if (!connections_equal(slot.active_connection, empty)) {
    slot.client->configure(empty);
    slot.active_connection = empty;
  }
}

void PrinterClientPool::set_background_network_ready(bool ready, uint8_t active_index) {
  for (uint8_t i = 0; i < kMaxPrinterProfiles; ++i) {
    if (i == active_index) {
      continue;
    }
    Slot& slot = slots_[i];
    if (slot.client != nullptr && slot.started) {
      slot.client->set_network_ready(ready);
    }
  }
}

void PrinterClientPool::sync_with_profiles(const std::vector<PrinterProfile>& profiles) {
  std::array<bool, kMaxPrinterProfiles> present{};
  size_t local_count = 0;
  for (const auto& p : profiles) {
    if (p.index >= kMaxPrinterProfiles) {
      continue;
    }
    present[p.index] = true;
    if (p.has_local_config()) {
      ensure_started(p);
      ++local_count;
    } else {
      clear(p.index);
    }
  }
  for (uint8_t i = 0; i < kMaxPrinterProfiles; ++i) {
    if (!present[i]) {
      clear(i);
    }
  }

  if (local_count > kSocketBudgetWarnThreshold) {
    ESP_LOGW(kTag,
             "%u printers with local config connected simultaneously - approaching the "
             "LWIP socket budget shared with cloud MQTT/HTTP/OTA",
             static_cast<unsigned>(local_count));
  }
}

}  // namespace printsphere
