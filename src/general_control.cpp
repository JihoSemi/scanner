#include "../include/general_control.h"
#include "../include/motor_control.h"

// 유효한 Wafer 영역을 설정
const bool kWaferArea[Control::Parameter::DIES][Control::Parameter::DIES] = {
    {0, 0, 0, 0, 1, 1, 0, 0, 0, 0},
    {0, 0, 1, 1, 1, 1, 1, 1, 0, 0},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {0, 0, 1, 1, 1, 1, 1, 1, 0, 0},
    {0, 0, 0, 0, 1, 1, 0, 0, 0, 0}};

// Position 클래스의 기본 메서드 정의
uint16_t Position::GetX() const { return x; }
uint16_t Position::GetY() const { return y; }
void Position::IncreaseX() { x++; }
void Position::IncreaseY() { y++; }
void Position::SetX(uint16_t x_value) { x = x_value; }
void Position::SetY(uint16_t y_value) { y = y_value; }

// Floor 클래스 메서드 정의
void Control::Floor::SetFloor(A4988 m_left, A4988 m_right, const int16_t lim_y, const int16_t lim_x)
{
    left = &m_left;
    right = &m_right;
    limit_x = lim_x;
    limit_y = lim_y;
}
void Control::Floor::Enable()
{
    left->enable();
    right->enable();
}
void Control::Floor::Disable()
{
    left->disable();
    right->disable();
}
void Control::Floor::Move(const char axis, const double distance, const bool is_exposure)
{
    int steps = Control::Parameter::SPD * distance;

    switch (axis)
    {
    case 'x':
        left->startMove(steps);
        right->startMove(steps);
        break;

    case 'y':
        if (is_exposure)
        {
            left->startMove(steps, Control::Parameter::EXPOSURE_TIME * 1E6);
            right->startMove(-1 * steps, Control::Parameter::EXPOSURE_TIME * 1E6);
        }
        else
        {
            left->startMove(steps);
            right->startMove(-1 * steps);
        }
        break;
    default:
        break;
    }
}
void Control::Floor::MoveBlock(const char axis, const double distance) {
    int steps = Control::Parameter::SPD * distance;
    long time = left->getTimeForMove(steps);

    switch (axis)
    {
    case 'x':
        left->startMove(steps);
        right->startMove(steps);
        break;

    case 'y':
        left->startMove(steps);
        right->startMove(-1 * steps);
        break;
    default:
        break;
    }
    delayMicroseconds(time);
}
void Control::Floor::MoveToInitial(const char axis) {
    // Micro Switch가 눌리지 않을 때 0을 출력한다고 가정
    int16_t limit;

    if (axis == 'x')
        limit = limit_x;
    else if (axis == 'y')
        limit = limit_y;
    
    if (limit != -1 && digitalRead(limit) != HIGH) 
    {
        Enable();
        Move(axis, -1000);  // 충분히 큰 거리 이동
        while (true) 
        {
            if (digitalRead(limit) == HIGH)
            {
                left->stop();
                right->stop();
                Disable();
                break;
            }
        }
    }
    if (axis == 'x')
        State::SetX(Parameter::INIT_X);
    else if (axis == 'y')
        State::SetY(Parameter::INIT_Y);
}

// 시스템 상태 변수 선언
Position Control::State::current_pos;
uint8_t Control::State::num_executions;
unsigned long Control::State::exposure_start_time;
bool Control::State::exposing;

// 1F, 2F Floor 클래스 선언
Control::Floor first_floor;
Control::Floor second_floor;

