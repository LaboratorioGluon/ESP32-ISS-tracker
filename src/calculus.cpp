
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
    double X = (N(lla.lat) + lla.alt )*cos(lla.lat*M_PI/180.0) * cos(lla.lon*M_PI/180.0);
    double Y = (N(lla.lat) + lla.alt )*cos(lla.lat*M_PI/180.0) * sin(lla.lon*M_PI/180.0);
    double Z = (N(lla.lat) + lla.alt )*sin(lla.lat*M_PI/180.0);

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

    //debugECEF("d", d);

    double cosElev = (ecefDevice.x*d.x + ecefDevice.y*d.y + ecefDevice.z*d.z) /
         sqrt(pow(ecefDevice.x,2)+pow(ecefDevice.y,2)+ pow(ecefDevice.z,2)*(d.x*d.x+d.y*d.y+d.z*d.z));
    double elev = acos(cosElev);
    double degHorizonElev = 90 - (elev*180/M_PI);

    double cosAzimuth = (-ecefDevice.z*ecefDevice.x*d.x - ecefDevice.z*ecefDevice.y*d.y + (ecefDevice.x*ecefDevice.x+ecefDevice.y*ecefDevice.y)*d.z) 
            / sqrt((ecefDevice.x*ecefDevice.x+ecefDevice.y*ecefDevice.y)*(ecefDevice.x*ecefDevice.x+ecefDevice.y*ecefDevice.y+ecefDevice.z*ecefDevice.z)*(d.x*d.x+d.y*d.y+d.z*d.z));
    double sinAzimuth = (-ecefDevice.y*d.x + ecefDevice.x*d.y)/ sqrt((ecefDevice.x*ecefDevice.x + ecefDevice.y*ecefDevice.y)*(d.x*d.x+ d.y*d.y + d.z*d.z));
    double azimuth = atan2(sinAzimuth, cosAzimuth);

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