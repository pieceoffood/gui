#include "main.h"
#include "cmath"
#include "motor.hpp"
#include "MiniPID.h"
#include "gui.h"



// task to print information on the screen to debug
// must use void* in parameters
void Tdisplay (void*) {
  char displaytext[100];
  while (1) {
    sprintf(displaytext,
            "arm potentiameter: %d, arm %8.2f \n"
            "tray: %8.2f, set zero: %d\n"
            "leftfront:%8.2f @: %5.1f rightfront:%8.2f @: %5.1f\n"
            "gyro:%8.2f\n",
            potentiameter.get_value(), arm.get_position(),
            tray.get_position(), limitswitch.get_value(),
            left_front.get_position(), left_front.get_actual_velocity(), right_front.get_position(),  right_front.get_actual_velocity(),
            gyro.get_value()
    );
    lv_label_set_text(debuglabel, displaytext);
    pros::delay(100);
  }
}

/**
 * mover the chassis with PID control
 *
 * @param tartget target of the move Distance in inches assume 4 inch wheels
 */
void basemovePID(double target) {
  char mytext[100];
  // move chassis in inches
  MiniPID pid=MiniPID(0.1,0,0.1); // need to tune those three parameters
  pid.setOutputLimits(-80,80); // set output lower and upper limit
  pid.setOutputRampRate(5); //how fast the motor reach max speed
  double start=left_front.get_position(); // get current start positon in ticks
  auto motorgear=left_front.get_gearing();
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
    printf("base start %8.2f, target %8.2f, base %8.2f\n", start, targetTick,left_front.get_position());
    sprintf(mytext, "base start %8.2f, target %8.2f\n, base %8.2f, output  base %8.2f\n",
            start, targetTick, left_front.get_position(), output
         );
    lv_label_set_text(debugpid, mytext);
    pros::delay(20);
    // exit while loop is the motor stopped because of obstacle before reach target
    if ( // wait until the motor stop/reach target
            left_front.get_actual_velocity()==0
            && right_front.get_actual_velocity()==0
            && left_back.get_actual_velocity()==0
            && right_back.get_actual_velocity()==0
           )   break;
  }
  // left_back.move(0);
  // left_front.move(0);
  // right_back.move(0);
  // right_front.move(0);
}


/**
 * turn the chassis with PID control
 * using gyro censor
 * @param tartget turning angle in degrees
 */
void baseturnPID(double target) {
  // turn chassis in angles, clockwise is positive and anti-clockwise is negative
  MiniPID pid=MiniPID(0.25,0,0.1); // need to tune those three parameters
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
    printf("base start %8.2f, target %8.2f, gyro %8.2f\n", start, turn,gyro.get_value());
    sprintf(mytext, "gyro start %8.2f, target %8.2f\n, gyro %8.2f\n", start, turn ,gyro.get_value()
         );
    lv_label_set_text(debugpid, mytext);
    pros::delay(20);
    // exit while loop is the motor stopped because of obstacle before reach target
    if ( // wait until the motor stop/reach target
            left_front.get_actual_velocity()==0
            && right_front.get_actual_velocity()==0
            && left_back.get_actual_velocity()==0
            && right_back.get_actual_velocity()==0
           )   break;
  }
}

/**
 * raise arm with PID control
 * using V5 motor encoder
 * starting position is zero tick
 * @param tartget position in tick
 */
void armPID(double target) {
  // raise arm to target position
  // arm start at 0 tick
  // target measured at ticks

  char mytext[100];
  MiniPID pid=MiniPID(0.3,0,0.1); // need to tune those three parameters
  pid.setOutputLimits(-80,80); // set output lower and upper limit
  pid.setOutputRampRate(5);
  double start=arm.get_position(); // get current start positon
  while (fabs(arm.get_position()-target)>10) {
    double output=pid.getOutput(arm.get_position(),
        target);
    arm.move(output);
    // print information on the screen to debug

    printf("arm start %8.2f, target %8.2f, base %8.2f\n", start, target , arm.get_position());
    sprintf(mytext, "base start %8.2f\n, target %8.2f\n, base %8.2f\n, output  base %8.2f\n",
            start, target, arm.get_position(), output
         );
    lv_label_set_text(debuglabel, mytext);
    pros::delay(10);
  }
}


/**
 * move the chassis
 * @param distance in inches
 * @param speed +-200 for gree cartridge.
                +-600 for blue cartridge.
                +-100 for red cartridge.
 */
void basemove(double distance, int speed)
{
  double ticks=(distance*900)/(4*M_PI);
  left_front.move_relative  (ticks, speed);
  left_back.move_relative   (ticks, speed);
  right_front.move_relative (ticks, speed);
  right_back.move_relative  (ticks, speed);
  pros::delay(50);
}

/**
 * turn the chassis
 * emperically determine the 735 tick/90 degrees
 * need to adjust base on your wheel and chasis size and
 * @param turn in angles degrees, + for clockwise
 * @param speed +-200 for gree cartridge.
                +-600 for blue cartridge.
                +-100 for red cartridge.
 */
void baseturn(int turn , int speed) // right=positive and left=negative
{
  double ticks=735/90*turn;
  left_front.move_relative  (ticks, speed);
  left_back.move_relative   (ticks, speed);
  right_front.move_relative (-ticks, speed);
  right_back.move_relative  (-ticks, speed);
  pros::delay(50);
}



//target position for tray
int t_target;


//set target position for tray
void set_tray_pid(int input) {
  t_target = input;
}


// PID control P loop for tray
void tray_pid(void*) {
	while (true) {
		tray.move((t_target-tray.get_position())*0.5);
		pros::delay(20);
	}
}

/*
tray PID Control
DIGITAL_Y activate PID
move tray to 1700 ticks with PID control
*/
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


/*
DIGITAL_B PID to the high tower
DIGITAL_DOWN PID to the lower tower
DIGITAL_R1 arm up
DIGITAL_R2 arm down
you will need to adjust arm_target in the function
*/
void  arm_control(void*) {
 int arm_target=0;
 bool was_pid=false; // registe for PID
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
