#ifndef CHECKSTATE_H
#define CHECKSTATE_H
#include "../include/functions.h"
//checks the current state of the rocket
int checkState(int s,int v,int t,int state){
  int rval;
  switch(state){
    default:
        rval=checkPrelaunch(s);
        break;
    case 0:
    //if the rocket is in state 0, it checks for lift off by monitoring state 1
        rval=checkInflight(s,t);
        break;
    case 1:
    // while in state 1 we check for burnout by monitoring state 2
         rval=checkCoasting(t,v);
    break;
    case 2:
    // we check for apogee for parachute deployment by monitoring state 3
         rval=checkApogee(v);
    break;
    case 3:
    // we check for descent by monitoring state 4
         rval=checkDescent(v,s);
    break;
    case 4:
    //we check for thud by monitoring state 5
        rval=checkGround(v,s);
    break;
  }
  return rval;
}
#endif