#include <freertos/FreeRTOS.h>
#include <freertos/timers.h>
#include <esp_http_client.h>
#include <nvs_flash.h>

#include <esp_log.h>
#include <esp_timer.h>

#include "calculus.h"


extern "C" void app_main();
extern "C" void wifi_init_sta();


static const char TAG[] = "ISS";

uint64_t start_time;
LATLONALT issPosition;

void getISSPosition()
{
    char buffer[500] = {0};
    esp_http_client_config_t config = {
        .url = "https://api.wheretheiss.at/v1/satellites/25544", // Cambialo por la URL donde estará tu binario compilado
        .method= HTTP_METHOD_GET,
        .disable_auto_redirect = true,
        .user_data = buffer  
    };
/*
    esp_http_client_handle_t client;
    client = esp_http_client_init(&config);
    
    esp_err_t err = esp_http_client_set_header(client, "Accept", "application/json");
    ESP_ERROR_CHECK(err);
    err = esp_http_client_perform(client);
    ESP_ERROR_CHECK(err);
    //esp_http_client_perform(client);
    uint64_t len = esp_http_client_get_content_length(client);
    err = esp_http_client_read_response(client, buffer, len);
    ESP_ERROR_CHECK(err);
    
    */

    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_http_client_open(client,0);
    esp_http_client_fetch_headers(client);
    esp_http_client_read(client, buffer, esp_http_client_get_content_length(client));
    ESP_LOGE(TAG,"Petition Status: %d", esp_http_client_get_status_code(client));
    ESP_LOGE(TAG,"Received html: %s", buffer);

    esp_http_client_close(client);
    esp_http_client_cleanup(client);
    /*ESP_LOGE(TAG,"Longitud: %llu",len);

    for( uint32_t i = 0; i < len; i++)
    {
        ESP_LOGE(TAG,"%c", buffer[i]);
    }*/

}

void app_main() 
{

    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    wifi_init_sta();
    getISSPosition();


    /*LATLONALT lla;
    ECEF ecef;
    
    
    lla.lat =  40.416775;
    lla.lon =  -3.703790;
    lla.alt = 650;
    
    LLAtoECEF(lla, ecef);

    ESP_LOGE(TAG, "LLA: %f, %f, %f \n", lla.lat, lla.lon, lla.alt);
    ESP_LOGE(TAG, "ECEF: %f, %f, %f \n", ecef.x, ecef.y, ecef.z);
    */

    // Test
    LATLONALT llaPilot ={
        .lat = 39,
        .lon = -75,
        .alt = 4000
    };

    LATLONALT llaJet = {
        .lat = 39,
        .lon = -76,
        .alt = 12000
    };

    result vector;
    start_time = esp_timer_get_time();
    calculateVector(llaPilot, llaJet, vector);
    ESP_LOGE(TAG, "Time: %llu", esp_timer_get_time() - start_time);
    ESP_LOGE(TAG, "vector: %f, %f", vector.azimuth, vector.elevation);

    while(1){
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

}