#include "main.h"

pros::Controller master(pros::E_CONTROLLER_MASTER);

pros::Motor left_back (12, MOTOR_GEARSET_18);
pros::Motor left_front (11, MOTOR_GEARSET_18);
pros::Motor right_back (19, MOTOR_GEARSET_18,true);
pros::Motor right_front (20, MOTOR_GEARSET_18, true);

pros::Motor tray(16, MOTOR_GEARSET_36, true);
pros::Motor arm(4, MOTOR_GEARSET_18, true);
pros::Motor left_roller(1, MOTOR_GEARSET_18);
pros::Motor right_roller(10, MOTOR_GEARSET_18, true);

pros::ADIDigitalIn limitswitch  (1);
pros::ADIAnalogIn potentiameter (2);
pros::ADIGyro gyro (3);
