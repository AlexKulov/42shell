/*    Code by Alexsandr Kulakov                              */
/*    Model of Propulsion System                             */

#include "42.h"
#include "PSModel.h"

typedef struct ThrParameters{
    struct ThrType * Main;
    double BuildUpTime; //время выхода на режим
    double TailoffTime;//время импульса последействия
    double FSecOn; //F^2/(2ThrTime)
    double FSecOff;
    long IsOpen; //OnOff
    int Incs; // кол-во включений
    double Consumption; //потребление удельное
    double Isp; //удельный импульс в секундах
    double OnF; //тяга после включения
    double OffF;//тяга на моменет отключения
    double TailF;//тяга последействия
    double StopStartTime;
    double OnTime;
    double OffTime;
    double Fuel;
    double MinThrottling;
    double MaxThrottling;
}ThrParameters;

typedef struct PropulsionSystemType{
    ThrParameters * Thrs;
    long Nthr;
    double Fuel;
    char ScLabel[20]; //like SC
}PropulsionSystemType;

static PropulsionSystemType ** propSystems;
static unsigned short Nprop = 0;

extern FILE *FileRead(const char *Path, const char *File);
void initPSModel(struct SCType *S){
    static long wasInitPropPoint = 0;
    if(!wasInitPropPoint){
        propSystems = (PropulsionSystemType **)
            calloc(Nsc, sizeof(PropulsionSystemType *));
        wasInitPropPoint = 1;
    }
    if(S->Nthr){
        propSystems[Nprop] = (PropulsionSystemType *)
            calloc(1, sizeof(PropulsionSystemType));
        propSystems[Nprop]->Thrs = (ThrParameters *)
            calloc(S->Nthr, sizeof(ThrParameters));
        int i=0;
        ThrParameters * thr;
        for(i=0;i<S->Nthr;i++){
            thr = &propSystems[Nprop]->Thrs[i];
            thr->Main = &S->Thr[i];
            thr->Incs = 0;
            thr->Fuel = 0;
            thr->BuildUpTime   =   0.25;
            thr->TailoffTime   =   1.0;
            thr->StopStartTime =   0.2;
            thr->Isp           = 200.0;
            thr->MaxThrottling = 1;
            thr->MinThrottling = 1;
            thr->TailF = 0;
        }
        strcpy(propSystems[Nprop]->ScLabel, S->Label);

        char junk[120],newline;
        char thrsFileName[40];
        sprintf(thrsFileName, "Thrs_%s.txt", S->Label);
        FILE * psFile=FileRead(InOutPath, thrsFileName);
        if(psFile){
            fscanf(psFile,"%[^\n] %[\n]",junk,&newline);
            fscanf(psFile,"%[^\n] %[\n]",junk,&newline);
            fscanf(psFile,"%[^\n] %[\n]",junk,&newline);
            fscanf(psFile,"%ld %[^\n] %[\n]",&propSystems[Nprop]->Nthr,junk,&newline);
            if(propSystems[Nprop]->Nthr != S->Nthr){
                printf("Error number of Thrs in Thrs_%s.txt\n", S->Label);
                exit(1);
            }
            else{
                for(i=0;i<S->Nthr;i++){
                    fscanf(psFile,"%[^\n] %[\n]",junk,&newline);
                    fscanf(psFile,"%lf %[^\n] %[\n]",&propSystems[Nprop]->Thrs[i].BuildUpTime,junk,&newline);
                    fscanf(psFile,"%lf %[^\n] %[\n]",&propSystems[Nprop]->Thrs[i].TailoffTime,junk,&newline);
                    fscanf(psFile,"%lf %[^\n] %[\n]",&propSystems[Nprop]->Thrs[i].StopStartTime,junk,&newline);
                    fscanf(psFile,"%lf %[^\n] %[\n]",&propSystems[Nprop]->Thrs[i].Isp,junk,&newline);
                    fscanf(psFile,"%lf %lf  %[^\n] %[\n]",&propSystems[Nprop]->Thrs[i].MinThrottling,
                                                          &propSystems[Nprop]->Thrs[i].MaxThrottling,junk,&newline);
                    propSystems[Nprop]->Thrs[i].MinThrottling = propSystems[Nprop]->Thrs[i].MinThrottling/100;
                    propSystems[Nprop]->Thrs[i].MaxThrottling = propSystems[Nprop]->Thrs[i].MaxThrottling/100;
                }
            }
            fclose(psFile);
        }

        double Fmax = 0;
        for(i=0;i<S->Nthr;i++){
            Fmax = thr->Main->Fmax;
            thr = &propSystems[Nprop]->Thrs[i];
            thr->FSecOn  = Fmax*Fmax/(2*thr->BuildUpTime);
            thr->FSecOff = Fmax*Fmax/(2*thr->TailoffTime);
            thr->OffTime = thr->StopStartTime;
        }
        Nprop++;
    }
}

