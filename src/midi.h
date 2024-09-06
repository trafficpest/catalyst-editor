// midi.h
#ifndef MIDI_H
#define MIDI_H

#include "setting.h"

#define MAX_CLIENTS 128
#define MAX_PORTS 16
#define MAX_DEVICES 128

typedef struct {
    int client;
    char client_name[64];
    char port_name[64];
} MidiClient;

int open_midi_sequencer();
void send_midi_event(int type, int value1, int value2);
void close_midi_sequencer();
void send_all_midi_events(Setting *settings, int num_settings);
MidiClient* get_midi_clients(int *client_count);
void subscribe_to_midi_port(int input_client, int input_port, int output_client, int output_port);
int get_app_client_id();

#endif
