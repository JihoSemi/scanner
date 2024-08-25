#include "motor_control.h"
#include "general_control.h"

A4988 motor_x(
    // x-axis motor
    Control::Parameter::MOTOR_STEPS, 
    ArduinoMega::MotorPin::kDirX, ArduinoMega::MotorPin::kStepX, 
    ArduinoMega::MotorPin::kEnableX, 
    ArduinoMega::MotorPin::kMsX[0], ArduinoMega::MotorPin::kMsX[1], ArduinoMega::MotorPin::kMsX[2]
    ); 

A4988 motor_y(
    // y-axis motor
    Control::Parameter::MOTOR_STEPS, 
    ArduinoMega::MotorPin::kDirY, ArduinoMega::MotorPin::kStepY, 
    ArduinoMega::MotorPin::kEnableY, 
    ArduinoMega::MotorPin::kMsY[0], ArduinoMega::MotorPin::kMsY[1], ArduinoMega::MotorPin::kMsY[2]
    ); 

A4988 motor_m(
    // mask motor
    Control::Parameter::MOTOR_STEPS, 
    ArduinoMega::MotorPin::kDirM, ArduinoMega::MotorPin::kStepM, 
    ArduinoMega::MotorPin::kEnableM, 
    ArduinoMega::MotorPin::kMsM[0], ArduinoMega::MotorPin::kMsM[1], ArduinoMega::MotorPin::kMsM[2]
    ); 

void Control::MotorControl::SetMotor(A4988 &motor, uint8_t rpm) {
    motor.begin(rpm);
}

void Control::MotorControl::Rotate(A4988 &motor, const double angle) {
    //Clock Wise rotate if angle >= 0 and Vice Verse
    motor.rotate(angle);
}

void Control::MotorControl::Move(const char axis, const double distance) {
    A4988* motor;

    if (axis == 'x') {
        motor = &motor_x;
    } else if (axis == 'y') {
        motor = &motor_y;
    } else 
        return;
    
    motor->enable();
    motor->move(distance * Parameter::SPD);
    motor->disable();
}

void Control::MotorControl::MoveToInitial(const char kAxis) {
    A4988 *motor;
    uint8_t ir_sensor;

    if (kAxis == 'x') {
        motor = &motor_x;
        ir_sensor = ArduinoMega::kIrX;
    } else {
        motor = &motor_y;
        ir_sensor = ArduinoMega::kIrY;
    }

    motor->enable();
    motor->startMove(-100 * Parameter::MOTOR_STEPS);
    while (digitalRead(ir_sensor)) {
        // Waiting until the IR sensor detects the initial position
    }
    motor->stop();
    motor->disable();

    if (kAxis == 'x') {
        State::SetX(Parameter::INIT_X);
    } else {
        State::SetY(Parameter::INIT_Y);
    }
}
