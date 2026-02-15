/* Created by Kulakov Aleksandr, russ69@bk.ru
 * This file contains the naive supply power system of SC.
 * This file is compatible with 42.
 * https://github.com/ericstoneking/42
 * But have specific git repo
 * https://github.com/AlexKulov/42shell
 */

#include "42.h"
#include "SPSModel.h"
extern FILE *FileRead(const char *Path, const char *File);

#define MONTH_SEC (2592000) //30 дней
#define SEC_IN_HOUR (3600)

#define PASSIVE_MODE_COMSUMPTION (30.0) //10 Watt
#define SUN_MODE_COMSUMPTION (30.0) //10 Watt
#define EARTH_MODE_COMSUMPTION (30.0) //10 Watt
#define PAYLOAD_MODE_COMSUMPTION (30.0) //10 Watt
#define MTB_MAX_COMSUMPTION (4.0) //10 Watt

struct SollarPanelType{

    double Area;
    double COP;
    double MaxTemperatureCoef;
    double MaxRadiationCoef;
    double MaxUFCoef;
    double ElectrizationCoef;
    double OccupancyCoef;
    double ResCoef;

    long TurnTag;
    double VecB0[3];//Nominal position SolPnl in B frame
    double VecBt[3];//Position SolPnl in B frame if TurnTag is true
    double CosAlf;
    double MaxCosAlf;
};

struct BatteryType{

    double SOC; //% Charge
    double NomCap; // Amper*hour
    double CurCap; // Amper*hour
    double DOD; //%
    double TimeLife; //sec
    double Discharge; // % per month
    double MaxIcharge;
    double MaxIdischarge;
    unsigned int MaxCicle;
    unsigned int CurCicle;
};

typedef struct SupplyPowerSystemType{
    char ScLabel[40];
    long Nsp;   /* Number of sollar panels */
    long Nbt;   /* Number of sollar battery */
    struct SollarPanelType * SolPnl;
    struct BatteryType * Bat;

    double OutPt; // sensors and actuators power compsumption
    //--------------------------Определение Входящей мощности-------------------------
    double SunRad; // Wt/m2
    double InPt;//Input Power InPt = SunRad * ResaultCoef * Area * cos(Alf)
    double Unom; //Nominal voltage for battary and sollar panel
    //---------------------------- Общая эенргия за всё время -------------------------
    double TotalInEn;
    double TotalOutEn;

} SupplyPowerSystemType;

static void TurnSolarPanel(double * vset, double * vcur)
{
  double vset_func[3];
  CopyUnitV(vset,vset_func);

   vcur[0] = vset_func[0];
   vcur[1] = vset_func[1];
   vcur[2] = vset_func[2];
}

static void SPDegradation(double * k, double t, double tmax)
{
    *k = 1.0 - t/(10.0*tmax);
}

/*static void CalcUI(double Pt, double *I, double * U)
{
    //U(I)=35 - 4/(12-I), где 12 - максимальный ток, а 35 - макс напряжение
    //решаем уравнение a*U^2-b*U+c=0 :
    double b = Pt + 416.0;
    *U = (b-sqrt(b*b-1680.0*Pt))/24.0;
    *I = Pt / *U;
}
static void CalcUIAb(double Pt, double *I, double * U)
{
    // для СБ U(I) = -7*I/(3+I) + 27, где 27 - максимальное напряжение
    double b = 81.0-Pt;
    *U = (b+sqrt(b*b+240.0*Pt))/6.0;
    *I = Pt / *U;
}*/

