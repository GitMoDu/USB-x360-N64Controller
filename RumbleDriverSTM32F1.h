// RumbleDriverSTM32F1.h

#ifndef _RUMBLEDRIVERSTM32F1_h
#define _RUMBLEDRIVERSTM32F1_h

#define _TASK_OO_CALLBACKS
#define _TASK_SLEEP_ON_IDLE_RUN
#include <TaskSchedulerDeclarations.h>

#include <HardwareTimer.h>
#include <Arduino.h>

#include <PowerCurve.h>


class RumbleDriver : private Task
{
private:
	static const uint32_t TIMEOUT_DURATION_MS = 1000;
	static const uint8_t TimerIndex = 1;
	static const uint8_t TimerChannel = TIMER_CH1; // Fixed pin PA8.
	static const uint8_t EnablePin = PA9; // Fixed pin.

	static const uint32_t CarrierFreqHz = 200000;
	static const uint8_t CurveSaturation = 180;
	static const uint32_t CarrierPeriodMicros = 1000000 / CarrierFreqHz;

	HardwareTimer Timer;

	uint32_t LastUpdated = 0;

	Power2CurveU16 ResponseCurve; // Response curve, soften low end to add margin at the top.

	volatile uint8_t InputValue = 0; // Mono value.

	uint16_t PWMRange = 1;

public:
	RumbleDriver(Scheduler* scheduler)
		: Task(0, TASK_FOREVER, scheduler, false)
		, Timer(TimerIndex)
		, ResponseCurve(CurveSaturation)
	{

	}

	void UpdateInput(const uint8_t left, const uint8_t right)
	{
		LastUpdated = millis();

		// Mono mix channels by largest value.
		InputValue = max(left, right);
	}

	void Setup()
	{
		pinMode(EnablePin, OUTPUT);
		digitalWrite(EnablePin, LOW);

		TurnOn();

		Task::enable();
	}

	bool Callback()
	{
		if (millis() - LastUpdated > TIMEOUT_DURATION_MS)
		{
			TurnOff();
			Task::disable();
		}
		else
		{
			uint16_t pwm = map(ResponseCurve.Get(InputValue), 0, UINT16_MAX, 0, PWMRange);

			Timer.setCompare(TimerChannel, pwm);
			digitalWrite(EnablePin, HIGH);	
		}

		return true;
	}

private:
	void TurnOn()
	{
		digitalWrite(EnablePin, LOW);	
		Timer.pause();
		Timer.setPeriod(CarrierPeriodMicros);

		PWMRange = Timer.getOverflow();
		Timer.setMode(TimerChannel, timer_mode::TIMER_PWM);
		Timer.refresh();
		Timer.resume();

		Timer.setCompare(TimerChannel, 0);
	}

	void TurnOff()
	{
		digitalWrite(EnablePin, LOW);
	}
};

#endif

