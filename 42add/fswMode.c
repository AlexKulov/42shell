/* Created by Kulakov Aleksandr, russ69@bk.ru
 * This file contains only the attitude mode.
 * This file is compatible with 42.
 * https://github.com/ericstoneking/42
 * But have specific git repo
 * https://github.com/AlexKulov/42shell
 */
#include "42.h"
#include "fswAlg.h"
#include "fswMode.h"

extern void GyroProcessing(struct AcType *AC);
extern void StarTrackerProcessing(struct AcType *AC);
extern void FssProcessing(struct AcType *AC);
extern void MagnetometerProcessing(struct AcType *AC);
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
double modeAng[3] = {0}; //degree
/* for one axis mode does matter modeAng[0]
 * for LvlhMode - all angles
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
       /* Chek sensors and actuators */
       if(AC->Nfss == 0 || AC->Ngyro<3){
           printf("EasySunMode: check SC sensors, Nfss=%li, Ngyro=%li\n",
                                              AC->Nfss, AC->Ngyro);
       }
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

    for(i=0;i<3;i++)
        modeAng[i] = 0;
    modeAng[0] = acos(VoV(S->svb,C.fvb))*R2D; //degree
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
         /* Chek sensors and actuators */
         if(AC->Nst == 0 || AC->Ngps==0 || AC->Ngyro<3){
             printf("EasyLvlhMode: check SC sensors, Nst=%li, Ngps=%li, Ngyro=%li\n",
                                                 AC->Nst, AC->Ngps, AC->Ngyro);
         }
         if(AC->Nmtb > 0 && AC->Nmtb != 3){
             printf("EasyLvlhMode: check MTBs, Nmtb=%li\n", AC->Nmtb);
         }
      }
      /* .. Sensor Processing */
      GyroProcessing(AC);
      GpsProcessing(AC);
      StarTrackerProcessing(AC);

      /* Find Attitude Command */
      FindCLN(AC->PosN,AC->VelN,CRN,wln);
      C2Q(CRN,qrn);

      /* Form Error Signals */
      QxQT(AC->qbn,qrn,qbr);
      RECTIFYQ(qbr);

      double wlb[3]={0,0,0};
      QxV(S->B[0].qn,wln,wlb);
      /* PD Control */
      for(i=0;i<3;i++) {
          C->Tcmd[i] = -C->Kr[i]*(AC->wbn[i]-wlb[i])-C->Kp[i]*(2.0*qbr[i]);
          //AC->Whl[i].Tcmd = -C->Tcmd[i];
          AC->Tcmd[i] = C->Tcmd[i];
      }
      WheelProcessing(AC);

      /* Momentum Management */
      if(AC->Nmtb == 3){
          for(i=0;i<3;i++)
             Herr[i] = AC->Whl[i].H - C->Hwcmd[i];
          VxV(Herr,AC->bvb,HxB);
          for(i=0;i<3;i++)
              AC->MTB[i].Mcmd = C->Kunl*HxB[i];
      }
      double angV[3];
      Q2AngleVec(qbr,angV);
      for(i=0;i<3;i++)
          modeAng[i] = angV[i]*R2D;
}
/**********************************************************************/
/*              Warning NadirMode haven't MTB detumbing               */
void NadirMode(struct SCType *S){
    double wln[3],CRN[3][3];
    double qrn[4],qbr[4];
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
        /* Chek sensors and actuators */
        if(AC->Nst == 0 || AC->Ngps==0 || AC->Ngyro<3){
            printf("NadirMode: check SC sensors, Nst=%li, Ngps=%li, Ngyro=%li\n",
                   AC->Nst, AC->Ngps, AC->Ngyro);
        }
    }
    /* .. Sensor Processing */
    GyroProcessing(AC);
    GpsProcessing(AC);
    StarTrackerProcessing(AC);

    /* Find Attitude Command */
    FindCLN(AC->PosN,AC->VelN,CRN,wln);
    C2Q(CRN,qrn);

    /* Form Error Signals */
    QxQT(AC->qbn,qrn,qbr);
    RECTIFYQ(qbr);
    double PosB[3];
    QxV(AC->qbn,AC->PosN,PosB);
    UNITV(PosB);
    double wlb[3];
    QxV(AC->qbn,wln,wlb);

    /* .. Control Low Processing */
    double genAxis[3] = {0,0,-1};
    double ArgUpr[3];
    VxV(PosB,genAxis,ArgUpr);
    for(i=0;i<3;i++)
        AC->Tcmd[i] = -C->Kr[i]*(AC->wbn[i]-wlb[i])
                      -C->Kp[i]*ArgUpr[i];

    /* .. Actuator Processing */
    WheelProcessing(AC);
    for(i=0;i<3;i++)
        modeAng[i] = 0;
    modeAng[0] = acos(VoV(PosB,genAxis))*R2D; //degree
}

