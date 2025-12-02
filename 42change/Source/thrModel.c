#include "42.h"

void precitionThrModel(struct ThrType *Thr,struct SCType *S)
{
      struct BodyType *B;
      struct NodeType *N;
      double F_Sec_On = 2.5;
      double H_time = 0.01;
      double F_Sec_Off = 5;
      long i;
      double ff = SimTime;
         if (Thr->PulseWidthCmd > 0) {
            Thr->TimeOff = 0;
            Thr->TimeOn = Thr->TimeOn + H_time;
            Thr->F = sqrt(2*F_Sec_On*Thr->TimeOn);
            Thr->Fuel_treb = 0.1;
            if (Thr->F > Thr->Fmax) {
                Thr->F = Thr->Fmax;
            }

         }
         if (Thr->PulseWidthCmd <= 0){
            Thr->TimeOn = 0;
            Thr->TimeOff = Thr->TimeOff + H_time;
            Thr->F = -sqrt(2*F_Sec_Off*Thr->TimeOff) + Thr->Fmax;
            if (Thr->F < 0) {
                Thr->F = 0;
            }
         }
      /*if (Thr->F < 0.0) Thr->F = 0.0;*/
      /*if (Thr->F > Thr->Fmax) Thr->F = Thr->Fmax;*/

      Thr->Frc[0] = Thr->F*Thr->A[0];
      Thr->Frc[1] = Thr->F*Thr->A[1];
      Thr->Frc[2] = Thr->F*Thr->A[2];

      B = &S->B[Thr->Body];
      B->Fuel = B->Fuel + Thr->Fuel_treb*H_time;
      N = &B->Node[Thr->Node];

      VxV(N->PosCm,Thr->Frc,Thr->Trq);

      if (S->FlexActive) {
         for(i=0;i<3;i++) {
            N->Trq[i] += Thr->Trq[i];
            N->Frc[i] += Thr->Frc[i];
         }
      }
}
