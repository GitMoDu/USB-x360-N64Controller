// PWMRumbleDriver.h

#ifndef _PWMRUMBLEDRIVER_h
#define _PWMRUMBLEDRIVER_h

#include <HardwareTimer.h>
#include "io.h"

class PWMRumbleDriver {

private:
	static const uint8_t Index = 2;
	static const timer_channel Channel = TIMER_CH1;
	static const uint8_t RumbleDriverPin = PA0;

	// Leaves >12 bits of range.
	static const uint16_t Overflow = UINT16_MAX / 10;

	// Motor is rated for ~3.0V but is supplied with 5.0V.
	// Mosfet is loses transfer efficiency over ~500 KHz.
	const uint16_t MaxPWM = map(4300, 0, 5000, 0, Overflow);

	HardwareTimer Timer;

public:
	PWMRumbleDriver()
		: Timer(Index)
	{
	}

	void Stop()
	{
		Timer.setCompare(Channel, 0);
	}

	void UpdateRumble(const uint8_t value)
	{
		// Scale power by lowered max PWM.
		// map(value, 0, UINT8_MAX, 0, MaxPWM);
		uint16_t output = (uint32_t)(value * MaxPWM) / UINT8_MAX;

		Timer.setCompare(Channel, output);
	}

	void Setup()
	{
		pinMode(RumbleDriverPin, PWM);

		Timer.pause();
		Timer.setPrescaleFactor(1);
		Timer.setCount(0);
		Timer.setOverflow(Overflow);

		Timer.setCompare(Channel, 0);

		Timer.refresh();
		Timer.resume();
	}
};
#endif

