/* Created by Kulakov Aleksandr, russ69@bk.ru
 * This file contains the functions of simplest attitude mode.
 * This file is compatible with 42.
 * https://github.com/ericstoneking/42
 */
#include "42.h"
#include "ballistic.h"

extern void GyroProcessing(struct AcType *AC);
extern void FssProcessing(struct AcType *AC);
extern void WheelProcessing(struct AcType *AC);
extern void GpsProcessing(struct AcType *AC);

struct SunModeCtrlType {
   /*~ Parameters ~*/
   double AngRateGain[3];
   double AngGain[3];

   /*~ Internal Variables ~*/
   long Init;
   double fvb[3];
   double ArgUpr[3];
};

static struct SunModeCtrlType C = {.Init=1};
/**********************************************************************/
double modeAng = 0; //rad
/* modeAng for EasySunMode is angle between
 * Sun direction & fvb;
 * for PointOrientation is Yb & earth point
 * direct; for EasyLvlhMode is just 0
 */
void EasySunMode(struct SCType *S){
    /* .. Initialize */
    struct AcType *AC;
    long i;
    AC = &S->AC;
    if (C.Init) {
       C.Init = 0;
       C.fvb[0]=0; C.fvb[1]=0; C.fvb[2]=1;
       for(i=0;i<3;i++)
          FindPDGains(AC->MOI[i][i],0.1,0.7,
                      &C.AngRateGain[i],&C.AngGain[i]);
    }

    /* .. Sensor Processing */
    GyroProcessing(AC);
    AC->SunValid = 0; // почему-то он не обнуляется в FssProcessing
    FssProcessing(AC);

    /* .. Control Low Processing */
    VxV(AC->svb,C.fvb,C.ArgUpr);
    for(i=0;i<3;i++)
        AC->Tcmd[i] = -C.AngRateGain[i]*AC->wbn[i]
                      -C.AngGain[i]*C.ArgUpr[i];

    /* .. Actuator Processing */
    WheelProcessing(AC);

    modeAng = acos(VoV(S->svb,C.fvb))*R2D; //rad
}

/**********************************************************************/
void EasyLvlhMode(struct SCType *S){
      double wln[3],CRN[3][3];
      double qrn[4],qbr[4];
      double Herr[3],HxB[3];
      long i;
      struct AcType *AC;
      struct AcThreeAxisCtrlType *C;

      AC = &S->AC;
      C = &AC->ThreeAxisCtrl;

      if (C->Init) {
         C->Init = 0;
         for(i=0;i<3;i++) {
            FindPDGains(AC->MOI[i][i],0.1,0.7,
                        &C->Kr[i],&C->Kp[i]);
            C->Hwcmd[i] = 0.0;
         }
         C->Kunl = 1.0E6;
         if(AC->Nmtb > 0 && AC->Nmtb != 3){
             printf("EasyLvlhMode: please, check MTBs, Nmtb=%li\n", AC->Nmtb);
         }
      }

      /* Find Attitude Command */
      GpsProcessing(AC);
      FindCLN(AC->PosN,AC->VelN,CRN,wln);
      C2Q(CRN,qrn);
      //MxV(CRN,AC->svn,svr);

      /* Form Error Signals */
      QxQT(AC->qbn,qrn,qbr);
      RECTIFYQ(qbr);

      /* PD Control */
      for(i=0;i<3;i++) {
         C->Tcmd[i] = -C->Kr[i]*AC->wbn[i]-C->Kp[i]*(2.0*qbr[i]);
         AC->Whl[i].Tcmd = -C->Tcmd[i];
      }

      /* Momentum Management */
      if(AC->Nmtb == 3){
          for(i=0;i<3;i++)
             Herr[i] = AC->Whl[i].H - C->Hwcmd[i];
          VxV(Herr,AC->bvb,HxB);
          for(i=0;i<3;i++)
              AC->MTB[i].Mcmd = C->Kunl*HxB[i];
      }
      modeAng = 0;
}

