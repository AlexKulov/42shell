#include "csg42Common.h"

#define MOUNT_EVEREST 8848
#define MAX_TIME_INTER 100
/***************  Common Function ***************/
static long Combination2(long n){
    long res = 0;
    if(n<2)
        return EXIT_FAILURE;
    long i;
    res = 1;
    for(i=n-1; i<=n; i++)
        res *=i;
    res = res/2;
    return res;
}

void NEGV(double V[3]){
    V[0] = -V[0];
    V[1] = -V[1];
    V[2] = -V[2];
}

void memoryPlanContactToJson(connectType type,
                             BOOL isVisiblePrev, BOOL isVisible,
                             double * beginTime, double * endTime,
                             long num1, long num2,
                             cJSON * contactArray){
    //4. Если признак сменился с FALSE на TRUE
    if(isVisiblePrev == FALSE && isVisible == TRUE){
        //4.1. Тогда заполняем время начала контакта
        *beginTime = CivilTime;
        if(*endTime>*beginTime){
            printf("Error in memoryPlanContactToJson(...)\n");
            exit(0);
        }
    }
    //5. Если признак сменился с TRUE на FALSE
    else if(isVisiblePrev == TRUE && isVisible == FALSE){
        //5.1. Заполняем время окончания контакта
        *endTime = CivilTime;
        if(*endTime<*beginTime){
            printf("Error in memoryPlanContactToJson(...)\n");
            exit(0);
        }
        //5.2. Записываем контакт в JSON вид
        cJSON * contact = cJSON_CreateObject();
        if(type == scGsConnect){
            cJSON_AddStringToObject(contact, "gsLabel", GroundStation[num1].Label);
            cJSON_AddStringToObject(contact, "scLabel", SC[num2].Label);
        }
        else{//scScPlan
            cJSON_AddStringToObject(contact, "scLable1", SC[num1].Label);
            cJSON_AddStringToObject(contact, "scLable2", SC[num2].Label);
        }
        cJSON_AddNumberToObject(contact, "begin", *beginTime+SEC_2000_1970);
        cJSON_AddNumberToObject(contact, "end", *endTime+SEC_2000_1970);
        cJSON_AddItemToArray(contactArray,contact);
    }
}

enum XYZ{
    X=0,
    Y=1,
    Z=2
};
static void calculateVisibleAngle(double PosSc1RefSc2N[3],
                                  struct SCType *SC1, struct SCType *SC2,
                                  double * acosVisAngleSc1, double * acosVisAngleSc2){
    //double PosSc2B1[3];
    //MxV(SC1->B[0].CN,PosSc2RefSc1N,PosSc2B1);
    //double acosVisAngle = acos(PosSc2B1,любой вектор в ССК);
    double directClnSc1[3] = {SC1->CLN[Y][0],
                              SC1->CLN[Y][1],
                              SC1->CLN[Y][2]};
    UNITV(PosSc1RefSc2N);
    double PosSc2RefSc1[3] = {-PosSc1RefSc2N[0],
                             -PosSc1RefSc2N[1],
                             -PosSc1RefSc2N[2]};
    *acosVisAngleSc1 = acos(VoV(PosSc2RefSc1, directClnSc1)) * R2D;

    double directClnSc2[3] = {SC2->CLN[Y][0],
                              SC2->CLN[Y][1],
                              SC2->CLN[Y][2]};
    *acosVisAngleSc2 = acos(VoV(PosSc1RefSc2N, directClnSc2)) * R2D;

}

