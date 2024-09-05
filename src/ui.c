// ui.c
#include <alsa/asoundlib.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include "midi.h"
#include "ui.h"

void initialize_ui() {
    initscr();
    start_color();
    use_default_colors();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    // Define color pairs
    init_pair(1, COLOR_WHITE, COLOR_RED);
    init_pair(2, COLOR_WHITE, COLOR_BLACK);
    init_pair(3, COLOR_BLACK, COLOR_WHITE);
    init_pair(4, COLOR_WHITE, -1);
    init_pair(5, COLOR_CYAN, COLOR_BLUE);

    bkgd(COLOR_PAIR(5));
}

#define MENU_WIDTH 70
#define MENU_HEIGHT 17

void display_main_menu(Setting *settings, int num_settings, int selected_index) {
    int rows, cols;
    getmaxyx(stdscr, rows, cols);  // Get the screen size

    // Create a new window for the settings box
    WINDOW *settings_win = newwin(MENU_HEIGHT, MENU_WIDTH, (rows - MENU_HEIGHT) / 2, (cols - MENU_WIDTH) / 2);
    wbkgd(settings_win, COLOR_PAIR(3));  
    box(settings_win, 0, 0);  // Draw a box around the window
                            

    // Display the title and instructions
    //attron(COLOR_PAIR(5));
    mvprintw(1, (cols - strlen("Line6 Catalyst Editor:")) / 2, "Line6 Catalyst Editor:");
    mvprintw(rows - 2, (cols - strlen("Use Arrow Keys to navigate, Enter to adjust, q to quit.")) / 2, "Use Arrow Keys to navigate, Enter to adjust, q to quit.");
    //attroff(COLOR_PAIR(5));

    // Display the settings in the centered window
    for (int i = 0; i < num_settings; ++i) {
        int col = i % 2;
        int row = 1 + (i / 2);

        char label[MAX_LABEL_LENGTH + 1];
        strncpy(label, settings[i].name, MAX_LABEL_LENGTH);
        label[MAX_LABEL_LENGTH] = '\0';

        if (i == selected_index) {
            wattron(settings_win, A_REVERSE);  // Highlight selected setting
        }

        mvwprintw(settings_win, row, 2 + col * (MENU_WIDTH / 2), "%-10s", label);
        display_setting(settings_win, &settings[i]);

        if (i == selected_index) {
            wattroff(settings_win, A_REVERSE);
        }
    }

    wrefresh(settings_win);  // Refresh the settings window
    refresh();  // Refresh the main screen
    delwin(settings_win);  // Delete the settings window
}
/*
void display_main_menu(Setting *settings, int num_settings, int selected_index) {
    clear();
    mvprintw(2, 33, "Line6 Catalyst Editor:");
    for (int i = 0; i < num_settings; ++i) {
        int col = i % 2;
        int row = 4 + (i / 2);

        char label[MAX_LABEL_LENGTH + 1];
        strncpy(label, settings[i].name, MAX_LABEL_LENGTH);
        label[MAX_LABEL_LENGTH] = '\0';

        if (i == selected_index)
            attron(A_REVERSE);

        mvprintw(row, 2 + col * COL_WIDTH, "%-10s", label);
        display_setting(stdscr, &settings[i], i, col);

        if (i == selected_index)
            attroff(A_REVERSE);
    }

    mvprintw(LINES - 2, 2, "Use Arrow Keys to navigate, Enter to adjust, q to quit.");
    refresh();
}
*/
void display_setting(WINDOW *win, Setting *setting) {
    int bar_length = (setting->value - setting->min) * DISPLAY_BAR_WIDTH / (setting->max - setting->min);  // Scale setting to bar length

    // Set color for the bar display
    wattron(win, COLOR_PAIR(2));
    //mvprintw(row, 10 + col * COL_WIDTH + 10, "[");
    wprintw(win, "[");
    for (int i = 0; i < DISPLAY_BAR_WIDTH; ++i) {
        if (i < bar_length)
            wprintw(win, "#");
        else
            wprintw(win, "-");
    }
    wprintw(win, "]");
    wattroff(win, COLOR_PAIR(2));
}

void show_slider_popup(Setting *setting) {
    int popup_height = 10;
    int popup_width = 60;
    int starty = (LINES - popup_height) / 2;
    int startx = (COLS - popup_width) / 2;

    WINDOW *popup_win = newwin(popup_height, popup_width, starty, startx);
    box(popup_win, 0, 0);
    wbkgd(popup_win, COLOR_PAIR(1));  // Set background color
    keypad(popup_win, TRUE);

    int ch;
    int pos = (setting->value - setting->min) * SLIDER_WIDTH / (setting->max - setting->min);

    while (1) {
        // Display the slider and its value
        mvwprintw(popup_win, 2, 2, "Adjust %s: %3d", setting->name, setting->value);
        mvwprintw(popup_win, 4, 2, "|");
        for (int i = 0; i < SLIDER_WIDTH; ++i) {
            if (i == pos) {
                wattron(popup_win, COLOR_PAIR(3));  // Highlight slider position
                wprintw(popup_win, "#");
                wattroff(popup_win, COLOR_PAIR(3));
            } else {
                wprintw(popup_win, "-");
            }
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

        pos = (setting->value - setting->min) * SLIDER_WIDTH / (setting->max - setting->min);  // Update slider position
        
        if (setting->value != old_value) {
            send_midi_event(SND_SEQ_EVENT_CONTROLLER, setting->control_number, setting->value);
        }
    }
}

