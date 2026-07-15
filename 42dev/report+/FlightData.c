#include "Common.h"

/*********** FlightData Calculate ***********/
/*
 * definers same in SMAO
 * FlightData same in SMAO + nSun/nEcl
 */
#define SMAO_STRING_SIZE (16)
#define MAX_REV (48)
typedef struct FlightData{
    uint8_t nRev;
    uint8_t nSun;
    uint8_t nEcl;
    double revTime[MAX_REV]; // время начала витка
    double sunTime[MAX_REV]; // время начала участка солнца
    double eclTime[MAX_REV]; // время начала теневого участка
} FlightData;

static FlightData * flDataArray;
static long * prevEclips;
static double * prevRevAng;
static unsigned int fdNsc = 0;
void initFlightData(void){
    if(Norb!=Nsc){
        printf("I can not accumulate FlightData\n"
              "Norb=%ld , Nsc=%ld. InOutPath=%s\n",
               Norb,      Nsc,     InOutPath);
        return;
    }
    fdNsc = (unsigned int)Nsc;
    flDataArray = (FlightData *)calloc(fdNsc,sizeof(FlightData));
    prevRevAng= (double *)calloc(fdNsc,sizeof(double));
    prevEclips = (long *)calloc(fdNsc,sizeof(long));
    for(unsigned int i=0;i<fdNsc;i++){
        if(SC[i].Eclipse){
            flDataArray[i].eclTime[0] = CivilTime;
            flDataArray[i].nEcl++;
        }
        else{
            flDataArray[i].sunTime[0] = CivilTime;
            flDataArray[i].nSun++;
        }
        prevEclips[i]=SC[i].Eclipse;
        prevRevAng[i]=Orb[i].anom-Orb[i].ArgP;
    }
}
static void checkRevSize(uint8_t nRev){
    if(nRev>=MAX_REV){
        printf("Error! nRev achive to MAX_REV(=%i)\n", MAX_REV);
        exit(1);
    }
}

void calculateFlightData(void){
    uint8_t iRev=0;
    uint8_t iSun=0;
    uint8_t iEcl=0;
    double revAng=0;
    for(unsigned int i=0;i<fdNsc;i++){
        iRev = flDataArray[i].nRev;
        iSun = flDataArray[i].nSun;
        iEcl = flDataArray[i].nEcl;
        revAng = Orb[i].anom-Orb[i].ArgP;
        if(prevEclips[i]>SC[i].Eclipse){
            flDataArray[i].sunTime[iSun] = CivilTime;
            flDataArray[i].nSun++;
            checkRevSize(flDataArray[i].nSun);
        }
        else if(prevEclips[i]<SC[i].Eclipse){
            flDataArray[i].eclTime[iEcl] = CivilTime;
            flDataArray[i].nEcl++;
            checkRevSize(flDataArray[i].nEcl);
        }
        prevEclips[i]=SC[i].Eclipse;
        if(prevRevAng[i]<0 && revAng >= 0){
            flDataArray[i].revTime[iRev] = CivilTime;
            flDataArray[i].nRev++;
            checkRevSize(flDataArray[i].nRev);
        }
        prevRevAng[i] = revAng;
    }
}

#ifdef _CJSON_PLUS_
void outputFlightData(void){
    FlightData * fd;
    FILE * fdFile = NULL;
    if(fdNsc)
        fdFile = FileOpen(InOutPath,"CsgFlightData.json","w");
    for(unsigned int i=0;i<fdNsc;i++){
        cJSON *note = cJSON_CreateObject();
        cJSON *label = cJSON_CreateString(SC[i].Label);
        cJSON *flData = cJSON_CreateObject();
        cJSON_AddItemToObject(note, "scLabel", label);
        cJSON_AddItemToObject(note, "flightData", flData);

        fd = &flDataArray[i];
        //добавляем время окончания моделирования
        if(SC[i].Eclipse){
            fd->nEcl++;
            checkRevSize(fd->nEcl);
            fd->eclTime[fd->nEcl-1]=CivilTime-DTSIM;
        }
        else{
            fd->nSun++;
            checkRevSize(fd->nSun);
            fd->sunTime[fd->nSun-1]=CivilTime-DTSIM;
        }
        //если 1 переход через витковый угол=>2 витка и тд
        cJSON * nRev = cJSON_CreateNumber(fd->nRev+1);
        cJSON_AddItemToObject(flData, "nRev", nRev);
        int t=0;
        for(t=0;t<fd->nRev;t++)
            fd->revTime[t]=fd->revTime[t]+SEC_2000_1970;
        cJSON * revTime = cJSON_CreateDoubleArray(fd->revTime,
                                                  fd->nRev);
        cJSON_AddItemToObject(flData, "revTime", revTime);

        for(t=0;t<fd->nSun;t++)
            fd->sunTime[t]=fd->sunTime[t]+SEC_2000_1970;
        cJSON * sunTime = cJSON_CreateDoubleArray(fd->sunTime,
                                                  fd->nSun);
        cJSON_AddItemToObject(flData, "sunTime", sunTime);

        for(t=0;t<fd->nEcl;t++)
            fd->eclTime[t]=fd->eclTime[t]+SEC_2000_1970;
        cJSON * eclTime = cJSON_CreateDoubleArray(fd->eclTime,
                                                  fd->nEcl);
        cJSON_AddItemToObject(flData, "eclTime", eclTime);

        char * jsonRes = cJSON_Print(note);
        printf("%s\n",jsonRes);
        fprintf(fdFile,"%s\n",jsonRes);
        cJSON_Delete(note);
    }
    if(fdFile)
        fclose(fdFile);
    return;
}
#endif
