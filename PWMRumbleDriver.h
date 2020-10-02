// PWMRumbleDriver.h

#ifndef _PWMRUMBLEDRIVER_h
#define _PWMRUMBLEDRIVER_h

#include <stdint.h>

class IRumbleStop {
public:
	virtual void Stop() {}
};

template <const uint8_t RumbleDriverPin>
class PWMRumbleDriver : public virtual IRumbleStop {

private:
	// Motor is rated for ~3.0V but is supplied with 5.0V.
	const uint8_t MaxPWM = map(3300, 0, 5000, 0, UINT8_MAX);

public:
	PWMRumbleDriver()
	{
	}

	void Stop()
	{
		analogWrite(RumbleDriverPin, 0);
	}

	void Update(const uint8_t left, const uint8_t right)
	{
		// N64 only has 1 rumble so we mix both channels.
		uint8_t raw = (uint8_t)constrain(left + right, 0, UINT8_MAX);

		// Scale power by lowered max PWM.
		analogWrite(RumbleDriverPin, map(raw, 0, UINT8_MAX, 0, MaxPWM));
	}

	void Setup()
	{
		pinMode(RumbleDriverPin, WiringPinMode::PWM);

		// Set Rumble to off at setup.
		Stop();
	}
};


#endif

