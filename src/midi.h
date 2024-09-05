// midi.h
#ifndef MIDI_H
#define MIDI_H

#define MAX_CLIENTS 128
#define MAX_PORTS 16
#define MAX_DEVICES 128

int open_midi_sequencer();
void send_midi_event(int type, int value1, int value2);
void close_midi_sequencer();

#endif
