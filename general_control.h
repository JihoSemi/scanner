#ifndef GENERAL_CONTROL_H
#define GENERAL_CONTROL_H

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
    class MotorControl;
    class State {
    private:
        static Position current_pos;
        static uint8_t num_excuse;                  // Memorizing if the current execution is the first execution
        static unsigned long exposure_start_time;   // 노광 시작 시간을 저장
        static bool exposing;                       // LED의 상태
        static bool toggle_up_down;

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
    private:
    public:
        static constexpr uint32_t DBNC_DLY  = 50;       // button debounce delay  
        static constexpr uint8_t ON         = 0x0;      // Active LOW ON/OFF
        static constexpr uint8_t OFF        = 0x1;      // Active LOW ON/OFF
        static constexpr bool UP            = false;    // direction of wafer movement for scanning
        static constexpr bool DOWN          = true;     // direction of wafer movement for scanning
        
        static void SetPinMode();
        static void SetMicroStep(const uint8_t* kMs, uint8_t step=1);
        static void InitializeSystem();
        static void WaitTillPressed(const uint8_t pin);
        static void WaitForStart();
        static void InitializePosition();
        static void ControlLED(uint8_t op);
        static void StartExposure();
        static void EndExposure();
        static void CheckExposure();
        static void Scan();
        static void Operate();
    };
};

#endif // GENERAL_CONTROL_H
