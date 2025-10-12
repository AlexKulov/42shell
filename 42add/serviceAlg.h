#ifndef SERVICE_ALG_H
#define SERVICE_ALG_H

typedef struct SCType SCType;
double angEarthPoint2BodyAxis(struct SCType *S,
                      double goalLat, double goalLng, double goalAlt, double bodyAxis[3]);
void WSG84ToECI(double lat, double lng, double alt, double PosN[3]);
double angEarthPointNegAxis(struct SCType *S, double eartPointN[3], double bodyAxis[3]);

#endif // SERVICE_ALG_H
