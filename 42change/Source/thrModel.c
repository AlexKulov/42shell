#include "42.h"
void PrecitionThrModel(struct ThrType *Thr,struct SCType *S,double DT)
{
      struct BodyType *B;
      struct NodeType *N;
      double ThrTimeOn = 1;
      double ThrTimeOff = 2;
      double FSecOn = (Thr->Fmax * Thr->Fmax) / (2*ThrTimeOn);
      double FSecOff = (Thr->Fmax * Thr->Fmax) / (2*ThrTimeOff);
      /* 0.25 1.5 */
      double H_time = 0.01;
      /* Счетчик вкл выкл и общего времени работы*/
      long i;
      double ff = SimTime;
         if (Thr->PulseWidthCmd > 0) {
            if(Thr->OnOff == 0){
            Thr->Inclusions = Thr->Inclusions + 1;
            Thr->OnOff = 1;
            }
            Thr->TimeOff = 0;
            Thr->TimeOn += DT;
            /*Thr->F = Thr->F + sqrt(2*FSecOn*Thr->TimeOn)*DT;*/
            Thr->F = sqrt(2*FSecOn*Thr->TimeOn);
            Thr->FuelConsumption = Thr->F / 450;
            if (Thr->F > Thr->Fmax) {
                Thr->F = Thr->Fmax;
            }
            Thr->F_Off = Thr->F;

         }
         if (Thr->PulseWidthCmd <= 0 && Thr->Inclusions > 0){
            Thr->OnOff = 0;
            Thr->TimeOn = 0;
            Thr->TimeOff += DT;
            Thr->F = -sqrt(2*FSecOff*Thr->TimeOff) + Thr->F_Off;
            if (Thr->F < 0) {
                Thr->F = 0;
            }
            Thr->F_On = Thr->F;
         }
      /*if (Thr->F < 0.0) Thr->F = 0.0;*/
      /*if (Thr->F > Thr->Fmax) Thr->F = Thr->Fmax;*/

      Thr->Frc[0] = Thr->F*Thr->A[0];
      Thr->Frc[1] = Thr->F*Thr->A[1];
      Thr->Frc[2] = Thr->F*Thr->A[2];

      B = &S->B[Thr->Body];
      B->Fuel = B->Fuel + Thr->FuelConsumption*DT;
      N = &B->Node[Thr->Node];

      VxV(N->PosCm,Thr->Frc,Thr->Trq);

      if (S->FlexActive) {
         for(i=0;i<3;i++) {
            N->Trq[i] += Thr->Trq[i];
            N->Frc[i] += Thr->Frc[i];
         }
      }
}
void THRProcessing(struct AcType *AC)
{
      struct AcThrType *W;
      long Iw;

      for(Iw=0;Iw<AC->Nthr;Iw++) {
         W = &AC->Thr[Iw];
         W->PulseWidthCmd = -Limit(-VoV(AC->Tcmd,W->rxA),-W->Fmax,W->Fmax);
      }
}