static void memoryAngleVisibleToJson(BOOL isVisiblePrev, BOOL isVisible,
                             double beginTime, double endTime,
                             long num1, long num2, cJSON * contactAngleArray,
                             double PosSc2RefSc1N[3], cJSON ** angle1Array, cJSON ** angle2Array){

    //1. Если признак вошёл в контакт - создаём массив углов
    if(isVisiblePrev == FALSE && isVisible == TRUE){
        *angle1Array = cJSON_CreateArray();
        *angle2Array = cJSON_CreateArray();
    }
    //2. Если аппарат в зоне видимости - считаем углы и записываем в массив
    if(isVisible == TRUE){
        double iDblTime = (CivilTime - beginTime)/DTSIM;
        long iTime = (long)iDblTime;
        /*//------------------------ TIME TEST ----------------------------------
        double iLongDblTime = (double)iTime;
        double errorT = iDblTime-iLongDblTime;
        if(fabs(errorT)>0){
            printf("double errorT = iDblTime-iLongDblTime = %lf\n", errorT);
            printf("iDblTime = %lf, iLongDblTime = %lf\n", iDblTime, iLongDblTime);
        }
        *///---------------------------------------------------------------------
        static long calcCntFilter = 6; // углы считаются раз в (calcCntFilter*DTSIM) секунд
        long calcCnt = iTime%calcCntFilter;
        if(calcCnt == 0){
            double sc1Angle=0, sc2Angle=0;
            calculateVisibleAngle(PosSc2RefSc1N, &SC[num1], &SC[num2], &sc1Angle, &sc2Angle);
            cJSON * angle1 = cJSON_CreateNumber(sc1Angle);
            cJSON_AddItemToArray(*angle1Array, angle1);
            cJSON * angle2 = cJSON_CreateNumber(sc2Angle);
            cJSON_AddItemToArray(*angle2Array, angle2);
        }
    }
    //3. Если аппарат вышел из контакта - создаём контакт в JSON виде
    if(isVisiblePrev == TRUE && isVisible == FALSE){
        cJSON * contact = cJSON_CreateObject();
        cJSON_AddStringToObject(contact, "scLable1", SC[num1].Label);
        cJSON_AddStringToObject(contact, "scLable2", SC[num2].Label);
        cJSON_AddNumberToObject(contact, "begin", beginTime+SEC_2000_1970);
        cJSON_AddNumberToObject(contact, "end", endTime+SEC_2000_1970);
        cJSON_AddItemToObject  (contact, "angleSc1DirectSc2", *angle1Array);
        cJSON_AddItemToObject  (contact, "angleSc2DirectSc1", *angle2Array);
        cJSON_AddItemToArray(contactAngleArray,contact);
        //-----------------------------------------------
        *angle1Array = NULL;
        *angle2Array = NULL;
    }
}

static void checkProgressContact(connectType type,
                                double * beginTime, double * endTime,
                                cJSON * contactArray){
    long Nobj1=0, Nobj2=0;
    if(type == scGsConnect){
        Nobj1 = Ngnd;
        Nobj2 = Nsc;
    }
    else{
        Nobj1 = Nsc-1;
        Nobj2 = Nsc;
    }
    long num = 0;
    for(long Iobj1=0;Iobj1<Nobj1;Iobj1++)
        for(long Iobj2=0;Iobj2<Nobj2;Iobj2++){
            if(type == scGsConnect ){
                //num = (Iobj1+1)*(Iobj2+1)-1;
                num = findCombinationNum(Iobj1, Iobj2, scGsConnect);
                if(beginTime[num]>endTime[num]){
                    memoryPlanContactToJson(scGsConnect, TRUE, FALSE, &beginTime[num], &endTime[num],
                                            Iobj1, Iobj2, contactArray); //Iobj1 - GS
                }
            }
            else if(type == scScConnect && Iobj1<Iobj2){
                //num = CombTransDec(Iobj1,Iobj2,Nsc);
                num = findCombinationNum(Iobj1,Iobj2, scScConnect);
                if(beginTime[num]>endTime[num]){
                    memoryPlanContactToJson(scScConnect, TRUE, FALSE, &beginTime[num], &endTime[num],
                                            Iobj1, Iobj2, contactArray); //Iobj1 - SC1
                }
            }
        }
}

static void checkProgressScScAngle(double * beginTime, cJSON * contactAngleArray,
                                   cJSON ** angle1Array, cJSON ** angle2Array){
    long Nobj1=0, Nobj2=0;
    Nobj1 = Nsc-1;
    Nobj2 = Nsc;
    long num = 0;
    for(long Iobj1=0;Iobj1<Nobj1;Iobj1++)
        for(long Iobj2=0;Iobj2<Nobj2;Iobj2++){
            if(Iobj1<Iobj2){
                num = CombTransDec(Iobj1,Iobj2,Nsc);
                if(angle1Array[num]){
                    double lScToSc[3]={0};
                    for(long i=0;i<3;i++) {
                        lScToSc[i] = SC[Iobj1].PosN[i] - SC[Iobj2].PosN[i];
                    }
                    memoryAngleVisibleToJson(TRUE, FALSE, beginTime[num], CivilTime,
                                             Iobj1, Iobj2, contactAngleArray, lScToSc,
                                             &angle1Array[num], &angle2Array[num]);
                }
            }
        }
}
/************* SC - SC Plan Contact *************/
static BOOL * isVisibleScBySc;
static BOOL * isVisibleScByScPrev;
static double * beginPcScScTime;
static double * endPcScScTime;
static cJSON * scScContactArray;
static cJSON * scScContactAngleArray;
static cJSON ** sc1Sc2AngleArray;
static cJSON ** sc2Sc1AngleArray;
static double scScDmax; // расстояние между КА в метрах
static double scScMaxHorizont;
static long isUncoditionDmax;

