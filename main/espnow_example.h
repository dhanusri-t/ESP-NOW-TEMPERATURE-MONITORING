#ifndef ESPNOW_EXAMPLE_H
#define ESPNOW_EXAMPLE_H

#include <string.h>
#include "esp_now.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_log.h"

#define ESPNOW_WIFI_MODE WIFI_MODE_STA
#define ESPNOW_WIFI_IF   ESP_IF_WIFI_STA
#define ESPNOW_CHANNEL   1

#define ESPNOW_MAX_DATA_LEN 250

typedef struct {
    uint8_t mac_addr[ESP_NOW_ETH_ALEN];
    uint8_t data[ESPNOW_MAX_DATA_LEN];
    int data_len;
} espnow_event_t;

#endif