static PropulsionSystemType * getScProp(struct SCType *S){
    for(long Is=0;Is<Nprop;Is++){
        if(!propSystems[Is]){
            break;
        }
        if(!strcmp(propSystems[Is]->ScLabel, S->Label)){
            return propSystems[Is];
        }
    }
    return NULL;
}
double getThrsFuel(struct SCType *S, long i){
    PropulsionSystemType * ps = getScProp(S);
    if(S->Nthr>i){
        return ps->Thrs[i].Fuel;
    }
    static long onlyPrint = 0;
    if(!onlyPrint){
        printf("Error getThrsFuel S->Nthr(%i)<=i(%i)\n", S->Nthr, i);
        onlyPrint = 1;
    }
    return 0;
}

long PSModel(struct SCType *S){
    PropulsionSystemType * prop = NULL;
    prop = getScProp(S);
    if(prop){
        struct BodyType *B;
        struct NodeType *N;
        ThrParameters * thr;
        for(long i=0; i<S->Nthr; i++){
            thr = &prop->Thrs[i];
            double F = 0;
            if (  thr->OffTime >= thr->StopStartTime &&
                ((thr->Main->Mode == THR_PULSED && thr->Main->PulseWidthCmd >0) ||
                 (thr->Main->Mode == THR_PROPORTIONAL && thr->Main->ThrustLevelCmd>thr->MinThrottling))){
                if(!thr->IsOpen){
                    thr->Incs++;
                    thr->IsOpen = TRUE;
                    if(thr->Main->Mode == THR_PROPORTIONAL){
                        if(thr->Main->ThrustLevelCmd > thr->MaxThrottling){
                            thr->Main->ThrustLevelCmd = thr->MaxThrottling;
                        }
                    }
                }
            }
            else{
                if(thr->IsOpen){
                    thr->OffTime = 0;
                    thr->OffF = thr->OnF;
                }
                thr->IsOpen = FALSE;
                thr->Consumption = 0;
                thr->OnTime = 0;
            }

            if(thr->IsOpen){
                thr->OnTime              += DTSIM;
                thr->Main->PulseWidthCmd -= DTSIM;
                F = sqrt(2*thr->FSecOn*thr->OnTime);
                if (F > thr->Main->Fmax) {
                    F = thr->Main->Fmax;
                }
                else if(thr->Main->Mode == THR_PROPORTIONAL){
                    double throttlingF = thr->Main->Fmax * thr->Main->ThrustLevelCmd;
                    if (F > throttlingF){
                        F = throttlingF;
                    }
                }
                thr->Consumption = F / thr->Isp;
                thr->OnF = F;
            }

            if ((!thr->IsOpen && thr->Incs > 0) ||
                thr->TailF>0){
                thr->OffTime += DTSIM;
                F = -sqrt(2*thr->FSecOff*thr->OffTime) + thr->OffF;
                if (F < 0) {
                    F = 0;
                }
                thr->TailF = F;
            }
            if(thr->IsOpen){
                thr->Main->F = thr->OnF + thr->TailF;
                if(thr->Main->F>thr->Main->Fmax) //???
                    thr->Main->F=thr->Main->Fmax;
            }
            else
                thr->Main->F = thr->TailF;

            thr->Main->Frc[0] = F*thr->Main->A[0];
            thr->Main->Frc[1] = F*thr->Main->A[1];
            thr->Main->Frc[2] = F*thr->Main->A[2];

            thr->Fuel = thr->Fuel + thr->Consumption*DTSIM;
            B = &S->B   [thr->Main->Body];
            N = &B->Node[thr->Main->Node];
            VxV(N->PosCm,thr->Main->Frc,thr->Main->Trq);

            if (S->FlexActive) {
                for(long j=0;j<3;j++) {
                    N->Trq[j] += thr->Main->Trq[j];
                    N->Frc[j] += thr->Main->Frc[j];
                }
            }
        }
        return 1;
    }//if(prop)
    return 0;
}
