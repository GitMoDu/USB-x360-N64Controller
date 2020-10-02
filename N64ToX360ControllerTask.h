// N64ToX360ControllerTask.h

#ifndef _N64TOX360CONTROLLERTASK_h
#define _N64TOX360CONTROLLERTASK_h

#include <NintendoInputControllersInclude.h>
#include <USBComposite.h>
#include "PWMRumbleDriver.h"
#include <USBComposite.h>


template<typename Calibration,
	const uint8_t Pin,
	const uint32_t UpdatePeriodMillis>
	class N64ToX360ControllerTask : public  N64ControllerTask<Calibration, Pin, UpdatePeriodMillis>
{
private:
	USBXBox360* X360 = nullptr;

	struct OutputRange
	{
		static const uint16_t CutOff = 200;
		static const int16_t XMin = INT16_MIN + CutOff;
		static const int16_t XMax = INT16_MAX - CutOff;

		static const int16_t YMin = INT16_MIN + CutOff;
		static const int16_t YMax = INT16_MAX - CutOff;

		static const int16_t TriggerMin = 0;
		static const int16_t TriggerMax = UINT8_MAX - 1;
	};

	// Base class is a template, this makes calling base methods much cleaner.
	using BaseClass = N64ControllerTask<Calibration, Pin, UpdatePeriodMillis>;

	// Rumble driver.
	PWMRumbleDriver RumbleDriver;
	//

public:
	N64ToX360ControllerTask(Scheduler* scheduler) :
		N64ControllerTask<Calibration, Pin, UpdatePeriodMillis>(scheduler)
	{
	}

	bool Setup(USBXBox360* x360)
	{
		X360 = x360;

		if (X360 != nullptr)
		{
			X360->setManualReportMode(true);

			// Setup Rumble.
			RumbleDriver.Setup();

			return true;
		}
		else
		{
			return false;
		}
	}

	void UpdateRumble(const uint8_t left, const uint8_t right)
	{
		// N64 only has 1 rumble so we mix both channels.
		uint8_t raw = (uint8_t)constrain(left + right, 0, UINT8_MAX);

		RumbleDriver.UpdateRumble(raw);
	}

protected:
	virtual void OnDataUpdated()
	{
		// Update mapped x360 controls on controller read.
		MapN64ToX360();

		// Safeguard for USB disconnected but power still on.
		if (!USBComposite.isReady())
		{
			RumbleDriver.Stop();
		}
	}

	virtual void OnStateChanged(const bool connected)
	{
		if (!connected)
		{
			// Set x360 neutral controls on disconnect.
			X360->buttons(0);
			X360->position(0, 0);
			X360->positionRight(0, 0);
			X360->sliderLeft(OutputRange::TriggerMin);
			X360->sliderRight(OutputRange::TriggerMin);

			// Stop rumble.
			RumbleDriver.Stop();
		}
	}

private:
	void MapN64ToX360()
	{
		uint16_t Buttons = 0;

		// Map D-Pad.
		Buttons += (BaseClass::GetRight() & 1) << XBOX_DRIGHT - 1;
		Buttons += (BaseClass::GetLeft() & 1) << XBOX_DLEFT - 1;
		Buttons += (BaseClass::GetDown() & 1) << XBOX_DDOWN - 1;
		Buttons += (BaseClass::GetUp() & 1) << XBOX_DUP - 1;

		// Map Start.
		Buttons += (BaseClass::GetButtonHome() & 1) << XBOX_START - 1;

		// Map L and R to shoulder buttons.
		Buttons += (BaseClass::GetButton5() & 1) << XBOX_LSHOULDER - 1;
		Buttons += (BaseClass::GetButton6() & 1) << XBOX_RSHOULDER - 1;

		// Map A to A.
		Buttons += (BaseClass::GetButton0() & 1) << XBOX_A - 1;

		// Map B to B and X, for compatibility with general UIs and N64 emulators.
		Buttons += (BaseClass::GetButton1() & 1) << XBOX_B - 1;
		Buttons += (BaseClass::GetButton1() & 1) << XBOX_X - 1;

		// Map Joystick.
		uint16_t xScaled = map((int32_t)BaseClass::GetJoy1X(), 0, UINT16_MAX, OutputRange::XMin, OutputRange::XMax);
		uint16_t yScaled = map((int32_t)BaseClass::GetJoy1Y(), 0, UINT16_MAX, OutputRange::YMin, OutputRange::YMax);

		// Mapping C-Buttons as a Right Joystick.
		uint16_t CxScaled = map((int32_t)BaseClass::GetJoy2X(), 0, UINT16_MAX, OutputRange::XMin, OutputRange::XMax);
		uint16_t CyScaled = map((int32_t)BaseClass::GetJoy2Y(), 0, UINT16_MAX, OutputRange::YMin, OutputRange::YMax);

		// Mapping Z to L Slider.
		uint16_t ZSlider;
		if (BaseClass::GetButton4())
		{
			ZSlider = OutputRange::TriggerMax;
		}
		else
		{
			ZSlider = OutputRange::TriggerMin;
		}

		X360->sliderLeft(ZSlider);
		X360->buttons(Buttons);
		X360->position(xScaled, yScaled);
		X360->positionRight(CxScaled, CyScaled);
		X360->send();

	}
};
#endif