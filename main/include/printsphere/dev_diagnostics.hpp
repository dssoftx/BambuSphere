#pragma once

#include <cstdint>

#include "esp_err.h"

namespace printsphere {

// On-screen CPU/heap/temperature overlay for the dev-diagnostics build
// (PRINTSPHERE_DEV_DIAGNOSTICS=1 only). In every other build this is a
// no-op: initialize() returns ESP_OK without touching any peripheral, and
// sample() always returns an unavailable snapshot, so callers never need
// their own #ifdef.
struct DevDiagnosticsSnapshot {
  bool available = false;
  uint8_t cpu_usage_percent = 0;
  // Internal SRAM free bytes (MALLOC_CAP_INTERNAL) - the scarce, contended
  // pool shared with Wi-Fi/TLS/LVGL.
  uint32_t free_heap_bytes = 0;
  // External PSRAM free bytes (MALLOC_CAP_SPIRAM) - 8 MB on this board,
  // plentiful, but worth watching once many concurrent MQTT/TLS
  // connections are all drawing from it (PrinterClientPool, cloud client).
  uint32_t free_psram_bytes = 0;
  float mcu_temp_c = 0.0f;
};

class DevDiagnostics {
 public:
  esp_err_t initialize();
  DevDiagnosticsSnapshot sample();

 private:
  bool initialized_ = false;
};

}  // namespace printsphere