long initScScContact(void){
    scScDmax = 4000000; //макс. расстояние КА-КА

    unsigned int nCombination = (unsigned int) Combination2(Nsc);
    isVisibleScBySc     = (BOOL *)calloc(nCombination,sizeof(BOOL));
    isVisibleScByScPrev = (BOOL *)calloc(nCombination,sizeof(BOOL));
    beginPcScScTime     = (double *)calloc(nCombination,sizeof(double));
    endPcScScTime = (double *)calloc(nCombination,sizeof(double));
    unsigned int i=0;
    for(i=0;i<nCombination;i++){
        isVisibleScBySc[i] = FALSE;
        isVisibleScByScPrev[i] = FALSE;
    }
    //Проверка на "макс. горизонт"
    double minRmin = Orb[0].rmin;
    for(long Iorb=1;Iorb<Norb;Iorb++){
        if(minRmin > Orb[Iorb].rmin)
            minRmin = Orb[Iorb].rmin;
    }
    double earthRad = World[EARTH].rad;
    double halfMaxHorizont = sqrt(minRmin*minRmin - earthRad*earthRad);
    scScMaxHorizont = 2* halfMaxHorizont;
    isUncoditionDmax = FALSE;
    if(scScMaxHorizont>scScDmax){
        isUncoditionDmax = TRUE;
    }

    scScContactArray = cJSON_CreateArray();
    scScContactAngleArray = cJSON_CreateArray();
    sc1Sc2AngleArray = (cJSON **)calloc(nCombination,sizeof(cJSON *));
    sc2Sc1AngleArray = (cJSON **)calloc(nCombination,sizeof(cJSON *));
    return 0;
}

void calculateScScContact(void){
    long Isc1=0;
    long Isc2=0;
    double lScToSc[3];
    double magL = 0;
    long num = 0;

    for(Isc1=0;Isc1<Nsc-1;Isc1++)
        for(Isc2=Isc1+1;Isc2<Nsc;Isc2++){
            //1. Вычисляем вектор L от КА2 к КА1
            long i;
            for(i=0;i<3;i++) {
                lScToSc[i] = SC[Isc1].PosN[i] - SC[Isc2].PosN[i];
            }
            //2. Вычисляем модуль вектора Л
            magL = MAGV(lScToSc);
            //num = CombTransDec(Isc1,Isc2,Nsc);
            num = findCombinationNum(Isc1,Isc2, scScConnect);

            isVisibleScBySc[num]=FALSE;
            //3. Определяем признак контакта КА-КА
            if(magL<scScDmax){
                if(isUncoditionDmax || magL<scScMaxHorizont){
                    isVisibleScBySc[num]=TRUE;
                }
                else{
                    double magPosSc1 = MAGV(SC[Isc1].PosN);
                    double alfa1 = VoV(SC[Isc1].PosN,lScToSc)/(magL*magPosSc1);
                    if(alfa1>M_PI/2){
                        isVisibleScBySc[num]=TRUE;
                    }
                    else{
                        double magPosSc2 = MAGV(SC[Isc2].PosN);
                        NEGV(lScToSc);
                        double alfa2 = VoV(SC[Isc1].PosN,lScToSc)/(magL*magPosSc2);
                        if(alfa2>M_PI/2){
                            isVisibleScBySc[num]=TRUE;
                        }
                        else{
                            double H = sin(alfa2)*magPosSc2;
                            if(H > World[EARTH].rad + MOUNT_EVEREST){
                                isVisibleScBySc[num]=TRUE;
                            }
                        }
                    }
                }
            }
            //4. Если признак сменился с FALSE на TRUE то запись beginPcScScTime
            //5. Если признак сменился с TRUE на FALSE то запись в JSON
            memoryPlanContactToJson(scScConnect, isVisibleScByScPrev[num], isVisibleScBySc[num],
                                       &beginPcScScTime[num],  &endPcScScTime[num],
                                        Isc1, Isc2, scScContactArray);
            //6. записываем JSON-контакт с узлами
            memoryAngleVisibleToJson(isVisibleScByScPrev[num], isVisibleScBySc[num],
                                       beginPcScScTime[num],  endPcScScTime[num],
                                    Isc1, Isc2, scScContactAngleArray, lScToSc,
                                     &sc1Sc2AngleArray[num], &sc2Sc1AngleArray[num]);

            isVisibleScByScPrev[num]=isVisibleScBySc[num];
        }
}

