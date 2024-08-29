#include <alsa/asoundlib.h>
#include <stdio.h>
#include <stdlib.h>

static snd_seq_t *seq_handle;
static int port_id;

// Function to open the MIDI sequencer
int open_midi_sequencer() {
    if (snd_seq_open(&seq_handle, "default", SND_SEQ_OPEN_OUTPUT, 0) < 0) {
        fprintf(stderr, "Error opening ALSA sequencer\n");
        return -1;
    }

    snd_seq_set_client_name(seq_handle, "Line6 Catalyst Editor");

    port_id = snd_seq_create_simple_port(seq_handle, "MIDI Output",
        SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_SUBS_READ,
        SND_SEQ_PORT_TYPE_APPLICATION);

    if (port_id < 0) {
        fprintf(stderr, "Error creating ALSA sequencer port\n");
        snd_seq_close(seq_handle);
        return -1;
    }

    return 0;
}

// Function to close the MIDI sequencer
void close_midi_sequencer() {
    if (seq_handle) {
        snd_seq_close(seq_handle);
        seq_handle = NULL;
    }
}

void send_midi_event(snd_seq_event_t *ev, int type, int value1, int value2, int port_id, int client_id) {

    open_midi_sequencer();

    // Initialize the event structure
    snd_seq_ev_clear(ev);

    // Set the event type
    switch (type) {
        case SND_SEQ_EVENT_CONTROLLER:
            ev->type = SND_SEQ_EVENT_CONTROLLER;
            ev->data.control.param = value1;  // Controller number
            ev->data.control.value = value2;  // Controller value
            break;
        case SND_SEQ_EVENT_PGMCHANGE:
            ev->type = SND_SEQ_EVENT_PGMCHANGE;
            ev->data.control.param = value1;  // Program number
            break;
        default:
            return; // Unsupported event type
    }

    // Set the destination
    snd_seq_ev_set_dest(ev, client_id, port_id);

    // Send the event
    // snd_seq_event_output() and snd_seq_event_output_pending() should be used to actually send the event
    close_midi_sequencer();
}

