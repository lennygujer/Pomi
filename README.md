# Pomi

Pomi is an open-source, DIY focus timer designed to help people learn and work with more focus and fewer distractions.

It is a small physical device built around a microcontroller, an OLED display, a rotary potentiometer, a push button, a buzzer, and an optional vibration motor.

The idea behind Pomi is simple:

> Make focused work easier by turning time management into something physical, visible, and distraction-free.

Unlike a phone-based timer, Pomi does not require opening an app, checking notifications, or interacting with a device full of distractions. You can put Pomi on your desk, configure your session, and focus on the work in front of you.

## Features

- Configurable total work time
- Configurable break duration
- Configurable number of work sessions
- Live calculation of work-session duration
- 128 × 64 SSD1306 OLED display
- Remaining-time display in MM:SS format
- Session progress bar
- Work-session numbering
- Physical push-button control
- Rotary potentiometer for configuration
- Audible feedback
- Optional vibration feedback
- Three-second countdown before starting
- Session cancellation confirmation
- Session-complete screen
- No smartphone or app required
- No custom PCB required
- No EEPROM or persistent storage required

## How It Works

Pomi uses a simple interaction model:

Turn the potentiometer to choose a value.

Press the button to confirm.

A session is configured in four steps:

1. Total work time
2. Break duration
3. Number of work sessions
4. Confirm and start

After the configuration is complete, Pomi starts a three-second countdown and then begins the first work session.

## Work Session Calculation

Pomi makes the relationship between total work time and individual work sessions visible before the timer starts.

The selected total work time is divided by the selected number of work sessions.

For example:

Total work time: 120 minutes

Number of work sessions: 4

120 / 4 = 30 minutes

Pomi therefore creates four 30-minute work sessions.

The calculation is displayed directly while configuring the number of sessions.

If the potentiometer is turned, the calculation changes immediately.

This allows the user to see exactly how the selected number of sessions affects the length of each work session before starting.

### Session Structure

The configured number determines how many work sessions the total work time is divided into.

Breaks are placed between work sessions.

For example, four work sessions consist of:

1. Work session
2. Break
3. Work session
4. Break
5. Work session
6. Break
7. Work session
8. Session complete

This means that four work sessions contain three breaks between them.

The break duration is independently configurable.

## User Interface

### Startup

When Pomi is powered on, it displays the Pomi logo and a short startup message.

After the startup screen, the configuration menu opens automatically.

### Total Work Time

The first menu allows the user to select the total amount of work time.

The available range is 5 to 480 minutes.

The value is selected in five-minute increments.

### Break Time

The second menu controls the duration of each break.

The available range is 1 to 60 minutes.

### Number of Work Sessions

The third menu controls how many work sessions the total work time is divided into.

The current range is 1 to 20 work sessions.

The screen also displays the calculation.

For example, if the total work time is 120 minutes and four work sessions are selected, Pomi calculates:

120 min / 4 = 30 min

If five work sessions are selected:

120 min / 5 = 24 min

This calculation is shown before the session starts.

The firmware currently uses the term "BREAKS" in some menu text, although the selected value determines the number of work sessions. This terminology may be changed to "WORK SESSIONS" in a future firmware version.

### Ready Screen

Before starting, Pomi displays a final overview of the selected configuration.

The user can see the number of work sessions, the duration of each work session, and the break duration.

Pressing the button starts the countdown.

## Countdown

After pressing the button on the ready screen, Pomi starts a three-second countdown.

The first work session begins immediately after the countdown.

## Work Sessions

During a work session, Pomi displays the current session number and remaining time.

For example, during the second session out of four, the display shows the current session and the remaining time.

A progress bar at the bottom of the display provides a visual indication of progress.

The timer is displayed in MM:SS format.

## Breaks

During a break, Pomi displays the remaining break time.

When the break finishes, Pomi automatically starts the next work session.

## Sound Feedback

Pomi provides different audible feedback for different events.

The current firmware includes sounds for:

- Button presses
- Countdown
- Starting a work session
- Halfway point
- Work-session completion
- Break completion
- Complete session
- Session cancellation

The sound switch can be used to disable audible feedback.

This allows Pomi to be used in situations where sound would be distracting.

## Vibration Feedback

Pomi can provide haptic feedback using a vibration motor.

The vibration motor can be enabled or disabled using the vibration switch.

For work and break periods longer than ten minutes, Pomi provides a halfway notification.

The halfway notification consists of a short vibration and a subtle sound.

Sessions shorter than ten minutes do not receive halfway feedback.

## Cancelling a Session

Pressing the button during a work session or break opens a confirmation screen instead of immediately ending the session.

The potentiometer is used to select between NO and YES.

Pressing the button confirms the selection.

If NO is selected, Pomi returns to the current work or break session.

If YES is selected, the current session is cancelled and Pomi returns to the configuration menu.

## Session Complete

After the final work session, Pomi displays a session completion message.

The display also shows the total amount of focused work time.

The completion screen remains visible for several seconds before Pomi returns to the configuration menu.

A distinctive sound pattern is also played.

## Hardware

Pomi is designed to use commonly available electronic components.

The current prototype does not use a custom PCB.

The electronics are assembled from individual components and modules.

The current hardware consists of:

- Arduino-compatible microcontroller
- 128 × 64 SSD1306 OLED display
- Rotary potentiometer
- Push button
- Buzzer
- Vibration motor
- Sound switch
- Vibration switch

