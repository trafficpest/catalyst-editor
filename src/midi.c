// midi.c
#include <alsa/asoundlib.h>
#include <ncurses.h>
#include <stdio.h>
#include <alloca.h>
#include <stdlib.h>
#include <string.h>
#include "ui.h"
#include "midi.h"

static snd_seq_t *seq_handle = NULL;
static int port_id = 0;

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

void send_all_midi_events(Setting *settings, int num_settings) {
    for (int i = 0; i < num_settings; ++i) {
        Setting *setting = &settings[i];
        send_midi_event(SND_SEQ_EVENT_CONTROLLER, setting->control_number, setting->value);
    }
}

MidiClient* get_midi_clients(int *client_count) {
    snd_seq_client_info_t *client_info;
    snd_seq_port_info_t *port_info;

    // Allocate stack space for client and port info
    snd_seq_client_info_alloca(&client_info);
    snd_seq_port_info_alloca(&port_info);

    // Allocate memory for clients
    MidiClient *clients = malloc(MAX_CLIENTS * sizeof(MidiClient));
    if (!clients) {
        *client_count = 0;
        return NULL;
    }

    int count = 0;
    snd_seq_client_info_set_client(client_info, -1);
    while (snd_seq_query_next_client(seq_handle, client_info) >= 0) {
        int client = snd_seq_client_info_get_client(client_info);
        snd_seq_port_info_set_client(port_info, client);
        snd_seq_port_info_set_port(port_info, -1);

        while (snd_seq_query_next_port(seq_handle, port_info) >= 0) {
            if (count >= MAX_CLIENTS) break;

            clients[count].client = client;
            snprintf(clients[count].client_name, sizeof(clients[count].client_name), "%s", snd_seq_client_info_get_name(client_info));
            snprintf(clients[count].port_name, sizeof(clients[count].port_name), "%s", snd_seq_port_info_get_name(port_info));
            count++;
        }
    }

    *client_count = count;
    return clients;
}

void subscribe_to_midi_port(int input_client, int input_port, int output_client, int output_port) {
    snd_seq_addr_t sender, dest;
    snd_seq_port_subscribe_t *subs;

    snd_seq_port_subscribe_alloca(&subs);

    sender.client = input_client;
    sender.port = input_port;
    dest.client = output_client;
    dest.port = output_port;

    snd_seq_port_subscribe_set_sender(subs, &sender);
    snd_seq_port_subscribe_set_dest(subs, &dest);

    // Set subscription attributes
    snd_seq_port_subscribe_set_exclusive(subs, 1);
    snd_seq_port_subscribe_set_time_update(subs, 1);
    snd_seq_port_subscribe_set_time_real(subs, 1);

    if (snd_seq_subscribe_port(seq_handle, subs) < 0) {
        fprintf(stderr, "Failed to subscribe to MIDI port.\n");
    }
}

int get_app_client_id() {
    if (!seq_handle || port_id < 0) {
      fprintf(stderr, "Error: ALSA sequencer or port not initialized.\n");
      return -1;
    }
    int client_id = snd_seq_client_id(seq_handle);
    if (client_id < 0) {
        fprintf(stderr, "Error retrieving client ID: %s\n", snd_strerror(client_id));
        return -1;
    }
    return client_id;
}