static SupplyPowerSystemType * SPS[32];
static long Nsps;
void InitSPSModel(char * ScLabel){
    char junk[120],newline;
    char spsFileName[40];
    sprintf(spsFileName, "SPS_%s.txt", ScLabel);
    FILE * spsFile=FileRead(InOutPath, spsFileName);
    if(spsFile){
        long Is;
        for(Is=0;Is<Nsps;Is++){
            if(!strcmp(SPS[Is]->ScLabel, ScLabel)){
               return;
            }
        }
        if(Nsps >= 32){
            printf("Number init SPSs more then 32\n");
            exit(1);
        }
        SPS[Nsps] = (SupplyPowerSystemType *) calloc(1,sizeof(SupplyPowerSystemType));
        SupplyPowerSystemType * SPSyst = SPS[Nsps++];
        fscanf(spsFile,"%[^\n] %[\n]",junk,&newline);
        fscanf(spsFile,"%[^\n] %[\n]",junk,&newline);
        fscanf(spsFile,"%[^\n] %[\n]",junk,&newline);
        fscanf(spsFile,"\"%[^\"]\" %[^\n] %[\n]",SPSyst->ScLabel,junk,&newline);
        fscanf(spsFile,"%lf %[^\n] %[\n]",&SPSyst->SunRad,junk,&newline);
        fscanf(spsFile,"%lf %[^\n] %[\n]",&SPSyst->Unom,junk,&newline);
        fscanf(spsFile,"%[^\n] %[\n]",junk,&newline);
        fscanf(spsFile,"%ld %[^\n] %[\n]",&SPSyst->Nsp,junk,&newline);
        long Ie=0;
        char response[40] = {0};
        if(SPSyst->Nsp == 0){
            printf("Error number of panel\n");
            exit(1);
        }
        else{
            SPSyst->SolPnl = (struct SollarPanelType *)
                    calloc(SPSyst->Nsp,sizeof(struct SollarPanelType));
            for(Ie=0;Ie<SPSyst->Nsp;Ie++){
                fscanf(spsFile,"%[^\n] %[\n]",junk,&newline);
                fscanf(spsFile,"%lf %[^\n] %[\n]",&SPSyst->SolPnl[Ie].Area,junk,&newline);
                fscanf(spsFile,"%lf %[^\n] %[\n]",&SPSyst->SolPnl[Ie].ResCoef,junk,&newline);
                fscanf(spsFile,"%lf %lf %lf %[^\n] %[\n]",
                   &SPSyst->SolPnl[Ie].VecB0[0],
                   &SPSyst->SolPnl[Ie].VecB0[1],
                   &SPSyst->SolPnl[Ie].VecB0[2],junk,&newline);
                UNITV(SPSyst->SolPnl[Ie].VecB0);
                double alfa = 0;
                fscanf(spsFile,"%lf %[^\n] %[\n]",&alfa,junk,&newline);
                SPSyst->SolPnl[Ie].MaxCosAlf = cos((90.0-alfa)*M_PI/180);
                fscanf(spsFile,"%s %[^\n] %[\n]",response,junk,&newline);
                SPSyst->SolPnl[Ie].TurnTag=DecodeString(response);
                fscanf(spsFile,"%lf %[^\n] %[\n]",&alfa,junk,&newline);
                fscanf(spsFile,"%lf %lf %lf %[^\n] %[\n]",
                   &SPSyst->SolPnl[Ie].VecBt[0],
                   &SPSyst->SolPnl[Ie].VecBt[1],
                   &SPSyst->SolPnl[Ie].VecBt[2],junk,&newline);
                UNITV(SPSyst->SolPnl[Ie].VecBt);
            }
        }

        fscanf(spsFile,"%[^\n] %[\n]",junk,&newline);
        fscanf(spsFile,"%ld %[^\n] %[\n]",&SPSyst->Nbt,junk,&newline);
        if(SPSyst->Nbt == 0){
            printf("Error number of battery\n");
            exit(1);
        }
        else{
            SPSyst->Bat = (struct BatteryType *)
                    calloc(SPSyst->Nbt,sizeof(struct BatteryType));
            for(Ie=0;Ie<SPSyst->Nbt;Ie++){
                fscanf(spsFile,"%[^\n] %[\n]",junk,&newline);
                fscanf(spsFile,"%lf %[^\n] %[\n]",&SPSyst->Bat[Ie].NomCap,junk,&newline);
                fscanf(spsFile,"%lf %[^\n] %[\n]",&SPSyst->Bat[Ie].CurCap,junk,&newline);
                fscanf(spsFile,"%lf %[^\n] %[\n]",&SPSyst->Bat[Ie].Discharge,junk,&newline);
                fscanf(spsFile,"%lf %[^\n] %[\n]",&SPSyst->Bat[Ie].MaxIcharge,junk,&newline);
                fscanf(spsFile,"%lf %[^\n] %[\n]",&SPSyst->Bat[Ie].MaxIdischarge,junk,&newline);
            }
        }
        fclose(spsFile);
    }
}

