#ifndef PS_MODEL_H
#define PS_MODEL_H

#ifdef __cplusplus
extern "C"
{
#endif

void   InitPSModel(struct SCType *S);
long        PSModel(struct SCType *S);
double getThrsFuel(struct SCType *S, long i);

#ifdef __cplusplus
}
#endif

#endif // SPS_H
