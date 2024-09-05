# Line6 Catalyst Editor

## Overview

The Line6 Catalyst Editor is a terminal-based application for managing and configuring Line 6 Catalyst amplifiers via MIDI. It provides a user-friendly interface to adjust various amplifier settings using `ncurses` and the ALSA sequencer.

## Features

- **Settings Management**: Adjust various amplifier settings such as Gain, Boost Level, and FX parameters.
- **MIDI Control**: Send and receive MIDI messages to control the amplifier.
- **User Interface**: Navigate and adjust settings through a clean, text-based interface.

## Installation

1. **Clone the Repository**:
   ```sh
   git clone <repository-url>
   cd <repository-directory>
   ```

2. **Install Dependencies**:  
   Make sure you have `ncurses` and `alsa-lib` installed on your system. On Debian-based systems, you can install them with:
   ```sh
   sudo apt-get install libncurses5-dev libasound2-dev
   ```

3. **Build the Application**:
   ```sh
   make
   ```

4. **Run the Application**:
   ```sh
   ./build/bin/line6_catalyst_editor
   ```

## Usage

- **Arrow Keys**: Navigate through the settings.
- **Enter**: Open a slider popup to adjust the selected setting.
- **q**: Quit the application.

## Connecting to MIDI Devices

To connect the Line6 Catalyst Editor to your MIDI device, you can use ALSA’s `aconnect` command.

1. **List Available MIDI Ports**:
   Use the command below to list all available MIDI input and output ports:
   ```sh
   aconnect -l
   ```
   This command will display a list of MIDI devices and their corresponding client numbers and ports.

2. **Identify Your MIDI Devices**:
   Look for the output that corresponds to your MIDI interface (e.g., `MidiSport 1x1`) and the Line6 Catalyst Editor (e.g., `Line6 Catalyst Editor`).

3. **Connect the Editor to the MIDI Device**:
   To connect the Line6 Catalyst Editor to the MIDI device, use the `aconnect` command with the respective client and port numbers:
   ```sh
   aconnect <Catalyst_Client>:<Catalyst_Port> <MIDI_Device_Client>:<MIDI_Device_Port>
   ```
   For example, if the Catalyst Editor is client `128:0` and your MIDI device is `20:0`, the command would be:
   ```sh
   aconnect 128:0 20:0
   ```
   This command routes the MIDI output of the Line6 Catalyst Editor to the input of your MIDI device.

4. **Verify the Connection**:
   Re-run `aconnect -l` to verify that the ports are connected correctly.

## File Descriptions

- **`main.c`**: Contains the main application logic, settings definition, and user interaction handling.
- **`midi.c`**: Manages MIDI sequencer operations such as opening/closing the sequencer and sending MIDI events.
- **`midi.h`**: Header file defining MIDI operations.
- **`ui.c`**: Manages the user interface, including the main menu and slider popups.
- **`ui.h`**: Header file defining UI functions and structures.

## Configuration

Settings are defined in `main.c` and include various parameters such as:

- **Amp Voicing Type**
- **Boost Level**
- **Gain**
- **Effect 1 Off/On**

Each setting has associated MIDI Control Change numbers and value ranges.

## Troubleshooting

- **ALSA Errors**: Ensure ALSA sequencer is correctly configured and that the application has access to MIDI ports.
- **ncurses Initialization**: Ensure `ncurses` library is installed and linked correctly during compilation.

## License

This project is licensed under the MIT License. See the `LICENSE` file for details.

