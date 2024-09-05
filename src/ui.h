// ui.h
#ifndef UI_H
#define UI_H

#include <ncurses.h>

#define SLIDER_WIDTH 50
#define COL_WIDTH 40
#define DISPLAY_BAR_WIDTH 10
#define LARGE_INCREMENT 10
#define SMALL_INCREMENT 1
#define MAX_LABEL_LENGTH 25

typedef struct {
    char name[30];
    int control_number;
    int value;
    int min;
    int max;
} Setting;


void initialize_ui();
void display_main_menu(Setting *settings, int num_settings, int selected_index);
void display_setting(WINDOW *win, Setting *setting);
void show_slider_popup(Setting *setting);

#endif // UI_H

