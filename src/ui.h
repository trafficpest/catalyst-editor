// ui.h
#ifndef UI_H
#define UI_H

#include "setting.h"

#define SLIDER_WIDTH 50
#define COL_WIDTH 40
#define DISPLAY_BAR_WIDTH 10
#define LARGE_INCREMENT 10
#define SMALL_INCREMENT 1
#define MAX_LABEL_LENGTH 25

void initialize_ui();
void display_main_menu(Setting *setting, int num_settings, int selected_index);
void display_setting(WINDOW *win, Setting *setting);
void show_slider_popup(Setting *setting);
void show_midi_clients();


#endif // UI_H

