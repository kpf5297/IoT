#include "esp_log.h"
#include "esp_http_server.h"
#include "rest_api.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "nvs_flash.h"



static const char *TAG = "REST_API";

esp_err_t hello_get_handler(httpd_req_t *req) {
    const char* resp = "ESP32 RESTful API is live";
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

httpd_uri_t hello_uri = {
    .uri = "/api/hello",
    .method = HTTP_GET,
    .handler = hello_get_handler,
};

void rest_api_start(void) {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;
    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_register_uri_handler(server, &hello_uri);
        ESP_LOGI(TAG, "Server started on /api/hello");
    } else {
        ESP_LOGE(TAG, "Failed to start server");
    }
}
