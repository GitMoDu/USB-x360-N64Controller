
//https://github.com/arpruss/USBComposite_stm32f1
#include <USBComposite.h>

#define _TASK_OO_CALLBACKS
#define _TASK_SLEEP_ON_IDLE_RUN
#include <TaskScheduler.h>

#include "N64Reader.h"
#include "RumbleDriverSTM32F1.h"

// Device info.
const uint16_t VendorId = 0x057E; // Nintendo Co. Ltd.
const uint16_t ProductId = 64;

const char ManufacturerName[] = "Amperum";
const char DeviceName[] = "Nintendo 64 Controller";
const char DeviceSerial[] = "13370133701337000001";
//

// Process scheduler.
Scheduler SchedulerBase;
//

// XBox controller API.
USBXBox360 XBox360;
//

// Rumble driver.
RumbleDriver Rumble(&SchedulerBase);
//

// N64 controller reader.
const uint8_t ReaderPin = PA5;
N64Reader Reader(&SchedulerBase, ReaderPin);
//

void setup() {

	// Set up device info to usb composite.
	USBComposite.setManufacturerString(ManufacturerName);
	USBComposite.setProductString(DeviceName);
	USBComposite.setSerialString(DeviceSerial);
	USBComposite.setVendorId(VendorId);
	USBComposite.setProductId(ProductId);
	
	// Set API callbacks to handlers.
	XBox360.setRumbleCallback(RumbleCallback);
	XBox360.setLEDCallback(LEDCallback);

	// Setup Rumble.
	Rumble.Setup();

	// Set up Lights.

	// Set up the controller reader.
	Reader.Setup(&XBox360);

	// Start the device.
	XBox360.begin();
	while (!USBComposite);
}

void RumbleCallback(uint8_t left, uint8_t right)
{
	Rumble.UpdateInput(left, right);
}

void LEDCallback(uint8_t pattern)
{

}

void loop() 
{
	SchedulerBase.execute();
}
