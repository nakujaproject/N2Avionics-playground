#ifndef CHECKSTATE_H
#define CHECKSTATE_H

//checks the current state of the rocket
int checkState(int s, int v, int t, int state)
{
  int rval;
  switch (state)
  {
  default:
    rval = checkPrelaunch(s);
    break;
  case 0:
    //if the rocket is in state 0, it checks for lift off by monitoring state 1
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
    //we check for thud by monitoring state 5
    rval = checkGround(v, s);
    break;
  }
  return rval;
}

//check state functions start
int checkPrelaunch(int s)
{
  // determines that the rocket is in prelaunch and hasn't taken off
  if (s == 0)
  {
    return 0;
  }
}
int checkInflight(int s, int t)
{
  //detects that the rocket is in flight
  if (s > 5 && t > 0 && t < 3)
  {
    return 1;
  }
}
int checkCoasting(int t, int v)
{
  //detects that burn out has occured and the rocket is coasting
  if (t > 3 && v > 1)
  {
    return 2;
  }
}
int checkApogee(int v)
{
  //detects that apogee has been achieved and ejection of parachute should take place
  if (v > -1 && v < 1)
  {
    return 3;
  }
}
int checkDescent(int v, int s)
{
  //detects descent of the rocket after parachute ejection
  if (v < -1 && s > 5)
  {
    return 4;
  }
}
int checkGround(int v, int s)
{
  //detects landing of the rocket
  if (v == 0 && s == 0)
  {
    return 5;
  }
#endif