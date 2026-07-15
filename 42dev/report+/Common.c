#include "Common.h"

long CombTransDec(long a, long b, long N){
   if(a>=b || a>=N || b>=N)
       return EXIT_FAILURE;

   long A = 0;
   long i;
   for(i=1;i<=a;i++)
       A = A + (N-i);
   return A + (b-a-1);
}

//Важно! Если scGsConnect, то первый элемент - это наземный объект!
long findCombinationNum(long Iobj1, long Iobj2, connectType type){
    if(type == scGsConnect){
        //Iobj1 = Ig
        return Iobj1*Nsc + Iobj2;
    }
    else{ //scScPlan
        return CombTransDec(Iobj1,Iobj2,Nsc);
    }
}

#ifdef _CJSON_PLUS_
static char fileName[128];
void outputConsolFile(cJSON * array, const char * arrayName){
    cJSON * outArray = cJSON_CreateObject();
    cJSON_AddItemToObject(outArray, arrayName, array);
    char * jsonPrint = cJSON_Print(outArray);
    printf(jsonPrint);
    printf("\n");
    sprintf(fileName,"%s.json",arrayName);
    FILE * file = FileOpen(InOutPath, fileName, "w");
    fprintf(file, jsonPrint);
    cJSON_Delete(outArray);
    fclose(file);
    return;
}
#endif
