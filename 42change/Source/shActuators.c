/*    This file is distributed with 42,                               */
/*    the (mostly harmless) spacecraft dynamics simulation            */
/*    created by Eric Stoneking of NASA Goddard Space Flight Center   */

/*    Copyright 2010 United States Government                         */
/*    as represented by the Administrator                             */
/*    of the National Aeronautics and Space Administration.           */

/*    No copyright is claimed in the United States                    */
/*    under Title 17, U.S. Code.                                      */

/*    All Other Rights Reserved.                                      */


#include "42.h"
#include "PSModel.h"

/* #ifdef __cplusplus
** namespace _42 {
** using namespace Kit;
** #endif
*/


extern void WhlDrag(struct WhlType *W);
extern void WhlModel(struct WhlType *W,struct SCType *S);
extern void MTBModel(struct MTBType *MTB,double bvb[3]);
extern void ThrusterPlumeFrcTrq(struct SCType *S);
extern void ThrModel(struct ThrType *Thr,struct SCType *S,double DT);
/**********************************************************************/
/*  This function is called at the simulation rate.  Sub-sampling of  */
/*  actuators should be done on a case-by-case basis.                 */

void shActuators(struct SCType *S){

      struct NodeType *N;
      long i,j;
      double FrcN[3],FrcB[3];
      struct AcType *AC;
      struct JointType *G;
      struct AcJointType *AG;
      struct ThrType *Thr;
      struct ShakerType *Sh;
      struct WhlType *W;

      AC = &S->AC;

      /* Ideal Actuators */
      for(i=0;i<3;i++) FrcB[i] = S->IdealAct[i].Fcmd;
      MTxV(S->B[0].CN,FrcB,FrcN);
      for(i=0;i<3;i++) {
         S->B[0].FrcB[i] += FrcB[i];
         S->B[0].FrcN[i] += FrcN[i];
         S->B[0].Trq[i] += S->IdealAct[i].Tcmd;
      }
      if (S->FlexActive) {
         N = &S->B[0].Node[0]; /* Arbitrarily put ideal actuators at Node 0 */
         for(i=0;i<3;i++) {
            N->Trq[i] += S->IdealAct[i].Tcmd;
            N->Frc[i] += S->IdealAct[i].Fcmd;
         }
      }

      /* Wheels */
      for(i=0;i<S->Nw;i++) {
         WhlModel(&S->Whl[i],S);
      }
      /* MTBs */
      for(i=0;i<S->Nmtb;i++){
         MTBModel(&S->MTB[i],S->bvb);
         for(j=0;j<3;j++){
            S->B[0].Trq[j] += S->MTB[i].Trq[j];
         }
      }
      
      /* Gimbal Drives */
      for(i=0;i<AC->Ng;i++) {
         G = &S->G[i];
         AG = &AC->G[i];
         if (G->Type == ACTUATED_JOINT) {
            for(j=0;j<G->RotDOF;j++) {
               G->AngRateCmd[j] = AG->GCmd.AngRate[j];
            }
            for(j=0;j<G->TrnDOF;j++) {
               G->PosRateCmd[j] = AG->GCmd.PosRate[j];
            }
         }
      }

      /* Thrusters */
      long isPsModel = PSModel(S);
      for(i=0;i<S->Nthr;i++) {
          Thr = &S->Thr[i];
          if(!isPsModel)
              ThrModel(Thr,S,DTSIM);
          MTxV(S->B[Thr->Body].CN,Thr->Frc,FrcN);
          for(j=0;j<3;j++) {
              S->B[Thr->Body].Trq[j] += Thr->Trq[j];
              S->B[Thr->Body].FrcN[j] += FrcN[j];
              S->B[Thr->Body].FrcB[j] += Thr->Frc[j];
          }
      }

      if (ThrusterPlumesActive) {
         ThrusterPlumeFrcTrq(S);
      }
      
      /* Wheel Jitter and Shakers only affect Flex */
      if (S->FlexActive) {
         for(i=0;i<S->Nsh;i++) {
            Sh = &S->Shaker[i];
            N = &S->B[Sh->Body].Node[Sh->Node];
            ShakerJitter(Sh,S);
            if (Sh->FrcTrq == FORCE) {
               for(j=0;j<3;j++) N->Frc[j] += Sh->Output*Sh->Axis[j];
            }
            else {
               for(j=0;j<3;j++) N->Trq[j] += Sh->Output*Sh->Axis[j];
            }
         }
         
         if (S->WhlJitterActive) {
            for(i=0;i<S->Nw;i++) {
               W = &S->Whl[i];
               N = &S->B[W->Body].Node[W->Node];
               WheelJitter(W,S);
               for(j=0;j<3;j++) {
                  N->Frc[j] += W->JitFrc[j];
                  N->Trq[j] += W->JitTrq[j];
               }
            }
         }
      }
            
}

/* #ifdef __cplusplus
** }
** #endif
*/

