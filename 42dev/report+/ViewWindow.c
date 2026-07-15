#include "Common.h"

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
typedef struct ViewWindows{

}ViewWindows;
#ifdef _CJSON_PLUS_
static cJSON * vwArray;
#endif
static double halfAngleView;

long initViewWindow(void){
    unsigned int nCombination = (unsigned int) (Nsc*Ngnd);
    isVisibleGoalBySc     = (BOOL *)calloc(nCombination,sizeof(BOOL));
    isVisibleGoalByScPrev = (BOOL *)calloc(nCombination,sizeof(BOOL));
    beginVwTime     = (double *)calloc(nCombination,sizeof(double));
    //переменная для контроля, вообще можно и без неё (!)
    endVwTime = (double *)calloc(nCombination,sizeof(double));

    halfAngleView = 35.0*D2R;
    #ifdef _CJSON_PLUS_
    vwArray = cJSON_CreateArray();
    #endif
    return 0;
}

static void checkMoreTimeThan(double moreTime , double lessTime){
    if(lessTime>moreTime){
        printf("Error in calculateViewWindow(...)\n");
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
                checkMoreTimeThan(endVwTime[num], beginVwTime[num]);
            }
            else if(isVisibleGoalByScPrev[num] == TRUE && isVisibleGoalBySc[num] == FALSE){
                endVwTime[num] = CivilTime;
                checkMoreTimeThan(beginVwTime[num], endVwTime[num]);

                #ifdef _CJSON_PLUS_
                cJSON * vw = cJSON_CreateObject();
                cJSON_AddStringToObject(vw, "goalLabel", GroundStation[Ig].Label);
                cJSON_AddStringToObject(vw, "scLabel"  , SC[Isc].Label);
                cJSON_AddNumberToObject(vw, "begin", beginVwTime[num]+SEC_2000_1970);
                cJSON_AddNumberToObject(vw, "end", endVwTime[num]+SEC_2000_1970);
                cJSON_AddItemToArray(vwArray,vw);
                #endif
            }
            else if(isVisibleGoalByScPrev[num] && isVisibleGoalBySc[num]){
                double CON[3][3] = {0};
                FindCON(SC[Isc].PosN,SC[Isc].VelN, CON);
                double gDir[3] = {0};
                MxV(CON, lGoalToSc, gDir);
                NEGV(gDir);
            }

            isVisibleGoalByScPrev[num]=isVisibleGoalBySc[num];
        }
}

#ifdef _CJSON_PLUS_
void outputViewWindow(void){
    outputConsolFile(vwArray,"csgViewWindow");
}
#endif
