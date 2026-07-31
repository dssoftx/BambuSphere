#pragma once

#include <array>
#include <cstdint>
#include <memory>
#include <vector>

#include "printsphere/config_store.hpp"
#include "printsphere/printer_client.hpp"

namespace printsphere {

// Owns one PrinterClient per printer profile slot so every configured
// printer's local MQTT connection stays alive in the background, regardless
// of which one is currently displayed. Switching the active printer then
// only means reading a different slot's snapshot instead of tearing down and
// rebuilding a shared MQTT/TLS session.
class PrinterClientPool {
 public:
  PrinterClientPool() = default;
  PrinterClientPool(const PrinterClientPool&) = delete;
  PrinterClientPool& operator=(const PrinterClientPool&) = delete;

  // Returns the client for `index`, lazily creating (but not starting) it on
  // first access. Safe to call for any profile index, including ones with no
  // local config — such a client simply stays unconfigured/idle.
  PrinterClient& client_for(uint8_t index);

  // Ensures a background connection is running for `profile`: creates the
  // slot if needed, (re)configures it only if the connection details
  // actually changed (configuring an already-current connection would tear
  // down a good MQTT session for no reason), and starts its task on first
  // use. Returns the slot so callers (e.g. the setup-portal live-test flow)
  // can poll its snapshot immediately after.
  PrinterClient& ensure_started(const PrinterProfile& profile);

  // Reconfigures `index`'s slot to an empty connection (idle, not
  // reconnecting) — used when a profile's local config is cleared or the
  // profile is deleted. No-op if the slot was never started.
  void clear(uint8_t index);

  // Ensures every profile with local config has a running background
  // connection and every other slot is idle. Call at boot and after any
  // profile add/edit/delete.
  void sync_with_profiles(const std::vector<PrinterProfile>& profiles);

  // Applies `ready` to every already-started slot except `active_index` (the
  // caller drives that one separately, with its own hybrid/handoff gating).
  // Lets the main loop keep every background printer's local MQTT alive
  // without re-scanning printer profiles every iteration.
  void set_background_network_ready(bool ready, uint8_t active_index);

 private:
  struct Slot {
    std::unique_ptr<PrinterClient> client;
    PrinterConnection active_connection;
    bool started = false;
  };

  Slot& slot_for(uint8_t index);

  std::array<Slot, kMaxPrinterProfiles> slots_{};
};

}  // namespace printsphere
