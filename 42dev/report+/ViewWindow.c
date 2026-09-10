#include "Common.h"

extern double halfAngleView;
extern long isTargetDirCalculate;
extern long maxCntOut;

/*
 * https://ru.wikipedia.org/wiki/Решение_треугольников
 */
static long triangleSolution(double *    a, double     b, double     c,
                             double * alfa, double betta, double * gamma){
    double D = (c/b)*sin(betta);
    if(D>1 || (betta>=M_PI_2 && b<=c)){
        printf("triangleSolution: D=%f, betta=%f, b[=%f]<=c[=%f]\n",
               D,betta,b,c);
        return FALSE;
    }
    else{
        if(D==1.0)
            *gamma=M_PI_2;
        else
            *gamma=asin(D);
        *alfa=M_PI - betta - *gamma;
        *a=b*(sin(*alfa)/sin(betta));
        if(*a>c){
            *alfa=*gamma - betta;
            *a=b*(sin(*alfa)/sin(betta));
        }
        return TRUE;
    }
}

/*********** ViewWindow Calculate ***********/
static BOOL * isVisibleGoalBySc;
static BOOL * isVisibleGoalByScPrev;
static double * beginVwTime;
static double * endVwTime;

#define LABLE_SIZE (10)
#define DIR_SIZE (20)
typedef struct ViewWindows{
    char goalLabel[LABLE_SIZE];
    char scLabel[LABLE_SIZE];
    double begin;
    double end;
    uint8_t cntDir;
    double dirArray[DIR_SIZE][3]; //направление на цель в ОСК
}ViewWindows;
static ViewWindows * vw;

#define MAX_VW (100)
static uint8_t cntVw = 0;
static ViewWindows * vwArrayTotal[MAX_VW];
long * cntOut;

#ifdef _CJSON_PLUS_

#endif

long initViewWindow(void){
    unsigned int nCombination = (unsigned int) (Nsc*Ngnd);
    isVisibleGoalBySc     = (BOOL *)calloc(nCombination,sizeof(BOOL));
    isVisibleGoalByScPrev = (BOOL *)calloc(nCombination,sizeof(BOOL));
    beginVwTime     = (double *)calloc(nCombination,sizeof(double));
    cntOut = (long *)calloc(nCombination,sizeof(long));
    for(int i=0;i<nCombination;i++){
        cntOut[i] = maxCntOut;
    }
    vw = (ViewWindows *)calloc(nCombination,sizeof(ViewWindows));
    //переменная для контроля, вообще можно и без неё (!)
    endVwTime = (double *)calloc(nCombination,sizeof(double));

    halfAngleView = halfAngleView*D2R;
    return 0;
}

static void checkMoreTimeThan(double moreTime , double lessTime){
    if(lessTime>moreTime){
        printf("Error in Report+:checkMoreTimeThan()\n");
        exit(0);
    }
}

/**********************************************************************/
static void FindCON(double r[3], double v[3], double CON[3][3]){
    double L1[3],L2[3],L3[3],m,h[3],rr,hh;
    long i;

    h[0] = r[1]*v[2]-r[2]*v[1];
    h[1] = r[2]*v[0]-r[0]*v[2];
    h[2] = r[0]*v[1]-r[1]*v[0];
    rr = r[0]*r[0]+r[1]*r[1]+r[2]*r[2];
    hh = h[0]*h[0]+h[1]*h[1]+h[2]*h[2];

    for(i=0;i<3;i++) {
        L2[i] =  r[i];
        L3[i] = -h[i];
    }

    m=sqrt(rr);
    L2[0] /= m;
    L2[1] /= m;
    L2[2] /= m;

    if (hh == 0.0) { /* Rectlinear Motion */
        PerpBasis(L2,L1,L3);
    }
    else {
        m=sqrt(hh);
        L3[0] /= m;
        L3[1] /= m;
        L3[2] /= m;

        L1[0] = L2[1]*L3[2]-L2[2]*L3[1];
        L1[1] = L2[2]*L3[0]-L2[0]*L3[2];
        L1[2] = L2[0]*L3[1]-L2[1]*L3[0];

        m=sqrt(L1[0]*L1[0]+L1[1]*L1[1]+L1[2]*L1[2]);
        L1[0] /= m;
        L1[1] /= m;
        L1[2] /= m;
    }

    for(i=0;i<3;i++){
        CON[0][i] = L1[i];
        CON[1][i] = L2[i];
        CON[2][i] = L3[i];
    }
}

static void NEGV(double v[3]){
    v[0] = -v[0];
    v[1] = -v[1];
    v[2] = -v[2];
}

