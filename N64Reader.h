// N64Reader.h

#ifndef _N64READER_h
#define _N64READER_h

#define _TASK_OO_CALLBACKS
#define _TASK_SLEEP_ON_IDLE_RUN
#include <TaskSchedulerDeclarations.h>

#include <GamecubeControllerReaderSTM32.h>
#include <USBComposite.h>

class N64Reader : Task
{
private:
	static const uint32_t POLL_PERIOD_MS = 3;

	N64Controller Reader;

	N64Data_t RawData;

	USBXBox360* X360 = nullptr;

	struct ControllerRange
	{
		static const int8_t XMin = INT8_MIN + 5;
		static const int8_t XMax = INT8_MAX - 5;

		static const int8_t YMin = INT8_MIN + 5;
		static const int8_t YMax = INT8_MAX - 5;
	};

public:
	N64Reader(Scheduler* scheduler, const uint8_t pin)
		: Task(POLL_PERIOD_MS, TASK_FOREVER, scheduler, false)
		, Reader(pin)
	{

	}

	bool Setup(USBXBox360* x360)
	{
		X360 = x360;

		return x360 != nullptr && Reader.begin();
	}

	bool Callback()
	{
		// Do polling, cancel further work if controller is not found.
		if (!Reader.read(&RawData))
		{
			// Set neutral controls.
			X360->buttons(0);
			X360->X(0);
			X360->Y(0);

			X360->XRight(0);
			X360->YRight(0);

			return true;
		}

		// Update x360 values.
		X360->X(map(RawData.joystickX, ControllerRange::XMin, ControllerRange::XMax, INT16_MIN, INT16_MAX));
		X360->Y(map(RawData.joystickY, ControllerRange::YMin, ControllerRange::YMax, INT16_MIN, INT16_MAX));

		uint16_t Buttons = 0;

		Buttons += (RawData.buttons & (1 << N64Buttons::Right)) << XBOX_DRIGHT;
		Buttons += (RawData.buttons & (1 << N64Buttons::Left)) << XBOX_DLEFT;
		Buttons += (RawData.buttons & (1 << N64Buttons::Down)) << XBOX_DDOWN;
		Buttons += (RawData.buttons & (1 << N64Buttons::Start)) << XBOX_DUP;

		Buttons += (RawData.buttons & (1 << N64Buttons::Start)) << XBOX_START;

		//Buttons += (RawData.buttons & (1 << N64Buttons::Z)) << XBOX_X;

		Buttons += (RawData.buttons & (1 << N64Buttons::B)) << XBOX_B;
		Buttons += (RawData.buttons & (1 << N64Buttons::A)) << XBOX_A;

		Buttons += (RawData.buttons & (1 << N64Buttons::R)) << XBOX_RSHOULDER;
		Buttons += (RawData.buttons & (1 << N64Buttons::L)) << XBOX_LSHOULDER;

		X360->buttons(Buttons);

		// Map z button to left analog trigger.
		if (RawData.buttons & (1 << N64Buttons::Z))
		{
			X360->sliderLeft(UINT8_MAX);
		}
		else
		{
			X360->XRight(0);
		}

		// Mapping C-Buttons as a stick.
		// Only press when counter button is also not pressed.
		if (RawData.buttons & (1 << N64Buttons::CRight) && !(RawData.buttons & (1 << N64Buttons::CLeft))) // C-Right button.
		{
			X360->XRight(INT16_MAX);
		}
		else if (RawData.buttons & (1 << N64Buttons::CLeft)) // C-Left button.
		{
			X360->XRight(INT16_MIN);
		}
		else
		{
			X360->XRight(0);
		}

		if (RawData.buttons & (1 << N64Buttons::CDown) && !(RawData.buttons & (1 << N64Buttons::CUp))) // C-Down button.
		{
			X360->YRight(INT16_MAX);
		}
		else if (RawData.buttons & (1 << N64Buttons::CUp)) // C-Up button.
		{
			X360->YRight(INT16_MIN);
		}
		else
		{
			X360->YRight(0);
		}

		return true;
	}
};

#endif

