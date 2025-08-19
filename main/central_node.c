#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_now.h"
#include "esp_system.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "cJSON.h"

static const char *TAG = "CENTRAL_NODE";

// ------------ ESP-NOW Receive Callback ------------
static void on_data_recv(const esp_now_recv_info_t *recv_info, const uint8_t *data, int len) {
    printf("Raw Data: %.*s\n", len, data);

    cJSON *root = cJSON_Parse((char *)data);
    if (root == NULL) {
        printf("Failed to parse JSON\n");
        return;
    }

    int id = cJSON_GetObjectItem(root, "id")->valueint;
    float temperature = (float)cJSON_GetObjectItem(root, "temperature")->valuedouble;
    float humidity = (float)cJSON_GetObjectItem(root, "humidity")->valuedouble;
    int timestamp = cJSON_GetObjectItem(root, "timestamp")->valueint;

    printf("Node ID: %d | Temp: %.2f °C | Humidity: %.2f %% | Timestamp: %d\n",
           id, temperature, humidity, timestamp);

    cJSON_Delete(root);
}

// ------------ ESP-NOW Init ------------
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

    printf("ESP-NOW Receiver Initialized\n");
}

// ------------ Main ------------
void app_main(void) {
    ESP_ERROR_CHECK(nvs_flash_init());
    espnow_init();
}
