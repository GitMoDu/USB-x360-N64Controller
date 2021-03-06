// N64Controllers.h

#ifndef _N64CONTROLLERS_h
#define _N64CONTROLLERS_h

// N64 controller calibrations.
// Calibration is different for each controller.

//struct N64ControllerConfiguration
//{
//	const char* SerialNumber = "00000000000000000001";
//	static const uint32_t ControllerPin = PA4;
//	static const uint8_t LedBrightness = 0;
//
//	static const int8_t		JoyXMin = -82;
//	static const int8_t		JoyXMax = 79;
//	static const int8_t		JoyXOffset = -1;
//
//	static const int8_t		JoyYMin = -81;
//	static const int8_t		JoyYMax = 83;
//	static const int8_t		JoyYOffset = 1;
//
//	static const uint8_t	JoyDeadZoneRadius = 0;
//};

//struct N64ControllerConfiguration
//{
//	const char* SerialNumber = "00000000000000000002";
//	static const uint32_t ControllerPin = 31;
//	static const uint8_t LedBrightness = 8;
//
//
//	static const int8_t		JoyXMin = -80;
//	static const int8_t		JoyXMax = 80;
//	static const int8_t		JoyXOffset = 0;
//
//	static const int8_t		JoyYMin = -80;
//	static const int8_t		JoyYMax = 80;
//	static const int8_t		JoyYOffset = 0;
//
//	static const uint8_t	JoyDeadZoneRadius = 1;
//};

// Serial 2 upgraded with JoyBus over Serial.
struct N64ControllerConfiguration
{
	const char* SerialNumber = "00000000000000000003";
	static const uint8_t LedBrightness = 8;

	static const int8_t		JoyXMin = -80;
	static const int8_t		JoyXMax = 80;
	static const int8_t		JoyXOffset = 0;

	static const int8_t		JoyYMin = -80;
	static const int8_t		JoyYMax = 80;
	static const int8_t		JoyYOffset = 0;

	static const uint8_t	JoyDeadZoneRadius = 0;
};
#endif