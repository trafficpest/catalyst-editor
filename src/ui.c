// ui.c
#include <alsa/asoundlib.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include "midi.h"
#include "ui.h"

void display_setting(WINDOW *win, Setting *setting, int index, int col) {
    int bar_length = (setting->value - setting->min) * DISPLAY_BAR_WIDTH / (setting->max - setting->min);  // Scale setting to bar length
    int row = 4 + (index / 2);  // Calculate row index based on setting index

    mvprintw(row, 10 + col * COL_WIDTH + 10, "[");
    for (int i = 0; i < DISPLAY_BAR_WIDTH; ++i) {
        if (i < bar_length)
            wprintw(win, "#");
        else
            wprintw(win, "-");
    }
    wprintw(win, "]");
}

void show_slider_popup(Setting *setting) {
    int popup_height = 10;
    int popup_width = 60;
    int starty = (LINES - popup_height) / 2;
    int startx = (COLS - popup_width) / 2;

    WINDOW *popup_win = newwin(popup_height, popup_width, starty, startx);
    box(popup_win, 0, 0);
    keypad(popup_win, TRUE);

    int ch;
    int pos = (setting->value - setting->min) * SLIDER_WIDTH / (setting->max - setting->min);  // Scale setting to slider position

    while (1) {
        // Display the slider
        mvwprintw(popup_win, 2, 2, "Adjust %s: %3d", setting->name, setting->value);
        mvwprintw(popup_win, 4, 2, "|");
        for (int i = 0; i < SLIDER_WIDTH; ++i) {
            if (i == pos)
                wprintw(popup_win, "#");
            else
                wprintw(popup_win, "-");
        }
        wprintw(popup_win, "|");

        mvwprintw(popup_win, 6, 2, "Use -/= to adjust, _/+ for fine increments.");
        mvwprintw(popup_win, 7, 2, "Press 'Enter' to confirm, 'q' to cancel.");
        wrefresh(popup_win);

        ch = wgetch(popup_win);

        int old_value = setting->value;  // Save the old value for comparison

        switch (ch) {
            case '+':
                if (setting->value < setting->max) setting->value += SMALL_INCREMENT;
                if (setting->value > setting->max) setting->value = setting->max;
                break;
            case '_':
                if (setting->value > setting->min) setting->value -= SMALL_INCREMENT;
                if (setting->value < setting->min) setting->value = setting->min;
                break;
            case '=':  // Large increment
                if (setting->value < setting->max) setting->value += LARGE_INCREMENT;
                if (setting->value > setting->max) setting->value = setting->max;
                break;
            case '-':  // Large decrement
                if (setting->value > setting->min) setting->value -= LARGE_INCREMENT;
                if (setting->value < setting->min) setting->value = setting->min;
                break;
            case '\n':  // Enter key to confirm selection
                delwin(popup_win);
                return;
            case 'q':  // Cancel
                delwin(popup_win);
                return;
        }

        // Update slider position and send MIDI event if value changed
        pos = (setting->value - setting->min) * SLIDER_WIDTH / (setting->max - setting->min);  // Update slider position
        
        if (setting->value != old_value) {
            send_midi_event(SND_SEQ_EVENT_CONTROLLER, setting->control_number, setting->value);
        }
    }
}

