#ifndef GENERAL_CONTROL_H
#define GENERAL_CONTROL_H

/*
 * 일반 설정, 제어에 관련된 클래스와 변수를 선언
 */
#include "parameter.h"
#include <AccelStepper.h>
#include <math.h>

extern const bool WAFER_AREA[Control::Parameter::DIES][Control::Parameter::DIES];

class Position {
    private:
        uint16_t x;
        uint16_t y;
    public:
        Position();
        void SetX(uint16_t x_value);                // x 좌표 설정
        void SetY(uint16_t y_value);                // y 좌표 설정
        void IncreaseX();                           // x 좌표 증가
        void IncreaseY();                           // y 좌표 증가
        uint16_t GetX() const;                      // x 좌표 반환
        uint16_t GetY() const;                      // y 좌표 반환
};

namespace Control {
    class MotorControl; // GeneralControl에서 MotorControl에 접근하기 위한 전방 선언

    class Floor {
    public:
        enum MotorIndex {
            LEFT = 0,                               // 왼쪽 Motor Index
            RIGHT = 1                               // 오른쪽 Motor Index
        };
        enum AxisIndex {
            X = 0,                                  // x 축 Index
            Y = 1                                   // y 축 Index
        };
    /*
     * 각 층의 두 Motor를 층별로 한 번에 제어하기 위한 클래스
     */
    private:
        AccelStepper* motor[2];                     // 두 Motor의 제어 객체
        uint8_t enable[2];                          // Motor Enable Pin
        int16_t limit[2];                           // Motor 제한 스위치

    public:
        void SetFloor(                              // Motor 및 제어 설정
            AccelStepper& m_left, AccelStepper& m_right, 
            const uint8_t en_left, const uint8_t en_right, 
            const int16_t lim_y, const int16_t lim_x=-1
            );
        void Enable();                              // Motor Enable
        void Disable();                             // Motor Disable
        void Move(const AxisIndex axis, const double distance); // 특정 축으로 Motor 이동
        void MoveToInitial(const AxisIndex axis, bool second);        // 초기 위치로 Motor 이동
    };
    
    class State {
    /*
     * 시스템의 상태를 저장하고 있는 클래스
     */
    private:
        static Position current_pos;                // 현재 웨이퍼와 광원의 상대 위치를 Position 클래스로 저장
        static uint8_t num_executions;              // 작업 횟수 저장
        static unsigned long exposure_start_time;   // 노광 시작 시간을 저장
        static bool exposing;                       // LED의 상태 저장

    public:
        static void SetX(uint16_t x_value);         // x 좌표 설정
        static void SetY(uint16_t y_value);         // y 좌표 설정
        static void IncreaseX();                    // x 좌표 증가
        static void IncreaseY();                    // y 좌표 증가
        static void IncreaseNumExecutions();        // 작업 횟수 증가
        static void InitState();                    // 시스템 상태 초기화
        static void SetExposing(bool state);        // 노광 상태 설정
        static void UpdateExposureStartTime();      // 노광 시작 시간 업데이트
        static uint16_t GetX();                     // x 좌표 반환
        static uint16_t GetY();                     // y 좌표 반환
        static uint8_t GetNumExecutions();          // 작업 횟수 반환
        static bool GetExposing();                  // 노광 상태 반환
        static unsigned long GetExposureStartTime();// 노광 시작 시간 반환
    };

    class GeneralControl {
    /*
     * 일반 제어에 필요한 Parameter와 기능을 담고 있는 클래스
     * 일반 제어: Motor 제어를 제외한 나머지 제어; 시스템 초기화, LED 제어, 상위 레벨 명령 등
     */
        enum Direction {
            REVERSE = true,
            FORWARD = false
        };

        enum FloorIndex {
            FIRST = 0,
            SECOND = 1
        };
        enum MotorIndex {
            LEFT_1F = 0,
            RIGHT_1F = 1,
            LEFT_2F = 2,
            RIGHT_2F = 3
        };
        enum AxisIndex {
            X_1F = 0,
            Y_1F = 1,
            Y_2F = 2
        };

    private:
        static Floor* floor[2];                            // 두 층의 제어 객체
        static AccelStepper* motor[4];                     // 네 Motor의 객체
        static uint8_t enable[4];                          // Motor Enable Pin
        static int16_t limit[3];                           // Motor 제한 스위치

    public:
        static constexpr uint32_t DBNC_DLY  = 50;       // button debounce delay
        static constexpr uint8_t ON         = 0x0;      // Active LOW ON/OFF
        static constexpr uint8_t OFF        = 0x1;      // Active LOW ON/OFF
        
        static void ConfigSystem(                       // 시스템 구성
            Floor& first, Floor& second,
            AccelStepper& m_1f_l, AccelStepper& m_1f_r, AccelStepper& m_2f_l, AccelStepper& m_2f_r,
            uint8_t en_1f_l, uint8_t en_1f_r, uint8_t en_2f_l, uint8_t en_2f_r,
            int16_t lim_x_1f, int16_t lim_y_1f, int16_t lim_y_2f
        );
        static void SetPinMode();                       // Pin 모드 설정
        static void InitializeSystem();                 // 시스템 초기화
        static void WaitTillPressed(const uint8_t pin); // 버튼이 눌릴 때까지 대기
        static void WaitForStart();                     // 시작 대기
        static void InitializePosition();               // 위치 초기화
        static void ControlLED(uint8_t op);             // LED 제어
        static void ControlFan(uint8_t op);             // 팬 제어
        static void StartExposure();                    // 노광 시작
        static void EndExposure();                      // 노광 종료
        static void CheckExposure();                    // 노광 상태 확인
        static void ScanMove(bool reverse=false);       // 스캔을 위한 Motor 제어
        static void Scan();                             // 스캔
        static void Operate();                          // 시스템 동작
    };
};

#endif // GENERAL_CONTROL_H
