// setting.h
#ifndef SETTING_H
#define SETTING_H

typedef struct {
    char name[30];
    int control_number;
    int value;
    int min;
    int max;
} Setting;

#endif // SETTING_H
