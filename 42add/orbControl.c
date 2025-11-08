/* Created by Kulakov Aleksandr, russ69@bk.ru
 * This file contains the functions of simplest attitude mode.
 * This file is compatible with 42.
 * https://github.com/ericstoneking/42
 */
#include "42.h"
#include <stdio.h>
#include <math.h>
#include <stdbool.h>

#define PI 3.14159265358979323846
#define DEG2RAD (PI/180.0)
#define RAD2DEG (180.0/PI)
#define MAX_DV 10.0     // максимальный импульс, м/с
#define MAX_ORBITS 25   // максимальное число витков

typedef enum {
    MINUS_V = 155,
    PLUS_V
}IMP_DIRECT;

typedef struct ManeuverImpulse{
    double dV;          // величина импульса, м/с
    bool isPlaning;    //планируется
    double tStart;   // время выдачи (модельное), с
    bool completed;         // выполнен ли импульс
    bool isExecuted;        // выполняется в данный момент
    IMP_DIRECT direct;
} ManeuverImpulse;

typedef struct OrbitType OrbitType;
static void CopyV(double V[3], double W[3]){
    W[0] = V[0];
    W[1] = V[1];
    W[2] = V[2];
}

static double addPerigeeVel(OrbitType * chaserOrb, double reqPeriod){
    double mu = World[EARTH].mu;
    double reqA = pow(mu * pow(reqPeriod, 2) / (4 * M_PI * M_PI), 1.0/3.0);
    double reqV = sqrt(2 * mu/chaserOrb->rmin - mu/reqA);
    double curV = sqrt(2 * mu/chaserOrb->rmin - mu/chaserOrb->SMA);
    return reqV-curV;
}

//dt - время через которое O достигнет перигея A
static int calcUpDownPeriod(double curPeriodA, double PeriodO, double dt,
                            double * upPeriod, double * downPeriod,int Nmax){
    *upPeriod=3*PeriodO;
    *downPeriod=0;
    double calcPeriod = 0;
    double dPeriod = 0;
    int N = 1;
    for(; N<Nmax; N++){
        calcPeriod = dt/N+PeriodO;
        dPeriod = calcPeriod - curPeriodA;
        if(dPeriod>0){
            *upPeriod =  calcPeriod;
        }
        else if(dPeriod<=0){
            *downPeriod =  calcPeriod;
            break;
        }
    }
    return N;
}


static void updateKepler(OrbitType * O, struct SCType * SC){
    CopyV(SC->PosN, O->PosN);
    CopyV(SC->VelN, O->VelN);
    RV2Eph(DynTime,O->mu,O->PosN,O->VelN,
           &O->SMA,&O->ecc,&O->inc,
           &O->RAAN,&O->ArgP,&O->anom,
           &O->tp,&O->SLR,&O->alpha,&O->rmin,
           &O->MeanMotion,&O->Period);
}

