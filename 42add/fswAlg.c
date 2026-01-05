/* Created by Kulakov Aleksandr, russ69@bk.ru
 * This file contains the support functions of flight software.
 * This file is compatible with 42.
 * https://github.com/ericstoneking/42
 * But have specific git repo
 * https://github.com/AlexKulov/42shell
 */

#include "42.h"
#include "fswAlg.h"

extern double D2R;

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

/**********************************************************************/
/**********************************************************************/
/*                    Pointing Orientation                            */
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

void SUMMV(const double a[3], const double b[3], double c[3]){
    c[0]=a[0]+b[0];
    c[1]=a[1]+b[1];
    c[2]=a[2]+b[2];
}

void FindNWref (double ScRi[3], const double ScVi[3], const double PointRi[3],
                      double nref[3], double wref[3], double dwref[3])
{
    static double Wz[3] = {0,0,7.292115E-5}; //скорость вращения Земли
#define mu_g (398600.4415888889) // постоянная
    double R3 = MAGV(ScRi);
    R3 = R3*R3*R3;

    double _PointR[3]={-PointRi[0], -PointRi[1], -PointRi[2]};
    //MTxV(Cie,PointRz,PointRi);
    double dR[3] = {0,0,0};
    SUMMV(ScRi, _PointR, dR); //Ri - Cie.tr() * Rze;   //  NPU to c.m.

    double _PointVi[3] = {0,0,0};
    VxV(Wz,_PointR,_PointVi); //на самом деле -PointVi, т.к. -PointRi
    double dV[3] = {0,0,0}; //= Vi - Wz.cross(Cie.tr()*Rze);
    SUMMV(ScVi, _PointVi, dV);

    double ScA[3] = {0,0,0};
    SxV(-mu_g/R3, ScRi,ScA);

    double _PointA[3] = {0,0,0};
    VxV(Wz,_PointVi,_PointA);
    double da[3] = {0,0,0}; //Ri * (-mu_g/R3) - Wz.cross(Wz.cross(Cie.tr()*Rze))
    SUMMV(ScA, _PointA, da);

    double nr = MAGV(dR);//= dR.getNorm();

    double dnref[3] = {0,0,0}, d2nref[3] = {0,0,0};
    double tau[3] = {0,0,0}, dtau[3] = {0,0,0};
    SxV(1/nr, dV, tau);//tau = dV / nr;
    SxV(1/nr, dR, nref);//nref = dR / nr;
    double ndt = VoV(nref,tau);//ndt = nref.dot(tau);
    double da_nr[3] = {0,0,0};
    SxV(1/nr, da, da_nr);
    double _ndtXtau[3] = {0,0,0};
    SxV(-ndt, tau, _ndtXtau);
    SUMMV(da_nr,_ndtXtau,dtau);//dtau = da / nr - tau*ndt;

    double _ndtXnref[3] = {0,0,0};
    SxV(-ndt, nref, _ndtXnref);
    SUMMV(tau,_ndtXnref,dnref);//dnref = tau - nref*ndt;

    double dtau_dnrefXndt[3] = {0,0,0}, _nrefX[3] = {0,0,0};
    double _ndtXdnref[3] = {0,0,0};
    SxV(-ndt, dnref, _ndtXdnref);
    SUMMV(dtau,_ndtXdnref,dtau_dnrefXndt);
    double dot = VoV(dnref,tau);
    dot = dot + VoV(nref,dtau);
    SxV(-dot,nref,_nrefX);
    //d2nref = dtau - dnref*ndt - nref*(dnref.dot(tau) + nref.dot(dtau));
    SUMMV(dtau_dnrefXndt,_nrefX,d2nref);

    VxV(nref,dnref,wref); //wref = nref.cross(dnref);
    VxV(nref,d2nref,dwref); //dwref = nref.cross(d2nref);

#undef mu_g
}

/**********************************************************************/
/**********************************************************************/
/*                        Thruster Control                            */
static double minDtThrOn = 0.2; //секунды
void ThrProcessing(struct AcType *AC, long FirstThr, long LastThr){
    struct AcThrType *W;
    long Iw;

    for(Iw=FirstThr;Iw<LastThr;Iw++) {
        W = &AC->Thr[Iw];
        W->PulseWidthCmd = Limit(VoV(AC->Tcmd,W->rxA),-W->Fmax,W->Fmax);
        if(fabs(W->PulseWidthCmd)<minDtThrOn){
            W->PulseWidthCmd = 0;
        }
        //else
        //    W->PulseWidthCmd = Limit(VoV(AC->Tcmd,W->rxA),-W->Fmax,W->Fmax);
    }
}

void ExcThrProcessing(struct AcType *AC, long FirstThr, long LastThr){
    struct AcThrType *W;
    long Iw;

    for(Iw=FirstThr;Iw<LastThr;Iw++) {
        W = &AC->Thr[Iw];
        W->PulseWidthCmd = Limit(VoV(AC->Tcmd,W->rxA),-W->Fmax,W->Fmax);
        if(W->PulseWidthCmd<0){
            W->PulseWidthCmd = 0.0;
        }
        else{
            W->PulseWidthCmd = 10.0;
        }
    }
}
