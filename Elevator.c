#include "Elevator.h"


// --------------------------------------------------------
// Forward declarations & Global variables
// --------------------------------------------------------

static void UpdateWaiting();
static void UpdateOpeningDoor();
static void UpdateClosingDoor();
static void OnArriveTargetFloor(int curFloor, BOOL thenGoUp);
static void UpdateGoingUp();
static void UpdateGoingDown();

// Called on each tick
static void (*g_elevatorUpdate)() = UpdateWaiting;

static BOOL g_goto1 = FALSE, g_goto2 = FALSE, 
	g_goto3 = FALSE, g_goto4 = FALSE;

// In ms
static int g_timeToWaitForPeople = 0;


// --------------------------------------------------------
// Helper functions
// --------------------------------------------------------

static inline void FeedTheDog()
{
    *P_Watchdog_Clear = 0x0001;
}


// Returns 0 if not found
static int FindCurFloor()
{
    if (g_IOA->floor1Arrived) return 1;
    if (g_IOA->floor2Arrived) return 2;
    if (g_IOA->floor3Arrived) return 3;
    if (g_IOA->floor4Arrived) return 4;
    return 0;
}

// Returns 0 if not found
static int FindTargetFloorUp(int curFloor, BOOL* thenGoUp)
{
    if (curFloor <= 1 && (g_goto1 || g_IOB->floor1Up))
    {
        if (thenGoUp != NULL) *thenGoUp = TRUE;
        return 1;
    }
    if (curFloor <= 2 && (g_goto2 || g_IOB->floor2Up))
    {
        if (thenGoUp != NULL) *thenGoUp = TRUE;
        return 2;
    }
    if (curFloor <= 3 && (g_goto3 || g_IOB->floor3Up))
    {
        if (thenGoUp != NULL) *thenGoUp = TRUE;
        return 3;
    }
    if (curFloor <= 4 && (g_goto4 || g_IOB->floor4Down))
    {
        if (thenGoUp != NULL) *thenGoUp = FALSE;
        return 4;
    }
    if (curFloor <= 3 && g_IOB->floor3Down)
    {
        if (thenGoUp != NULL) *thenGoUp = FALSE;
        return 3;
    }
    if (curFloor <= 2 && g_IOB->floor2Down)
    {
        if (thenGoUp != NULL) *thenGoUp = FALSE;
        return 2;
    }
    return 0;
}

// Returns 0 if not found
static int FindTargetFloorDown(int curFloor, BOOL* thenGoUp)
{
    if (curFloor >= 4 && (g_goto4 || g_IOB->floor4Down))
    {
        if (thenGoUp != NULL) *thenGoUp = FALSE;
        return 4;
    }
    if (curFloor >= 3 && (g_goto3 || g_IOB->floor3Down))
    {
        if (thenGoUp != NULL) *thenGoUp = FALSE;
        return 3;
    }
    if (curFloor >= 2 && (g_goto2 || g_IOB->floor2Down))
    {
        if (thenGoUp != NULL) *thenGoUp = FALSE;
        return 2;
    }
    if (curFloor >= 1 && (g_goto1 || g_IOB->floor1Up))
    {
        if (thenGoUp != NULL) *thenGoUp = TRUE;
        return 1;
    }
    if (curFloor >= 2 && g_IOB->floor2Up)
    {
        if (thenGoUp != NULL) *thenGoUp = TRUE;
        return 2;
    }
    if (curFloor >= 3 && g_IOB->floor3Up)
    {
        if (thenGoUp != NULL) *thenGoUp = TRUE;
        return 3;
    }
    return 0;
}


static inline void ElevatorGoUp()
{
    g_IOB->goUp = TRUE;
    g_IOB->elevatorRun = TRUE;
    g_elevatorUpdate = UpdateGoingUp;
}

static inline void ElevatorGoDown()
{
    g_IOB->goUp = FALSE;
    g_IOB->elevatorRun = TRUE;
    g_elevatorUpdate = UpdateGoingDown;
}

static inline void ElevatorStop()
{
    g_IOB->elevatorRun = FALSE;
    g_elevatorUpdate = UpdateWaiting;
}

static inline void ElevatorOpenDoor(int timeToWaitForPeople)
{
    g_IOB->openDoor = TRUE;
    g_IOB->doorRun = TRUE;
    g_elevatorUpdate = UpdateOpeningDoor;
    g_timeToWaitForPeople = timeToWaitForPeople;
}

static inline void ElevatorCloseDoor()
{
    g_IOB->openDoor = FALSE;
    g_IOB->doorRun = TRUE;
    g_elevatorUpdate = UpdateClosingDoor;
    g_timeToWaitForPeople = 0;
}


// --------------------------------------------------------
// Elevator main logic
// --------------------------------------------------------

void ElevatorInit()
{
    // Initialize I/O ports
    
    *P_IOA_Dir = 0;
    *P_IOA_Attrib = 0;
    *P_IOA_Data = 0;
    *P_IOB_Dir = 0xF3FF;
    *P_IOB_Attrib = 0xF3FF;
    *P_IOB_Data = 0;
    
    // Initialize current floor
    
    while ((g_IOB->curFloor = FindCurFloor()) == 0)
    {
        // Goto a certain floor
        ElevatorGoDown();
        
        FeedTheDog();
    }
    ElevatorStop();
    
    ElevatorOpenDoor(0);
}

