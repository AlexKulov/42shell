#ifndef SPS_H
#define SPS_H


#ifdef __cplusplus
extern "C"
{
#endif

void InitSpsModel(char * ScLabel);
int SpsModel(struct SCType *S);
long getSpsSoc(struct SCType *S, float * batSOC);
long getSpsCosAlfa(struct SCType *S, float * cosAlf);

#ifdef __cplusplus
}
#endif

#endif // SPS_H
