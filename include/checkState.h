#ifndef CHECKSTATE_H
#define CHECKSTATE_H

#include "functions.h"
#include "defs.h"

static float base_altitude;

// check state functions start
int checkPrelaunch(float s)
{
  // determines that the rocket is in prelaunch and hasn't taken off
  if (s == 0)
  {
    return 0;
  }
  return 0;
}
int checkInflight(float s, float t)
{
  // detects that the rocket is in flight
  if (s > 5 && t > 0 && t < 3)
  {
    return 1;
  }
  return 0;
}
int checkCoasting(float t, float v)
{
  // detects that burn out has occured and the rocket is coasting
  if (t > 3 && v > 1)
  {
    return 2;
  }
  return 1;
}
int checkApogee(float v)
{
  // detects that apogee has been achieved and ejection of parachute should take place
  if (v > -1 && v < 1)
  {
    ejection();
    return 3;
  }
  return 2;
}
int checkDescent(float v, float s)
{
  // detects descent of the rocket after parachute ejection
  if (v < -1 && s > 5)
  {
    return 4;
  }
  return 3;
}
int checkGround(float v, float s)
{
  // detects landing of the rocket
  // TODO: please review  s might be greater than 0
  if (v == 0 && s == 0)
  {
    return 5;
  }
  return 4;
}
// checks the current state of the rocket
int checkState(float s, float v, float t, int state)
{
  int rval;
  s = s - base_altitude;
  switch (state)
  {
  default:
    rval = checkPrelaunch(s);
    break;
  case 0:
    // if the rocket is in state 0, it checks for lift off by monitoring state 1
    rval = checkInflight(s, t);
    break;
  case 1:
    // while in state 1 we check for burnout by monitoring state 2
    rval = checkCoasting(t, v);
    break;
  case 2:
    // we check for apogee for parachute deployment by monitoring state 3
    rval = checkApogee(v);
    break;
  case 3:
    // we check for descent by monitoring state 4
    rval = checkDescent(v, s);
    break;
  case 4:
    // we check for thud by monitoring state 5
    rval = checkGround(v, s);
    break;
  }
  return rval;
}

#endif