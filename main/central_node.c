#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_now.h"
#include "esp_event.h"

static const char *TAG = "CENTRAL_NODE";

// ------------------- ESP-NOW Callback -------------------
static void on_data_recv(const esp_now_recv_info_t *recv_info, const uint8_t *data, int len) {
    if (len == sizeof(float)) {
        float received_temp;
        memcpy(&received_temp, data, sizeof(float));

        // Print to serial so Python script can read it
        printf("TEMP: %.2f\n", received_temp);
        fflush(stdout); // flush to make sure it appears immediately

        ESP_LOGI(TAG, "Received Temperature: %.2f °C", received_temp);
    } else {
        ESP_LOGW(TAG, "Unexpected data size: %d", len);
    }
}

// ------------------- ESP-NOW Init -------------------
static void espnow_init(void) {
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE));

    ESP_ERROR_CHECK(esp_now_init());
    ESP_ERROR_CHECK(esp_now_register_recv_cb(on_data_recv));

    ESP_LOGI(TAG, "ESP-NOW Initialized");
}

// ------------------- Main -------------------
void app_main(void) {
    ESP_ERROR_CHECK(nvs_flash_init());
    espnow_init();
}
