#include "printsphere/dev_diagnostics.hpp"

#ifdef PRINTSPHERE_DEV_DIAGNOSTICS

#include <cstring>
#include <vector>

#include "driver/temperature_sensor.h"
#include "esp_check.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

namespace printsphere {

namespace {

constexpr char kTag[] = "printsphere.dev_diag";
// uxTaskGetSystemState() needs a snapshot buffer sized to the current task
// count; a small headroom margin avoids a second call when a task or two
// gets added between the count and the snapshot.
constexpr UBaseType_t kTaskArrayHeadroom = 4;
constexpr uint32_t kSampleIntervalMs = 1000;

temperature_sensor_handle_t s_tsens = nullptr;

uint8_t compute_cpu_usage_percent() {
  const UBaseType_t task_count = uxTaskGetNumberOfTasks() + kTaskArrayHeadroom;
  std::vector<TaskStatus_t> status_array(task_count);

  uint32_t total_run_time = 0;
  const UBaseType_t filled =
      uxTaskGetSystemState(status_array.data(), task_count, &total_run_time);

  uint64_t idle_run_time = 0;
  for (UBaseType_t i = 0; i < filled; ++i) {
    // ESP-IDF's SMP FreeRTOS port names per-core idle tasks "IDLE0"/"IDLE1"
    // (single-core falls back to plain "IDLE") - both match this prefix.
    if (std::strncmp(status_array[i].pcTaskName, "IDLE", 4) == 0) {
      idle_run_time += status_array[i].ulRunTimeCounter;
    }
  }

  if (total_run_time == 0) {
    return 0;
  }

  const uint64_t idle_percent = (idle_run_time * 100ULL) / total_run_time;
  return static_cast<uint8_t>(idle_percent >= 100 ? 0 : 100 - idle_percent);
}

}  // namespace

esp_err_t DevDiagnostics::initialize() {
  if (initialized_) {
    return ESP_OK;
  }

  const temperature_sensor_config_t tsens_config =
      TEMPERATURE_SENSOR_CONFIG_DEFAULT(20, 100);
  ESP_RETURN_ON_ERROR(temperature_sensor_install(&tsens_config, &s_tsens), kTag,
                      "temperature_sensor_install failed");
  ESP_RETURN_ON_ERROR(temperature_sensor_enable(s_tsens), kTag,
                      "temperature_sensor_enable failed");

  initialized_ = true;
  ESP_LOGI(kTag, "Dev diagnostics ready (CPU/heap/temp overlay)");
  return ESP_OK;
}

DevDiagnosticsSnapshot DevDiagnostics::sample() {
  static DevDiagnosticsSnapshot s_cached;
  static uint32_t s_last_sample_ms = 0;

  if (!initialized_) {
    return {};
  }

  const uint32_t now_ms = static_cast<uint32_t>(esp_timer_get_time() / 1000);
  if (s_cached.available && (now_ms - s_last_sample_ms) < kSampleIntervalMs) {
    return s_cached;
  }

  DevDiagnosticsSnapshot snapshot;
  snapshot.available = true;
  snapshot.cpu_usage_percent = compute_cpu_usage_percent();
  snapshot.free_heap_bytes =
      static_cast<uint32_t>(heap_caps_get_free_size(MALLOC_CAP_INTERNAL));

  float celsius = 0.0f;
  if (temperature_sensor_get_celsius(s_tsens, &celsius) == ESP_OK) {
    snapshot.mcu_temp_c = celsius;
  }

  s_cached = snapshot;
  s_last_sample_ms = now_ms;
  return snapshot;
}

}  // namespace printsphere

#else  // !PRINTSPHERE_DEV_DIAGNOSTICS

namespace printsphere {

esp_err_t DevDiagnostics::initialize() { return ESP_OK; }

DevDiagnosticsSnapshot DevDiagnostics::sample() { return {}; }

}  // namespace printsphere

#endif  // PRINTSPHERE_DEV_DIAGNOSTICS
