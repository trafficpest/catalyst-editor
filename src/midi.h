#ifndef MIDI_H
#define MIDI_H

int open_midi_sequencer();
void send_midi_event(int type, int channel, int value1, int value2);
void close_midi_sequencer();

#endif
