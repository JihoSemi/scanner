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
    {0, 0, 0, 0, 1, 1, 0, 0, 0, 0}
};

// Position 클래스의 기본 메서드 정의
uint16_t Position::GetX() const { return x; }
uint16_t Position::GetY() const { return y; }
void Position::IncreaseX() { x++; }
void Position::IncreaseY() { y++; }
void Position::SetX(uint16_t x_value) { x = x_value; }
void Position::SetY(uint16_t y_value) { y = y_value; }

// 시스템 상태 변수 선언
Position Control::State::current_pos; 
uint8_t Control::State::num_excuse; 
unsigned long Control::State::exposure_start_time;   
bool Control::State::exposing;                       
bool Control::State::toggle_up_down;

// State 클래스의 메서드 정의
void Control::State::SetX(uint16_t x_value) { current_pos.SetX(x_value); }
void Control::State::SetY(uint16_t y_value) { current_pos.SetY(y_value); }
void Control::State::IncreaseX() { current_pos.IncreaseX(); }
void Control::State::IncreaseY() { current_pos.IncreaseY(); }
void Control::State::InvertDirection() { toggle_up_down = !toggle_up_down; }
void Control::State::SetExposing(bool state) { exposing = state; }
void Control::State::IncreaseNumExcuse() { num_excuse++; }
void Control::State::InitState() { 
    num_excuse = 0; 
    exposing = false;
    toggle_up_down = GeneralControl::UP;
}
void Control::State::UpdateExposureStartTime() { exposure_start_time = millis(); }
uint16_t Control::State::GetX() { return current_pos.GetX(); }
uint16_t Control::State::GetY() { return current_pos.GetY(); }
uint8_t Control::State::GetNumExcuse() { return num_excuse; }
bool Control::State::GetExposing() { return exposing; }
bool Control::State::GetDirection() { return toggle_up_down; }
unsigned long Control::State::GetExposureStartTime() { return exposure_start_time; }

// GeneralControl 클래스의 메서드 정의
void Control::GeneralControl::SetPinMode() {
    /*
     * Arduino의 각종 제어 Pin mode 설정
     */
    pinMode(ArduinoMega::kStartButton, INPUT);
    pinMode(ArduinoMega::kRestartButton, INPUT);
    pinMode(ArduinoMega::kExposureControl, OUTPUT);
    pinMode(ArduinoMega::kFanPwr, OUTPUT);
    
    for (size_t i = 0; i < 3; i++)
    {
        pinMode(ArduinoMega::MotorPin::kMsX[i], OUTPUT);
        pinMode(ArduinoMega::MotorPin::kMsY[i], OUTPUT);
        pinMode(ArduinoMega::MotorPin::kMsM[i], OUTPUT);
    }
    
    pinMode(ArduinoMega::kIrX, INPUT);
    pinMode(ArduinoMega::kIrY, INPUT);
}

void Control::GeneralControl::InitializeSystem() {
    /*
     * 시스템 초기화
     * 모터 설정, PinMode 설정
     * 제어 기본값 적용
     * 초기 위치 이동
     */

    // Serial.begin(115200);
    State::InitState();
    SetPinMode();
    Control::MotorControl::SetMotor(motor_x, 60, 16); // Set the speed and Microstepping for the x-motor
    Control::MotorControl::SetMotor(motor_y, 60, 16); // Set the speed and Microstepping for the y-motor
    Control::MotorControl::SetMotor(motor_m, 60, 16); // Set the speed and Microstepping for the mask motor
    ControlLED(OFF);
    ControlFan(ON);
    InitializePosition();
};  

void Control::GeneralControl::WaitTillPressed(const uint8_t pin) {
    /*
     * Waiting for pressing Pull-Up switch
     */ 
    bool is_pressed = false;
    while (!is_pressed) {
        if (digitalRead(pin) == LOW) {      // Button pressed
            delay(DBNC_DLY);                // Delay for Debouncing
            if (digitalRead(pin) == HIGH) { // Button Released
                is_pressed = true;
            }
        }
    }
}

void Control::GeneralControl::WaitForStart() {
    /*
     * Wating until the start button is pressed if an initial excution
     */
    if (!State::GetNumExcuse()) { 
        WaitTillPressed(ArduinoMega::kStartButton);
        State::IncreaseNumExcuse();
    }
}

