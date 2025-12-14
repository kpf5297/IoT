idf_component_register(SRCS "rest_api.c"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES esp_http_server esp_netif esp_event nvs_flash)


