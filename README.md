# EEG Signal Acquisition

## Description
This project involves the hardware, firmware, and software required for acquiring EEG signals.

## Table of Contents
- [Introduction](#introduction)
- [Features](#features)
- [Hardware Requirements](#hardware-requirements)
- [Software Requirements](#software-requirements)
- [Installation](#installation)
- [Usage](#usage)
- [Firmware Development](#firmware-development)
- [PCB Design](#pcb-design)
- [Troubleshooting](#troubleshooting)
- [License](#license)
- [Acknowledgements](#acknowledgements)
- [Contact](#contact)

## Introduction
In this project, we designed a PCB, its related firmware, and desktop software to acquire EEG signals. The PCB can acquire signals from multiple channels simultaneously.

## Features
- Acquires EEG signals from multiple channels
- Supports real-time signal processing
- User-friendly interface

## Hardware Requirements
- ESP32 microcontroller
- FT232RL USB to UART interface
- ADS1299 (or similar ADC)
- Resistors and capacitors
- Electrodes for EEG signal acquisition

## Software Requirements
- Python
- asyncio
- numpy
- pymongo
- aiohttp
- aiohttp_cors
- mne

## Installation
1. Clone the repository.
2. Install the required Python packages:
   ```bash
   pip install -r requirements.txt

3. Run the `server-merged.py` script:

   ```bash
   python server-merged.py

4. Open index.html in a web browser to access the user interface.



## Usage

1. Assemble the PCB.
2. Program the ESP32 microcontroller with the provided firmware.
3. Connect the electrodes to the subject.
4. Use the UI in the software folder to start acquiring EEG signals. The signals will be displayed in real-time and can be saved for further analysis.

## Firmware Development

The ESP32 microcontroller receives EEG signals from the ADS1299 via SPI and sends them to the host system via Wi-Fi. The host system processes and displays the signals in real-time.

## PCB Design

The PCB was designed using Altium with an emphasis on minimizing noise and ensuring signal integrity.

## Troubleshooting

If you encounter noise or signal quality issues:

- Ensure all connections are secure.
- Use shielded cables for connections.
- Verify that the electrodes are properly placed and making good contact with the skin.


## Acknowledgements

Thanks to the Mehbang Group R&D team for their guidance.

# Contact

For any inquiries, please contact [mahdi.sharif20001@gmail.com](mailto:mahdi.sharif20001@gmail.com).

Feel free to let me know if there are any additional changes or details you'd like to include.


