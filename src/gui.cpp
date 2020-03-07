#include "gui.h"
#include "main.h"
#include "motor.hpp"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

// credit to https://github.com/timeconfusing/v5gui with modify
// credit to https://github.com/kunwarsahni01/Vex-Autonomous-Selector

// forward declaration of following objects
// text to diplay motor and others
lv_obj_t * debugtxt ;
lv_obj_t * tabview ;
lv_obj_t * debuglabel ;
lv_obj_t * debugpid;

int auton_sel = 0;
float kP=0.0;
float kI=0.0;
float kD=0.0;
float kM=1.0;


static lv_res_t btnm_action(lv_obj_t * btnm, const char * bmtxt) {

  int btnm_num = atoi(bmtxt);

  switch (btnm_num) {
  case 1:
    lv_label_set_text(g_sb_label, "Red Big Auton");
    auton_sel = 1;
    break;
  case 2:
    lv_label_set_text(g_sb_label, "Red Small Auton");
    auton_sel = 2;
    break;
  case 3:
    lv_label_set_text(g_sb_label, "Blue Big Auton");
    auton_sel = 3;
break;
  case 4:
    lv_label_set_text(g_sb_label, "Blue Small Auton");
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
  lv_obj_align(g_sb_label, NULL, LV_ALIGN_CENTER, 0, 0);

  return LV_RES_OK; /*Return OK because the button matrix is not deleted*/
}

void gui_btnm(void) {
  static const char * btnm_map[] = { "\2241 red big", "\2242 red small", "\2243 blue big", "\2244 blue small", "\n",
                                     "\2245 Skill 1", "\2246 Skill 2", "" };
  //The escape section prevents a press of the button being interpreted as a multipress of the button
  // Create a default button matrix* no repeat
  lv_obj_t *btnm = lv_btnm_create(tab1, NULL);
  lv_obj_set_size(btnm, lv_obj_get_width(tab1)-20,
                        lv_obj_get_height(tab1)-30);
  lv_btnm_set_map(btnm, btnm_map);
  lv_btnm_set_action(btnm, btnm_action);

  g_sb_label = lv_label_create(tab1, NULL);
  lv_label_set_text(g_sb_label, "auto select");
  lv_obj_align(g_sb_label, NULL,  LV_ALIGN_CENTER, 0, 0);
}

static lv_res_t pidbtnm_action(lv_obj_t * btnm, const char * bmtxt) {
  char pidtext[100];

  int btnm_num = atoi(bmtxt);
  if (strcmp(bmtxt, "redfront")==0) {};
  switch (btnm_num) {
  case 1:
    kP += kM;
    break;
  case 2:
    kP -= kM;
    break;
  case 3:
    kI += kM;

    break;
  case 4:
    kI -= kM;
    break;
  case 5:
    kD += kM;
  break;
  case 6:
    kD -= kM;
   break;
   case 7:
    kM = kM*10;
   break;
   case 8:
    kM = kM/10;
   break;
  }
  kP = (kP >= 0 ? kP : 0);
  kI = (kI >= 0 ? kI : 0);
  kD = (kD >= 0 ? kD : 0);
  sprintf(pidtext, "kP %3.5f kI %3.5f  kD %3.5f  kM %4.5f  ",
              kP, kI, kD, kM
  );
  lv_label_set_text(pid_label, pidtext);
  // must be after set_text
  lv_obj_align(pid_label, btnm, LV_ALIGN_OUT_BOTTOM_MID,0,0);
  return LV_RES_OK; /*Return OK because the button matrix is not deleted*/
}


void pid_btnm(void) {
  // Create a button descriptor string array w/ no repeat "\224"
  static const char * btnm_map[] = { "1 P+", "3 I+", "5 D+", "7 M+", "\n",
                                     "2 P-", "4 I-", "6 D-", "8 M-","" };

  lv_obj_t *btnm = lv_btnm_create(tab4, NULL);
  lv_obj_set_size(btnm, lv_obj_get_width(tab4)-30,
                        lv_obj_get_height(tab4)-60);
  lv_btnm_set_map(btnm, btnm_map);
  lv_btnm_set_action(btnm, pidbtnm_action);

  pid_label = lv_label_create(tab4, NULL);
  lv_label_set_text(pid_label, "PID tuning");
  lv_obj_align(pid_label, btnm, LV_ALIGN_OUT_BOTTOM_MID,0,0);
}


void odometer() {
  /*Set the values*/
  lv_gauge_set_value(gauge1, 0, fabs(left_front.get_actual_velocity()));
  lv_gauge_set_value(gauge1, 1, fabs(right_front.get_actual_velocity()));
  lv_gauge_set_value(gauge1, 2, fabs(left_back.get_actual_velocity()));
  lv_gauge_set_value(gauge1, 3, fabs(right_back.get_actual_velocity()));
}

void lv_ex_gauge_1(void)
{
    /*Create a style*/
    static lv_style_t style;
    lv_style_copy(&style, &lv_style_pretty_color);
    style.body.main_color = lv_color_hex3(0x666);     /*Line color at the beginning*/
    style.body.grad_color =  lv_color_hex3(0x666);    /*Line color at the end*/
    // style.body.padding.left = 10;                      /*Scale line length*/
    style.body.padding.inner = 8 ;                    /*Scale label padding*/
    style.body.border.color = lv_color_hex3(0x333);   /*Needle middle circle color*/
    style.line.width = 3;
    // style.text.color = lv_color_hex3(0x333);
    style.text.color = LV_COLOR_WHITE;
    style.line.color = LV_COLOR_RED;                  /*Line color after the critical value*/

    /*Describe the color for the needles*/
    static lv_color_t needle_colors[4];
    needle_colors[0] = LV_COLOR_BLUE;
    needle_colors[1] = LV_COLOR_ORANGE;
    needle_colors[2] = LV_COLOR_PURPLE;
    needle_colors[3] = LV_COLOR_WHITE;

    /*Create a gauge*/
    gauge1 = lv_gauge_create(tab5, NULL);
    // lv_gauge_set_style(gauge1, LV_GAUGE_STYLE_MAIN, &style);
    lv_gauge_set_range(gauge1, 0, 200);
    lv_gauge_set_critical_value(gauge1, 160);
    lv_gauge_set_needle_count(gauge1, 4, needle_colors);
    lv_obj_set_size(gauge1, 150, 150);
    lv_obj_align(gauge1, NULL, LV_ALIGN_CENTER, 0, 20);

}

void lv_ex_tabview_1(void)
{
    // lvgl theme
    lv_theme_t *th = lv_theme_night_init(240, NULL); //Set a HUE 240 and keep font default BLUE
    //Set a HUE 360 value and keep font default RED
    lv_theme_set_current(th);

    /*Create a Tab view object*/

    tabview = lv_tabview_create(lv_scr_act(), NULL);
		lv_tabview_set_sliding(tabview, false);

    /*Add 4 tabs (the tabs are page (lv_page) and can be scrolled*/
		tab1 = lv_tabview_add_tab(tabview, "Select");
    tab2 = lv_tabview_add_tab(tabview, "Auto");
    tab3 = lv_tabview_add_tab(tabview, "Driver");
    tab4 = lv_tabview_add_tab(tabview, "PID");
    tab5 = lv_tabview_add_tab(tabview, "Odometer");

    debuglabel = lv_label_create(tab2, NULL);
    lv_label_set_text(debuglabel, "auto debug");
    debugpid = lv_label_create(tab2, NULL);
    lv_obj_align(debugpid, debuglabel, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 80);

    debugtxt = lv_label_create(tab3, NULL);
    lv_label_set_long_mode(debugtxt, LV_LABEL_LONG_BREAK);     /*Break the long lines*/
    lv_label_set_recolor(debugtxt, true);                      /*Enable re-coloring by commands in the text*/
    lv_label_set_align(debugtxt, LV_LABEL_ALIGN_LEFT);       /*Center aligned lines*/
    lv_label_set_text(debugtxt, NULL);
    lv_obj_set_width(debugtxt, 500);                           /*Set a width*/
    lv_obj_align(debugtxt, NULL, LV_ALIGN_IN_TOP_LEFT, 10, 20);      /*Align to center*/


    gui_btnm();
    pid_btnm();
    lv_ex_gauge_1();
}
