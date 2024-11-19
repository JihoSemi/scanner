#include "../include/motor_control.h"
#include "../include/general_control.h"

// Motor의 최대 속도를 설정하는 함수
void Control::MotorControl::SetMotor(AccelStepper &motor) {
    Serial.print("Setting motor max speed to: ");
    Serial.println(Parameter::MOTOR_MAX_SPEED);
    motor.setMaxSpeed(Parameter::MOTOR_MAX_SPEED * Parameter::MICROSTEP); // Motor의 최대 속도를 설정
    Serial.println("Motor max speed set successfully.");
}

// 주어진 MicroStep Pin을 사용하여 Motor의 MicroStep 모드를 설정하는 함수
void Control::MotorControl::SetMicroStep(const uint8_t* microstep_pin_arr, uint8_t microstep) {
    Serial.print("Setting microstep to: ");
    Serial.println(microstep);

    // 다양한 MicroStep 설정에 대한 MicroStep 해상도 테이블
    const uint8_t microstep_table[5][3] = {
        {LOW,   LOW,    LOW},    // Full Step
        {HIGH,  LOW,    LOW},    // Half Step
        {LOW,   HIGH,   LOW},    // Quarter Step
        {HIGH,  HIGH,   LOW},    // Eighth Step
        {HIGH,  HIGH,   HIGH}    // Sixteenth Step
    };
    
    // 주어진 MicroStep 값에 따라 테이블의 적절한 Index를 결정
    uint8_t index = (microstep == 1 || microstep == 2 || microstep == 4 || microstep == 8 || microstep == 16) ? __builtin_ctz(microstep) : 0;

    // 선택된 MicroStep 모드에 따라 MicroStep Pin을 올바른 값으로 설정
    for (uint8_t i = 0; i < 3; ++i) {
        Serial.print("Setting microstep pin ");
        Serial.print(microstep_pin_arr[i]);
        Serial.print(" to: ");
        Serial.println(microstep_table[index][i] == HIGH ? "HIGH" : "LOW");
        digitalWrite(microstep_pin_arr[i], microstep_table[index][i]); // 테이블에 따라 Pin을 HIGH 또는 LOW로 설정
    }
    Serial.println("Microstep settings applied successfully.");
}
