#include "main.h"

int auton_sel = 0;
lv_obj_t * g_sb_label;
lv_obj_t * tab1;

static lv_res_t btn_click_action(lv_obj_t * btn) {
   uint8_t id = lv_obj_get_free_num(btn);
   static char buffer[32];
   auton_sel = id;

   snprintf(buffer, 32, "Selection is %d \n", id);
   lv_label_set_text(g_sb_label, buffer);
   lv_obj_align(g_sb_label, NULL, LV_ALIGN_CENTER, 0, 0); // must be after set_text

   return LV_RES_OK; /*Return OK if the button is not deleted*/
}

static lv_res_t btnm_action(lv_obj_t * btnm, const char *txt) {

  int btnm_num = atoi(txt);

  switch (btnm_num) {
  case 1:
    lv_label_set_text(g_sb_label, "Red Right Auton");
    auton_sel = 1;
    break;
  case 2:
    lv_label_set_text(g_sb_label, "Red Left Auton");
    auton_sel = 2;
    break;
  case 3:
    lv_label_set_text(g_sb_label, "Blue Right Auton");
    auton_sel = 3;
break;
  case 4:
    lv_label_set_text(g_sb_label, "Blue Left Auton");
    auton_sel = 4;
break;
  case 5:
    lv_label_set_text(g_sb_label, "Skills Auton1");
    auton_sel = 5;
break;
  case 6:
    lv_label_set_text(g_sb_label, "Skills Auton2");
    auton_sel = 6;
break;
  }

  lv_obj_align(g_sb_label, NULL, LV_ALIGN_CENTER, 0, 0); // must be after set_text

  return LV_RES_OK; /*Return OK because the button matrix is not deleted*/
}

void gui_btnm(void) {
  // Create a button descriptor string array w/ no repeat "\224"
  static const char * btnm_map[] = { "\2241", "\2242", "\2243", "\n",
                                     "\2244", "\2245", "\2246", "" };

  // Create a default button matrix* no repeat
  lv_obj_t *btnm = lv_btnm_create(tab1, NULL);
  lv_obj_set_size(btnm, lv_obj_get_width(tab1),
      lv_obj_get_height(tab1) - 32);

  lv_btnm_set_map(btnm, btnm_map);
  lv_btnm_set_action(btnm, btnm_action);
}


void lv_ex_tabview_1(void)
{
    /*Create a Tab view object*/
    lv_obj_t *tabview;
    tabview = lv_tabview_create(lv_scr_act(), NULL);
		lv_tabview_set_sliding(tabview, false);

    /*Add 4 tabs (the tabs are page (lv_page) and can be scrolled*/
		lv_obj_t *tab1 = lv_tabview_add_tab(tabview, "Select");
    lv_obj_t *tab2 = lv_tabview_add_tab(tabview, "Auto");
    lv_obj_t *tab3 = lv_tabview_add_tab(tabview, "Driver");
    lv_obj_t *tab4 = lv_tabview_add_tab(tabview, "PID");



    /*Add content to the tabs*/
    lv_obj_t * label = lv_label_create(tab1, NULL);
    lv_label_set_text(label, "select your autonomous");

		g_sb_label = lv_label_create(tab1, NULL);
		lv_obj_set_style(g_sb_label, &lv_style_pretty_color);
		lv_obj_align(g_sb_label, NULL, LV_ALIGN_CENTER, 0, 0);


    label = lv_label_create(tab2, NULL);
    lv_label_set_text(label, "Second tab");

    label = lv_label_create(tab3, NULL);
    lv_label_set_text(label, "Third tab");

		label = lv_label_create(tab4, NULL);
    lv_label_set_text(label, "tune your PID");

		//gui_btnm();
		static const char * btnm_map[] = { "\2241", "\2242", "\2243", "\n",
	                                     "\2244", "\2245", "\2246", "" };

	  // Create a default button matrix* no repeat
	  lv_obj_t *btnm = lv_btnm_create(tab1, NULL);
	  lv_obj_set_size(btnm, lv_obj_get_width(tab1),
	      lv_obj_get_height(tab1)-30);

	  lv_btnm_set_map(btnm, btnm_map);
	  lv_btnm_set_action(btnm, btnm_action);
}

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
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
void autonomous() {}

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
	pros::Controller master(pros::E_CONTROLLER_MASTER);
	pros::Motor left_mtr(1);
	pros::Motor right_mtr(2);

	while (true) {

		int left = master.get_analog(ANALOG_LEFT_Y);
		int right = master.get_analog(ANALOG_RIGHT_Y);

		left_mtr = left;
		right_mtr = right;
		pros::delay(20);
	}
}