void outputScScContact(void){
    //1. проверка незавершённых контактов
    checkProgressContact(scScConnect,
                         beginPcScScTime, endPcScScTime,
                         scScContactArray);
    checkProgressScScAngle(beginPcScScTime, scScContactAngleArray,
                           sc1Sc2AngleArray, sc2Sc1AngleArray);
    //2. вывод в файл и/или в консоль
    outputConsolFile(scScContactArray     , "csgScScContact"     );
    outputConsolFile(scScContactAngleArray, "csgScScContactAngle");
}
/************* SC - GS Plan Contact *************/

static BOOL * isVisibleScByGs;
static BOOL * isVisibleScByGsPrev;
static double * beginPcScGsTime;
static double * endPcScGsTime;
static cJSON * scGsContactArray;
static double horizontGsAngleRad; // угол видимости над горизонтом
static double scGsDmax; // расстояние между КА и НП в метрах

long initScGsContact(void){
    scGsDmax = 8000000;//макс. расстояние КА-НП в м.,
    horizontGsAngleRad = 7.5*D2R; //15.0*D2R угол КА над местным горизонтом

    unsigned int nCombination = (unsigned int) (Nsc*Ngnd);
    isVisibleScByGs     = (BOOL *)calloc(nCombination,sizeof(BOOL));
    isVisibleScByGsPrev = (BOOL *)calloc(nCombination,sizeof(BOOL));
    beginPcScGsTime     = (double *)calloc(nCombination,sizeof(double));
    endPcScGsTime = (double *)calloc(nCombination,sizeof(double));
    unsigned int i=0;
    for(i=0;i<nCombination;i++){
        isVisibleScByGs[i] = FALSE;
        isVisibleScByGsPrev[i] = FALSE;
    }
    scGsContactArray = cJSON_CreateArray();
    return 0;
}

void calculateScGsContact(void){
    long Ig;
    long Isc;
    double lScToGs[3];
    double scPosW[3];
    double magL = 0;
    long num = 0;
    for(Ig=0;Ig<Ngnd;Ig++ )
        for(Isc=0;Isc<Nsc;Isc++){
            MxV(World[EARTH].CWN,SC[Isc].PosN,scPosW);
            long i;
            for(i=0;i<3;i++) {
                lScToGs[i] = GroundStation[Ig].PosW[i] - scPosW[i];
            }

            magL = MAGV(lScToGs);
            //num = (Isc+1)*(Ig+1)-1;
            num = findCombinationNum(Ig, Isc, scGsConnect);

            isVisibleScByGs[num]=FALSE;
            if(magL<scGsDmax){
                double magGS = MAGV(GroundStation[Ig].PosW);
                double fi = acos(VoV(lScToGs,GroundStation[Ig].PosW)/(magL*magGS));
                if(fi>(M_PI_2 + horizontGsAngleRad))
                    isVisibleScByGs[num]=TRUE;
            }
            //4. Если признак сменился с FALSE на TRUE то запись beginPcScGsTime
            //5. Если признак сменился с TRUE на FALSE то запись в JSON
            memoryPlanContactToJson(scGsConnect, isVisibleScByGsPrev[num], isVisibleScByGs[num],
                                       &beginPcScGsTime[num],  &endPcScGsTime[num],
                                        Ig, Isc, scGsContactArray);

            isVisibleScByGsPrev[num]=isVisibleScByGs[num];
        }
}

void outputScGsContact(void){
    //1. проверка незавершённых контактов
    checkProgressContact(scGsConnect, beginPcScGsTime,
                         endPcScGsTime, scGsContactArray);
    //2. вывод в файл и/или в консоль
    outputConsolFile(scGsContactArray,"csgScGsContact");
}
