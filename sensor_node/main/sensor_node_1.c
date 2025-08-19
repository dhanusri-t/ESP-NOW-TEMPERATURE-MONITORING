#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"   // for esp_random()
#include "esp_random.h"
#include "esp_log.h"      // for esp_log_timestamp()
#include "esp_now.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "cJSON.h"


#define SENSOR_ID 1   // Change this for each node (1, 2, 3, ...)

typedef struct {
    float temperature;
    float humidity;
} sensor_data_t;

// ------------ ESP-NOW Send Callback ------------
static void on_data_sent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    printf("Send Status: %s\n", status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

// ------------ Fake Sensor Read (Replace with real DHT/SHT sensor) ------------
void read_sensor(sensor_data_t *data) {
    data->temperature = 25.0 + (esp_random() % 100) / 10.0; // random 25–35
    data->humidity = 50.0 + (esp_random() % 200) / 10.0;    // random 50–70
}

// ------------ Send Task ------------
void send_task(void *pvParameter) {
    while (1) {
        sensor_data_t data;
        read_sensor(&data);

        // Create JSON
        cJSON *root = cJSON_CreateObject();
        cJSON_AddNumberToObject(root, "id", SENSOR_ID);
        cJSON_AddNumberToObject(root, "temperature", data.temperature);
        cJSON_AddNumberToObject(root, "humidity", data.humidity);
        cJSON_AddNumberToObject(root, "timestamp", (int)esp_log_timestamp());
        char *json_str = cJSON_PrintUnformatted(root);

        // Broadcast to all peers (FF:FF:FF:FF:FF:FF)
        uint8_t broadcast_addr[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
        esp_err_t result = esp_now_send(broadcast_addr, (uint8_t *)json_str, strlen(json_str)+1);

        if (result == ESP_OK) {
            printf("Sent JSON: %s\n", json_str);
        } else {
            printf("Error sending the data\n");
        }

        cJSON_Delete(root);
        free(json_str);

        vTaskDelay(5000 / portTICK_PERIOD_MS); // every 5s
    }
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
    ESP_ERROR_CHECK(esp_now_register_send_cb(on_data_sent));

    // Add broadcast peer
    esp_now_peer_info_t peerInfo = {0};
    memcpy(peerInfo.peer_addr, (uint8_t[]){0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, 6);
    peerInfo.channel = 1;
    peerInfo.encrypt = false;
    if (!esp_now_is_peer_exist(peerInfo.peer_addr)) {
        ESP_ERROR_CHECK(esp_now_add_peer(&peerInfo));
    }
}

// ------------ Main ------------
void app_main(void) {
    ESP_ERROR_CHECK(nvs_flash_init());
    espnow_init();
    xTaskCreate(send_task, "send_task", 4096, NULL, 1, NULL);
}
