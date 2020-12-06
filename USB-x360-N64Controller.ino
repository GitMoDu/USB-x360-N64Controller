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
#define _TASK_SLEEP_ON_IDLE_RUN
#include <TaskScheduler.h>

#include "N64ToX360ControllerTask.h"
#include <USBComposite.h>
#include "N64Controllers.h"

// Device info.
const uint16_t VendorId = 0x057E;	// Nintendo Co. Ltd.
const uint16_t ProductId = 64;		// Nintendo 64.

const char ManufacturerName[] = "Nintendo";
const char DeviceName[] = "Nintendo 64 Controller";
//

// Process scheduler.
Scheduler SchedulerBase;
//

// XBox controller API.
USBXBox360 XBox360;
//

// ~5 updates per frame (assuming 60 FPS) should present the most up to date values without saturating the USB HID interface.
const uint32_t ControllerUpdatePeriodMillis = 3;

N64ToX360ControllerTask<N64ControllerConfiguration, N64ControllerConfiguration::ControllerPin, ControllerUpdatePeriodMillis>
	Controller(&SchedulerBase, &XBox360, N64ControllerConfiguration::LedBrightness);
//

void setup()
{
	// Disable Serial USB. Not really needed, but just to make sure.
	Serial.end();

	// Set up device info to usb composite.
	USBComposite.setManufacturerString(ManufacturerName);
	USBComposite.setProductString(DeviceName);
	USBComposite.setSerialString(N64ControllerConfiguration().SerialNumber);
	USBComposite.setVendorId(VendorId);
	USBComposite.setProductId(ProductId);

	// Start USB the device.
	XBox360.begin();
	while (!USBComposite);

	// Set API callbacks to rumble handler.
	XBox360.setRumbleCallback(RumbleCallback);

	// Start the controller.
	Controller.Start();
}

void RumbleCallback(const uint8_t left, const uint8_t right)
{
	Controller.UpdateRumble(left, right);
}

void loop()
{
	SchedulerBase.execute();
}