#include <freertos/FreeRTOS.h>
#include <freertos/timers.h>
#include <esp_log.h>
#include <esp_timer.h>

#include "calculus.h"

extern "C" void app_main();

static const char TAG[] = "ISS";

uint64_t start_time;

void app_main() 
{

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