
#include "calculus.h"
#include "math.h"

#include <esp_log.h>

constexpr double EARTH_RADIUS = 6400000.0;
constexpr double wgs84a =  6378.137;
constexpr double wgs84f =  1.0/298.257223563;
constexpr double wgs84b =  wgs84a * ( 1.0 - wgs84f );
constexpr double f = 1 - (wgs84a/wgs84b);

static const char TAG[] ="CALC";

inline void debugECEF(const char * name, ECEF e)
{
    ESP_LOGE(TAG, "%s : %f, %f, %f \n", name, e.x, e.y, e.z);
}

double N(double sigma)
{
    // Todo: Mirar como se hace bien
    return EARTH_RADIUS;
}

void LLAtoECEF( LATLONALT lla, ECEF &ecef)
{
    float coslat = cosf(lla.lat*M_PI/180.0);
    double X = (N(lla.lat) + lla.alt )*coslat * cosf(lla.lon*M_PI/180.0);
    double Y = (N(lla.lat) + lla.alt )*coslat * sinf(lla.lon*M_PI/180.0);
    double Z = (N(lla.lat) + lla.alt )*sinf(lla.lat*M_PI/180.0);

    ecef.x = X;
    ecef.y = Y;
    ecef.z = Z;
}

void calculateVector(LATLONALT llaDevice, LATLONALT llaTarget, result &output)
{

    ECEF ecefDevice;
    ECEF ecefTarget;

    LLAtoECEF(llaDevice, ecefDevice);
    LLAtoECEF(llaTarget, ecefTarget);


    /*ESP_LOGE(TAG, "llaDevice: %f, %f, %f \n", llaDevice.lat, llaDevice.lon, llaDevice.alt);
    debugECEF("ecefDevice", ecefDevice);
    ESP_LOGE(TAG, "llaTarget: %f, %f, %f \n", llaTarget.lat, llaTarget.lon, llaTarget.alt);
    debugECEF("ecefTarget", ecefTarget);*/

    ECEF d;

    d.x = ecefTarget.x - ecefDevice.x;
    d.y = ecefTarget.y - ecefDevice.y;
    d.z = ecefTarget.z - ecefDevice.z;

    ECEF device2;
    device2.x = ecefDevice.x * ecefDevice.x;
    device2.y = ecefDevice.y * ecefDevice.y;
    device2.z = ecefDevice.z * ecefDevice.z;


    //debugECEF("d", d);
    float x2y2 = device2.x + device2.y;
    float dxyz2 = d.x*d.x+d.y*d.y+d.z*d.z;


    float cosElev = (ecefDevice.x*d.x + ecefDevice.y*d.y + ecefDevice.z*d.z) /
         sqrtf(x2y2 + powf(ecefDevice.z,2)*(dxyz2));
    float elev = acosf(cosElev);
    float degHorizonElev = 90 - (elev*180/M_PI);

    float cosAzimuth = (-ecefDevice.z*ecefDevice.x*d.x - ecefDevice.z*ecefDevice.y*d.y + (x2y2)*d.z) 
            / sqrtf((x2y2)*(x2y2+ecefDevice.z*ecefDevice.z)*(dxyz2));
    float sinAzimuth = (-ecefDevice.y*d.x + ecefDevice.x*d.y)/ sqrtf((device2.y + device2.y)*(dxyz2));
    float azimuth = atan2f(sinAzimuth, cosAzimuth);

    if( azimuth < 0)
    {
        azimuth += 2*M_PI;
    }
    /*
    ESP_LOGE(TAG, "cos elevation: %f", cosElev);
    ESP_LOGE(TAG, "Elevation radians: %f", elev);
    ESP_LOGE(TAG, "cosAzimuth: %f", cosAzimuth);
    ESP_LOGE(TAG, "sinAzimuth: %f", sinAzimuth);
    ESP_LOGE(TAG, "azimuth radians: %f", azimuth);
    */
    output.azimuth = azimuth*180/M_PI;
    output.elevation = degHorizonElev;

}
