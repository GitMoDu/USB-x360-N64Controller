// N64ToX360ControllerTask.h

#ifndef _N64TOX360CONTROLLERTASK_h
#define _N64TOX360CONTROLLERTASK_h

#include <NintendoInputControllersInclude.h>
#include <USBComposite.h>
#include "PWMRumbleDriver.h"
#include <USBComposite.h>


template<typename Calibration,
	const uint32_t UpdatePeriodMillis>
	class N64ToX360ControllerTask : public virtual IDispatcher
{
private:
	struct OutputRange
	{
		static const uint16_t CutOff = 5;
		static const int16_t XMin = INT16_MIN + CutOff;
		static const int16_t XMax = INT16_MAX - CutOff;

		static const int16_t YMin = INT16_MIN + CutOff;
		static const int16_t YMax = INT16_MAX - CutOff;

		static const int16_t TriggerMin = 0;
		static const int16_t TriggerMax = UINT8_MAX - 1;
	};

	// Rumble driver.
	PWMRumbleDriver RumbleDriver;
	//

	// Controller driver with dispatcher.
	N64ControllerTask<Calibration, UpdatePeriodMillis> ControllerDriver;
	//

	// XBox 360 Controller instance.
	USBXBox360* X360 = nullptr;

	// Local tracking of USB failure.
	bool CompositeFailed = true;

	// LED indicator.
	static const uint8_t LEDPin = LED_BUILTIN;
	const uint8_t LedBrightness;
	//

public:
	N64ToX360ControllerTask(Scheduler* scheduler, USBXBox360* x360, HardwareSerial* serial, const uint8_t ledBrightness = 8)
		: ControllerDriver(scheduler, serial)
		, RumbleDriver()
		, X360(x360)
		, LedBrightness(ledBrightness)
	{
		ControllerDriver.SetDispatcher(this);

		if (X360 != nullptr)
		{
			X360->setManualReportMode(true);
		}

		pinMode(LEDPin, PWM);
		analogWrite(LEDPin, 0);
	}

	void Start()
	{
		if (X360 != nullptr)
		{
			ControllerDriver.StartController();
		}
	}

	void Stop()
	{
		ControllerDriver.StopController();
	}

	void UpdateRumble(const uint8_t left, const uint8_t right)
	{
		// N64 only has 1 rumble so we mix both channels.
		uint8_t raw = min(left + right, UINT8_MAX);

		RumbleDriver.UpdateRumble(raw);
	}

protected:
	virtual void OnDataUpdated()
	{
		// Safeguard for USB disconnected but power still on.
		if (USBComposite.isReady())
		{
			// Update mapped x360 controls on controller read and USB ok.
			MapN64ToX360();

			// Check if previous USB state was FAIL, show LED if it's back.
			if (CompositeFailed)
			{
				CompositeFailed = false;
				analogWrite(LED_BUILTIN, LedBrightness);
			}
		}
		else
		{
			CompositeFailed = true;
			RumbleDriver.Stop();
			analogWrite(LED_BUILTIN, 0);
		}
	}

	virtual void OnStateChanged(const bool connected)
	{
		if (connected && !CompositeFailed)
		{
			analogWrite(LED_BUILTIN, LedBrightness);
		}
		else
		{
			// Set x360 neutral controls on disconnect.
			X360->buttons(0);
			X360->position(0, 0);
			X360->positionRight(0, 0);
			X360->sliderLeft(OutputRange::TriggerMin);
			X360->sliderRight(OutputRange::TriggerMin);

			// Stop rumble.
			RumbleDriver.Stop();

			// Disable LED indicator.
			analogWrite(LED_BUILTIN, 0);
		}
	}

private:
	void MapN64ToX360()
	{
		uint16_t Buttons = 0;

		// Map D-Pad.
		Buttons += (ControllerDriver.GetRight() & 1) << XBOX_DRIGHT - 1;
		Buttons += (ControllerDriver.GetLeft() & 1) << XBOX_DLEFT - 1;
		Buttons += (ControllerDriver.GetDown() & 1) << XBOX_DDOWN - 1;
		Buttons += (ControllerDriver.GetUp() & 1) << XBOX_DUP - 1;

		// Map Start.
		Buttons += (ControllerDriver.GetButtonHome() & 1) << XBOX_START - 1;

		// Map L and R to shoulder buttons.
		Buttons += (ControllerDriver.GetButton5() & 1) << XBOX_LSHOULDER - 1;
		Buttons += (ControllerDriver.GetButton6() & 1) << XBOX_RSHOULDER - 1;

		// Map A to A.
		Buttons += (ControllerDriver.GetButton0() & 1) << XBOX_A - 1;

		// Map B to B and X, for compatibility with general UIs and N64 emulators.
		Buttons += (ControllerDriver.GetButton1() & 1) << XBOX_B - 1;
		Buttons += (ControllerDriver.GetButton1() & 1) << XBOX_X - 1;

		// Map Joystick.
		uint16_t xScaled = map((int32_t)ControllerDriver.GetJoy1X(), 0, UINT16_MAX, OutputRange::XMin, OutputRange::XMax);
		uint16_t yScaled = map((int32_t)ControllerDriver.GetJoy1Y(), 0, UINT16_MAX, OutputRange::YMin, OutputRange::YMax);

		// Mapping C-Buttons as a Right Joystick.
		uint16_t CxScaled = map((int32_t)ControllerDriver.GetJoy2X(), 0, UINT16_MAX, OutputRange::XMin, OutputRange::XMax);
		uint16_t CyScaled = map((int32_t)ControllerDriver.GetJoy2Y(), 0, UINT16_MAX, OutputRange::YMin, OutputRange::YMax);

		// Mapping Z to L Slider.
		uint16_t ZSlider;
		if (ControllerDriver.GetButton4())
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