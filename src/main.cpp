#include "main.h"
#include <iomanip>
#include "motor.hpp"
#include "okapi/api.hpp"
#include "gui.h"
#include "MiniPID.h"
#include "userFn.hpp"
#include <fstream>

/*
following are prosv5 commands
cd (change directory)
cd .. (go up one level)
prosv5 make clean (clean everything)
prosv5 build-compile-commands (compile the code)
prosv5 make (compile the code)
prosv5 upload --slot 5 (upload the program to V5 slot 5)
prosv5 v5 rm-all
prosv5 v5 rm-file slot_4.bin --erase-all
*/

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
  pros::delay(10);
	lv_ex_tabview_1();
  pros::delay(100);
  pros::Task T_display(Tdisplay);

}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {

}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void autonomous() {
  lv_tabview_set_tab_act(tabview, 1, LV_ANIM_NONE);


    switch (auton_sel) {
      case 1:
        // auton1();
        basemove(24,50);
        while ( // wait until the motor stop/reach target
                left_front.get_actual_velocity()!=0
                || right_front.get_actual_velocity()!=0
                || left_back.get_actual_velocity()!=0
                || right_back.get_actual_velocity()!=0
               ) {
          pros::delay(20);
        }
        basemove(-12,50);
        while ( // wait until the motor stop/reach target
                left_front.get_actual_velocity()!=0
                || right_front.get_actual_velocity()!=0
                || left_back.get_actual_velocity()!=0
                || right_back.get_actual_velocity()!=0
               ) {
          pros::delay(20);
        }
        pros::delay(50);
        left_front.move(0);
        left_back.move(0);
        right_front.move(0);
        right_back.move(0);
      break;
      case 2:
        // auton2();
        basemovePID(24);
        basemovePID(-12);
        // baseturnPID(-90);

      break;
      case 3:
        // auton3();

      break;
      case 4:
        // auton4();

      break;
      case 5:
        // auton5();

      break;
      case 6:
        // auton6();

      break;
      default:
        // empty_auton();
      break;
    }
}

/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */
void opcontrol() {


  //prepare to write to SD card
  FILE * sdfile =fopen("/usd/rerun.txt", "w");
  if (sdfile!=NULL) {
    fprintf(sdfile, "// generate code by driving \n");
  }
  fclose(sdfile);
  int timeOld = 0;
  int timeNew = 0;
  int deltaTime = 0;
  int lfSpeed = 0;
  int lbSpeed = 0;
  int rfSpeed = 0;
  int rbSpeed = 0;
  int armSpeed = 0;
  int left_rollerSpeed = 0;

  /** register to allow user to control motor
  * it allow user to control the motor with buttons
  * when buttons pressed, it is reset to true
  * when buttons released, it is reset to false,
  * only when it is true, the moter be asked to stop
  * brain will not keep telling motor to stop
  * thus allow other task/macro to control the same motor without conflict
  */
  bool RollerUserAllow = false;

  lv_tabview_set_tab_act(tabview, 1, LV_ANIM_NONE);

  master.clear();
  master.print(0, 0, "VEX");
  auto timeFlag=pros::millis();
  pros::Task tray_control_t(tray_control);
  pros::Task t_arm(arm_control);

	while (true) {

    // update control screen very 1 second
    if(pros::millis()-timeFlag>=1000){
      master.print(1, 0, "arm:%8.2f", arm.get_position());
      timeFlag=pros::millis();
    }

    //arcade drive
    int forwardback = curveJoystick(master.get_analog (ANALOG_LEFT_Y));
    int turn        = curveJoystick (master.get_analog (ANALOG_RIGHT_X));

    // chasis arcade
    left_front.move  (forwardback + turn );
    left_back.move   (forwardback + turn );
    right_front.move (forwardback - turn );
    right_back.move  (forwardback - turn );

    // roller intake
    if (master.get_digital(DIGITAL_L1)) {
      RollerUserAllow=true; // allow stop the moter when release button
			left_roller.move_velocity(200);
      right_roller.move_velocity(200);
		} else if (master.get_digital(DIGITAL_L2)) {
      RollerUserAllow=true; // allow stop the moter when release button
      left_roller.move_velocity(-50);
      right_roller.move_velocity(-50);
		} else if (RollerUserAllow==true)  {
      RollerUserAllow=false; // butten released and not need to keep telling the motor to stop
      left_roller.move_velocity(0);
      right_roller.move_velocity(0);
		}


    // write to SD card
    sdfile =fopen("/usd/rerun.txt", "a");
    if (sdfile!=NULL) {
      lfSpeed = left_front.get_actual_velocity();
  		lbSpeed = left_back.get_actual_velocity();
  		rfSpeed = right_front.get_actual_velocity();
  		rbSpeed = right_back.get_actual_velocity();
  		armSpeed = arm.get_actual_velocity();
  		left_rollerSpeed = left_roller.get_target_velocity();
      fprintf(sdfile, "left_front.move_velocity(%i); \n", lfSpeed);
  		fprintf(sdfile, "left_back.move_velocity(%i); \n", lbSpeed);
  		fprintf(sdfile, "right_front.move_velocity(%i); \n", rfSpeed);
  		fprintf(sdfile, "right_back.move_velocity(%i); \n", rbSpeed);
  		fprintf(sdfile, "arm.move_velocity(%i); \n", armSpeed);
  		fprintf(sdfile, "left_roller.move_velocity(%i); \n", left_rollerSpeed);
      fprintf(sdfile, "right_roller.move_velocity(%i); \n", left_rollerSpeed);
      timeNew=pros::millis();
      deltaTime=timeNew-timeOld;
      timeOld=pros::millis();
      fprintf(sdfile, "pros:delay(%d);\n", deltaTime);
    }
    fclose(sdfile);
    odometer();

    //std::ofstream LogFile;
    //LogFile.open("/usd/logfile.txt");
    //LogFile<< " left_front.move_velocity("  << lfSpeed << ");\n" ;
    //LogFile.close();
    pros::delay(10);
	}
}