// State 클래스의 메서드 정의
void Control::State::SetX(uint16_t x_value) { current_pos.SetX(x_value); }
void Control::State::SetY(uint16_t y_value) { current_pos.SetY(y_value); }
void Control::State::IncreaseX() { current_pos.IncreaseX(); }
void Control::State::IncreaseY() { current_pos.IncreaseY(); }
void Control::State::SetExposing(bool state) { exposing = state; }
void Control::State::IncreaseNumExecutions() { num_executions++; }
void Control::State::InitState()
{
    num_executions = 0;
    exposing = false;
}
void Control::State::UpdateExposureStartTime() { exposure_start_time = millis(); }
uint16_t Control::State::GetX() { return current_pos.GetX(); }
uint16_t Control::State::GetY() { return current_pos.GetY(); }
uint8_t Control::State::GetNumExecutions() { return num_executions; }
bool Control::State::GetExposing() { return exposing; }
unsigned long Control::State::GetExposureStartTime() { return exposure_start_time; }

// GeneralControl 클래스의 메서드 정의
void Control::GeneralControl::SetPinMode()
{
    /*
     * Arduino의 각종 제어 Pin mode 설정
     */
    pinMode(ArduinoMega::kStartButton, INPUT);
    pinMode(ArduinoMega::kRestartButton, INPUT);
    pinMode(ArduinoMega::kExposureControl, OUTPUT);
    pinMode(ArduinoMega::kFanPwr, OUTPUT);

    for (size_t i = 0; i < 3; i++)
    {
        pinMode(ArduinoMega::MotorPin::kMs1FR[i], OUTPUT);
        pinMode(ArduinoMega::MotorPin::kMs1FL[i], OUTPUT);
        pinMode(ArduinoMega::MotorPin::kMs2FR[i], OUTPUT);
        pinMode(ArduinoMega::MotorPin::kMs2FL[i], OUTPUT);
    }

    pinMode(ArduinoMega::kLimitX1F, INPUT);
    pinMode(ArduinoMega::kLimitY1F, INPUT);
    pinMode(ArduinoMega::kLimitY2F, INPUT);
}

void Control::GeneralControl::InitializeSystem()
{
    /*
     * 시스템 초기화
     * 모터 설정, PinMode 설정
     * Floor 설정
     * 제어 기본값 적용
     * 초기 위치 이동
     */

    // Serial.begin(115200);
    State::InitState();
    SetPinMode();
    Control::MotorControl::SetMotor(motor_1f_right, 60, 16); // Set the speed and Microstepping for the 1F right motor
    Control::MotorControl::SetMotor(motor_1f_left, 60, 16);  // Set the speed and Microstepping for the 1F left motor
    Control::MotorControl::SetMotor(motor_2f_right, 60, 16); // Set the speed and Microstepping for the 2F right motor
    Control::MotorControl::SetMotor(motor_2f_left, 60, 16);  // Set the speed and Microstepping for the 2F left motor
    
    first_floor.SetFloor(
        // Set the first floor with two motors and two position limiting switches
        motor_1f_left, motor_1f_right,
        ArduinoMega::kLimitY1F, ArduinoMega::kLimitX1F
        ); 
    second_floor.SetFloor(
        // Set the second floor with two motors and one position limiting switches
        motor_2f_left, motor_2f_right,
        ArduinoMega::kLimitY2F
        ); 

    ControlLED(OFF);
    ControlFan(ON);
    InitializePosition();
};

void Control::GeneralControl::WaitTillPressed(const uint8_t pin)
{
    /*
     * Waiting for pressing Pull-Up switch
     */
    bool is_pressed = false;
    while (!is_pressed)
    {
        if (digitalRead(pin) == LOW)
        {                    // Button pressed
            delay(DBNC_DLY); // Delay for Debouncing
            if (digitalRead(pin) == HIGH)
            { // Button Released
                is_pressed = true;
            }
        }
    }
}

void Control::GeneralControl::WaitForStart()
{
    /*
     * Wating until the start button is pressed if an initial excution
     */
    if (!State::GetNumExecutions())
    {
        WaitTillPressed(ArduinoMega::kStartButton);
        State::IncreaseNumExecutions();
    }
}

void Control::GeneralControl::InitializePosition()
{
    first_floor.MoveToInitial('x');
    first_floor.MoveToInitial('y');
    second_floor.MoveToInitial('y');
}