The exact components and wiring will be documented in the electronics directory.

## Electronics

The current firmware uses the following connections:

| Component | Pin |
|---|---|
| Push button | D12 |
| Potentiometer | A0 |
| Buzzer | D9 |
| Vibration motor | D6 |
| Sound switch | D10 |
| Vibration switch | D11 |
| OLED | I2C |

The OLED uses I2C and is configured for address 0x3C.

There is currently no custom PCB.

The goal is to keep the electronics accessible and reproducible using commonly available components.

A complete schematic and wiring documentation will be included in the electronics directory.

## 3D-Printed Parts

The physical enclosure and mechanical components of Pomi are designed to be 3D printed.

The repository will contain the printable files required to build the device.

Where possible, both printable files and original CAD files will be provided.

The planned structure is:

3d-models/
- enclosure/
- buttons/
- mounts/
- other-parts/

Print settings and assembly instructions will be documented as the mechanical design develops.

## Firmware

The Pomi firmware is written for Arduino-compatible microcontrollers.

It uses the following libraries:

- Wire
- Adafruit GFX
- Adafruit SSD1306

The firmware is organized around several states:

- STARTUP
- MENU
- COUNTDOWN
- WORK
- BREAK
- CANCEL_CONFIRM
- SESSION_COMPLETE

This state-based architecture keeps the different parts of the user interface and timer behavior separate.

Timing is handled using millis() rather than blocking the program during work and break periods.

The button uses software debouncing.

The potentiometer input is smoothed using a weighted moving average so that the selected value does not jump unnecessarily.

The firmware does not use EEPROM or persistent storage.

Session configuration exists only for the current session and is reset when Pomi is restarted.

## Repository Structure

The planned repository structure is:

pomi/
- README.md
- firmware/
  - pomi/
    - pomi.ino
- electronics/
  - schematic/
  - wiring/
  - bill-of-materials.md
- 3d-models/
  - enclosure/
  - buttons/
  - mounts/
  - other-parts/
- documentation/
  - assembly.md
  - firmware.md
  - development.md
- LICENSE

The structure can evolve as the project grows.

## Getting Started

### 1. Clone the Repository

Clone the repository and open the project on your computer.

### 2. Install the Required Libraries

Install the following libraries through the Arduino IDE Library Manager:

- Adafruit GFX Library
- Adafruit SSD1306

The Wire library is included with the Arduino environment.

### 3. Connect the Electronics

Connect the components according to the schematic and wiring documentation in the electronics directory.

### 4. Upload the Firmware

Open firmware/pomi/pomi.ino in the Arduino IDE.

Select the appropriate microcontroller board and port, then upload the firmware.

### 5. Print the Parts

Print the required components from the 3d-models directory.

### 6. Assemble Pomi

Follow the assembly documentation once the electronics and 3D-printed components are ready.

## Current Status

Pomi V2 is a software-focused prototype.

The core timer, configuration system, feedback system, cancellation system, and user interface are implemented in the current firmware.

The physical enclosure, electronics documentation, schematic, and 3D models are still being developed.

The current firmware should therefore be considered an early development version rather than a final product.

## Roadmap

- [ ] Finalize the 3D-printed enclosure
- [ ] Complete the electronics schematic
- [ ] Complete the bill of materials
- [ ] Add detailed assembly instructions
- [ ] Improve OLED graphics
- [ ] Refine the session calculation model
- [ ] Improve sound patterns
- [ ] Improve vibration patterns
- [ ] Add additional session modes
- [ ] Improve firmware documentation
- [ ] Test different microcontrollers
- [ ] Test different displays
- [ ] Optimize power consumption
- [ ] Improve accessibility of the interface
- [ ] Add community build documentation

The roadmap is intentionally flexible. Pomi is an open project and its development can evolve through testing, contributions, and community feedback.

## Contributing

Pomi is open source, and contributions are welcome.

There are many ways to contribute:

- Improve the firmware
- Design or improve 3D-printed parts
- Improve the electronics
- Test the device
- Report bugs
- Improve documentation
- Suggest features
- Build your own Pomi
- Share modifications
- Create alternative enclosure designs

If you build your own Pomi, sharing your build and modifications can help improve the project for everyone.

## Design Philosophy

Pomi is not intended to make people work more.

It is intended to make focused time easier to create.

Modern devices provide access to information, communication, entertainment, and notifications. While useful, these things can also make uninterrupted attention more difficult.

Pomi takes a different approach.

It is a dedicated physical object whose purpose is to help structure focused time without requiring another screen-based application.

Set the timer.

Put distractions aside.

Focus on the task.

Take a break.

Repeat.

## Open Source

Pomi is designed to be transparent and reproducible.

The goal is to provide the source code, electronics documentation, and 3D-printable mechanical parts so that anyone can understand how the device works, build their own version, modify it, and contribute improvements.

The project does not require a custom PCB.

The electronics are based on individual components and commonly available modules, making the project easier to experiment with and reproduce.

## License

This project is open source.

Specific licensing information for the firmware, hardware documentation, 3D models, and other project files will be added to the repository.

## Project Status

Pomi is actively being developed.

The hardware, firmware, 3D models, and documentation may change as the project evolves.

If you are interested in building Pomi, experimenting with the design, or contributing to the project, you are welcome to follow its development here.
