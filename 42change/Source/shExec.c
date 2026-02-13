/*    This file is distributed with 42,                               */
/*    the (mostly harmless) spacecraft dynamics simulation            */
/*    created by Eric Stoneking of NASA Goddard Space Flight Center   */

/*    Copyright 2010 United States Government                         */
/*    as represented by the Administrator                             */
/*    of the National Aeronautics and Space Administration.           */

/*    No copyright is claimed in the United States                    */
/*    under Title 17, U.S. Code.                                      */

/*    All Other Rights Reserved.                                      */


#define DECLARE_GLOBALS
#include "42.h"
#undef DECLARE_GLOBALS
#include "shHeaders.h"
#include "SPSModel.h"
#include "PSModel.h"

/* #ifdef __cplusplus
** namespace _42 {
** using namespace Kit;
** #endif
*/

#ifdef _ENABLE_GUI_
   extern int HandoffToGui(int argc, char **argv);
#endif

/**********************************************************************/
void ReportProgress(void)
{
#define PROGRESSPERCENT 10

      static long ProgressPercent = 0;
      static long ProgressCtr = 0;
      static double ProgressTime = 0.0;

      if (TimeMode == FAST_TIME) {

         if (SimTime >= ProgressTime) {
            ProgressCtr++;
            ProgressTime = (double) (ProgressCtr*PROGRESSPERCENT)/100.0*STOPTIME;
            printf("    42 Case %s is %3.1li%% Complete at Time = %12.3f\n",
               InOutPath,ProgressPercent,SimTime);
            ProgressPercent += PROGRESSPERCENT;
         }
      }
}
/**********************************************************************/
void ManageFlags(void)
{
      long nout,GLnout;
      static long iout = 1000000;
      static long GLiout = 1000000;

      nout = ((long) (DTOUT/DTSIM + 0.5));
      GLnout = ((long) (DTOUTGL/DTSIM + 0.5));

      iout++;
      if(iout >= nout) {
         iout=0;
         OutFlag = TRUE;
      }
      else OutFlag = FALSE;

      GLiout++;
      if(GLiout >= GLnout) {
         GLiout=0;
         GLOutFlag = TRUE;
      }
      else GLOutFlag = FALSE;

}
/**********************************************************************/
long AdvanceTime(void)
{
      static long itime = 0;
      static long PrevTick = 0;
      static long CurrTick = 1;
      long Done;

      /* Advance time to next Timestep */
      switch (TimeMode) {
         case FAST_TIME :
            SimTime += DTSIM;
            itime = (long) ((SimTime+0.5*DTSIM)/(DTSIM));
            SimTime = ((double) itime)*DTSIM;
            DynTime = DynTime0 + SimTime;

            AtomicTime = DynTime - 32.184; /* TAI */
            CivilTime = AtomicTime - LeapSec; /* UTC "clock" time */
            GpsTime = AtomicTime - 19.0;

            TT.JulDay = TimeToJD(DynTime);
            TimeToDate(DynTime,&TT.Year,&TT.Month,&TT.Day,
               &TT.Hour,&TT.Minute,&TT.Second,DTSIM);
            TT.doy = MD2DOY(TT.Year,TT.Month,TT.Day);

            UTC.JulDay = TimeToJD(CivilTime);
            TimeToDate(CivilTime,&UTC.Year,&UTC.Month,&UTC.Day,
               &UTC.Hour,&UTC.Minute,&UTC.Second,DTSIM);
            UTC.doy = MD2DOY(UTC.Year,UTC.Month,UTC.Day);

            GpsTimeToGpsDate(GpsTime,&GpsRollover,&GpsWeek,&GpsSecond);

            break;
         case REAL_TIME :
            usleep(1.0E6*DTSIM);
            SimTime += DTSIM;
            itime = (long) ((SimTime+0.5*DTSIM)/(DTSIM));
            SimTime = ((double) itime)*DTSIM;
            DynTime = DynTime0 + SimTime;

            AtomicTime = DynTime - 32.184; /* TAI */
            CivilTime = AtomicTime - LeapSec; /* UTC "clock" time */
            GpsTime = AtomicTime - 19.0;

            TT.JulDay = TimeToJD(DynTime);
            TimeToDate(DynTime,&TT.Year,&TT.Month,&TT.Day,
               &TT.Hour,&TT.Minute,&TT.Second,DTSIM);
            TT.doy = MD2DOY(TT.Year,TT.Month,TT.Day);

            UTC.JulDay = TimeToJD(CivilTime);
            TimeToDate(CivilTime,&UTC.Year,&UTC.Month,&UTC.Day,
               &UTC.Hour,&UTC.Minute,&UTC.Second,DTSIM);
            UTC.doy = MD2DOY(UTC.Year,UTC.Month,UTC.Day);

            GpsTimeToGpsDate(GpsTime,&GpsRollover,&GpsWeek,&GpsSecond);

            break;
         case EXTERNAL_TIME :
            while(CurrTick == PrevTick) {
               CurrTick = (long) (1.0E-6*usec()/DTSIM);
            }
            PrevTick = CurrTick;
            SimTime += DTSIM;
            itime = (long) ((SimTime+0.5*DTSIM)/(DTSIM));
            SimTime = ((double) itime)*DTSIM;

            RealSystemTime(&UTC.Year,&UTC.doy,&UTC.Month,&UTC.Day,
               &UTC.Hour,&UTC.Minute,&UTC.Second,DTSIM);
            CivilTime = DateToTime(UTC.Year,UTC.Month,UTC.Day,
               UTC.Hour,UTC.Minute,UTC.Second);
            AtomicTime = CivilTime + LeapSec;
            DynTime = AtomicTime + 32.184;
            GpsTime = AtomicTime - 19.0;

            TT.JulDay = TimeToJD(DynTime);
            TimeToDate(DynTime,&TT.Year,&TT.Month,&TT.Day,
               &TT.Hour,&TT.Minute,&TT.Second,DTSIM);
            TT.doy = MD2DOY(TT.Year,TT.Month,TT.Day);

            UTC.JulDay = TimeToJD(CivilTime);
            UTC.doy = MD2DOY(UTC.Year,UTC.Month,UTC.Day);

            GpsTimeToGpsDate(GpsTime,&GpsRollover,&GpsWeek,&GpsSecond);
            DynTime0 = DynTime - SimTime;

            break;
         /*case NOS3_TIME :
            NOS3Time(&UTC.Year,&UTC.doy,&UTC.Month,&UTC.Day,
               &UTC.Hour,&UTC.Minute,&UTC.Second);
            CivilTime = DateToTime(UTC.Year,UTC.Month,UTC.Day,
               UTC.Hour,UTC.Minute,UTC.Second);
            AtomicTime = CivilTime + LeapSec;
            DynTime = AtomicTime + 32.184;
            GpsTime = AtomicTime - 19.0;

            TT.JulDay = TimeToJD(DynTime);
            TimeToDate(DynTime,&TT.Year,&TT.Month,&TT.Day,
               &TT.Hour,&TT.Minute,&TT.Second,DTSIM);
            TT.doy = MD2DOY(TT.Year,TT.Month,TT.Day);

            UTC.JulDay = TimeToJD(CivilTime);
            UTC.doy = MD2DOY(UTC.Year,UTC.Month,UTC.Day);

            GpsTimeToGpsDate(GpsTime,&GpsRollover,&GpsWeek,&GpsSecond);
            SimTime = DynTime - DynTime0;
            break;*/
      }

      /* Check for end of run */
      if (SimTime > STOPTIME) Done = 1;
      else Done = 0;

      return(Done);
}
/*********************************************************************/
/* The SC Bounding Box is referred to the origin of B0,              */
/* and expressed in B0                                               */
void UpdateScBoundingBox(struct SCType *S)
{
#define REFPT_CM 0
    struct BodyType *B, *B0;
    struct BoundingBoxType *BBox;
    struct MeshType *M;
    double ctrB[3],ctrN[3],ctrB0[3],maxB0,minB0,r[3];
    long Ib,i;

    B0 = &S->B[0];
    BBox = &S->BBox;

    for(Ib=0;Ib<S->Nb;Ib++) {
        B = &S->B[Ib];
        M = &Mesh[B->MeshTag];
        for(i=0;i<3;i++) {
            ctrB[i] = M->BBox.center[i];
            if (S->RefPt == REFPT_CM) {
                ctrB[i] -= B->cm[i];
            }
        }
        MTxV(B->CN,ctrB,ctrN);
        for(i=0;i<3;i++) {
            ctrN[i] += (B->pn[i]-B0->pn[i]);
        }
        MxV(B0->CN,ctrN,ctrB0);
        for(i=0;i<3;i++) {
            if (S->RefPt == REFPT_CM) {
                ctrB0[i] += B0->cm[i];
            }
            maxB0 = ctrB0[i] + M->BBox.radius;
            minB0 = ctrB0[i] - M->BBox.radius;
            if(BBox->max[i] < maxB0) BBox->max[i] = maxB0;
            if(BBox->min[i] > minB0) BBox->min[i] = minB0;
        }
    }
    for(i=0;i<3;i++) {
        BBox->center[i] = 0.5*(BBox->max[i]+BBox->min[i]);
        r[i] = BBox->max[i]-BBox->center[i];
    }
    BBox->radius = MAGV(r);
#undef REFPT_CM
}
/**********************************************************************/
void ManageBoundingBoxes(void)
{
      static long BBoxCtr = 100;
      long Isc;
      struct SCType *S;

      BBoxCtr++;
      if (BBoxCtr > 100) {
         BBoxCtr = 0;
         for(Isc=0;Isc<Nsc;Isc++) {
            S = &SC[Isc];
            if (S->Exists) {
               UpdateScBoundingBox(S);
            }
         }
      }
}
/**********************************************************************/
/* Zero forces and torques                                            */
void ZeroFrcTrq(void)
{
      struct SCType *S;
      struct BodyType *B;
      struct JointType *G;
      struct NodeType *FN;
      long Isc,Ib,Ig,In;

      for(Isc=0;Isc<Nsc;Isc++) {
         S = &SC[Isc];
         S->FrcN[0] = 0.0;
         S->FrcN[1] = 0.0;
         S->FrcN[2] = 0.0;

         for(Ib=0;Ib<S->Nb;Ib++) {
            B = &S->B[Ib];
            B->FrcN[0] = 0.0;
            B->FrcN[1] = 0.0;
            B->FrcN[2] = 0.0;
            B->FrcB[0] = 0.0;
            B->FrcB[1] = 0.0;
            B->FrcB[2] = 0.0;
            B->Trq[0] = 0.0;
            B->Trq[1] = 0.0;
            B->Trq[2] = 0.0;
         }
         for(Ig=0;Ig<S->Ng;Ig++) {
            G = &S->G[Ig];
            G->Frc[0] = 0.0;
            G->Frc[1] = 0.0;
            G->Frc[2] = 0.0;
            G->Trq[0] = 0.0;
            G->Trq[1] = 0.0;
            G->Trq[2] = 0.0;
         }
         for(Ib=0;Ib<S->Nb;Ib++) {
            B = &S->B[Ib];
            for(In=0;In<B->NumNodes;In++) {
              FN = &B->Node[In];
              FN->Frc[0] = 0.0;
              FN->Frc[1] = 0.0;
              FN->Frc[2] = 0.0;
              FN->Trq[0] = 0.0;
              FN->Trq[1] = 0.0;
              FN->Trq[2] = 0.0;
            }
         }
      }
}
/**********************************************************************/
#ifdef _USE_QTPLOT_
    extern void ToPlot(double Time);
