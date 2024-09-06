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
    init_pair(1, COLOR_BLACK, COLOR_WHITE); //pop slider
    init_pair(2, COLOR_WHITE, COLOR_BLACK); //setting display
    init_pair(3, COLOR_BLACK, COLOR_WHITE); //main settings
    init_pair(4, COLOR_WHITE, -1);
    init_pair(5, COLOR_CYAN, COLOR_BLUE); // background

    bkgd(COLOR_PAIR(5) | A_BOLD);
}

#define MENU_WIDTH 70
#define MENU_HEIGHT 17

void display_main_menu(Setting *settings, int num_settings, int selected_index) {
    int rows, cols;
    getmaxyx(stdscr, rows, cols);  // Get the screen size

    refresh();  // Refresh the main screen
                
    // Display the title and instructions
    mvprintw(1, (cols - strlen("Line6 Catalyst Editor:")) / 2, "Line6 Catalyst Editor:");
    mvprintw(rows - 2, (cols - strlen("Arrow Keys - navigate, Enter - adjust, m - MIDI, q - quit.")) / 2, "Arrow Keys - navigate, Enter - adjust, m - MIDI, q - quit.");

    // Create a new window for the settings box
    WINDOW *settings_win = newwin(MENU_HEIGHT, MENU_WIDTH, (rows - MENU_HEIGHT) / 2, (cols - MENU_WIDTH) / 2);
    wbkgd(settings_win, COLOR_PAIR(3));  
    box(settings_win, 0, 0);  // Draw a box around the window
                            
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
}

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

void show_midi_clients() {
    int client_count;
    MidiClient *clients = get_midi_clients(&client_count);

    if (clients == NULL) {
        return; // Handle memory allocation failure
    }

    // Create select_midi window
    int select_midi_height = 20;
    int select_midi_width = 60;
    int starty = (LINES - select_midi_height) / 2;
    int startx = (COLS - select_midi_width) / 2;

    WINDOW *select_midi_win = newwin(select_midi_height, select_midi_width, starty, startx);
    wbkgd(select_midi_win, COLOR_PAIR(1));
    box(select_midi_win, 0, 0);
    keypad(select_midi_win, TRUE);

    int ch;
    int line = 1;
    int selected_client = -1;
    int selected_port = -1;

    mvwprintw(select_midi_win, 0, 2, "Available MIDI Clients and Ports:");
    wrefresh(select_midi_win);

    // Display available MIDI clients and ports
    for (int i = 0; i < client_count; i++) {
        if (line < select_midi_height - 2) {
            mvwprintw(select_midi_win, line, 2, "%3d: %s - %s", i + 1, clients[i].client_name, clients[i].port_name);
            line++;
        } else {
            mvwprintw(select_midi_win, line, 2, "Press Enter to scroll or 'q' to exit.");
            wrefresh(select_midi_win);
            ch = wgetch(select_midi_win);

            if (ch == 'q') {
                break;
            } else {
                werase(select_midi_win);
                box(select_midi_win, 0, 0);
                mvwprintw(select_midi_win, 0, 2, "Available MIDI Clients and Ports:");
                line = 1;
            }
        }
    }

    mvwprintw(select_midi_win, line, 2, "Select client number and press Enter:");
    wrefresh(select_midi_win);

    while (1) {
        ch = wgetch(select_midi_win);

        if (ch == 'q') {
            break;
        } else if (ch >= '1' && ch <= '0' + client_count) {
            selected_client = ch - '1'; // Convert to zero-based index
            break;
        }
    }

    if (selected_client >= 0 && selected_client < client_count) {
        // Assuming the port is 0 for simplicity; you might want to add logic for selecting ports
        selected_port = 0;

        // Subscribe to the selected client
        int output_client = clients[selected_client].client;
        int output_port = selected_port;
        int input_client = get_app_client_id(); 
        int input_port = 0; // Replace with your port ID

        subscribe_to_midi_port(input_client, input_port, output_client, output_port);

        mvwprintw(select_midi_win, line + 1, 2, "Subscribed to client %d.", selected_client + 1);
        wrefresh(select_midi_win);
        wgetch(select_midi_win); // Wait for user input to close the window
    }

    delwin(select_midi_win);
    free(clients);  // Free the allocated memory
}

