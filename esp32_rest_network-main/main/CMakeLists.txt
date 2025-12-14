idf_component_register(SRCS "app_main.c"
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES nvs_flash esp_netif esp_event rest_api
                                  sensor_driver actuator_driver wifi_manager)

