/* Created by Kualkov Aleksandr, russ69@bk.ru
 * This file contains the functions of simplest attitude mode.
 * This file is compatible with 42.
 * https://github.com/ericstoneking/42
 */
#include "42.h"

extern void GyroProcessing(struct AcType *AC);
extern void FssProcessing(struct AcType *AC);
extern void WheelProcessing(struct AcType *AC);

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
double sunAng = 0;
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
    double test1[3];
    double test2[3];
    VxV(AC->svb,C.fvb,C.ArgUpr);
    for(i=0;i<3;i++){
        test1[i] = C.ArgUpr[i];
        test2[i] = S->svb[i];
    }
    for(i=0;i<3;i++)
        AC->Tcmd[i] = -C.AngRateGain[i]*AC->wbn[i]
                      -C.AngGain[i]*C.ArgUpr[i];

    /* .. Actuator Processing */
    WheelProcessing(AC);

    sunAng = acos(VoV(S->svb,C.fvb))*R2D;
}

/**********************************************************************/
void EasyLvlhMode(struct SCType *S){
      double wln[3],CRN[3][3];
      double qrn[4],qbr[4],svr[3];
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
      FindCLN(AC->PosN,AC->VelN,CRN,wln);
      C2Q(CRN,qrn);
      MxV(CRN,AC->svn,svr);

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
}
