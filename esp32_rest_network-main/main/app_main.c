#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

// Required for NVS
#include "nvs_flash.h"

// Required for networking setup
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_log.h"

#include "rest_api.h"
#include "sensor_driver.h"
#include "actuator_driver.h"
#include "wifi_manager.h"

void initialize_network_stack(void) {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
}


void app_main(void) {
    ESP_LOGI("MAIN", "Starting RESTful ESP32 Sensor and Control Network...");
    initialize_network_stack();
    wifi_manager_init();

    sensor_driver_init();
    actuator_driver_init();
    rest_api_start();

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
