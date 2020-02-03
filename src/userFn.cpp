#include "main.h"
#include "cmath"
#include "motor.hpp"
#include "MiniPID.h"


void basemovePID(double target) {
  MiniPID pid=MiniPID(0.3,0,0.1);
  char mytext[100];
  lv_obj_t * txt = lv_label_create(lv_scr_act(), NULL);

  pid.setOutputLimits(-80,80);
  pid.setOutputRampRate(5);
  double start=left_front.get_position();
  // convert the target in inch to tick
  double targetTick = (target*900)/(4*M_PI)+start;
  while (fabs(left_front.get_position()-targetTick)>10) {
    double output=pid.getOutput(left_front.get_position(),
        targetTick);
    left_back.move(output);
    left_front.move(output);
    right_back.move(output);
    right_front.move(output);

    printf("base start %8.2f, target %8.2f, base %8.2f\n", start, targetTick,left_front.get_position());
    sprintf(mytext, "base start %8.2f\n, target %8.2f\n, base %8.2f\n, output  base %8.2f\n",
            start, targetTick,left_front.get_position(), output
         );
    lv_label_set_text(txt, mytext);
    pros::delay(10);
  }
}

void baseturnPID(double target) {
  MiniPID pid=MiniPID(0.3,0,0.1);
  char mytext[100];
  lv_obj_t * txt = lv_label_create(lv_scr_act(), NULL);

  pid.setOutputLimits(-50,50);
  pid.setOutputRampRate(5);
  double start=gyro.get_value();
  double turn = target*10+start;
  while (fabs(gyro.get_value()-turn)>3) {
    double output=pid.getOutput(gyro.get_value(),
        turn);
    left_back.move(output);
    left_front.move(output);
    right_back.move(-output);
    right_front.move(-output);
    lv_label_set_text(txt, NULL);
    printf("base start %8.2f, target %8.2f, gyro %8.2f\n", start, turn,gyro.get_value());
    sprintf(mytext, "\n\n\n\n\ngyro start %8.2f\n, target %8.2f\n, gyro %8.2f\n", start, turn,gyro.get_value()
         );
    lv_label_set_text(txt, mytext);
    pros::delay(10);
  }
}


void basemovement(double distance, int speed)
{
  double ticks=(distance*900)/(4*M_PI);
  left_front.move_relative  (ticks, speed);
  left_back.move_relative   (ticks, speed);
  right_front.move_relative (ticks, speed);
  right_back.move_relative  (ticks, speed);
}

void baseturn(int right , int speed) // right=positive and left=negative
{
  double ticks=735*right;
  left_front.move_relative  (ticks, speed);
  left_back.move_relative   (ticks, speed);
  right_front.move_relative (-ticks, speed);
  right_back.move_relative  (-ticks, speed);
}




void  tray_control(void*) {
 pros::Controller master(CONTROLLER_MASTER);
 bool b_toggle = false;
 while (true) {
   if (master.get_digital(DIGITAL_Y)) {
     b_toggle = !b_toggle;

     if (b_toggle) {
       for(int i=0;i<1700;i=i+3) {
         tray.move((i-tray.get_position())*0.5);;
         pros::delay(5);
       }
     } else {
       tray.move(0);
     }
   }

   pros::delay(20);
 }
}

void  arm_control(void*) {
 pros::Controller master(CONTROLLER_MASTER);
 //pros::Task arm_t(arm_pid);
 bool was_pid;
 while (true) {
   if (master.get_digital(DIGITAL_B)) {
     was_pid = true;
     int a_target =1300;
     arm.move((a_target-arm.get_position())*0.5);
   } else if (master.get_digital(DIGITAL_DOWN)) {
     was_pid = true;
     int a_target =800;
     arm.move((a_target-arm.get_position())*0.5);
   } else {
     if (master.get_digital(DIGITAL_R1)||master.get_digital(DIGITAL_R2)) {
       was_pid = false;
       arm.move((master.get_digital(DIGITAL_R1)*80-master.get_digital(DIGITAL_R2)*40));// set arm to slow speed
     } else {
       if (!was_pid) {
         arm.move(0);
       }
     }
   }

   if (!was_pid) {
     //arm_t.suspend();
   }

   pros::delay(20);
 }
}
