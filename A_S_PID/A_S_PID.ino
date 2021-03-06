#include <Arduino.h>
#include <Servo.h>
#include <Encoder.h>

// select the motor driver
// p.s. in this lab, you are only allowed to use h-bridge
// #define VEX_MOTOR_DRIVER
#define H_BRIDGE

Encoder myEnc(2, 3);

#ifdef VEX_MOTOR_DRIVER
Servo myservo;
#endif

// gear reduction ratio times encoder cpr
const double encoder_tick_per_output_shaft_rotation = 34.014 * 48.;

// the maximum adjustable range is ftom 0 to (1024 * sensitivity)
const double kp_knob_sensitivity = 1.;
const double ki_knob_sensitivity = 0.001;
const double kd_knob_sensitivity = 1.;

// pin config
const int pin_control_indicate = 13;
const int pin_pot_p = A0;
const int pin_pot_i = A1;
const int pin_pot_d = A2;
// only effective when using vex motor driver
const int pin_servo = 11;
// only effective when using h bridge
// if pin_pwm_fwd is high and pin_pwm_rev is low, the motor should increment the encoder
const int pin_pwm_fwd = 5;
const int pin_pwm_rev = 6;

// set the control interval
const int control_interval = 10; // ms
// set how many digits serial prints the floats
const int print_precision = 6; // digits

// set the preset setpoints and time between step
// how long the setpoint stay at the same value for
const int step_interval = 1000; // ms
// setpoint will alternate between these two values
const double setpoint_1 = -M_PI/6; // rad
const double setpoint_2 = M_PI/6.; // rad

// --- do not touch these ---
// persistent variables for PID control
double setpoint = 0; // rad
double current_position = 0;
unsigned long last_control_time = 0;
double last_error = 0;
double sum_error = 0;
// persistent variables for stepping
unsigned long last_step_time = 0;
boolean setpoint_select = 0;
// for detecting coefficient change
int sum_coefficients = 0;
int last_sum_coefficients = 0;


void setup() {
  pinMode(pin_pot_p, INPUT);
  pinMode(pin_pot_i, INPUT);
  pinMode(pin_pot_d, INPUT);
  pinMode(pin_control_indicate, OUTPUT);

  #ifdef VEX_MOTOR_DRIVER
  pinMode(pin_servo, OUTPUT);
  myservo.attach(11, 1000, 2000);
  #endif

  #ifdef H_BRIDGE
  pinMode(pin_pwm_fwd, OUTPUT);
  pinMode(pin_pwm_rev, OUTPUT);
  #endif

  myEnc.write(0);
  Serial.begin(115200);

  // Auto home... Uncomment if you have the encoder and motor are wired in
  // 'correct' direction (when the control input (motor voltage) > 0, the motor
  // increases the encoder count
  /*
  myservo.write(75);
  boolean moving = 1;
  long last_encoder = myEnc.read();
  while (moving) {
    delay(300);
    long current_encoder = myEnc.read();
    moving = abs(current_encoder - last_encoder) > 10 ? 1 : 0;
    last_encoder = current_encoder;
  }
  myEnc.write(-1100);
  */
}

void loop() {
  unsigned long now = millis();
  if (now - last_control_time >= control_interval) {
    // use oscilloscope to verify the control rate and execution time
    digitalWrite(pin_control_indicate, 1);

    // read the pot
    int kp_knob = analogRead(pin_pot_p);
    int ki_knob = analogRead(pin_pot_i);
    int kd_knob = analogRead(pin_pot_d);
    double kp = kp_knob * kp_knob_sensitivity;
    double ki = ki_knob * ki_knob_sensitivity;
    double kd = kd_knob * kd_knob_sensitivity;
    sum_coefficients = abs(kp_knob) + abs(ki_knob) + abs(kd_knob);


    // read the current position from encoder
    long current_position_encoder_tick = myEnc.read();
    // convert it to radians
    current_position = current_position_encoder_tick * 2 * M_PI / encoder_tick_per_output_shaft_rotation; // rad

    // calculate error
    double error = current_position - setpoint;
    // calculate derivative of error
    double diff_error = error - last_error;
    // integrate error
    sum_error += error;

    // calculate how many volts should be applied to motor terminals
    double control_input = (kp*error + ki*sum_error + kd*diff_error)*-1;

    drive_the_motor(control_input);

    // update the "last" values
    last_error = error;
    last_control_time = now;

    digitalWrite(pin_control_indicate, 0);

    // clear the i term when you adjust the pid coefficients
    if (abs(sum_coefficients - last_sum_coefficients) > 10) {
      sum_error = 0;
      last_sum_coefficients = sum_coefficients;
    }

    // print data to graph
    Serial.print(current_position, print_precision);
    Serial.print(',');
    Serial.print(setpoint, print_precision);
    Serial.print(',');
    Serial.print(kp, print_precision);
    Serial.print(',');
    Serial.print(ki, print_precision);
    Serial.print(',');
    Serial.print(kd, print_precision);
    Serial.print(',');
    Serial.print(control_input, print_precision);
    Serial.print(',');
    Serial.print(error, print_precision);
    Serial.print(',');
    Serial.print(sum_error, print_precision);
    Serial.print(',');
    Serial.print(diff_error, print_precision);
    Serial.print('\n');
  }

  // change the setpoint
  if (now - last_step_time >= step_interval) {
    setpoint_select ^= 1;
    set_setpoint(setpoint_select ? setpoint_1 : setpoint_2);
    last_step_time = now;
  }
}

// set the setpoint
void set_setpoint(double _setpoint) {
  setpoint = _setpoint;
  // avoid getting a spike in diff_error when stepping setpoint.
  // alternatively you can calculate the d term with diff of sensor reading
  // instead of error
  last_error = current_position - setpoint;
  // clear the i term. the i term sometimes have negative impact when your
  // setpoint is a step function. but if you update the setpoint very often,
  // or your setpoint is continuous, you should probably not clear the i term
  // here.
  // sum_error = 0;
}

// deliver the terminal voltage to the motor
void drive_the_motor(double terminal_voltage) {
  #ifdef VEX_MOTOR_DRIVER
  // convert motor terminal voltage to servo signal
  int servo_signal = map(terminal_voltage, -5., 5., 70, 90);
  // actually drive the motor
  myservo.write(servo_signal);
  #endif
  #ifdef H_BRIDGE
  int h_bridge_signal = constrain(map(terminal_voltage, -12., 12., -255, 255),-255,255);
  if (h_bridge_signal >= 0) {
    analogWrite(pin_pwm_fwd, h_bridge_signal);
    analogWrite(pin_pwm_rev, 0);
  } else {
    analogWrite(pin_pwm_fwd, 0);
    analogWrite(pin_pwm_rev, -h_bridge_signal);
  }
  #endif
}
