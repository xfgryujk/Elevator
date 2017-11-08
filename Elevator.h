#ifndef __ELEVATOR_H__
#define __ELEVATOR_H__

#include "SPCE061A.h"


// --------------------------------------------------------
// Types
// --------------------------------------------------------

typedef unsigned int BOOL;
#define FALSE 0
#define TRUE 1

#define NULL ((void*)0)


// --------------------------------------------------------
// Definations for I/O port
// --------------------------------------------------------

typedef struct IOA
{
    BOOL goto1 : 1;
    BOOL goto2 : 1;
    BOOL goto3 : 1;
    BOOL goto4 : 1;
    
    BOOL floor1Up : 1;
    BOOL floor2Up : 1;
    BOOL floor2Down : 1;
    BOOL floor3Up : 1;
    BOOL floor3Down : 1;
    BOOL floor4Down : 1;
    
    BOOL floor1Arrived : 1;
    BOOL floor2Arrived : 1;
    BOOL floor3Arrived : 1;
    BOOL floor4Arrived : 1;
    
    BOOL openDoor : 1;
    BOOL closeDoor : 1;
} IOA;

typedef struct IOB
{
    BOOL goUp : 1;
    BOOL elevatorRun : 1;
    
    BOOL openDoor : 1;
    BOOL doorRun : 1;
    
    BOOL floor1Up : 1;
    BOOL floor2Up : 1;
    BOOL floor2Down : 1;
    BOOL floor3Up : 1;
    BOOL floor3Down : 1;
    BOOL floor4Down : 1;
    
    BOOL doorOpened : 1;
    BOOL doorClosed : 1;
    
    unsigned int curFloor : 3;
    
    BOOL preserved : 1;
} IOB;

static volatile IOA* const g_IOA = (IOA*)P_IOA_Data;
static volatile IOB* const g_IOB = (IOB*)P_IOB_Data;


// --------------------------------------------------------
// Constants
// --------------------------------------------------------

#define MIN_FLOOR 1
#define MAX_FLOOR 4


// --------------------------------------------------------
// Functions
// --------------------------------------------------------

void ElevatorInit();
void ElevatorMainLoop();

#endif
