# Xbox One controller for EPS4400-pico

The main goals for this tool is to provide:
* Easy to use (mainly for debugging & development of new features) controller for [eps4400-pico](https://github.com/OpenCorsa/eps4400-pico) board
* Alternative way of controlling car's steering wheel (i.e. for people with disabilities)
* Car remote steering mechanism (depending on your controller's range)

Project requirements:
* Linux machine (i.e. Raspberry Pi) with CAN-Bus kernel module
* Xbox One pad

## Building
* Clone using `git clone --recurse-submodules https://github.com/OpenCorsa/eps4400-xpad.git`
* Import project in IDE of choice or simply run `cmake . && make`

## Controller mapping
* Xbox button - toggle state (enable or disable output)
* Left joystick - steer using x-axis
* Right joystick - zero-out position by pressing
* Menu button - toggle angle & torque steering modes (only when output is active)

## To-do
* [ ] Handle CAN-Bus output to sync with board state (listen to output)
