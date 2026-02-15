#ifndef FSW_ALG_H
#define FSW_ALG_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SCType SCType;
double angEarthPoint2BodyAxis(struct SCType *S,
                              double goalLat, double goalLng, double goalAlt, double bodyAxis[3]);
void WSG84ToECI(double lat, double lng, double alt, double PosN[3]);
double angEarthPointNegAxis(struct SCType *S, double eartPointN[3], double bodyAxis[3]);

void SUMMV(const double a[3], const double b[3], double c[3]);
void FindNWref (double ScRi[3], const double ScVi[3], const double PointRi[3],
               double nref[3], double wref[3], double dwref[3]);

void ThrProcessing(struct AcType *AC, long FirstThr, long LastThr);
void ExcThrProcessing(struct AcType *AC, long FirstThr, long LastThr);

#ifdef __cplusplus
}
#endif

#endif // FSW_ALG_H
