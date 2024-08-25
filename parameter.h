#ifndef PARAMETER_H
#define PARAMETER_H

#include <stdint.h>

namespace Control {
    namespace Parameter {
        /*
        * Scanner의 시스템 매개변수
        */
        constexpr uint8_t INIT_X        = 0;    // 초기 x 좌표
        constexpr uint8_t INIT_Y        = 0;    // 초기 y 좌표
        constexpr uint8_t MOTOR_STEPS   = 200;  // 200/360 [Steps/Degree] <=> 1.8 [Degree/Step]
        constexpr uint8_t SPD           = 50;   // Steps per 1mm Distance [Steps/mm]
        constexpr uint8_t DIES          = 10;   // wafer의 지름에 들어갈 수 있는 최대 다이 개수
        constexpr double MASK_X         = 30;   // mask x dimension [mm]
        constexpr double MASK_Y         = 30;   // mask y dimension [mm]
        constexpr double EXPOSURE_TIME  = 0.5;  // exposure time [sec]
        constexpr double LINEAR_SD_RATIO = 3;   // linear scale down ratio
    };
}

namespace ArduinoMega {
    /*
    * 아두이노 I/O 핀
    */
    constexpr uint8_t kStartButton      = 50; // Start button port: Digital 50
    constexpr uint8_t kExposureControl  = 48; // Exposure unit relay control signal port: Digital 48
    constexpr uint8_t kRestartButton    = 51; // Restart button port: Digital 51
    constexpr uint8_t kIrX              = 15; // x-axis IR sensor
    constexpr uint8_t kIrY              = 14; // y-axis IR sensor

    namespace MotorPin {
        constexpr uint8_t kEnableX      = 22; // Enable signal for the x-axis motor
        constexpr uint8_t kEnableY      = 23; // Enable signal for the y-axis motor
        constexpr uint8_t kEnableM      = 24; // Enable signal for the mask motor
        constexpr uint8_t kStepX        = 2;
        constexpr uint8_t kStepY        = 4;
        constexpr uint8_t kStepM        = 6;
        constexpr uint8_t kDirX         = 3;
        constexpr uint8_t kDirY         = 5;
        constexpr uint8_t kDirM         = 7;
        constexpr uint8_t kMsX[]        = {25, 26, 27}; // Microstepping signal for the x-axis motor 
        constexpr uint8_t kMsY[]        = {28, 29, 30}; // Microstepping signal for the y-axis motor
        constexpr uint8_t kMsM[]        = {31, 32, 33}; // Microstepping signal for the mask motor 
    };
}

#endif // PARAMETER_H