#endif

static void shDynamics(struct SCType *S){
    if(S->DynMethod == DYN_ONE_BODY) {
        OneBodyRK4(S);
        struct OrbitType *O;

        O = &Orb[S->RefOrb];

        switch(O->Regime) {
           case ORB_ZERO :
           case ORB_FLIGHT :
              if (O->PolyhedronGravityEnabled) {
                 PolyhedronCowellRK4(S);
              }
              else CowellRK4(S);
              break;
           case ORB_CENTRAL :
              switch(S->OrbDOF) {
                 case ORBDOF_FIXED :
                    FixedOrbitPosition(S);
                    break;
                 case ORBDOF_EULER_HILL :
                    EulHillRK4(S);
                    break;
                 case ORBDOF_COWELL :
                    CowellRK4(S);
                    break;
                 default :
                    EnckeRK4(S);
              }
              break;
           case ORB_THREE_BODY :
              switch(S->OrbDOF) {
                 case ORBDOF_FIXED :
                    FixedOrbitPosition(S);
                    break;
                 case ORBDOF_EULER_HILL :
                    EulHillRK4(S);
                    break;
                 case ORBDOF_COWELL :
                    CowellRK4(S);
                    break;
                 default :
                    ThreeBodyEnckeRK4(S);
              }
              break;
           default :
              printf("Unknown Orbit Regime in Dynamics.  Bailing out.\n");
              exit(1);
        }
    }
    else{
        Dynamics(S);
    }
}