/**********************************************************************/
/*  This function modeling simplest power supply system of SC         */
/*  This function is called at the simulation rate.                   */
static SupplyPowerSystemType * getScSps(struct SCType *S){
    for(long Is=0;Is<Nsps;Is++){
        if(!strcmp(SPS[Is]->ScLabel, S->Label)){
            return SPS[Is];
        }
    }
    return NULL;
}
int SPSModel(struct SCType *S){
    //----------------- Выбираем соответствующую СЭП
    SupplyPowerSystemType * SPSyst = getScSps(S);//S->SPS;
    if(SPSyst == NULL)
        return 0;

    long Ib;
    for(Ib=0;Ib<SPSyst->Nbt;Ib++){
        //double dAbCt = SPSyst->AbC - SPSyst->AbCt;
    }

    SPSyst->InPt = 0.0;
    double FabsCosAlf = 0.0;
    for(long Ip=0;Ip<SPSyst->Nsp;Ip++){
        struct SollarPanelType * SolPnl = &SPSyst->SolPnl[Ip];
        //------ Поворачиваем панель, если поворачивается ----------
        if(SolPnl->TurnTag){
            // TurnSolarPanel(AC->spvb, SPSyst->VecSPinBt);
        }
        else{
            CopyUnitV(SolPnl->VecB0, SolPnl->VecBt);
        }

        SolPnl->CosAlf=VoV(SolPnl->VecBt,S->svb);
        // если угол меньше 75 гр, то считаем dKt и соответственно Pin
        if(SolPnl->CosAlf>SolPnl->MaxCosAlf && S->Eclipse == FALSE) {
            //SPDegradation(&SPSyst->dKt, SimTime, STOPTIME);
            SPSyst->InPt += SPSyst->SunRad * SolPnl->ResCoef * SolPnl->Area * SolPnl->CosAlf;
        }
    }

    SPSyst->OutPt = 0.0;
    //if(S->FswTag == PASSIVE_FSW)
    SPSyst->OutPt = PASSIVE_MODE_COMSUMPTION;
    long i;
    /*************** Sensor consumption ***********/
    /************** Actuator consumption **********/
    for(i=0;i<S->Nw;i++)
        SPSyst->OutPt = SPSyst->OutPt + fabs(S->Whl[i].w * S->Whl[i].Trq);

    for(i=0;i<S->Nmtb;i++)
        SPSyst->OutPt = SPSyst->OutPt + MTB_MAX_COMSUMPTION*(S->MTB[i].M/S->MTB[i].Mmax);

    SPSyst->TotalInEn += SPSyst->InPt;
    SPSyst->TotalOutEn += SPSyst->OutPt;
    double dP = SPSyst->InPt - SPSyst->OutPt;
    double dE = dP*DTSIM; //Wt*sec
    //-------------------- Batterys modeling ------------------------
    for(Ib=0;Ib<SPSyst->Nbt;Ib++){
        struct BatteryType * Bat = &SPSyst->Bat[Ib];
        Bat->SOC = Bat->CurCap/Bat->NomCap;
        if(dE< 0.0 &&  (-dE/SEC_IN_HOUR)>(Bat->CurCap * SPSyst->Unom)){ // Battery is over
            //SPSyst->InPt = 0.0; //??????
            Bat->CurCap = 0.0;
        }
        else{
            double dEb = 0;
            if(dP>Bat->MaxIcharge*SPSyst->Unom)
                dEb = Bat->MaxIcharge*SPSyst->Unom*DTSIM; // Charge with max I
            dE -= dEb;

            if (dEb> 0.0 && (dEb/SEC_IN_HOUR+Bat->CurCap * SPSyst->Unom)> Bat->NomCap * SPSyst->Unom)
                Bat->CurCap = Bat->NomCap; // Battery is full
            else//Charge or discharge battery
                Bat->CurCap = Bat->CurCap + dE/(SPSyst->Unom * SEC_IN_HOUR);
            Bat->CurCap = Bat->CurCap - Bat->NomCap * Bat->Discharge * DTSIM/MONTH_SEC;
        }
    }
    return 0;
}

long getSpsSoc(struct SCType *S, float * batSOC){
    SupplyPowerSystemType * SPSyst = getScSps(S);
    if(SPSyst){
        for(long i=0;i<SPSyst->Nbt;i++){
            batSOC[i] =  SPSyst->Bat[i].SOC;
        }
        return SPSyst->Nbt;
    }
    else{
        return 0;
    }
}

long getSpsCosAlfa(struct SCType *S, float * cosAlf){
    SupplyPowerSystemType * SPSyst = getScSps(S);
    if(SPSyst){
        for(long i=0;i<SPSyst->Nsp;i++){
            cosAlf[i] =  SPSyst->SolPnl[i].CosAlf;
        }
        return SPSyst->Nsp;
    }
    else{
        return 0;
    }
}
