// midi.c
#include <alsa/asoundlib.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include "midi.h"

static snd_seq_t *seq_handle = NULL;
static int port_id = 0;
//static int client_id = 128;

int open_midi_sequencer() {
    // Open the ALSA sequencer
    int status = snd_seq_open(&seq_handle, "default", SND_SEQ_OPEN_OUTPUT, 0);
    if (status < 0) {
        fprintf(stderr, "Error opening ALSA sequencer: %s\n", snd_strerror(status));
        return -1;
    }

    // Set the client name
    snd_seq_set_client_name(seq_handle, "Line6 Catalyst Editor");

    // Create a port
    port_id = snd_seq_create_simple_port(seq_handle, "MIDI Output",
                                         SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_SUBS_READ,
                                         SND_SEQ_PORT_TYPE_APPLICATION);
    if (port_id < 0) {
        fprintf(stderr, "Error creating ALSA sequencer port: %s\n", snd_strerror(port_id));
        snd_seq_close(seq_handle);
        return -1;
    }

    return 0;
}

void close_midi_sequencer() {
    if (seq_handle) {
        snd_seq_close(seq_handle);
    }
}

void send_midi_event(int type, int value1, int value2) {
    if (!seq_handle || port_id < 0) {
      fprintf(stderr, "Error: ALSA sequencer or port not initialized.\n");
      return;
    }
    snd_seq_event_t ev;

    // Initialize and clear the event structure
    snd_seq_ev_clear(&ev);

    // Set the event type and data
    switch (type) {
        case SND_SEQ_EVENT_CONTROLLER:
            ev.type = SND_SEQ_EVENT_CONTROLLER;
            ev.data.control.param = value1;
            ev.data.control.value = value2;
            break;
        case SND_SEQ_EVENT_PGMCHANGE:
            ev.type = SND_SEQ_EVENT_PGMCHANGE;
            ev.data.control.param = value1;
            break;
        default:
            fprintf(stderr, "Unsupported MIDI event type: %d\n", type);
            return;
    }

    // Debugging
    //printf("Sending MIDI event: Type: %d, Value1: %d, Value2: %d, Client: %d, Port: %d\n", type, value1, value2, client_id, port_id);

    // Set the source and destination
    snd_seq_ev_set_source(&ev, port_id);
    snd_seq_ev_set_subs(&ev);
    snd_seq_ev_set_direct(&ev);
 
    snd_seq_event_output(seq_handle, &ev);

    // Drain the output to ensure the event is sent
    snd_seq_drain_output(seq_handle);
}



