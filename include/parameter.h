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
        constexpr uint8_t INIT_X                = 0;   	    // 초기 x 좌표
        constexpr uint8_t INIT_Y                = 0;   	    // 초기 y 좌표
        constexpr uint8_t MOTOR_STEPS           = 200; 	    // 200/360 [Steps/Degree] <=> 1.8 [Degree/Step]
        constexpr uint8_t DIES                  = 10;  	    // wafer의 지름에 들어갈 수 있는 최대 다이 개수
        constexpr uint8_t MICROSTEP             = 16;   	// MicroStepping 설정
        constexpr uint8_t SERBVO_DELAY          = 10;       // Servo 이동 딜레이
        constexpr uint16_t MOTOR_MAX_SPEED      = 500;	    // Motor 최대 속도 설정
        constexpr uint16_t MOTOR_SPEED          = 30; 	    // 통상 Motor 속도
        constexpr long INITIAL_MOVE_DISTANCE    = 200;      // 초기 위치 이동을 위한 충분히 큰 step 값
        constexpr float DPM                     = 0.3125;   // Step motor distance per millimeter
        constexpr double DIE_X                  = 10;       // die x dimension [mm]
        constexpr double DIE_Y                  = 7;       // die y dimension [mm]
        constexpr double MASK_X                 = 30;  	    // mask x dimension [mm]
        constexpr double MASK_Y                 = 30;  	    // mask y dimension [mm]
        constexpr double EXPOSURE_TIME          = 5; 	    // exposure time [sec]
        constexpr double LINEAR_SD_RATIO        = 3;   	    // linear scale down ratio
        constexpr double INITIAL_DIST_1F_X      = 85;      // 각 축의 한계 지점까지 이동한 뒤 초기 위치까지의 거리
        constexpr double INITIAL_DIST_1F_Y      = 25;
        constexpr double INITIAL_DIST_2F        = 110;
    };
}

namespace ArduinoMega {
    // Arduino I/O Pin
    constexpr uint8_t START            = 48; 	// Start button port: Digital 50
    constexpr uint8_t EXPOSURE_CTRL    = 50; 	// Exposure unit relay control signal port: Digital 48
    constexpr uint8_t RESTART          = 46; 	// Restart button port: Digital 51
    constexpr uint8_t FAN_CTRL         = 52; 	// Cooling fan power relay control signal port: Digital 49
    constexpr uint8_t LIMIT_1F_X       = 47; 	// Position limiting switch for 1F x-axis
    constexpr uint8_t LIMIT_1F_Y       = 49; 	// Position limiting switch for 1F y-axis
    constexpr uint8_t LIMIT_2F_Y       = 51; 	// Position limiting switch for 2F y-axis

    namespace MotorPin {
        // Motor control pins
        constexpr uint8_t ENABLE_1F_R   = 22; 	// Enable signal for the 1F right motor
        constexpr uint8_t ENABLE_1F_L   = 23; 	// Enable signal for the 1F left motor
        constexpr uint8_t ENABLE_2F_R   = 34; 	// Enable signal for the 2F right motor
        constexpr uint8_t ENABLE_2F_L   = 35; 	// Enable signal for the 2F left motor
        constexpr uint8_t STEP_1F_R     = 30;  	// Step signal for the 1F right motor
        constexpr uint8_t STEP_1F_L     = 31;  	// Step signal for the 1F left motor
        constexpr uint8_t STEP_2F_R     = 42;  	// Step signal for the 2F right motor
        constexpr uint8_t STEP_2F_L     = 43;  	// Step signal for the 2F left motor
        constexpr uint8_t DIR_1F_R      = 32;  	// Direction signal for the 1F right motor
        constexpr uint8_t DIR_1F_L      = 33;  	// Direction signal for the 1F left motor
        constexpr uint8_t DIR_2F_R      = 44;  	// Direction signal for the 2F right motor
        constexpr uint8_t DIR_2F_L      = 45;  	// Direction signal for the 2F left motor
        constexpr uint8_t MICROSTEP_1F_R[] = {24, 26, 28}; 	// Microstepping signal for the 1F right motor
        constexpr uint8_t MICROSTEP_1F_L[] = {25, 27, 29}; 	// Microstepping signal for the 1F left motor
        constexpr uint8_t MICROSTEP_2F_R[] = {36, 38, 40}; 	// Microstepping signal for the 2F right motor
        constexpr uint8_t MICROSTEP_2F_L[] = {37, 39, 41}; 	// Microstepping signal for the 2F left motor

        constexpr uint8_t SERVOPWM      = 10;   // PWM signal for the z-axis servo motor
    };
}

#endif // PARAMETER_H