/**********************************************************************/
/**********************************************************************/
/*                    Pointing Orientation                            */
void PointOrientation(struct SCType *S)
{
    struct AcType *AC;
    struct CmdType *Cmd;
    long i;

    AC = &S->AC;
    Cmd = &AC->Cmd;

    static double kw[3] = {0.5,0.5,0.5};
    static double ka[3] = {0.01,0.01,0.01};
    static double lat = 0, lng = 0, alt = 0;
    static long Init = 1;
    if (Init){
        Init = 0;
        lat = -2.5, lng = 140.71, alt = 1; //Джаяпура
        for(i=0;i<3;i++){
            FindPDGains(AC->MOI[i][i],0.1,0.7,
                        &kw[i],&ka[i]);
        }
        /* Chek sensors and actuators */
        if(AC->Nst == 0 || AC->Ngps==0 || AC->Ngyro<3){
            printf("PointOrientation: check SC sensors, Nst=%li, Ngps=%li, Ngyro=%li\n",
                   AC->Nst, AC->Ngps, AC->Ngyro);
        }
    }
    /* .. Sensor Processing */
    GyroProcessing(AC);
    GpsProcessing(AC);
    StarTrackerProcessing(AC);

    /* Form attitude error signals */
    static long isFeakLvlhForTest = FALSE;
    if(isFeakLvlhForTest){
        lat = R2D*S->GPS[0].WgsLat;
        lng = R2D*S->GPS[0].WgsLng;
    }

    double wref[3] ={0,0,0};
    double dwref[3]={0,0,0};
    double nref[3] ={0,0,0};
    double eartPointN[3] = {0};
    WSG84ToECI(lat, lng, alt, eartPointN);
    FindNWref(S->PosN, S->VelN, eartPointN, nref, wref, dwref);

    QxV(S->B[0].qn,wref,Cmd->wrn);//D*wref
    double werr[3];
    for(i=0;i<3;i++)
        werr[i] = AC->wbn[i] - Cmd->wrn[i];

    double tempV[3];
    double wJw[3];
    MxV(AC->MOI,AC->wbn,tempV);
    VxV(AC->wbn,tempV,wJw);
    double Jwexwr[3];
    VxV(werr,Cmd->wrn,tempV);
    MxV(AC->MOI,tempV,Jwexwr);

    double Jdwr[3];
    QxV(S->B[0].qn,dwref,tempV);//D*dwref
    MxV(AC->MOI,tempV,Jdwr);
    double nrxYb[3];
    double Yb[3] = {0,0,-1};
    QxV(S->B[0].qn,nref,tempV); //D*nref
    VxV(tempV,Yb,nrxYb);

    for(i=0;i<3;i++)
        AC->Tcmd[i] = wJw[i]-Jwexwr[i]+Jdwr[i]-kw[i]*werr[i]-ka[i]*nrxYb[i];

    WheelProcessing(AC);

    for(i=0;i<3;i++)
        modeAng[i] = 0;
    modeAng[0] = angEarthPointNegAxis(S,eartPointN,Yb) * R2D; //rad

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

/**********************************************************************/
/**********************************************************************/
/*                       Thruster Mode                                */
void ThrLvlhMode(struct SCType *S){
    double wln[3],CRN[3][3];
    double qrn[4],qbr[4];
    long i;
    struct AcType *AC;
    struct AcThreeAxisCtrlType *C;

    AC = &S->AC;
    C = &AC->ThreeAxisCtrl;
    struct CmdType * Cmd = &AC->Cmd;

    if (C->Init) {
        C->Init = 0;
        for(i=0;i<3;i++) {
            C->Kr[i] = AC->MOI[i][i] * 0.55;
            C->Kp[i] = AC->MOI[i][i] * 0.75;
        }

        //C->Kunl = 1.0E6;
        Cmd->qrl[0] = 1; Cmd->qrl[1] = 0;
        Cmd->qrl[2] = 0; Cmd->qrl[3] = 0;
        /* Chek sensors and actuators */
        if(AC->Nst == 0 || AC->Ngps==0 || AC->Ngyro<3){
            printf("EasyLvlhMode: check SC sensors, Nst=%li, Ngps=%li, Ngyro=%li\n",
                   AC->Nst, AC->Ngps, AC->Ngyro);
        }
        if(AC->Nmtb > 0 && AC->Nmtb != 3){
            printf("EasyLvlhMode: check MTBs, Nmtb=%li\n", AC->Nmtb);
        }

    }
    /* .. Sensor Processing */
    GyroProcessing(AC);
    GpsProcessing(AC);
    StarTrackerProcessing(AC);

    /* Find Attitude Command */
    FindCLN(AC->PosN,AC->VelN,CRN,wln);
    C2Q(CRN,qrn);

    /* Form Error Signals */

    double qln[4] = {1,0,0,0};
    if(Cmd->Parm == PARM_EULER_ANGLES) {
        double DC[3][3] = {0};
        A2C(Cmd->RotSeq,Cmd->Ang[0],Cmd->Ang[1],Cmd->Ang[2],DC);
        if (Cmd->Frame == FRAME_L){
            C2Q(DC,Cmd->qrl);
        }
        else{
            Cmd->qrl[0] = 1; Cmd->qrl[1] = 0;
            Cmd->qrl[2] = 0; Cmd->qrl[3] = 0;
        }
        QxQ(Cmd->qrl,qrn, qln);
        QxQT(AC->qbn,qln,qbr);
    }
    else{
        QxQT(AC->qbn,qrn,qbr);
    }
    RECTIFYQ(qbr);

    double wlb[3]={0,0,0};
    QTxV(qbr,wln,wlb);

    /* PD Control */
    static long wasChangePD = 0;
    double therr[3] = {0};
    Q2AngleVec(qbr, therr);
    for(i=0;i<3;i++){
        if(fabs(therr[i]) > 5*D2R){
            break;
        }
    }
    if(i==3 && wasChangePD){
        for(i=0;i<3;i++) {
            C->Kr[i] = AC->MOI[i][i] * 0.55;
        }
        wasChangePD = 0;
    }
    else if(i<3 && !wasChangePD){
        for(i=0;i<3;i++) {
            C->Kr[i] = AC->MOI[i][i] * 5.5;
        }
        wasChangePD = 1;
    }

    for(i=0;i<3;i++) {
        C->Tcmd[i] = -C->Kr[i]*(AC->wbn[i]-wlb[i])-C->Kp[i]*(therr[i]); //2.0*qbr[i]
        AC->Tcmd[i] = C->Tcmd[i];
    }

    ThrProcessing(AC, 0, 4);

    double angV[3];
    Q2AngleVec(qbr,angV);
    for(i=0;i<3;i++)
        modeAng[i] = angV[i]*R2D;
}