void calculateViewWindow(void){

    double lGoalToSc[3] = {0};
    double scPosW[3] = {0};
    double magL = 0;
    long num = 0;
    double alfa =0, gamma =0;
    double magLmax = 0;
    long Isc; long Ig;
    for(Isc=0;Isc<Nsc;Isc++)
        for(Ig=0;Ig<Ngnd;Ig++){
            MxV(World[EARTH].CWN,SC[Isc].PosN,scPosW);
            long i;
            for(i=0;i<3;i++) {
                lGoalToSc[i] = scPosW[i] - GroundStation[Ig].PosW[i];
            }

            magL = MAGV(lGoalToSc);
            num = findCombinationNum(Ig, Isc, scGsConnect);
            triangleSolution(&magLmax, World[EARTH].rad, MAGV(scPosW),
                                &alfa,    halfAngleView, &gamma);
            isVisibleGoalBySc[num]=FALSE;
            if(magL<=magLmax){
               isVisibleGoalBySc[num]=TRUE;
            }

            if(isVisibleGoalByScPrev[num] == FALSE && isVisibleGoalBySc[num] == TRUE){
                beginVwTime[num] = CivilTime;
                checkMoreTimeThan(beginVwTime[num], endVwTime[num]);
                vw[num].cntDir = 0;
                cntOut[num] = maxCntOut;
            }
            else if(isVisibleGoalByScPrev[num] == TRUE && isVisibleGoalBySc[num] == FALSE){
                endVwTime[num] = CivilTime;
                checkMoreTimeThan(endVwTime[num], beginVwTime[num]);
                strcpy(vw[num].goalLabel, GroundStation[Ig].Label);
                strcpy(vw[num].scLabel, SC[Isc].Label);
                vw[num].begin = beginVwTime[num]+SEC_2000_1970;
                vw[num].end   = endVwTime[num]+SEC_2000_1970;
                vwArrayTotal[cntVw] = (ViewWindows *)calloc(1,sizeof(ViewWindows));
               *vwArrayTotal[cntVw] = vw[num];
                cntVw++;
            }
            else if(isTargetDirCalculate &&
                    isVisibleGoalByScPrev[num] && isVisibleGoalBySc[num]){
                if(cntOut[num] >= maxCntOut){
                    static char frameName = 'L';
                    double CN[3][3] = {0};
                    double tDir[3] = {0};
                    double lGoalToScN[3] = {0};
                    if(frameName == 'O'){
                        FindCON(SC[Isc].PosN,SC[Isc].VelN, CN);
                    }
                    else{
                        double wln[3] = {0};
                        FindCLN(SC[Isc].PosN,SC[Isc].VelN, CN,wln);
                    }
                    MTxV(World[EARTH].CWN, lGoalToSc, lGoalToScN);
                    MxV(CN, lGoalToScN, tDir);
                    UNITV(tDir);
                    NEGV(tDir);
                    uint8_t cntDir = vw[num].cntDir >= (DIR_SIZE-1) ? DIR_SIZE-1 : vw[num].cntDir;
                    CopyUnitV(tDir, vw[num].dirArray[cntDir]);
                    cntDir++;
                    vw[num].cntDir = cntDir;
                    cntOut[num] = 0;
                }
                else
                    cntOut[num]++;
            }//if(isTargetDirCalculate
            isVisibleGoalByScPrev[num]=isVisibleGoalBySc[num];
        }
}

#ifdef _CJSON_PLUS_
void outputViewWindow(void){
    cJSON * jsonVwArray = cJSON_CreateArray();
    for(int i=0; i<cntVw; i++){
        cJSON * jsonVw = cJSON_CreateObject();
        cJSON_AddStringToObject(jsonVw, "goalLabel" , vwArrayTotal[i]->goalLabel);
        cJSON_AddStringToObject(jsonVw, "scLabel"   , vwArrayTotal[i]->scLabel);
        cJSON_AddNumberToObject(jsonVw, "begin"     , vwArrayTotal[i]->begin);
        cJSON_AddNumberToObject(jsonVw, "end"       , vwArrayTotal[i]->end);
        if(isTargetDirCalculate){
            cJSON_AddNumberToObject(jsonVw, "cntDir", vwArrayTotal[i]->cntDir);
            cJSON * jsonDirArray = cJSON_AddArrayToObject(jsonVw, "dirArray");
            for(int j=0; j<vwArrayTotal[i]->cntDir; j++){
                cJSON * jsonDir = cJSON_CreateObject();
                cJSON_AddNumberToObject(jsonDir, "x", vwArrayTotal[i]->dirArray[j][0]);
                cJSON_AddNumberToObject(jsonDir, "y", vwArrayTotal[i]->dirArray[j][1]);
                cJSON_AddNumberToObject(jsonDir, "z", vwArrayTotal[i]->dirArray[j][2]);
                cJSON_AddItemToArray(jsonDirArray,jsonDir);
            }
        }
        cJSON_AddItemToArray(jsonVwArray,jsonVw);
    }
    outputConsolFile(jsonVwArray,"csgViewWindow");
}
#endif
