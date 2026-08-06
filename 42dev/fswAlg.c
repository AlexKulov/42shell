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
extern double R2D;

/**********************************************************************/
/**********************************************************************/
/*                 Support Analys Function                            */

/********* lat, lng reference by degree ***************/
static void ECIToWGS84(double PosN[3],
                double * lat, double * lng, double * alt){
    double GoalW[3] = {0};
    VxMT(PosN ,World[EARTH].CWN, GoalW);
    ECEFToWGS84(GoalW, lat, lng, alt);
    *lat = *lat * R2D;
    *lng = *lng * R2D;
}

void WSG84ToECI(double lat, double lng, double alt, double PosN[3]){

    double latRad = lat * D2R;
    double lngRad = lng * D2R;
    double GoalW[3];
    WGS84ToECEF(latRad, lngRad, alt, GoalW);
    //may be in feature be note how calc CWN
    VxM(GoalW,World[EARTH].CWN,PosN);
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

/**
 * @brief Находит расстояние до пересечения луча с эллипсоидом Земли (WGS84).
 *
 * @param PosN массив — точка старта луча (геоцентрические координаты, м).
 * @param DirN массив — единичный вектор направления луча.
 * @return long признак наличия пересечения
 */
long crossEarthSurface(double PosN[3], double DirN[3], double PosE[3]){
    // Параметры эллипсоида WGS84
    const double a = 6378137.0;
    const double f = 1.0 / 298.257223563;
    const double b = a * (1.0 - f);
    const double a2 = a * a;
    const double b2 = b * b;

    //elipsoid func x^2/a^2+y^2/a^2+z^2/b^2=1
    //line func p=p0+d⋅t
    //make A*t^2 + B*t + C = 0
    double A =  (DirN[0]*DirN[0] + DirN[1]*DirN[1]) / a2 + (DirN[2]*DirN[2]) / b2;
    double B = ((PosN[0]*DirN[0] + PosN[1]*DirN[1]) / a2 + (PosN[2]*DirN[2]) / b2) * 2.0;
    double C =  (PosN[0]*PosN[0] + PosN[1]*PosN[1]) / a2 + (PosN[2]*PosN[2]) / b2 - 1.0;

    double D = B*B - 4.0*A*C;

    double t1=0, t2=0;
    double tmin = 0.0;   // ближайший корень по модулю
    if (D > 0.0){
        double sqrtD = sqrt(D);
        t1 = (-B - sqrtD) / (2.0 * A);
        t2 = (-B + sqrtD) / (2.0 * A);

        // Обработка t1
        if (t1 > 0.0 && t2 > 0.0){
            tmin = t1 >= t2 ? t2 : t1;
        }
        else
            return 0;
    }
    else
        return 0;

    PosE[0] = PosN[0] + DirN[0] * tmin;
    PosE[1] = PosN[1] + DirN[1] * tmin;
    PosE[2] = PosN[2] + DirN[2] * tmin;

    return 1;
}

long lagLngPointing(struct SCType *S, double axisB[3],
                    double * lat, double * lng){
    double axisN[3] = {0};
    QTxV(S->B[0].qn,axisB, axisN); //- перевод из ССК в ИСК
    UNITV(axisN);
    double surfacePosN[3] = {0};
    if(crossEarthSurface(S->PosN, axisN, surfacePosN)){
        double alt[1] = {0};
        ECIToWGS84(surfacePosN, lat, lng, alt);
        return 1;
    }
    return 0;
}

/**********************************************************************/
/**********************************************************************/
/*                    Pointing Orientation                            */
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
/**********************************************************************/
/*                 Support Math Function                            */

void SUMMV(const double a[3], const double b[3], double c[3]){
    c[0]=a[0]+b[0];
    c[1]=a[1]+b[1];
    c[2]=a[2]+b[2];
}

/**
 * @brief Разложение вектора b на две ортогональные компоненты:
 *        ba – проекция b на направление a,
 *        bn – составляющая b, перпендикулярная a.
 *
 * @param a  указатель на массив из 3 элементов (вектор направления)
 * @param b  указатель на массив из 3 элементов (разлагаемый вектор)
 * @param ba выходной массив для соосной компоненты
 * @param bn выходной массив для перпендикулярной компоненты
 */
void vecDecompose(const double a[3], const double b[3],
                      double ba[3], double bn[3]){
    // Скалярная проекция b на a
    double proj =(b[0]*a[0] + b[1]*a[1] + b[2]*a[2])/
                 (a[0]*a[0] + a[1]*a[1] + a[2]*a[2]);

    // Соосная компонента: proj * a
    ba[0] = proj * a[0];
    ba[1] = proj * a[1];
    ba[2] = proj * a[2];

    // Перпендикулярная компонента: d - ba
    bn[0] = b[0] - ba[0];
    bn[1] = b[1] - ba[1];
    bn[2] = b[2] - ba[2];
}
