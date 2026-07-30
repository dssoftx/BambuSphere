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
  uint32_t free_heap_bytes = 0;
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
