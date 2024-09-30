#ifndef PARAMETER_H
#define PARAMETER_H
/*
 * Scanner의 시스템 매개변수와 ArduinoMega의 일반 제어 신호 Pin, 모터 제어 Pin을 저장
 * 시스템 매개변수: 초기 좌표, 모터 정보, 각종 dimension, Exposure time, Scale down ratio 등
 */
#include <stdint.h>

namespace Control {
    namespace Parameter {
        /*
        * Scanner의 시스템 매개변수
        */
        constexpr uint8_t INIT_X        = 0;    // 초기 x 좌표
        constexpr uint8_t INIT_Y        = 0;    // 초기 y 좌표
        constexpr uint8_t MOTOR_STEPS   = 200;  // 200/360 [Steps/Degree] <=> 1.8 [Degree/Step]
        constexpr uint8_t APD           = 3;    // Angle per 1mm Distance [Degree/mm]
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
    // constexpr uint8_t kIrX              = 15; // x-axis IR sensor
    // constexpr uint8_t kIrY              = 14; // y-axis IR sensor
    constexpr uint8_t kFanPwr           = 49; // Cooling fan power relay control signal port Digital 49
    constexpr uint8_t kLimitX1F         = 14; // Position limiting switch for 1F x-axis
    constexpr uint8_t kLimitY1F         = 15; // Position limiting switch for 1F y-axis
    constexpr uint8_t kLimitY2F         = 16; // Position limiting switch for 2F y-axis

    namespace MotorPin {
        constexpr uint8_t kEnable1FR      = 22; // Enable signal for the 1F right motor
        constexpr uint8_t kEnable1FL      = 23; // Enable signal for the 1F left motor
        constexpr uint8_t kEnable2FR      = 24; // Enable signal for the 2F right motor
        constexpr uint8_t kEnable2FL      = 25; // Enable signal for the 2F left motor
        constexpr uint8_t kStep1FR        = 2;  // Step signal for the 1F right motor
        constexpr uint8_t kStep1FL        = 4;  // Step signal for the 1F left motor
        constexpr uint8_t kStep2FR        = 6;  // Step signal for the 2F right motor
        constexpr uint8_t kStep2FL        = 8;  // Step signal for the 2F left motor
        constexpr uint8_t kDir1FR         = 3;  // Direction signal for the 1F right motor
        constexpr uint8_t kDir1FL         = 5;  // Direction signal for the 1F left motor
        constexpr uint8_t kDir2FR         = 7;  // Direction signal for the 2F right motor
        constexpr uint8_t kDir2FL         = 9;  // Direction signal for the 2F left motor
        constexpr uint8_t kMs1FR[]        = {26, 27, 28}; // Microstepping signal for the 1F right motor 
        constexpr uint8_t kMs1FL[]        = {29, 30, 31}; // Microstepping signal for the 1F left motor
        constexpr uint8_t kMs2FR[]        = {32, 33, 34}; // Microstepping signal for the 2F right motor
        constexpr uint8_t kMs2FL[]        = {35, 36, 37}; // Microstepping signal for the 2F left motor 
    };
}

#endif // PARAMETER_H