void Control::GeneralControl::MoveToInitial(const char kAxis) {
    /*
     * Move the relative position of the wafer and the light system to the initial position using IR sensors
     */
    A4988 *motor;
    uint8_t ir_sensor;

    if (kAxis == 'x') {
        motor = &motor_x;
        ir_sensor = ArduinoMega::kIrX;
    } else {
        motor = &motor_y;
        ir_sensor = ArduinoMega::kIrY;
    }

    motor->enable();
    motor->startMove(-100 * Parameter::MOTOR_STEPS);    // 충분한 거리를 이동하도록 명령 
    while (digitalRead(ir_sensor)) {
        // Waiting until the IR sensor detects the initial position
    }
    motor->stop();  // 초기 위치임이 감지되면 이동하던 모터를 정지시킴
    motor->disable();

    if (kAxis == 'x') {
        State::SetX(Parameter::INIT_X);
    } else {
        State::SetY(Parameter::INIT_Y);
    }
}

void Control::GeneralControl::InitializePosition() {
    GeneralControl::MoveToInitial('x'); 
    GeneralControl::MoveToInitial('y'); 
}

void Control::GeneralControl::ControlLED(uint8_t op) {
    /*
     * LED is ON during LOW CTRL
     */
    digitalWrite(ArduinoMega::kExposureControl, op); 
}

void Control::GeneralControl::ControlFan(uint8_t op) {
    /*
     * Fan is ON during LOW CTRL
     */
    digitalWrite(ArduinoMega::kFanPwr, op);
}

void Control::GeneralControl::StartExposure() {
    /*
     * Start LED exposure by non-blocking method
     */
    ControlFan(OFF); // OFF the cooling fan while LED exposes due to lack of power
    ControlLED(ON);
    State::UpdateExposureStartTime(); // 노출 시작 시간을 저장
    State::SetExposing(true); 
}

void Control::GeneralControl::EndExposure() {
    while (State::GetExposing()) {
        CheckExposure();
    }
    ControlFan(ON); // Turn on the fan again
}

void Control::GeneralControl::CheckExposure() {
    /*
     * non-blocking method를 사용함에 있어 exposure 종료 조건을 확인하는 메서드
     */
    if (
        State::GetExposing() && 
        millis() - State::GetExposureStartTime() >= Parameter::EXPOSURE_TIME * 1000
        ) {
        ControlLED(OFF);
        State::SetExposing(false); 
    }
}

void Control::GeneralControl::Scan() {
    /* 
     * 스캔 동작 동안 mask는 웨이퍼와 y축 상에서 반대 방향으로 이동해야 함
     * 웨이퍼 이동 거리는 축소 linear ratio에 따라 달라짐
     * 위 또는 아래 방향으로 번갈아가며 웨이퍼를 이동시켜야 함
     * State::toggle_up_down은 웨이퍼의 이동 방향
     */
    motor_m.enable();
    motor_y.enable();

    StartExposure();
    
    int move_mask_direction = (State::GetDirection() == DOWN) ? 1 : -1;
    long move_mask_steps = move_mask_direction * (Parameter::MASK_Y / 2) * Parameter::SPD;

    motor_y.startMove(
        // non-blocking으로 wafer 이동 시작
        -1 * move_mask_steps / Parameter::LINEAR_SD_RATIO, // 축소 비율만큼 짧은 거리로 이동
        Parameter::EXPOSURE_TIME * 1E6
        );
    motor_m.startMove(
        // non-blocking으로 mask 이동 시작
        move_mask_steps,
        Parameter::EXPOSURE_TIME * 1E6
    );

    EndExposure();

    motor_m.disable();
    motor_y.disable();

    State::InvertDirection();
}

void Control::GeneralControl::Operate() {
    /*
     * 전체 Wafer에 대한 동작을 수행하는 High level method
     */
    while (
        State::GetX() < Parameter::DIES && 
        State::GetY() < Parameter::DIES
        ) { 
        // 현재 광원의 위치가 wafer를 벗어나지 않을 때까지

        if (kWaferArea[State::GetX()][State::GetY()]) { 
            // 유효한 wafer 영역 위에 광원이 위치하는 경우 Scan 수행
            Scan();
        }

        if (State::GetX() < Parameter::DIES - 1) { 
            // x 방향 끝까지 가지 않은 경우
            MotorControl::Move('x', Parameter::MASK_X);
            State::IncreaseX();
        } 
        else { 
            // x 방향 끝까지 간 경우
            if (State::GetY() < Parameter::DIES - 1) { 
                // y 방향 끝까지 가지 않은 경우
                MotorControl::Move('y', Parameter::MASK_Y);
                State::IncreaseY();
                GeneralControl::MoveToInitial('x');
            } 
            else {
                // x 방향과 y 방향 모두 끝까지 간 경우, 즉 모든 Die가 완료된 경우
                break;
            }
        }
    }
}
