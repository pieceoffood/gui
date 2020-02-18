#include "main.h"
#include <iomanip>
#include "motor.hpp"
#include "okapi/api.hpp"
#include "gui.h"
#include "MiniPID.h"
#include "userFn.hpp"
#include <fstream>

/*
cd (change directory)
cd .. (go up one level)
prosv5 make clean (clean everything)
prosv5 build-compile-commands (compile the code)
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

      break;
      case 2:
        // auton2();

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

  std::ofstream LogFile;
  LogFile.open("/usd/logfile.txt");

  lv_tabview_set_tab_act(tabview, 2, LV_ANIM_NONE);
  char mytext[100];


  master.clear();
  master.print(0, 0, "VEX");
  auto timeFlag=pros::millis();
  pros::Task tray_control_t(tray_control);
  pros::Task t_arm(arm_control);
  //pros::Task T_display(Tdisplay);
	while (true) {

    sprintf(mytext,
            "arm potentiameter: %d, arm %8.2f \n"
            "tray: %8.2f, set zero: %d\n"
            "leftfront:%8.2f velocity: %lf rightfront:%8.2f velocity: %lf\n"
            "gyro:%8.2f\n",
            potentiameter.get_value(), arm.get_position(),
            tray.get_position(), limitswitch.get_value(),
            left_front.get_position(), left_front.get_actual_velocity(), right_front.get_position(),  right_front.get_actual_velocity(),
            gyro.get_value()
    );
    lv_label_set_text(txt, mytext);

    // update control screen very 1 second
    if(pros::millis()-timeFlag>=1000)
             {
                    master.print(1, 0, "arm:%8.2f", arm.get_position());
                    timeFlag=pros::millis();
             }

    int forwardback = master.get_analog (ANALOG_LEFT_Y);
    int turn        = master.get_analog (ANALOG_RIGHT_X);

    		// chasis
    	left_front.move  (forwardback + turn );
    	left_back.move   (forwardback + turn );
      right_front.move (forwardback - turn );
      right_back.move  (forwardback - turn );
		pros::delay(10);

    LogFile<< " LF,"  << left_front.get_actual_velocity() << ", RF, "<< right_front.get_actual_velocity() << "\n" ;

	}
    LogFile.close();
}
