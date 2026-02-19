#ifndef __SH_HEADERS_H__
#define __SH_HEADERS_H__
#include <stdio.h>
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

/*42dynamics func*/
typedef struct SCType SCType;
extern void OneBodyRK4(struct SCType *S);
extern void PolyhedronCowellRK4(struct SCType *S);
extern void FixedOrbitPosition(struct SCType *S);
extern void EulHillRK4(struct SCType *S);
extern void CowellRK4(struct SCType *S);
extern void EnckeRK4(struct SCType *S);
extern void ThreeBodyEnckeRK4(struct SCType *S);

/*iokit func*/
extern FILE *FileRead(const char *Path, const char *File);

#endif /* __SH_HEADERS_H__ */
