/*    This file is distributed with 42,                               */
/*    the (mostly harmless) spacecraft dynamics simulation            */
/*    created by Eric Stoneking of NASA Goddard Space Flight Center   */

/*    Copyright 2010 United States Government                         */
/*    as represented by the Administrator                             */
/*    of the National Aeronautics and Space Administration.           */

/*    No copyright is claimed in the United States                    */
/*    under Title 17, U.S. Code.                                      */

/*    All Other Rights Reserved.                                      */


#include "42.h"
#include "shHeaders.h"

void AcFsw(struct AcType *AC);
void WriteToSocket(SOCKET Socket, char **Prefix, long Nprefix, long EchoEnabled);

/* #ifdef __cplusplus
** namespace _42 {
** using namespace Kit;
** #endif
*/
/**********************************************************************/
/*  This function is called at the simulation rate.  Sub-sampling of  */
/*  control loops should be done on a case-by-case basis.             */
/*  Mode handling, command generation, error determination, feedback  */
/*  and failure detection and correction all fall within the scope of */
/*  this file.                                                        */
/**********************************************************************/
extern void EasySunMode(struct SCType *S);
extern void EasyLvlhMode(struct SCType *S);
extern void PointOrientation(struct SCType *S);
extern void NadirMode(struct SCType *S);
extern void MapCmdsToActuators(struct SCType *S);
extern void thrControl(struct SCType *S);
void shFlightSoftWare(struct SCType *S)
{
      #ifdef _AC_STANDALONE_
      struct IpcType *I;
      long Iipc;
      #endif
            
      S->FswSampleCounter++;
      if (S->FswSampleCounter >= S->FswMaxCounter) {
         S->FswSampleCounter = 0;
         
         switch(S->FswTag){
            case PASSIVE_FSW:
               break;
            case CFS_FSW:
               AcFsw(&S->AC);
               break;
            case SUN_FSW:
                EasySunMode(S);
                break;
            case LVLH_FSW:
                EasyLvlhMode(S);
                break;
            case POINT_FSW:
                PointOrientation(S);
                break;
            case NADIR_FSW:
                NadirMode(S);
                break;
            case THR_FSW:
                EasyLvlhMode(S);
                if(SimTime>100.0){
                    thrControl(S);
                }
                break;
         }

      }
      MapCmdsToActuators(S);
}

/* #ifdef __cplusplus
** }
** #endif
*/

