#ifndef PARAMETER_H
#define PARAMETER_H

/*
 * Scanner의 시스템 매개변수와 ArduinoMega의 일반 제어 신호 Pin, Motor 제어 Pin을 저장
 * 시스템 매개변수: 초기 좌표, Motor 정보, 각종 dimension, Exposure time, Scale down ratio 등
 */

#include <stdint.h>

namespace Control {
    namespace Parameter {
        // Scanner의 시스템 매개변수
        constexpr uint8_t INIT_X           = 0;   	        // 초기 x 좌표
        constexpr uint8_t INIT_Y           = 0;   	        // 초기 y 좌표
        constexpr uint8_t MOTOR_STEPS      = 200; 	        // 200/360 [Steps/Degree] <=> 1.8 [Degree/Step]
        constexpr uint8_t SPD              = 50;  	        // 1mm 당 Step 수 [Steps/mm]
        constexpr uint8_t DIES             = 10;  	        // wafer의 지름에 들어갈 수 있는 최대 다이 개수
        constexpr uint8_t MICROSTEP        = 1;   	        // MicroStepping 설정
        constexpr uint16_t MOTOR_MAX_SPEED = 1000;	        // Motor 최대 속도 설정
        constexpr uint16_t MOTOR_SPEED     = 300; 	        // 통상 Motor 속도
        constexpr int16_t INITIAL_MOVE_DISTANCE = -1000;    // 초기 위치 이동을 위한 충분히 큰 step 값
        constexpr double MASK_X            = 30;  	        // mask x dimension [mm]
        constexpr double MASK_Y            = 30;  	        // mask y dimension [mm]
        constexpr double EXPOSURE_TIME     = 0.5; 	        // exposure time [sec]
        constexpr double LINEAR_SD_RATIO   = 3;   	        // linear scale down ratio
    };
}

namespace ArduinoMega {
    // Arduino I/O Pin
    constexpr uint8_t START            = 50; 	// Start button port: Digital 50
    constexpr uint8_t EXPOSURE_CTRL    = 48; 	// Exposure unit relay control signal port: Digital 48
    constexpr uint8_t RESTART          = 51; 	// Restart button port: Digital 51
    constexpr uint8_t FAN_CTRL         = 49; 	// Cooling fan power relay control signal port: Digital 49
    constexpr uint8_t LIMIT_1F_X       = 14; 	// Position limiting switch for 1F x-axis
    constexpr uint8_t LIMIT_1F_Y       = 15; 	// Position limiting switch for 1F y-axis
    constexpr uint8_t LIMIT_2F_Y       = 16; 	// Position limiting switch for 2F y-axis

    namespace MotorPin {
        // Motor control pins
        constexpr uint8_t ENABLE_1F_R   = 22; 	// Enable signal for the 1F right motor
        constexpr uint8_t ENABLE_1F_L   = 23; 	// Enable signal for the 1F left motor
        constexpr uint8_t ENABLE_2F_R   = 24; 	// Enable signal for the 2F right motor
        constexpr uint8_t ENABLE_2F_L   = 25; 	// Enable signal for the 2F left motor
        constexpr uint8_t STEP_1F_R     = 2;  	// Step signal for the 1F right motor
        constexpr uint8_t STEP_1F_L     = 4;  	// Step signal for the 1F left motor
        constexpr uint8_t STEP_2F_R     = 6;  	// Step signal for the 2F right motor
        constexpr uint8_t STEP_2F_L     = 8;  	// Step signal for the 2F left motor
        constexpr uint8_t DIR_1F_R      = 3;  	// Direction signal for the 1F right motor
        constexpr uint8_t DIR_1F_L      = 5;  	// Direction signal for the 1F left motor
        constexpr uint8_t DIR_2F_R      = 7;  	// Direction signal for the 2F right motor
        constexpr uint8_t DIR_2F_L      = 9;  	// Direction signal for the 2F left motor
        constexpr uint8_t MICROSTEP_1F_R[] = {26, 27, 28}; 	// Microstepping signal for the 1F right motor
        constexpr uint8_t MICROSTEP_1F_L[] = {29, 30, 31}; 	// Microstepping signal for the 1F left motor
        constexpr uint8_t MICROSTEP_2F_R[] = {32, 33, 34}; 	// Microstepping signal for the 2F right motor
        constexpr uint8_t MICROSTEP_2F_L[] = {35, 36, 37}; 	// Microstepping signal for the 2F left motor
    };
}

#endif // PARAMETER_H