ManeuverImpulse impulsPlaninig(OrbitType * chaserOrb, OrbitType * targetOrb){
    // 1. -------------------- НАЧАЛЬНЫЕ УСЛОВИЯ -------------------
    double r1 = targetOrb->SMA;  // целевая орбита (д.б. круговая!!!)
    double ra = chaserOrb->SMA*(1+chaserOrb->ecc);  // начальная орбита активного

    // Расчет апогейного импульса
    double mu = World[EARTH].mu;
    double apogeeVel1 = sqrt(2 * mu / ra - mu / chaserOrb->SMA);
    // Большая полуось переходной орбиты
    double aTransfer = (ra + r1)/2.0;
    // уравнение Vis-viva
    double apogeeVel2 = sqrt(2 * mu / ra - mu / aTransfer);
    double dVa = (apogeeVel1 - apogeeVel2); // м/с

    // Расчет перигейного импульса
    double circularV = sqrt(mu / r1);
    double perigeeV = sqrt(2 * mu / r1 - mu / aTransfer);
    double dVp = (perigeeV - circularV); // м/с
    // 2. ------------------- ИМПУЛЬСЫ В АПОГЕЕ И ПЕРИГЕЕ, 2 ТИПА ------------
    const double dVmin = 5; // м/с
    ManeuverImpulse
            aImp = {.dV = dVa, .direct = MINUS_V},
            pImp = {.dV = dVp, .direct = MINUS_V};
    if(aImp.dV>dVmin){
        aImp.dV = dVmin;
        aImp.isPlaning = true;
    }
    else if(aImp.dV>0.1){
        aImp.isPlaning = true;
    }

    if(pImp.dV>dVmin && dVp>MAX_DV){
        pImp.dV = dVmin;
        pImp.isPlaning = true;
    }
    double rightNow = SimTime;//это модельное время
    static long phasePlaning = 0;
    if((aImp.isPlaning || pImp.isPlaning) && !phasePlaning){
        // 3. ----- ВЫБИРАЕМ ВРЕМЯ СТАРТА, ДЛИТЕЛЬНОСТЬ И ТИП ИМПУЛЬСА ----------
        if(chaserOrb->ecc<0.0008 && aImp.isPlaning){ // значит орбита круговая
            aImp.tStart = rightNow;
            pImp.isPlaning = false;
            return aImp;
        }
        else{
            double prevTa=0, postTa=0, Tp=0;
            Tp = chaserOrb->tp;
            double halfPeriod = chaserOrb->Period/2.0;
            prevTa = Tp - halfPeriod;
            postTa = Tp + halfPeriod;
            double nowT = DynTime;
            if(prevTa > nowT){//планируем апогейный, резерв - перигейный
                //здесь также д.б. модельное время, поэтому +rightNow
                aImp.tStart = (prevTa - DynTime)+rightNow;
                if(aImp.isPlaning){
                    pImp.isPlaning = false;
                }
                else{
                    pImp.tStart = aImp.tStart + halfPeriod;
                }
            }
            else if(prevTa < nowT && nowT < Tp){
                //планируем перигейный, резерв - апогейный
                pImp.tStart = (Tp - DynTime)+rightNow;
                if(pImp.isPlaning){
                    aImp.isPlaning = false;
                }
                else{
                    aImp.tStart = pImp.tStart + halfPeriod;
                }
            }
            else{//планируем апогейный, резерв - перигейный
                aImp.tStart = (postTa - nowT)+rightNow;
                if(aImp.isPlaning){
                    pImp.isPlaning = false;
                }
                else{
                    pImp.tStart = aImp.tStart + halfPeriod;
                }
            }
            if(aImp.isPlaning)
                return aImp;
            else
                return pImp;
        }
    }
    else{
        phasePlaning = 1;
        // 4. ---- УСЛОВИЕ, ЕСЛИ МЫ НА ЭЛ. ОРБИТЕ, БЛИЗКОЙ К ЦЕЛЕВОЙ ---------
        // 4.1 Расчёт разницы фаз двух КА: активного и пассивного
        double argLatO = targetOrb->ArgP + targetOrb->anom;
        double argPerA = chaserOrb->ArgP;
        if(argLatO<0)
            argLatO = 2*PI + argLatO;
        if(argPerA<0)
            argPerA = 2*PI + argPerA;

        double dArg = argPerA - argLatO;
        if(dArg<0)
            dArg = (2*PI + dArg);
        //4.2 Разница фаз в секундах
        double dt = targetOrb->Period*dArg/(2*PI);
        double upP=0, downP=0;
        //4.3 вычисляем на каком витке пассивный КА догонит активный в перигее
        int downN = calcUpDownPeriod(chaserOrb->Period, targetOrb->Period, dt,
                         &upP, &downP, 1000);
        //4.4 чтобы пассивный быстрее догнал активный, выбираем
        //либо орбиту чуть выше
        double upV   = addPerigeeVel(chaserOrb, upP);
        double downV = 0;
        //либо орбиту чуть ниже
        if(downP>0)
            downV = addPerigeeVel(chaserOrb, downP);
        double dtP = chaserOrb->tp - DynTime;
        //4.5 Кол-во витков за которые аппараты сблизятся д.б. меньше 10
        if(downN<10){
            if(upV<20.0){
                pImp.dV = upV;
                pImp.direct = PLUS_V;
                pImp.completed = false;
                pImp.tStart = dtP + rightNow;
            }
            else if(fabs(downV)<dVp && downV > 0){
                pImp.dV = downV;
                pImp.completed = false;
                pImp.tStart = dtP + rightNow;
            }
        }//иначе просто летаем без импульса и ждём
        else
            pImp.completed = true;
        return pImp;
    }
}

void thrControl(struct SCType *S){
    static struct OrbitType chaserOrb = {0};
    static struct OrbitType targetOrb = {0};
    static struct SCType *chaser;
    static struct SCType *target;
    static ManeuverImpulse actualMI = {0};
    static long isInit = false;
    if(!isInit){
        long iN=0;
        for(iN=0; iN<Nsc;iN++){
            if(strstr(SC[iN].Label, "KA")){
                chaser = &SC[iN];
                chaserOrb = Orb[iN];
                updateKepler(&chaserOrb, chaser);
            }
            else if(strstr(SC[iN].Label, "OO")){
                target = &SC[iN];
                targetOrb = Orb[iN];
                updateKepler(&targetOrb, target);
            }
        }
        actualMI = impulsPlaninig(&chaserOrb, &targetOrb);
        isInit = true;
    }
    else if(actualMI.completed){
        updateKepler(&chaserOrb, chaser);
        updateKepler(&targetOrb, target);
        actualMI = impulsPlaninig(&chaserOrb, &targetOrb);
    }

    // Обновление фазового угл
    //ctrl.phase_angle = limitePhaseAngle(chaserOrb.anom, targetOrb.anom);

    // Выполнение запланированных импульсов
    static double prevT = 0;
    struct AcType *AC = &S->AC;
    struct AcThrType *T = NULL;
    if(actualMI.direct == MINUS_V)
        T = &AC->Thr[0];
    else
        T = &AC->Thr[1];

    if (actualMI.tStart<SimTime && !actualMI.completed) {
        T->PulseWidthCmd = 5.0;
        double dt = SimTime - prevT;
        actualMI.dV = actualMI.dV - dt * T->Fmax/S->mass;
        if(actualMI.dV<0){
            actualMI.completed = true;
            T->PulseWidthCmd = 0.0;
        }
    }
    else {
        T->PulseWidthCmd = 0.0;
    }

    prevT = SimTime;
}
