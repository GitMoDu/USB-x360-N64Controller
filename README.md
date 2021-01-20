# USB-x360-N64Controller
N64 to x360 controller conversion using Maple Mini (STM32F1).


![](https://raw.githubusercontent.com/GitMoDu/USB-x360-N64Controller/master/media/front.jpg)


![](https://raw.githubusercontent.com/GitMoDu/USB-x360-N64Controller/master/media/back.jpg)


# Features
- Controller presents as an XBox controller through USB.

- Native force feedback supported.

- Calibrated joystick.

- 2 meter braided USB cable.


# Controller Mapping
The mapping used is compatible with RetroArch emulator, so the defaults should work out of the box the menus and emulator. 

The B button is assigned to both the B and X button.


# Technical Specifications
- USB 2.0 HID XBox device.

- Update period: 3 milliseconds.

- Power draw: 
	- Min(0.250 W) (No Rumble)
	- Max(0.600 W) (Rumble full power)
	
	
# Dependencies
Task Scheduler - https://github.com/arkhipenko/TaskScheduler

IInputController Device - https://github.com/GitMoDu/IInputController

Nintendo Controler Reader - https://github.com/GitMoDu/NintendoControllerReader

USB HID Composite- https://github.com/arpruss/USBComposite_stm32f1

Arduino STM32 Core - https://github.com/rogerclarkmelbourne/Arduino_STM32
