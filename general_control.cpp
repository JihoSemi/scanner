#include "general_control.h"
#include "motor_control.h"

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

uint16_t Position::GetX() const { return x; }
uint16_t Position::GetY() const { return y; }
void Position::IncreaseX() { x++; }
void Position::IncreaseY() { y++; }
void Position::SetX(uint16_t x_value) { x = x_value; }
void Position::SetY(uint16_t y_value) { y = y_value; }

Position Control::State::current_pos;
uint8_t Control::State::num_excuse;                
unsigned long Control::State::exposure_start_time;   
bool Control::State::exposing;                       
bool Control::State::toggle_up_down;

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


void Control::GeneralControl::SetPinMode() {
    pinMode(ArduinoMega::kStartButton, INPUT);
    pinMode(ArduinoMega::kRestartButton, INPUT);
    pinMode(ArduinoMega::kExposureControl, OUTPUT);
    // pinMode(ArduinoMega::MotorPin::kEnableM, OUTPUT);
    
    for (size_t i = 0; i < 3; i++)
    {
        pinMode(ArduinoMega::MotorPin::kMsX[i], OUTPUT);
        pinMode(ArduinoMega::MotorPin::kMsY[i], OUTPUT);
        pinMode(ArduinoMega::MotorPin::kMsM[i], OUTPUT);
    }
    
    pinMode(ArduinoMega::kIrX, INPUT);
    pinMode(ArduinoMega::kIrY, INPUT);
}

void Control::GeneralControl::SetMicroStep(const uint8_t* kMs, uint8_t step) {
    /*
     * 각 모터의 Microstepping 설정 핀 배열과 설정할 스텝을 매개변수로 받음
     * 1, 2, 4, 8, 16 순서대로 Full, Half, Quarter, Eighth, Sixteenth Step
     * 기본값 Full Step이며 위 숫자 외 다른 숫자를 매개변수로 입력할 경우 Full step으로 설정
     * digitalWrite 함수로 Microstepping resolution table에 맞게 각 핀의 값을 설정
     */
    // Microstepping resolution table: {MS1, MS2, MS3}
    const uint8_t microstep_table[5][3] = {
        {LOW,   LOW,    LOW},   // Full Step
        {HIGH,  LOW,    LOW},   // Half Step
        {LOW,   HIGH,   LOW},   // Quarter Step
        {HIGH,  HIGH,   LOW},   // Eighth Step
        {HIGH,  HIGH,   HIGH}   // Sixteenth Step
    };
    
    uint8_t index = (step == 1 || step == 2 || step == 4 || step == 8 || step == 16) ? __builtin_ctz(step) : 0;

    for (int i = 0; i < 3; ++i) {
        digitalWrite(kMs[i], microstep_table[index][i]);
    }
}

void Control::GeneralControl::InitializeSystem() {
    // Serial.begin(115200);

    State::InitState();

    MotorControl::SetMotor(motor_x, 60); // Set the speed for the x-motor
    MotorControl::SetMotor(motor_y, 60); // Set the speed for the y-motor
    MotorControl::SetMotor(motor_m, 60); // Set the speed for the mask motor

    SetPinMode();

    digitalWrite(ArduinoMega::kExposureControl, OFF);
    digitalWrite(ArduinoMega::MotorPin::kEnableX, OFF);
    digitalWrite(ArduinoMega::MotorPin::kEnableY, OFF);
    digitalWrite(ArduinoMega::MotorPin::kEnableM, OFF);

    SetMicroStep(ArduinoMega::MotorPin::kMsX, 16);
    SetMicroStep(ArduinoMega::MotorPin::kMsY, 16);
    SetMicroStep(ArduinoMega::MotorPin::kMsM, 16);

    InitializePosition();
};  

void Control::GeneralControl::WaitTillPressed(const uint8_t pin) {
    // Waiting for pressing Pull-Up switch
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
    if (!State::GetNumExcuse()) { 
        // Wating until the start button is pressed if an initial excution
        WaitTillPressed(ArduinoMega::kStartButton);
        State::IncreaseNumExcuse();
    }
}

void Control::GeneralControl::InitializePosition() {
    MotorControl::MoveToInitial('x'); 
    MotorControl::MoveToInitial('y'); 
}

void Control::GeneralControl::ControlLED(uint8_t op) {
    digitalWrite(ArduinoMega::kExposureControl, op); // LED is ON during LOW CTRL
}

void Control::GeneralControl::StartExposure() {
    ControlLED(ON);
    State::UpdateExposureStartTime(); // 노출 시작 시간을 저장
    State::SetExposing(true); 
}

void Control::GeneralControl::EndExposure() {
    while (State::GetExposing()) {
        CheckExposure();
    }
}

void Control::GeneralControl::CheckExposure() {
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
     * 스캔 동작 동안 y축 웨이퍼와 반대 방향으로 이동해야 함
     * 웨이퍼 이동 거리는 축소 linear ratio에 따라 달라짐
     * 위 또는 아래 방향으로 번갈아가며 웨이퍼를 이동시켜야 함
     * State::toggle_up_down은 웨이퍼의 이동 방향
     * motor_y와 motor_mask의 STEP 단자에 병렬로 연결
     * MotorPin::kDirY를 inverting하여 motor_mask의 dir 단자에 연결 -> 반대 방향 동작 수행
     * MotorPin::kEnableM 신호를 이용해 원하는 때만 Mask를 움직이도록 구현
     */
    // digitalWrite(ArduinoMega::MotorPin::kEnableM, ON);
    motor_m.enable();
    motor_y.enable();

    StartExposure();
    
    int move_mask_direction = (State::GetDirection() == DOWN) ? 1 : -1;
    long move_mask_steps = move_mask_direction * (Parameter::MASK_Y / 2) * Parameter::SPD;

    motor_y.startMove(
        -1 * move_mask_steps / Parameter::LINEAR_SD_RATIO,
        Parameter::EXPOSURE_TIME * 1E6
        );
    motor_m.startMove(
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
     * DIE 하나에 대한 작동
     */
    while (
        State::GetX() < Parameter::DIES && 
        State::GetY() < Parameter::DIES
        ) { // 현재 광원의 위치가 wafer를 벗어나지 않을 때까지
        if (kWaferArea[State::GetX()][State::GetY()]) { // 유효한 wafer 영역 위에 광원이 위치하는 경우
            Scan();
        }

        if (State::GetX() < Parameter::DIES - 1) { // x 방향 끝까지 가지 않은 경우
            MotorControl::Move('x', Parameter::MASK_X);
            State::IncreaseX();
        } else { 

            if (State::GetY() < Parameter::DIES - 1) { // y 방향 끝까지 가지 않은 경우
                MotorControl::Move('y', Parameter::MASK_Y);
                State::IncreaseY();
                MotorControl::MoveToInitial('x');
            } else {
                break;
            }
        }
    }
}
