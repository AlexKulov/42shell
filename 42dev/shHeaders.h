#ifndef __SH_HEADERS_H__
#define __SH_HEADERS_H__
#include <stdio.h>

/* Time Modes */
#define SLEEP_TIME 4

/*Model mode*/
#define DYN_ONE_BODY (33)

/*FSW tag*/
#define PROTOTYPE_FSW (330)
#define AD_HOC_FSW (331)
#define SUN_FSW (332)
#define LVLH_FSW (333)
#define POINT_FSW (334)
#define NADIR_FSW (335)
#define THR_LVLH_FSW (336)
#define RANDEZVOUS_FSW (337)
#define NADIR_SUN_FSW (338)

/* Modes for InterProcess Comm */
#define IPC_CAN 9

/*42dynamics func*/
typedef struct SCType SCType;
extern void OneBodyRK4(struct SCType *S);
extern void PolyhedronCowellRK4(struct SCType *S);
extern void FixedOrbitPosition(struct SCType *S);
extern void EulHillRK4(struct SCType *S);
extern void CowellRK4(struct SCType *S);
extern void EnckeRK4(struct SCType *S);
extern void ThreeBodyEnckeRK4(struct SCType *S);

#ifdef __cplusplus
extern "C"{
#endif

/*iokit func*/
extern FILE *FileRead(const char *Path, const char *File);
/*math common func*/
extern void CopyV(double V[3], double W[3]);

#ifdef __cplusplus
}
#endif

#endif /* __SH_HEADERS_H__ */
