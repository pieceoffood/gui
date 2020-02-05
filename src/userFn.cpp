#include "main.h"
#include "cmath"
#include "motor.hpp"
#include "MiniPID.h"


void basemovePID(double target) {
  // move chassis in inches
  MiniPID pid=MiniPID(0.3,0,0.1); // need to tune those three parameters
  pid.setOutputLimits(-80,80); // set output lower and upper limit
  pid.setOutputRampRate(5); //how fast the motor reach max speed
  double start=left_front.get_position(); // get current start positon in ticks
  // convert the target in inch to tick
  double targetTick = (target*900)/(4*M_PI)+start; // calculate the destination position in ticks
  while (fabs(left_front.get_position()-targetTick)>10) {
    double output=pid.getOutput(left_front.get_position(),
        targetTick);
    // move the chassis motors
    left_back.move(output);
    left_front.move(output);
    right_back.move(output);
    right_front.move(output);

    // print information on the screen to debug
    char mytext[100];
    lv_obj_t * txt = lv_label_create(lv_scr_act(), NULL);
    printf("base start %8.2f, target %8.2f, base %8.2f\n", start, targetTick,left_front.get_position());
    sprintf(mytext, "base start %8.2f\n, target %8.2f\n, base %8.2f\n, output  base %8.2f\n",
            start, targetTick,left_front.get_position(), output
         );
    lv_label_set_text(txt, mytext);
    pros::delay(10);
  }
}

void baseturnPID(double target) {
  // turn chassis in angles, clockwise is positive and anti-clockwise is negative
  MiniPID pid=MiniPID(0.3,0,0.1); // need to tune those three parameters
  pid.setOutputLimits(-50,50); // set output lower and upper limit
  pid.setOutputRampRate(5); //how fast the motor reach max speed
  double start=gyro.get_value(); // get current/start angle from gyro
  double turn = target*10+start; // destination angle
  while (fabs(gyro.get_value()-turn)>3) {
    double output=pid.getOutput(gyro.get_value(),
        turn);
    // power the chassis motors to turn
    left_back.move(output);
    left_front.move(output);
    right_back.move(-output);
    right_front.move(-output);
    // print information on the screen to debug
    char mytext[100];
    lv_obj_t * txt = lv_label_create(lv_scr_act(), NULL);
    printf("base start %8.2f, target %8.2f, gyro %8.2f\n", start, turn,gyro.get_value());
    sprintf(mytext, "\n\n\n\n\ngyro start %8.2f\n, target %8.2f\n, gyro %8.2f\n", start, turn,gyro.get_value()
         );
    lv_label_set_text(txt, mytext);
    pros::delay(10);
  }
}


void armPID(double target) {
  // raise arm to target position
  // arm start at 0 tick
  // target measured at ticks
  MiniPID pid=MiniPID(0.3,0,0.1); // need to tune those three parameters
  pid.setOutputLimits(-80,80); // set output lower and upper limit
  pid.setOutputRampRate(5);
  double start=arm.get_position(); // get current start positon
  while (fabs(arm.get_position()-target)>10) {
    double output=pid.getOutput(arm.get_position(),
        target);
    arm.move(output);
    // print information on the screen to debug
    char mytext[100];
    lv_obj_t * txt = lv_label_create(lv_scr_act(), NULL);
    printf("arm start %8.2f, target %8.2f, base %8.2f\n", start, target , arm.get_position());
    sprintf(mytext, "base start %8.2f\n, target %8.2f\n, base %8.2f\n, output  base %8.2f\n",
            start, target, arm.get_position(), output
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



void set_tray(int input) {
  tray.move(input);
}

void set_arm(int input) {
  arm.move(input);
}

void set_rollers(int input) {
  left_roller.move(input);
  right_roller.move(input);
}

//PID
int t_target;
void set_tray_pid(int input) {
  t_target = input;
}

void tray_pid(void*) {
	while (true) {
		set_tray((t_target-tray.get_position())*0.2);
		pros::delay(20);
	}
}

int a_target;
void set_arm_pid(int input) {
  a_target = input;
}
void arm_pid(void*) {
  while (true) {
    set_arm((a_target-arm.get_position())*0.2);
    pros::delay(20);
  }
}


void  tray_control(void*) {
 pros::Task tray_t(tray_pid);
 bool b_toggle = false;
 while (true) {
   if (master.get_digital(DIGITAL_Y)) {
     b_toggle = !b_toggle;

     if (b_toggle) {
       for(int i=0;i<1700;i=i+3) {
         set_tray_pid(i);
         pros::delay(5);
       }
     } else {
       set_tray_pid(0);
     }

     while (master.get_digital(DIGITAL_Y)) {
       pros::delay(10);
     }
   }

   pros::delay(20);
 }
}


void  arm_control(void*) {
 /*
 DIGITAL_B PID to the high tower
 DIGITAL_DOWN PID to the lower tower
 DIGITAL_R1 arm up
 DIGITAL_R2 arm down
 */
 int arm_target=0;
 bool was_pid=false;
 while (true) {
   if (master.get_digital(DIGITAL_B)) {
     was_pid = true;
     arm_target =1300;
   } else if (master.get_digital(DIGITAL_DOWN)) {
     was_pid = true;
     arm_target =800;
   } else {
     if (master.get_digital(DIGITAL_R1)||master.get_digital(DIGITAL_R2)) {
       was_pid = false;
       arm.move((master.get_digital(DIGITAL_R1)*80-master.get_digital(DIGITAL_R2)*40));
       // set arm to slow speed
     } else {
       if (!was_pid) {
         arm.move(0);
       }
     }
   }
   MiniPID pid=MiniPID(0.5,0,0.1);
   pid.setOutputLimits(-100,100);
   pid.setOutputRampRate(5);
   double output=pid.getOutput(arm.get_position(), arm_target);
   if (was_pid) {
     //arm.move((arm_target-arm.get_position())*0.4); // PID with P only not useing now
	   arm.move(output);
   }

   pros::delay(20);
 }
}
