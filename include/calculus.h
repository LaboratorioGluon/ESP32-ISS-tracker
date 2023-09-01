#ifndef __CALCULUS_H__
#define __CALCULUS_H__

typedef struct _LATLONALT{
    float lat;
    float lon;
    float alt;
} LATLONALT;

typedef struct _ECEF{
    float x;
    float y;
    float z;
} ECEF;

typedef struct _UNE{
    double up;
    double north;
    double east;
} UNE;

typedef struct _result{
    double azimuth;
    double elevation;
} result;

void LLAtoECEF( LATLONALT lla, ECEF &ecef);
void calculateVector(LATLONALT llaDevice, LATLONALT llaTarget, result &output);

#endif //__CALCULUS_H__