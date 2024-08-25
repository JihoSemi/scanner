#ifndef GENERAL_CONTROL_H
#define GENERAL_CONTROL_H
/*
 * 일반 설정, 제어에 관련된 클래스와 변수를 선언
 */
#include "parameter.h"

extern const bool kWaferArea[Control::Parameter::DIES][Control::Parameter::DIES];

class Position {
    private:
        uint16_t x;
        uint16_t y;
    public:
        void SetX(uint16_t x_value);
        void SetY(uint16_t y_value);
        void IncreaseX();
        void IncreaseY();
        uint16_t GetX() const;
        uint16_t GetY() const;
};

namespace Control {
    class MotorControl; // GeneralControl에서 MotorControl에 접근하기 위한 전방 선언
    class State {
    /*
     * 시스템의 상태를 저장하고 있는 클래스
     */
    private:
        static Position current_pos;                // 현재 웨이퍼와 광원의 상대 위치를 Position 클래스로 저장
        static uint8_t num_excuse;                  // The number of executions
        static unsigned long exposure_start_time;   // 노광 시작 시간을 저장
        static bool exposing;                       // LED의 상태
        static bool toggle_up_down;                 // 위에서 바라본 웨이퍼의 이동 방향

    public:
        static void SetX(uint16_t x_value);  
        static void SetY(uint16_t y_value);
        static void IncreaseX();
        static void IncreaseY();
        static void IncreaseNumExcuse();
        static void InitState();
        static void SetExposing(bool state);
        static void InvertDirection();
        static void UpdateExposureStartTime();
        static uint16_t GetX();
        static uint16_t GetY();
        static uint8_t GetNumExcuse();
        static bool GetExposing();
        static bool GetDirection();
        static unsigned long GetExposureStartTime();
        static Position GetPos();
    };

    class GeneralControl {
    /*
     * 일반 제어에 필요한 Parameter와 기능을 담고 있는 클래스
     * 일반 제어: 모터 제어를 제외한 나머지 제어. 시스템 초기화, LED 제어, 상위 레벨 명령 등
     */
    private:
    public:
        static constexpr uint32_t DBNC_DLY  = 50;       // button debounce delay  
        static constexpr uint8_t ON         = 0x0;      // Active LOW ON/OFF
        static constexpr uint8_t OFF        = 0x1;      // Active LOW ON/OFF
        static constexpr bool UP            = false;    // direction of wafer movement for scanning
        static constexpr bool DOWN          = true;     // direction of wafer movement for scanning
        
        static void SetPinMode();
        static void InitializeSystem();
        static void WaitTillPressed(const uint8_t pin);
        static void WaitForStart();
        static void MoveToInitial(const char kAxis);
        static void InitializePosition();
        static void ControlLED(uint8_t op);
        static void ControlFan(uint8_t op);
        static void StartExposure();
        static void EndExposure();
        static void CheckExposure();
        static void Scan();
        static void Operate();
    };
};

#endif // GENERAL_CONTROL_H