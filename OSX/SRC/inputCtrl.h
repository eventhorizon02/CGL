// input control


#ifndef __inputCtrl__
#define __inputCtrl__

#include <stdio.h>

enum Events { enterEv, gridEv, quitEv, zoomEv, startEv,delayEv, stepEv, clearEv, noEv };
enum States { mvLeftSt, mvRightSt, mvUpSt, mvDownSt, noSt };

typedef enum {false,true} bool;

typedef enum Events inputEvent;
typedef enum States inputStates;

void initCtrl();
inputEvent getEvent(); // which event happended
bool isState(inputStates); // is a state

#endif /* __inputCtrl__ */
