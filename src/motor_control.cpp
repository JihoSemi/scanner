#include "../include/motor_control.h"
#include "../include/general_control.h"

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

void Control::MotorControl::SetMotor(A4988 &motor, uint8_t rpm, uint8_t step) {
    motor.begin(rpm);
    motor.setMicrostep(step);
    motor.disable();
}

void Control::MotorControl::_SetMicroStep(const uint8_t* kMs, uint8_t step) {
    /*
     * !! NOT USED ANYMORE !!
     * 각 모터의 Microstepping 설정 핀 배열과 설정할 스텝을 매개변수로 받음
     * 1, 2, 4, 8, 16 순서대로 Full, Half, Quarter, Eighth, Sixteenth Step
     * 기본값 Full Step이며 위 숫자 외 다른 숫자를 매개변수로 입력할 경우 Full step으로 설정
     * digitalWrite 함수로 Microstepping resolution table에 맞게 각 핀의 값을 설정
     */
    // Microstepping resolution table: {MS1, MS2, MS3}
    const uint8_t microstep_table[5][3] = {
        {LOW,   LOW,    LOW},   // Full Step
        {HIGH,  LOW,    LOW},   // Half Step
        {LOW,   HIGH,   LOW},   // Quarter Step
        {HIGH,  HIGH,   LOW},   // Eighth Step
        {HIGH,  HIGH,   HIGH}   // Sixteenth Step
    };
    
    uint8_t index = (step == 1 || step == 2 || step == 4 || step == 8 || step == 16) ? __builtin_ctz(step) : 0;

    for (int i = 0; i < 3; ++i) {
        digitalWrite(kMs[i], microstep_table[index][i]);
    }
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
