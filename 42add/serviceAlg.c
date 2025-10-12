#include "42.h"
#include "serviceAlg.h"

extern double D2R;
/********* lat, lng reference by degree ***************/
void WSG84ToECI(double lat, double lng, double alt, double PosN[3]){

    double latRad = lat * D2R;
    double lngRad = lng *D2R;
    double GoalW[3];
    WGS84ToECEF(latRad, lngRad, alt, GoalW);
    //may be in feature be note how calc CWN
    VxM(GoalW,World[EARTH].CWN,PosN);
}
/* return radian */
double angEarthPointNegAxis(struct SCType *S, double eartPointN[3], double bodyAxis[3]){
    double  GoalYN[3];
    long i;
    for(i=0;i<3;i++){
        GoalYN[i] = eartPointN[i] - S->PosN[i];
    }
    double GoalYB[3];
    QxV(S->B[0].qn,GoalYN,GoalYB); //- перевод из ИСК в ССК
    UNITV(GoalYB);
    for(i=0;i<3;i++)
        bodyAxis[i] = -bodyAxis[i];
    return acos(VoV(GoalYB,bodyAxis));
}

double angEarthPoint2BodyAxis(struct SCType *S,
                      double goalLat, double goalLng, double goalAlt, double bodyAxis[3]){
    double angGoalDirectInBody = 0;
    if (Orb[S->RefOrb].World == EARTH) {
        double eartPointN[3]={0};
        WSG84ToECI(goalLat, goalLng, goalAlt, eartPointN);
        angGoalDirectInBody = angEarthPointNegAxis(S, eartPointN, bodyAxis);
    }
    return angGoalDirectInBody;
}
