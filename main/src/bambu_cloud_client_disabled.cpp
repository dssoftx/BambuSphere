// No-cloud test build (PRINTSPHERE_CLOUD=off) replacement for
// bambu_cloud_client.cpp. Keeps BambuCloudClient's interface so the rest of
// the firmware builds unchanged, but never starts the cloud task: no Bambu
// Cloud login, no cloud MQTT, no REST/preview HTTPS traffic. Every snapshot
// reports "not configured", commands report failure, and ConfigStore forces
// Local Only mode in this build (see load_source_mode()).

#ifdef PRINTSPHERE_NO_CLOUD

#include "printsphere/bambu_cloud_client.hpp"

#include "esp_log.h"

namespace printsphere {

namespace {
constexpr char kTag[] = "printsphere.cloud";
constexpr char kDisabledDetail[] = "Cloud disabled in this build";

BambuCloudSnapshot disabled_snapshot() {
  BambuCloudSnapshot snapshot;
  snapshot.detail = kDisabledDetail;
  return snapshot;
}
}  // namespace

const char* to_string(CloudSetupStage stage) {
  switch (stage) {
    case CloudSetupStage::kLoggingIn:
      return "logging_in";
    case CloudSetupStage::kEmailCodeRequired:
      return "email_code_required";
    case CloudSetupStage::kTfaRequired:
      return "tfa_required";
    case CloudSetupStage::kCodeSubmitted:
      return "code_submitted";
    case CloudSetupStage::kBindingPrinter:
      return "binding_printer";
    case CloudSetupStage::kConnectingMqtt:
      return "connecting_mqtt";
    case CloudSetupStage::kConnected:
      return "connected";
    case CloudSetupStage::kFailed:
      return "failed";
    case CloudSetupStage::kIdle:
    default:
      return "idle";
  }
}

void BambuCloudClient::configure(BambuCloudCredentials, std::vector<std::string>, std::string) {}

void BambuCloudClient::set_active_serial(std::string) {}

void BambuCloudClient::set_fetch_paused(bool) {}

void BambuCloudClient::set_preview_fetch_enabled(bool) {}

void BambuCloudClient::submit_verification_code(std::string) {}

bool BambuCloudClient::set_chamber_light(bool) { return false; }

bool BambuCloudClient::set_print_command(PrintCommand) { return false; }

esp_err_t BambuCloudClient::start() {
  ESP_LOGI(kTag, "Cloud features disabled in this build (PRINTSPHERE_CLOUD=off)");
  return ESP_OK;
}

BambuCloudSnapshot BambuCloudClient::snapshot() const { return disabled_snapshot(); }

BambuCloudSnapshot BambuCloudClient::refreshed_snapshot() { return disabled_snapshot(); }

std::vector<CloudDeviceInfo> BambuCloudClient::get_cloud_devices() const { return {}; }

MqttTelemetry BambuCloudClient::mqtt_telemetry() const { return {}; }

}  // namespace printsphere

#endif  // PRINTSPHERE_NO_CLOUD
