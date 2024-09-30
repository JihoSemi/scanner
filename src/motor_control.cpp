#include "../include/motor_control.h"
#include "../include/general_control.h"

A4988 motor_1f_right(
    // 1F right, wafer motor
    Control::Parameter::MOTOR_STEPS, 
    ArduinoMega::MotorPin::kDir1FR, ArduinoMega::MotorPin::kStep1FR, 
    ArduinoMega::MotorPin::kEnable1FR, 
    ArduinoMega::MotorPin::kMs1FR[0], ArduinoMega::MotorPin::kMs1FR[1], ArduinoMega::MotorPin::kMs1FR[2]
    ); 

A4988 motor_1f_left(
    // 1F left, wafer motor
    Control::Parameter::MOTOR_STEPS, 
    ArduinoMega::MotorPin::kDir1FL, ArduinoMega::MotorPin::kStep1FL, 
    ArduinoMega::MotorPin::kEnable1FL, 
    ArduinoMega::MotorPin::kMs1FL[0], ArduinoMega::MotorPin::kMs1FL[1], ArduinoMega::MotorPin::kMs1FL[2]
    ); 

A4988 motor_2f_right(
    // 2F right, mask motor
    Control::Parameter::MOTOR_STEPS, 
    ArduinoMega::MotorPin::kDir2FR, ArduinoMega::MotorPin::kStep2FR, 
    ArduinoMega::MotorPin::kEnable2FR, 
    ArduinoMega::MotorPin::kMs2FR[0], ArduinoMega::MotorPin::kMs2FR[1], ArduinoMega::MotorPin::kMs2FR[2]
    ); 

A4988 motor_2f_left(
    // 2F left, mask motor
    Control::Parameter::MOTOR_STEPS, 
    ArduinoMega::MotorPin::kDir2FL, ArduinoMega::MotorPin::kStep2FL, 
    ArduinoMega::MotorPin::kEnable2FL, 
    ArduinoMega::MotorPin::kMs2FL[0], ArduinoMega::MotorPin::kMs2FL[1], ArduinoMega::MotorPin::kMs2FL[2]
    ); 

void Control::MotorControl::SetMotor(A4988 &motor, uint8_t rpm, uint8_t step) {
    motor.begin(rpm);
    motor.setMicrostep(step);
    motor.disable();
}

void Control::MotorControl::Rotate(A4988 &motor, const double angle) {
    //Clock Wise rotate if angle >= 0 and Vice Verse
    motor.rotate(angle);
}

void Control::MotorControl::Move(const char axis, const double distance) {
    A4988* motor;

    if (axis == 'x') {
        motor = &motor_1f_right;
    } else if (axis == 'y') {
        motor = &motor_1f_left;
    } else 
        return;
    
    motor->enable();
    motor->move(distance * Parameter::SPD);
    motor->disable();
}
