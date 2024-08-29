#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include "ui.h"

int main() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    // Define settings
Setting settings[] = {
    {"Amp Voicing Type", 5, 0, 0, 11},  // MIDI CC 5
    {"A/B Select", 30, 63, 0, 64},  // MIDI CC 30, 0-63=A, 64-127=B
    {"Boost Off/On", 25, 63, 0, 64},  // MIDI CC 25, 0-63=Off, 64-127=On
    {"Boost Level", 26, 0, 0, 127},  // MIDI CC 26
    {"Gain", 13, 0, 0, 127},  // MIDI CC 13
    {"Bass", 14, 0, 0, 127},  // MIDI CC 14
    {"Mid", 15, 0, 0, 127},  // MIDI CC 15
    {"Treble", 16, 0, 0, 127},  // MIDI CC 16
    {"Presence", 21, 0, 0, 127},  // MIDI CC 21
    {"Channel Volume", 17, 0, 0, 127},  // MIDI CC 17
    {"Gate Off/On", 22, 63, 0, 64},  // MIDI CC 22, 0-63=Off, 64-127=On
    {"Gate Threshold", 23, 0, 0, 127},  // MIDI CC 23, with special dB range handling
    {"Gate Decay", 24, 0, 0, 127},  // MIDI CC 24, 0=0.1ms, 127=3000ms
    {"Expression", 4, 0, 0, 127},  // MIDI CC 4
    {"Volume Pedal Off/On", 6, 63, 0, 127},  // MIDI CC 6, 0-63=Off, 64-127=On
    {"Volume Pedal Level", 7, 0, 0, 127},  // MIDI CC 7
    {"Effect 1 Off/On", 28, 63, 0, 64},  // MIDI CC 28, 0-63=Off, 64-127=On
    {"Effect 1 Model Select", 19, 0, 0, 23},  // MIDI CC 19
    {"Effect 1 Amount", 34, 0, 0, 127},  // MIDI CC 34
    {"Effect 1 Pre/Post Amp", 74, 63, 0, 64},  // MIDI CC 74, 0-63=Pre, 64-127=Post
    {"Effect 2 Off/On", 29, 63, 0, 64},  // MIDI CC 29, 0-63=Off, 64-127=On
    {"Effect 2 Model Select", 37, 0, 0, 23},  // MIDI CC 37
    {"Effect 2 Amount", 18, 0, 0, 127},  // MIDI CC 18
    {"Effect 2 Pre/Post Amp", 41, 0, 0, 127},  // MIDI CC 41, 0-63=Pre, 64-127=Post
    {"Expression Off/On", 43, 0, 0, 127},  // MIDI CC 43, 0-63=Off, 64-127=On
    {"Volume Pedal Pre/Post Amp", 47, 0, 0, 127},  // MIDI CC 47, 0-63=Pre, 64-127=Post
    {"Tap Tempo", 64, 64, 64, 127},  // MIDI CC 64, 64-127=Tap Tempo
    {"Cabinet Select", 71, 0, 0, 2},  // MIDI CC 71, Cabinet 1-3
    {"Direct Out Mic Select", 70, 0, 0, 14},  // MIDI CC 70, Mic type 1-15
    {"FX Loop Off/On", 107, 63, 0, 64}  // MIDI CC 107, 0-63=Off, 64-127=On
};

    int num_settings = sizeof(settings) / sizeof(settings[0]);
    int selected_index = 0;
    int ch;

    while (1) {
        clear();
        mvprintw(2, 33, "Catalyst Edit:");
        for (int i = 0; i < num_settings; ++i) {
            int col = i % 2;  // Calculate column index
            int row = 4 + (i / 2);  // Calculate row index based on column

            // Create a buffer for the label and truncate it if necessary
            char label[MAX_LABEL_LENGTH + 1];  // +1 for null terminator
            strncpy(label, settings[i].name, MAX_LABEL_LENGTH);
            label[MAX_LABEL_LENGTH] = '\0';  // Ensure null termination

            if (i == selected_index)
                attron(A_REVERSE);  // Highlight selected setting

            mvprintw(row, 2 + col * COL_WIDTH, "%-10s", label);
            display_setting(stdscr, &settings[i], i, col);

            if (i == selected_index)
                attroff(A_REVERSE);
        }

        ch = getch();

        switch (ch) {
            case KEY_UP:
                selected_index = (selected_index > 0) ? selected_index - 1 : num_settings - 1;
                break;
            case KEY_DOWN:
                selected_index = (selected_index + 1) % num_settings;
                break;
            case '\n':  // Enter key
                show_slider_popup(&settings[selected_index]);
                break;
            case 'q':
                endwin();
                return 0;
        }
    }

    endwin();
    return 0;
}

