#ifndef SPS_MODEL_H
#define SPS_MODEL_H

#ifdef __cplusplus
extern "C"
{
#endif

void InitSPSModel(char * ScLabel);
int SPSModel(struct SCType *S);
long getSpsSoc(struct SCType *S, float * batSOC);
long getSpsCosAlfa(struct SCType *S, float * cosAlf);

#ifdef __cplusplus
}
#endif

#endif // SPS_H
