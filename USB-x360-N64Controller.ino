// Dependencies:
// Task Scheduler - https://github.com/arkhipenko/TaskScheduler
//
// IInputController Device - https ://github.com/GitMoDu/IInputController
//
// Nintendo Controler Reader - https ://github.com/GitMoDu/NintendoControllerReaderSTM32
//
// USB HID Composite - https ://github.com/arpruss/USBComposite_stm32f1
//
// Arduino STM32 Core - https ://github.com/rogerclarkmelbourne/Arduino_STM32


#define _TASK_OO_CALLBACKS
#include <TaskScheduler.h>

#include "N64ToX360ControllerTask.h"
#include <USBComposite.h>

// Device info.
const uint16_t VendorId = 0x057E;	// Nintendo Co. Ltd.
const uint16_t ProductId = 64;		// Nintendo 64.

const char ManufacturerName[] = "Nintendo";
const char DeviceName[] = "Nintendo 64 Controller";
const char DeviceSerial[] = "00000000000000000001";
//

// Process scheduler.
Scheduler SchedulerBase;
//

// XBox controller API.
USBXBox360 XBox360;
//

// Rumble driver.
static const uint8_t RumbleDriverPin = PA0;
//

// LED driver.
static const uint8_t LEDPin = LED_BUILTIN;
//

// N64 controller reader and calibrations.

class N64ControllerCalibration
{
public:
	// Calibration is different for each controller.

	//Joystick.
	static const int8_t		JoyXMin = -80;
	static const int8_t		JoyXMax = 80;
	static const int8_t		JoyXOffset = 0;

	static const uint8_t	JoyYMin = -80;
	static const uint8_t	JoyYMax = 80;
	static const int8_t		JoyYOffset = 0;

	static const uint8_t	JoyDeadZoneRadius = 1;
};
const uint32_t CONTROLLER_PIN = PA4;

// Motor is rated for ~3.0V but is supplied with 5.0V.
const uint8_t MaxPWM = map(3000, 0, 5000, 0, UINT8_MAX);

// 2 updates per frame (assuming 60 FPS) should present the most up to date values without saturating the USB HID interface.
const uint32_t ControllerUpdatePeriodMillis = 8; 

N64ToX360ControllerTask<N64ControllerCalibration, CONTROLLER_PIN, ControllerUpdatePeriodMillis> Controller(&SchedulerBase, &XBox360);
//

void setup()
{
	// Disable Serial USB. Not really needed, but just to make sure.
	Serial.end();

	// Set up device info to usb composite.
	USBComposite.setManufacturerString(ManufacturerName);
	USBComposite.setProductString(DeviceName);
	USBComposite.setSerialString(DeviceSerial);
	USBComposite.setVendorId(VendorId);
	USBComposite.setProductId(ProductId);

	// Setup Rumble.
	pinMode(RumbleDriverPin, WiringPinMode::PWM);

	// Set Rumble to off at setup.
	RumbleOff();

	// Set up controller with rumble off callback.
	Controller.Setup(RumbleOff);

	// Start the device.
	XBox360.begin();
	while (!USBComposite);

	// Set API callbacks to rumble handler.
	XBox360.setRumbleCallback(RumbleCallback);

	// Start the controller.
	Controller.StartController();
}

void RumbleCallback(const uint8_t left, const uint8_t right)
{
	// N64 only has 1 rumble so we mix both channels.
	uint8_t raw = (uint8_t)constrain(left + right, 0, UINT8_MAX);

	// Scale power by lowered max PWM.
	analogWrite(RumbleDriverPin, map(raw, 0, UINT8_MAX, 0, MaxPWM));
}

void RumbleOff()
{
	analogWrite(RumbleDriverPin, 0);
}

void loop()
{
	SchedulerBase.execute();
}
