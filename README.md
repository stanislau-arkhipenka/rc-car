# Remote Control for hobby models
This is arduino project to add remote control to hobby models using generally available components.

This project consists of four main "parts":
* Controller
* rc_car_v1
* rc_tank_v2
* Debugging

## Controller
[Mode details and STL files](https://zzbot.org/projects/remote-controller/)
### Hardware
To build controller you need following components:
* Arduino Uno
* Joystick shield v1.A
* nRF24l01 module
* 9-volt battery
* Nine-volt battery snap
* DC-DC converter (from 9v to 5v)

### Software
There are some configuration variables defined in the source code for tuning your controller.
Other than that nothing special- just upload sketch to your uno board and you ready to go.

## RC Car v1
Software to control RC Car
[STL files](https://www.thingiverse.com/thing:5421100)
### Hardware
To build vehicle you will need following components:
* Arduino micro
* nRF24l01 module
* 9-volt battery
* Nine-volt battery snap
* DC-DC converter (from 9v to 5v)
* Arduino compatible DC motor board (or shield) 

### Software
Nothing special here- upload your sketch and you ready to go.

## RC Tank v2
Software to control RC Tank, that is based on arduino.
It was designed for [this tank](https://zzbot.org/projects/rc-tank-2/)

## Debugging
### Hardware
To build debugging kit you need following components:
* Arduino Uno
* nRF24l01 module