static void UpdateLights()
{
    int curFloor;
	
	// Current floor
    
    curFloor = FindCurFloor();
    if (curFloor != 0)
        g_IOB->curFloor = curFloor;
	
	// Internal buttons
	
	if (curFloor != 1) g_goto1 = g_goto1 || g_IOA->goto1;
	if (curFloor != 2) g_goto2 = g_goto2 || g_IOA->goto2;
	if (curFloor != 3) g_goto3 = g_goto3 || g_IOA->goto3;
	if (curFloor != 4) g_goto4 = g_goto4 || g_IOA->goto4;
	
	// External buttons
	
    if (g_IOA->floor1Up)   g_IOB->floor1Up   = TRUE;
    if (g_IOA->floor2Up)   g_IOB->floor2Up   = TRUE;
    if (g_IOA->floor2Down) g_IOB->floor2Down = TRUE;
    if (g_IOA->floor3Up)   g_IOB->floor3Up   = TRUE;
    if (g_IOA->floor3Down) g_IOB->floor3Down = TRUE;
    if (g_IOA->floor4Down) g_IOB->floor4Down = TRUE;
}

void ElevatorMainLoop()
{
    while (TRUE)
    {
        UpdateLights();
        g_elevatorUpdate();
        
        FeedTheDog();
    }
}


// --------------------------------------------------------
// Elevator update functions
// --------------------------------------------------------

static void UpdateWaiting()
{
	int i;
	
    int curFloor;
    int targetFloor;
    BOOL thenGoUp;
	
    // Elevator should arrive a certain floor now
    //ASSERT(FindCurFloor() != 0);
    
    
    // Open or close door
    
    if (g_IOA->openDoor)
    {
        ElevatorOpenDoor(1000);
        return;
    }
    if (g_IOA->closeDoor)
    {
        ElevatorCloseDoor();
        return;
    }
	
	// Wait for people
	
	if (g_timeToWaitForPeople > 0)
    {
		--g_timeToWaitForPeople;
        // Delay for about 1 ms
		for (i = 0; i <= 80; ++i);
        return;
    }
    
    // Find target floor
    
    curFloor = g_IOB->curFloor;
    targetFloor = 0;
    thenGoUp = FALSE;
    
    if (g_IOB->goUp) // Determine order
    {
        if ((targetFloor = FindTargetFloorUp(curFloor, &thenGoUp)) == 0)
            targetFloor = FindTargetFloorDown(curFloor, &thenGoUp);
    }
    else
    {
        if ((targetFloor = FindTargetFloorDown(curFloor, &thenGoUp)) == 0)
            targetFloor = FindTargetFloorUp(curFloor, &thenGoUp);
    }
    
    if (targetFloor != 0) // Found target floor
    {
		if (curFloor == targetFloor) // No need to go
		{
			OnArriveTargetFloor(curFloor, thenGoUp);
			return;
		}
		
        if (g_IOB->doorOpened)
            ElevatorCloseDoor();
        else
        {
            if (curFloor < targetFloor)
                ElevatorGoUp();
            else
                ElevatorGoDown();
        } 
    }
}

static void UpdateOpeningDoor()
{
    if (g_IOA->closeDoor)
    {
        ElevatorCloseDoor();
        return;
    }
    
    if (g_IOB->doorOpened)
    {
        g_IOB->doorRun = FALSE;
        g_elevatorUpdate = UpdateWaiting;
    }
}

static void UpdateClosingDoor()
{
    if (g_IOA->openDoor)
    {
        ElevatorOpenDoor(1000);
        return;
    }
    
    if (g_IOB->doorClosed)
    {
        g_IOB->doorRun = FALSE;
        g_elevatorUpdate = UpdateWaiting;
    }
}

static void OnArriveTargetFloor(int curFloor, BOOL thenGoUp)
{
    // Stop
    
    ElevatorStop();
    ElevatorOpenDoor(3000);
    g_IOB->goUp = thenGoUp;
    
    
    // Turn off light
    
    switch (curFloor)
    {
    case 1:
		g_goto1 = FALSE;
        g_IOB->floor1Up = FALSE;
        break;
        
    case 2:
		g_goto2 = FALSE;
        if (thenGoUp)
            g_IOB->floor2Up = FALSE;
        else
            g_IOB->floor2Down = FALSE;
        break;
        
    case 3:
		g_goto3 = FALSE;
        if (thenGoUp)
            g_IOB->floor3Up = FALSE;
        else
            g_IOB->floor3Down = FALSE;
        break;
        
    case 4:
		g_goto4 = FALSE;
        g_IOB->floor4Down = FALSE;
        break;
    }
}

static void UpdateGoingUp()
{
	int curFloor;
	BOOL thenGoUp;
	
    curFloor = FindCurFloor();
    if (curFloor == 0) // Between 2 floors
        return;
    
    thenGoUp = TRUE;
    if (FindTargetFloorUp(curFloor, &thenGoUp) == curFloor
        || curFloor == MAX_FLOOR) // Arrive target floor
        OnArriveTargetFloor(curFloor, thenGoUp);
}

static void UpdateGoingDown()
{
	int curFloor;
	BOOL thenGoUp;
	
    curFloor = FindCurFloor();
    if (curFloor == 0) // Between 2 floors
        return;
    
    thenGoUp = FALSE;
    if (FindTargetFloorDown(curFloor, &thenGoUp) == curFloor
        || curFloor == MIN_FLOOR) // Arrive target floor
        OnArriveTargetFloor(curFloor, thenGoUp);
}
