#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H
/*
 * 모터 객체를 선언하고 모터 제어와 관련된 클래스를 선언
 */
#include <A4988.h>
#include "parameter.h"

extern A4988 motor_1f_right;
extern A4988 motor_1f_left;
extern A4988 motor_2f_right;
extern A4988 motor_2f_left;

namespace Control {
    class State; // MotorControl 클래스에서 State 클래스에 접근하기 위한 전방 선언
    class MotorControl {
    /*
     * 모터 설정, 제어와 관련된 클래스
     */
    private:
    public:
        static void SetMotor(A4988 &motor, uint8_t rpm, uint8_t step=1);  
        static void Rotate(A4988 &motor, const double angle);
        static void Move(const char axis, const double distance);
    };
};

#endif // MOTOR_CONTROL_H
