#ifndef USERFN_H
#define USERFN_H


void baseturnPID(double target);
void basemovePID(double target);

void basemovement(double distance, int speed);
void baseturn(int left, int speed);

void  tray_control(void*) ;
void  arm_control(void*) ;
#endif