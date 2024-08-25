#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include <A4988.h>
#include "parameter.h"

extern A4988 motor_x;
extern A4988 motor_y;
extern A4988 motor_m;

namespace Control {
    class State;
    class MotorControl {
    private:
    public:
        static void SetMotor(A4988 &motor, uint8_t rpm);
        static void Rotate(A4988 &motor, const double angle);
        static void Move(const char axis, const double distance);
        static void MoveToInitial(const char kAxis);
    };
};

#endif // MOTOR_CONTROL_H