void Control::GeneralControl::ControlLED(uint8_t op)
{
    /*
     * LED is ON during LOW CTRL
     */
    digitalWrite(ArduinoMega::kExposureControl, op);
}

void Control::GeneralControl::ControlFan(uint8_t op)
{
    /*
     * Fan is ON during LOW CTRL
     */
    digitalWrite(ArduinoMega::kFanPwr, op);
}

void Control::GeneralControl::StartExposure()
{
    /*
     * Start LED exposure by non-blocking method
     */
    ControlFan(OFF); // OFF the cooling fan while LED exposes due to lack of power
    ControlLED(ON);
    State::UpdateExposureStartTime(); // 노출 시작 시간을 저장
    State::SetExposing(true);
}

void Control::GeneralControl::EndExposure()
{
    while (State::GetExposing())
    {
        CheckExposure();
    }
    ControlFan(ON); // Turn on the fan again
}

void Control::GeneralControl::CheckExposure()
{
    /*
     * non-blocking method를 사용함에 있어 exposure 종료 조건을 확인하는 메서드
     */
    if (
        State::GetExposing() &&
        millis() - State::GetExposureStartTime() >= Parameter::EXPOSURE_TIME * 1000)
    {
        ControlLED(OFF);
        State::SetExposing(false);
    }
}

void Control::GeneralControl::Scan()
{
    /*
     * 스캔 동작 동안 mask는 웨이퍼와 y축 상에서 반대 방향으로 이동해야 함
     * 웨이퍼 이동 거리는 축소 linear ratio에 따라 달라짐
     * 스캔을 1회 완료하면 스캔하는 동안 달라진 위치를 다시 원상복귀 시켜야 함
     * ㄹ 자로 이동하면 인접한 die의 pattern이 상하 반전된 상태로 있으므로 후속 공정이 어려움
     */

    StartExposure();

    first_floor.Enable();
    second_floor.Enable();

    first_floor.Move(
        'y',                                                    // non-blocking으로 wafer 이동 시작
        -1 * (Parameter::MASK_Y / Parameter::LINEAR_SD_RATIO),  // 축소 비율만큼 짧은 거리, mask와 반대로 이동
        true                                                    // Exposure time동안 이동하도록 설정
        ); 
    second_floor.Move(
        'y',                                                    // non-blocking으로 mask 이동 시작
        Parameter::MASK_Y,               
        true
        );

    EndExposure();

    first_floor.Move('y', Parameter::MASK_Y / Parameter::LINEAR_SD_RATIO);  // Scan 동안 움직인 위치를 초기화
    second_floor.MoveBlock('y', -1 * Parameter::MASK_Y);                    // Blocking method로 움직여 이동이 끝나기 전에 Disable하는 것을 방지

    first_floor.Disable();
    second_floor.Disable();
}

void Control::GeneralControl::Operate()
{
    /*
     * 전체 Wafer에 대한 동작을 수행하는 High level method
     */
    while (
        State::GetX() < Parameter::DIES &&
        State::GetY() < Parameter::DIES)
    {
        // 현재 광원의 위치가 wafer를 벗어나지 않을 때까지

        if (kWaferArea[State::GetX()][State::GetY()])
        {
            // 유효한 wafer 영역 위에 광원이 위치하는 경우 Scan 수행
            Scan();
        }

        if (State::GetX() < Parameter::DIES - 1)
        {
            // x 방향 끝까지 가지 않은 경우
            first_floor.MoveBlock('x', -1 * Parameter::MASK_X);
            State::IncreaseX();
        }
        else
        {
            // x 방향 끝까지 간 경우
            if (State::GetY() < Parameter::DIES - 1)
            {
                // y 방향 끝까지 가지 않은 경우
                first_floor.MoveBlock('y', Parameter::MASK_Y);
                State::IncreaseY();
                first_floor.MoveToInitial('x');
            }
            else
            {
                // x 방향과 y 방향 모두 끝까지 간 경우, 즉 모든 Die가 완료된 경우
                break;
            }
        }
    }
}