/**********************************************************************/
/**********************************************************************/
/* 20.10.23:                Pointing Orientation                      */
void SUMMV(const double a[3], const double b[3], double c[3]){
    c[0]=a[0]+b[0];
    c[1]=a[1]+b[1];
    c[2]=a[2]+b[2];
}

void NPUoneAxis (double ScRi[3], const double ScVi[3], const double PointRi[3],
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


extern void MagnetometerProcessing(struct AcType *AC);
extern void WheelProcessing(struct AcType *AC);
extern void ThreeAxisAttitudeCommand(struct SCType *S);

void PointOrientation(struct SCType *S)
{
    struct AcType *AC;
    struct AcPrototypeCtrlType *C;
    struct CmdType *Cmd;
    long i;
    static double kw[3] = {0.5,0.5,0.5};
    static double ka[3] = {0.01,0.01,0.01};

    AC = &S->AC;
    C = &AC->PrototypeCtrl;
    Cmd = &AC->Cmd;

    if (C->Init){
        C->Init = 0;
        static double w0=1.0;
        static double kkw=2;
        for(i=0;i<3;i++){
            kw[i] = kkw*AC->MOI[i][i]*w0;
            ka[i] =     AC->MOI[i][i]*w0*w0;
        }
        kw[2] = 2*kw[2];
        if(S->Ngps == 0){
            printf("Have't GPS, PointOrientation unpossible!\n");
            exit(1);
        }
    }
    /* .. Sensor Processing */
    GyroProcessing(AC);
    MagnetometerProcessing(AC);
    /* Find qrn, wrn and joint angle commands */
    ThreeAxisAttitudeCommand(S);

    /* Form attitude error signals */
    double wref[3] ={0,0,0};
    double dwref[3]={0,0,0};
    double nref[3] ={0,0,0};

    double eartPointN[3] = {0};
    static double lat = -2.5, lng = 140.71, alt = 1; //Джаяпура
    GpsProcessing(AC);
    static long isFeakLvlh = FALSE;
    if(isFeakLvlh){
        lat = R2D*S->GPS[0].WgsLat;
        lng = R2D*S->GPS[0].WgsLng;
    }

    WSG84ToECI(lat, lng, alt, eartPointN);
    NPUoneAxis(S->PosN, S->VelN, eartPointN, nref, wref, dwref);
    QxV(S->B[0].qn,wref,Cmd->wrn);//D*wref
    for(i=0;i<3;i++)
        C->werr[i] = AC->wbn[i] - Cmd->wrn[i];

    double tempV[3];
    double wJw[3];
    MxV(AC->MOI,AC->wbn,tempV);
    VxV(AC->wbn,tempV,wJw);
    double Jwexwr[3];
    VxV(C->werr,Cmd->wrn,tempV);
    MxV(AC->MOI,tempV,Jwexwr);

    double Jdwr[3];
    QxV(S->B[0].qn,dwref,tempV);//D*dwref
    MxV(AC->MOI,tempV,Jdwr);
    double nrxYb[3];
    double Yb[3] = {0,1,0};
    QxV(S->B[0].qn,nref,tempV); //D*nref
    VxV(tempV,Yb,nrxYb);

    for(i=0;i<3;i++)
        AC->Tcmd[i] = wJw[i]-Jwexwr[i]+Jdwr[i]-kw[i]*C->werr[i]-ka[i]*nrxYb[i];

    WheelProcessing(AC);

    modeAng = angEarthPointNegAxis(S,eartPointN,Yb); //rad

    #if 0
    double Herr[3]={0};
    double HxB[3]={0,0,0};
    double Kunl=1.0E6;
    /* Momentum Management */

    for(i=0;i<3;i++) {
        for(int j=0;j<AC->Nwhl;j++)
            Herr[i] += AC->Whl[j].H*AC->Whl[j].Axis[i];
    }

    VxV(Herr,AC->bvb,HxB);
    for(i=0;i<3;i++){
        AC->MTB[i].Mcmd = Kunl*HxB[i];
    }
    #endif
}
