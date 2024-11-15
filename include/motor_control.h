#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H
/*
 * Motor 객체를 선언하고 Motor 제어와 관련된 클래스를 선언
 */
#include <AccelStepper.h>
#include "parameter.h"

// extern AccelStepper motor_1f_right;
// extern AccelStepper motor_1f_left;
// extern AccelStepper motor_2f_right;
// extern AccelStepper motor_2f_left;

namespace Control {
    class State; // MotorControl 클래스에서 State 클래스에 접근하기 위한 전방 선언
    class MotorControl {
    /*
     * Motor 설정, 제어와 관련된 클래스
     */
    private:
    public:
        static void SetMotor(AccelStepper& motor, uint16_t max_speed);          // Motor 설정
        static void SetMicroStep(const uint8_t* micro_step, uint8_t step=1);    // MicroStep 설정
    };
};

#endif // MOTOR_CONTROL_H
