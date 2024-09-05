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

void show_slider_popup(Setting *setting);
void display_setting(WINDOW *win, Setting *setting, int index, int col);

#endif // UI_H

