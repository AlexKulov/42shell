#include "Common.h"

extern void initFlightData(void);
extern void calculateFlightData(void);

extern long initViewWindow(void);
extern void calculateViewWindow(void);

/*extern long initScGsContact(void);
extern void calculateScGsContact(void);

extern long initScScContact(void);
extern void calculateScScContact(void);*/


/*
 * Интерфейc с прекотом 42
 */
static long isFlightDataCalculate  = FALSE;
static long isViewWindowCalculate  = FALSE;
double halfAngleView = 0;
long isTargetDirCalculate  = FALSE;
static double outTime = 0;
long maxCntOut = 0;

static long isScGsPlanContactCalculate  = FALSE;
static long isScScPlanContactCalculate  = FALSE;

extern FILE *FileRead(const char *Path, const char *File);
void initExtReport(void){
    FILE * csgFlagFile = FileRead(InOutPath,"Report+.txt");
    printf("\n============== ExtREPORT+42 =============\n");
    printf(" Include report+ calculate module: \n");
    if(csgFlagFile == NULL){
        printf("But haven't Report+.txt file.\n");
        return;
    }
    char junk[120],newline;
    fscanf(csgFlagFile,"%[^\n] %[\n]",junk,&newline);
    fscanf(csgFlagFile,"%[^\n] %[\n]",junk,&newline);
    char response[120];
    fscanf(csgFlagFile,"%s %[^\n] %[\n]",response,junk,&newline);
    isFlightDataCalculate = DecodeString(response);
    printf("Flag Flight Data calculate: %s\n",response);

    fscanf(csgFlagFile,"%s %[^\n] %[\n]",response,junk,&newline);
    isViewWindowCalculate = DecodeString(response);
    fscanf(csgFlagFile,"%lf %s %lf %[^\n] %[\n]", &halfAngleView,
           response, &outTime,junk,&newline);
    isTargetDirCalculate = DecodeString(response);
    if (outTime < DTSIM){
        printf("Warning! initExtReport: outTime < DTSIM\n");
        maxCntOut = 1;
    }
    else{
        maxCntOut = (long) (outTime/DTSIM+0.5);
    }
    printf("Flag View Window calculate: %s\n",response);


    fscanf(csgFlagFile,"%s %[^\n] %[\n]",response,junk,&newline);
    isScGsPlanContactCalculate = DecodeString(response);
    printf("Flag Sc to Gs Plan Contact calculate: %s\n",response);

    fscanf(csgFlagFile,"%s %[^\n] %[\n]",response,junk,&newline);
    isScScPlanContactCalculate = DecodeString(response);
    printf("Flag Sc to Sc Plan Contact calculate: %s\n",response);

    if(isFlightDataCalculate)
        initFlightData();
    if(isViewWindowCalculate)
        initViewWindow();
    /*if(isScGsPlanContactCalculate)
        initScGsContact();
    if(isScScPlanContactCalculate)
        initScScContact();*/
    printf("============== ExtREPORT+42 =============\n");
}

void ExtReport(void){
    if(isFlightDataCalculate)
        calculateFlightData();
    if(isViewWindowCalculate)
        calculateViewWindow();
    /*if(isScGsPlanContactCalculate)
        calculateScGsContact();
    if(isScScPlanContactCalculate)
        calculateScScContact();*/
}

#ifdef _CJSON_PLUS_
extern void outputFlightData(void);
extern void outputViewWindow(void);
//extern void outputScGsContact(void);
//extern void outputScScContact(void);

void ExtReportToFile(void){
    if(isFlightDataCalculate)
        outputFlightData();
    if(isViewWindowCalculate)
        outputViewWindow();
    /*if(isScGsPlanContactCalculate)
        outputScGsContact();
    if(isScScPlanContactCalculate)
        outputScScContact();*/
}
#endif
