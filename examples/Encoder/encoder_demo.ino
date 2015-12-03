// This file contains a demo which reads an encoder attached
// to the motor included with the SeeedStudio Hercules robot.
#include <arduino.h>
#include <Hercules.h>

// This demo shows how to use the encoders that came with the
// Hercules 4WD robot (25GA-370). The demo will set the motors
// to run in a one direction at a fixed power. An ISR is
// attached to PD3/INT0 that will run every time the encoder
// signal's rising edge is detected (transition from 0 to 1).
// The demo will send the speed in signals per second and
// in RPM over the serial interface. To view the output,
// use Arduino's Serial Monitor at 57600 baud.
//
// Some assumptions I made about how the encoder and the
// Hercules controller work.
//   1. I assume the motors (25GA-370) send out a digital
//      pulse per once per revolution of the motor shaft
//      (not the geared output shaft).
//      I assume it does not use a quadrature or other kind
//      of encoding.  This assumption seems ok.
//   2. The motor is geared at 35:1 ratio.  I measured this.
//      It seems correct.  But I couldn't find documentation
//      to support it.

// The arduino digital pin 3 appears to be PD3
// which is INT1 on the Atmega 328. This is one
// of the pins used for encoders on the Hercules.
const int kEncoderPin3 = 3;

// The count of encoder signals (flip from 0 to 1) that will
// be received in one revolution of the output shaft.
const int kEncoderSignalsPerRev = 35;

void setup()
{
  // Set PD3 as an input pin so we can read the signal.
  DDRD &= ~_BV(DDD3);
  // Turn on the Pull-up for PD3.
  PORTD |= (1 << PORTD3);

  // Attach an interrupt handler that fires whenever the
  // encoder pin (PD3) changes its value from low to high.
  attachInterrupt(digitalPinToInterrupt(kEncoderPin3),
                  encoder_handler, RISING);
  sei();  // Enable interrupts.

  // Enable the motor and set it to a constant speed.
  MOTOR.begin();
  MOTOR.setSpeedDir(15, DIRF);

  // Enable serial to allow reading the speed information
  // from the controller.
  Serial.begin(57600);
}

// This is the timestamp of the last time a signal
// was received from the encoder (transition
// from 0 to 1).
volatile unsigned long prev_timestamp_micros = 0;

// This is the speed of the motor in signals per second
// where a signal is when the interrup is triggered.
volatile unsigned long motor_speed_signals_per_second = 0;

void loop()
{
  // Calculate motor speed in RPM
  long motor_speed_rpm = motor_speed_signals_per_second
      * 60 / kEncoderSignalsPerRev;

  Serial.print("Speed(sig/sec): ");
  Serial.print(motor_speed_signals_per_second);
  Serial.print(", Speed(RPM): ");
  Serial.print(motor_speed_rpm);
  Serial.print(", prev timestamp (us): ");
  Serial.print(prev_timestamp_micros);
  Serial.println();

  delay(100);
}

void encoder_handler() {
  unsigned long timestamp = micros();

  // Calculate the speed in signals per second.
  motor_speed_signals_per_second =
      1000000 / (timestamp - prev_timestamp_micros);
  prev_timestamp_micros = timestamp;
}

/*********************************************************************************************************
 * END FILE
 *********************************************************************************************************/
 
