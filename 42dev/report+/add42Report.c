#include "42.h"

static FILE **GpsFiles;
static long Isc;

void extReportInit(){
    GpsFiles = (FILE **) calloc(Nsc,sizeof(FILE *));
    char s[40] = {0};
    char Fmt[40] = {0};
    for(Isc=0;Isc<Nsc;Isc++) {
        if (SC[Isc].Exists) {
            if (Nsc == 1)
                sprintf(Fmt,"");
            else if (Nsc <= 10)
                sprintf(Fmt,"%1ld",Isc);
            else
                sprintf(Fmt,"%02ld",Isc);

            sprintf(s,"gpsSc%s.42",Fmt);
            if (SC[Isc].Ngps > 0){
                GpsFiles[Isc] = FileOpen(InOutPath,s,"w");
                fprintf(GpsFiles[Isc],"Lat, Rad  Lng, Rad  Alt,m \n");
            }
            else
                printf("SC[%ld] have't gps\n", Isc);
        }
    }
}

void extReportExec(){
      if (OutFlag) {
         for(Isc=0;Isc<Nsc;Isc++) {
            if (SC[Isc].Exists) {
               if (SC[Isc].Ngps > 0) {
                  fprintf(GpsFiles[Isc],"%le %le %le\n",
                     SC[Isc].GPS[0].Lat, SC[Isc].GPS[0].Lng, SC[Isc].GPS[0].Alt);
               }
            }
         }
      }
}

void extReportComplite(){
    for(Isc=0;Isc<Nsc;Isc++) {
        if (SC[Isc].Exists) {
            if (SC[Isc].Ngps > 0)
                fclose(GpsFiles[Isc]);
        }
    }
    return;

}