static void shFeakReport(){

}

extern void shFlightSoftWare(struct SCType *S);
extern void shActuators(struct SCType *S);

long SimStep(void)
{
      long Isc;
      static long First = 1;
      struct SCType *S;
      long SimComplete;
      double TotalRunTime;

      if (First) {
         First = 0;
         SimTime = 0.0;
         /* First call just initializes timer */
         RealRunTime(&TotalRunTime,DTSIM);
         ManageFlags();

         Ephemerides(); /* Sun, Moon, Planets, Spacecraft, Useful Auxiliary Frames */

         ZeroFrcTrq();
         for(Isc=0; Isc<Nsc; Isc++) {
            S = &SC[Isc];
            if (S->Exists) {
               Environment(S);    /* Magnetic Field, Atmospheric Density */
               Perturbations(S);  /* Environmental Forces and Torques */
               Sensors(S);
               shFlightSoftWare(S);
               initPSModel(S);
               Actuators(S);
               PartitionForces(S); /* Orbit-affecting and "internal" */
               InitSPSModel(S->Label);
            }
         }
         shFeakReport();  /* File Output */
      }

      ReportProgress();
      ManageFlags();

      /* Read and Interpret Command Script File */
      CmdInterpreter();

      /* Update Dynamics to next Timestep */
      for(Isc=0;Isc<Nsc;Isc++) {
         if (SC[Isc].Exists) shDynamics(&SC[Isc]);
      }
      SimComplete = AdvanceTime();
      OrbitMotion(DynTime);

      /* Update SC Bounding Boxes occasionally */
      ManageBoundingBoxes();

      InterProcessComm(); /* Send and receive from external processes */
      Ephemerides(); /* Sun, Moon, Planets, Spacecraft, Useful Auxiliary Frames */
      ZeroFrcTrq();
      for(Isc=0;Isc<Nsc;Isc++) {
         S = &SC[Isc];
         if (S->Exists) {
            Environment(S);    /* Magnetic Field, Atmospheric Density */
            Perturbations(S);  /* Environmental Forces and Torques */
            Sensors(S);
            shFlightSoftWare(S);
            shActuators(S);
            SPSModel(S);
            PartitionForces(S); /* Orbit-affecting and "internal" */
         }
      }
      shFeakReport();  /* File Output */

      #ifdef _USE_QTPLOT_
      ToPlot(SimTime);
      #endif
      /* Exit when Stoptime is reached */
      if (SimComplete) {
         if (TimeMode == FAST_TIME) {
            RealRunTime(&TotalRunTime,DTSIM);
            printf("     Total Run Time = %9.2lf sec\n", TotalRunTime);
            printf("     Sim Speed = %8.2lf x Real\n",
               STOPTIME/TotalRunTime);
         }
      }
      return(SimComplete);

}
/**********************************************************************/
extern FILE *FileRead(const char *Path, const char *File);
int exec(int argc,char **argv)
{
      long Done = 0;

      MapTime = 0.0;
      JointTime = 0.0;
      PathTime = 0.0;
      PVelTime = 0.0;
      FrcTrqTime = 0.0;
      AssembleTime = 0.0;
      LockTime = 0.0;
      TriangleTime = 0.0;
      SubstTime = 0.0;
      SolveTime = 0.0;

      InitSim(argc,argv);
      CmdInterpreter();

      FILE * infile = FileRead(InOutPath,"Inp_IPC.txt");
      if(infile){
          InitInterProcessComm();
      }
      else{
          Nipc = 0;
      }

      #ifdef _ENABLE_GUI_
      static long imGuiPlotEnable = 1;
         if (GLEnable) {
            HandoffToGui(argc,argv);
         }
         else if (imGuiPlotEnable) {
             extern int HandoffToPlotGui(int argc, char **argv);
             HandoffToPlotGui(argc,argv);
         }
         else {
            while(!Done) {
               Done = SimStep();
            }
         }
      #else
         /* Crunch numbers till done */
         while(!Done) {
            Done = SimStep();
         }
      #endif

      //printf("\n\nMap Time = %lf sec\n",MapTime);
      //printf("Joint Partial Time = %lf sec\n",JointTime);
      //printf("Path Time = %lf sec\n",PathTime);
      //printf("PVel Time = %lf sec\n",PVelTime);
      //printf("FrcTrq Time = %lf sec\n",FrcTrqTime);
      //printf("Assemble Time = %lf sec\n",AssembleTime);
      //printf("Lock Time = %lf sec\n",LockTime);
      //printf("Triangularize Time = %lf sec\n",TriangleTime);
      //printf("Fwd Substitution Time = %lf sec\n",SubstTime);
      //printf("Solve Time = %lf sec\n",SolveTime);
      return(0);
}

/* #ifdef __cplusplus
** }
** #endif
*/
