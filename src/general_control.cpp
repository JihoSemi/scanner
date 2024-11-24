#include "../include/general_control.h"
#include "../include/motor_control.h"

// 유효한 Wafer 영역을 설정
const bool WAFER_AREA[Control::Parameter::DIES][Control::Parameter::DIES] = {
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
Position::Position() : x(0), y(0) {
    Serial.println("Position initialized: x = 0, y = 0");
}
uint16_t Position::GetX() const {
    Serial.print("GetX called, returning: ");
    Serial.println(x);
    return x;
}
uint16_t Position::GetY() const {
    Serial.print("GetY called, returning: ");
    Serial.println(y);
    return y;
}
void Position::IncreaseX() {
    x++;
    Serial.print("IncreaseX called, new x: ");
    Serial.println(x);
}
void Position::IncreaseY() {
    y++;
    Serial.print("IncreaseY called, new y: ");
    Serial.println(y);
}
void Position::SetX(uint16_t x_value) {
    x = x_value;
    Serial.print("SetX called, new x: ");
    Serial.println(x);
}
void Position::SetY(uint16_t y_value) {
    y = y_value;
    Serial.print("SetY called, new y: ");
    Serial.println(y);
}

// GeneralControl 클래스의 정적 멤버 변수 정의 추가
Control::Floor* Control::GeneralControl::floor[2] = {nullptr, nullptr};       // 두 층의 제어 객체
AccelStepper* Control::GeneralControl::motor[4] = {nullptr, nullptr, nullptr, nullptr};  // 네 Motor 객체
uint8_t Control::GeneralControl::enable[4] = {0};  // Motor Enable Pin 초기화
int16_t Control::GeneralControl::limit[3] = {0};   // Motor 제한 스위치 초기화

AccelStepper motor_1f_right = AccelStepper(
    AccelStepper::MotorInterfaceType::DRIVER,   // Motor Interface 타입을 DRIVER로 설정 (Step 및 Direction 제어)
    ArduinoMega::MotorPin::STEP_1F_R,           // 1F 오른쪽 Motor의 Step Pin
    ArduinoMega::MotorPin::DIR_1F_R             // 1F 오른쪽 Motor의 Direction Pin
    );
AccelStepper motor_1f_left = AccelStepper(
    AccelStepper::MotorInterfaceType::DRIVER, 
    ArduinoMega::MotorPin::STEP_1F_L, 
    ArduinoMega::MotorPin::DIR_1F_L
    );
AccelStepper motor_2f_right = AccelStepper(
    AccelStepper::MotorInterfaceType::DRIVER, 
    ArduinoMega::MotorPin::STEP_2F_R, 
    ArduinoMega::MotorPin::DIR_2F_R
    );
AccelStepper motor_2f_left = AccelStepper(
    AccelStepper::MotorInterfaceType::DRIVER, 
    ArduinoMega::MotorPin::STEP_2F_L, 
    ArduinoMega::MotorPin::DIR_2F_L
    );

Servo motor_z; // z축 이동을 위한 서보 모터
int current_angle = 30;

// Floor 클래스 메서드 정의
void Control::Floor::SetFloor(AccelStepper& m_left, AccelStepper& m_right, const uint8_t en_left, const uint8_t en_right, const int16_t lim_y, const int16_t lim_x)
{
    motor[LEFT] = &m_left;
    motor[RIGHT] = &m_right;
    enable[LEFT] = en_left;
    enable[RIGHT] = en_right;
    limit[X] = lim_x;
    limit[Y] = lim_y;

    Disable();
    Serial.println("SetFloor called, motors and limits set.");
}
void Control::Floor::Enable()
{
    // Active Low
    digitalWrite(enable[LEFT], LOW);
    digitalWrite(enable[RIGHT], LOW);
    Serial.println("Motors enabled.");
}
void Control::Floor::Disable()
{
    digitalWrite(enable[LEFT], HIGH);
    digitalWrite(enable[RIGHT], HIGH);
    Serial.println("Motors disabled.");
}
void Control::Floor::Move(const AxisIndex axis, const double distance)
{
    Serial.print("Move called, axis: ");
    Serial.print(axis == X ? "X" : "Y");
    Serial.print(", distance: ");
    Serial.println(distance);

    int16_t steps = Parameter::DPM * distance * Parameter::MICROSTEP;
    uint16_t speed = Parameter::MOTOR_SPEED * Parameter::MICROSTEP;
    int32_t left_destination;
    int32_t right_destination;

    if (axis == X)
    {
        left_destination = motor[LEFT]->currentPosition() + steps;
        right_destination = motor[RIGHT]->currentPosition() + steps;
    }
    else if (axis == Y)
    {
        left_destination = motor[LEFT]->currentPosition() + steps;
        right_destination = motor[RIGHT]->currentPosition() - steps;
    }
    motor[LEFT]->moveTo(left_destination);
    motor[RIGHT]->moveTo(right_destination);
    motor[LEFT]->setSpeed(speed);
    motor[RIGHT]->setSpeed(speed);

    Serial.print("Motors moving to positions: left = ");
    Serial.print(left_destination);
    Serial.print(", right = ");
    Serial.println(right_destination);

    Enable();
    while (motor[LEFT]->distanceToGo() != 0 || motor[RIGHT]->distanceToGo() != 0)
    {
        motor[LEFT]->runSpeedToPosition();
        motor[RIGHT]->runSpeedToPosition();

        // Emergency stop mechanism
        if (!digitalRead(ArduinoMega::START)) // START button can be used as an emergency stop button
        {
            motor[LEFT]->stop();
            motor[RIGHT]->stop();
            Disable();
            Serial.println("Emergency stop triggered during movement.");
            return;
        }
    }
    Disable();
    Serial.println("Move completed.");
}
void Control::Floor::MoveToInitial(const AxisIndex axis, bool second=false) {
    Serial.print("MoveToInitial called, axis: ");
    Serial.println(axis == X ? "X" : "Y");

    int16_t lim;

    if (axis == X)
        lim = limit[X];
    else if (axis == Y)
        lim = limit[Y];
    
    if (lim != -1 && digitalRead(lim) == HIGH)
    {   
        // 충분히 큰 거리 설정
        if (axis == X)
        {
            motor[LEFT]->moveTo(motor[LEFT]->currentPosition() + Parameter::INITIAL_MOVE_DISTANCE * Parameter::MICROSTEP);
            motor[RIGHT]->moveTo(motor[RIGHT]->currentPosition() + Parameter::INITIAL_MOVE_DISTANCE * Parameter::MICROSTEP);
            motor[LEFT]->setSpeed(Parameter::MOTOR_SPEED * Parameter::MICROSTEP);
            motor[RIGHT]->setSpeed(Parameter::MOTOR_SPEED * Parameter::MICROSTEP);
        }
        else if (axis == Y)
        {
            if (second) {
                motor[LEFT]->moveTo(motor[LEFT]->currentPosition() + Parameter::INITIAL_MOVE_DISTANCE * Parameter::MICROSTEP);
                motor[RIGHT]->moveTo(motor[RIGHT]->currentPosition() - Parameter::INITIAL_MOVE_DISTANCE * Parameter::MICROSTEP);
            }
            else {
                motor[LEFT]->moveTo(motor[LEFT]->currentPosition() - Parameter::INITIAL_MOVE_DISTANCE * Parameter::MICROSTEP);
                motor[RIGHT]->moveTo(motor[RIGHT]->currentPosition() + Parameter::INITIAL_MOVE_DISTANCE * Parameter::MICROSTEP);
            }
            motor[LEFT]->setSpeed(Parameter::MOTOR_SPEED * Parameter::MICROSTEP);
            motor[RIGHT]->setSpeed(Parameter::MOTOR_SPEED * Parameter::MICROSTEP);
        }
        else
        {
            Serial.println("Invalid axis");
            return;
        }
        Enable();
        while (digitalRead(lim) == HIGH)
        {
            if (digitalRead(lim) != HIGH)
            {
                motor[LEFT]->stop();
                motor[RIGHT]->stop();
                Serial.println("Reached initial position.");
                break;
            }

            if (!digitalRead(ArduinoMega::START)) // START button can be used as an emergency stop button
            {
                motor[LEFT]->stop();
                motor[RIGHT]->stop();
                Serial.println("Emergency stop triggered.");
                return;
            }
            motor[LEFT]->runSpeedToPosition();
            motor[RIGHT]->runSpeedToPosition();
        }
    }
    if (!second) {
        if (axis == X)
            Move(Floor::X, -Parameter::INITIAL_DIST_1F_X);
        else
            Move(Floor::Y, Parameter::INITIAL_DIST_1F_Y);
    }
    else {
        Move(Floor::Y, -Parameter::INITIAL_DIST_2F);
    }
    if (axis == X)
        State::SetX(Parameter::INIT_X);
    else if (axis == Y)
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
void Control::State::SetExposing(bool state) {
    exposing = state;
    Serial.print("SetExposing called, exposing set to: ");
    Serial.println(exposing ? "true" : "false");
}
void Control::State::IncreaseNumExecutions() {
    num_executions++;
    Serial.print("IncreaseNumExecutions called, new num_executions: ");
    Serial.println(num_executions);
}
void Control::State::InitState()
{
    num_executions = 0;
    exposing = false;
    Serial.println("InitState called, num_executions set to 0, exposing set to false");
}
void Control::State::UpdateExposureStartTime() {
    exposure_start_time = millis();
    Serial.print("UpdateExposureStartTime called, exposure_start_time set to: ");
    Serial.println(exposure_start_time);
}
uint16_t Control::State::GetX() { return current_pos.GetX(); }
uint16_t Control::State::GetY() { return current_pos.GetY(); }
uint8_t Control::State::GetNumExecutions() { return num_executions; }
bool Control::State::GetExposing() { return exposing; }
unsigned long Control::State::GetExposureStartTime() { return exposure_start_time; }

// GeneralControl 클래스의 메서드 정의
void Control::GeneralControl::ConfigSystem(
            Floor& first, Floor& second,
            AccelStepper& m_1f_l, AccelStepper& m_1f_r, AccelStepper& m_2f_l, AccelStepper& m_2f_r,
            uint8_t en_1f_l, uint8_t en_1f_r, uint8_t en_2f_l, uint8_t en_2f_r,
            int16_t lim_x_1f, int16_t lim_y_1f, int16_t lim_y_2f
        )
{   
    /*
     * 시스템 구성 설정
     */
    floor[FIRST] = &first;
    floor[SECOND] = &second;
    motor[LEFT_1F] = &m_1f_l;
    motor[RIGHT_1F] = &m_1f_r;
    motor[LEFT_2F] = &m_2f_l;
    motor[RIGHT_2F] = &m_2f_r;
    enable[LEFT_1F] = en_1f_l;
    enable[RIGHT_1F] = en_1f_r;
    enable[LEFT_2F] = en_2f_l;
    enable[RIGHT_2F] = en_2f_r;
    limit[X_1F] = lim_x_1f;
    limit[Y_1F] = lim_y_1f;
    limit[Y_2F] = lim_y_2f;

    Serial.println("System configuration complete.");
}
        
void Control::GeneralControl::SetPinMode()
{
    Serial.println("SetPinMode called.");
    /*
     * Arduino의 각종 제어 Pin mode 설정
     */
    pinMode(ArduinoMega::START, INPUT);
    pinMode(ArduinoMega::RESTART, INPUT);
    pinMode(ArduinoMega::EXPOSURE_CTRL, OUTPUT);
    pinMode(ArduinoMega::FAN_CTRL, OUTPUT);

    for (size_t i = 0; i < 3; i++)
    {
        // MicroStepping Pin
        pinMode(ArduinoMega::MotorPin::MICROSTEP_1F_R[i], OUTPUT);
        pinMode(ArduinoMega::MotorPin::MICROSTEP_1F_L[i], OUTPUT);
        pinMode(ArduinoMega::MotorPin::MICROSTEP_2F_R[i], OUTPUT);
        pinMode(ArduinoMega::MotorPin::MICROSTEP_2F_L[i], OUTPUT);
    }

    // Limit Switch Pin
    pinMode(ArduinoMega::LIMIT_1F_X, INPUT);
    pinMode(ArduinoMega::LIMIT_1F_Y, INPUT);
    pinMode(ArduinoMega::LIMIT_2F_Y, INPUT);

    // Motor Enable Pin
    pinMode(ArduinoMega::MotorPin::ENABLE_1F_L, OUTPUT);
    pinMode(ArduinoMega::MotorPin::ENABLE_1F_R, OUTPUT);
    pinMode(ArduinoMega::MotorPin::ENABLE_2F_L, OUTPUT);
    pinMode(ArduinoMega::MotorPin::ENABLE_2F_R, OUTPUT);
}

void Control::GeneralControl::InitializeSystem()
{
    /*
     * 시스템 초기화
     * Motor 설정, PinMode 설정
     * Floor 설정
     * 제어 기본값 적용
     * 초기 위치 이동
     */

    Serial.println("InitializeSystem called.");
    Serial.begin(115200);
    State::InitState();
    SetPinMode();

    // Set the speed and Microstepping
    Control::MotorControl::SetMotor(motor_1f_right);
    Control::MotorControl::SetMotor(motor_1f_left);  
    Control::MotorControl::SetMotor(motor_2f_right);
    Control::MotorControl::SetMotor(motor_2f_left);  

    motor_z.attach(ArduinoMega::MotorPin::SERVOPWM);
    motor_z.write(current_angle);

    first_floor.SetFloor(
        // Set the first floor with two motors and two position limiting switches
        motor_1f_left, motor_1f_right,
        ArduinoMega::MotorPin::ENABLE_1F_L, ArduinoMega::MotorPin::ENABLE_1F_R,
        ArduinoMega::LIMIT_1F_Y, ArduinoMega::LIMIT_1F_X
        );

    second_floor.SetFloor(
        // Set the second floor with two motors and one position limiting switches
        motor_2f_left, motor_2f_right,
        ArduinoMega::MotorPin::ENABLE_2F_L, ArduinoMega::MotorPin::ENABLE_2F_R,
        ArduinoMega::LIMIT_2F_Y
        );

    ControlLED(OFF);
    // ControlFan(ON);

    ConfigSystem(
        first_floor, second_floor, 
        motor_1f_left, motor_1f_right, motor_2f_left, motor_2f_right, 
        ArduinoMega::MotorPin::ENABLE_1F_L, ArduinoMega::MotorPin::ENABLE_1F_R,
        ArduinoMega::MotorPin::ENABLE_2F_L, ArduinoMega::MotorPin::ENABLE_2F_R,
        ArduinoMega::LIMIT_1F_X, ArduinoMega::LIMIT_1F_Y, ArduinoMega::LIMIT_2F_Y
    );
}

void Control::GeneralControl::WaitTillPressed(const uint8_t pin)
{
    Serial.print("WaitTillPressed called, waiting for pin: ");
    Serial.println(pin);
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
                Serial.print("Button pressed and released, pin: ");
                Serial.println(pin);
            }
        }
    }
}

void Control::GeneralControl::WaitForStart()
{
    Serial.println("WaitForStart called.");
    /*
     * Wating until the start button is pressed if an initial excution
     */
    if (!State::GetNumExecutions())
    {
        WaitTillPressed(ArduinoMega::START);
        State::IncreaseNumExecutions();
    }
}

void Control::GeneralControl::InitializePosition()
{
    Serial.println("InitializePosition called.");
    first_floor.MoveToInitial(Floor::X);
    first_floor.MoveToInitial(Floor::Y);
    second_floor.MoveToInitial(Floor::Y, true);
}

void Control::GeneralControl::ControlLED(uint8_t op)
{
    Serial.print("ControlLED called, operation: ");
    Serial.println(op == OFF ? "OFF" : "ON");
    /*
     * LED is ON during LOW CTRL
     */
    digitalWrite(ArduinoMega::EXPOSURE_CTRL, op);
}

void Control::GeneralControl::ControlFan(uint8_t op)
{
    Serial.print("ControlFan called, operation: ");
    Serial.println(op == OFF ? "OFF" : "ON");
    /*
     * Fan is ON during LOW CTRL
     */
    digitalWrite(ArduinoMega::FAN_CTRL, op);
}

void Control::GeneralControl::StartExposure()
{
    Serial.println("StartExposure called.");
    /*
     * Start LED exposure by non-blocking method
     */
    // ControlFan(OFF); // OFF the cooling fan while LED exposes due to lack of power
    ControlLED(ON);
    State::UpdateExposureStartTime(); // 노출 시작 시간을 저장
    State::SetExposing(true);
}

void Control::GeneralControl::EndExposure()
{
    Serial.println("EndExposure called.");
    while (State::GetExposing())
    {
        if (
        State::GetExposing() &&
        millis() - State::GetExposureStartTime() >= Parameter::EXPOSURE_TIME * 1000)
        {
            ControlLED(OFF);
            State::SetExposing(false);
            Serial.println("Exposure completed, LED turned off.");
        }
    }
    // ControlFan(ON); // Turn on the fan again
    Serial.println("Exposure ended.");
}

void Control::GeneralControl::ZAlign() {
    Serial.println("Enter z axis distance to move within 0 and 180");

    while (!Serial.available()) {
        // 입력 대기 (여기서 멈춤)
    }
    String input = Serial.readStringUntil('\n');
    Serial.println(input);
    input.trim();
    int target_angle = constrain(input.toInt(), 0, 180); // 목표 각도 (0~180도 사이)

    MotorControl::moveServo(motor_z, current_angle, target_angle);

    current_angle = target_angle;
    Serial.print("Z axis position moved to ");
    Serial.print(target_angle);
    Serial.println(".");
}

void Control::GeneralControl::ScanMove(bool reverse)
{
    Serial.print("ScanMove called, reverse: ");
    Serial.println(reverse ? "true" : "false");

    int16_t steps[2];
    steps[SECOND] = Parameter::MASK_Y * Parameter::DPM * Parameter::MICROSTEP;      // steps for mask motor
    steps[FIRST] = -1 * Parameter::DIE_Y * Parameter::DPM * Parameter::MICROSTEP;   // steps for wafer motor

    uint16_t speed[2];
    // 단위 검토 필요
    speed[FIRST] = static_cast<uint16_t>(round(static_cast<double>(abs(steps[FIRST])) / (Parameter::EXPOSURE_TIME)));   // 단위 확인하여 수정 필요
    speed[SECOND] = static_cast<uint16_t>(round(static_cast<double>(abs(steps[SECOND])) / (Parameter::EXPOSURE_TIME)));

    if (reverse)
    {
        // reverse==true인 경우 반대 Direction으로 이동할 수 있도록 step 수를 반전 및 속도 재설정
        steps[FIRST] *= -1;
        steps[SECOND] *= -1;
        speed[FIRST] = Parameter::MOTOR_SPEED * Parameter::MICROSTEP;
        speed[SECOND] = Parameter::MOTOR_SPEED * Parameter::MICROSTEP;
    }

    int32_t destinations[4];
    destinations[LEFT_1F] = motor[LEFT_1F]->currentPosition() + steps[FIRST];       // destination of 1F LEFT motor to go
    destinations[RIGHT_1F] = motor[RIGHT_1F]->currentPosition() - steps[FIRST];     // destination of 1F RIGHT motor to go
    destinations[LEFT_2F] = motor[LEFT_2F]->currentPosition() - steps[SECOND];      // destination of 2F LEFT motor to go
    destinations[RIGHT_2F] = motor[RIGHT_2F]->currentPosition() + steps[SECOND];    // destination of 2F RIGHT motor to go

    motor[LEFT_1F]->moveTo(destinations[LEFT_1F]);
    motor[RIGHT_1F]->moveTo(destinations[RIGHT_1F]);
    motor[LEFT_2F]->moveTo(destinations[LEFT_2F]);
    motor[RIGHT_2F]->moveTo(destinations[RIGHT_2F]);

    motor[LEFT_1F]->setSpeed(speed[FIRST]);
    motor[RIGHT_1F]->setSpeed(speed[FIRST]);
    motor[LEFT_2F]->setSpeed(speed[SECOND]);
    motor[RIGHT_2F]->setSpeed(speed[SECOND]);

    Serial.println("Motors moving to destinations.");

    first_floor.Enable();
    second_floor.Enable();
    while (
        motor[LEFT_1F]->distanceToGo() != 0 ||
        motor[RIGHT_1F]->distanceToGo() != 0 ||
        motor[LEFT_2F]->distanceToGo() != 0 ||
        motor[RIGHT_2F]->distanceToGo() != 0
    )
    {
        motor[LEFT_1F]->runSpeedToPosition();
        motor[RIGHT_1F]->runSpeedToPosition();
        motor[LEFT_2F]->runSpeedToPosition();
        motor[RIGHT_2F]->runSpeedToPosition();

        // Emergency stop condition
        if (digitalRead(ArduinoMega::START) == LOW) // START button can be used as an emergency stop button
        {
            motor[LEFT_1F]->stop();
            motor[RIGHT_1F]->stop();
            motor[LEFT_2F]->stop();
            motor[RIGHT_2F]->stop();
            first_floor.Disable();
            second_floor.Disable();
            Serial.println("Emergency stop triggered.");
            return;
        }
    }
    first_floor.Disable();
    second_floor.Disable();
    Serial.println("ScanMove completed.");
}

void Control::GeneralControl::Scan()
{
    Serial.println("Scan called.");
    /*
     * 스캔 동작 동안 mask는 웨이퍼와 y축 상에서 반대 Direction으로 이동해야 함
     * 웨이퍼 이동 거리는 축소 linear ratio에 따라 달라짐
     * 스캔을 1회 완료하면 스캔하는 동안 달라진 위치를 다시 원상복귀 시켜야 함
     * ㄹ 자로 이동하면 인접한 die의 pattern이 상하 반전된 상태로 있으므로 후속 공정이 어려움
     */

    StartExposure();
    ScanMove(FORWARD);
    EndExposure();
    ScanMove(REVERSE); // Scan 동안 움직인 위치를 초기화
    Serial.println("Scan completed.");
}

void Control::GeneralControl::Operate()
{
    int n_valid_die = 0;
    Serial.println("Operate called.");
    /*
     * 전체 Wafer에 대한 동작을 수행하는 High level method
     */
    while (
        State::GetX() < Parameter::DIES &&
        State::GetY() < Parameter::DIES
        )
    {
        // 현재 광원의 위치가 wafer를 벗어나지 않을 때까지
        if (WAFER_AREA[State::GetX()][State::GetY()])
        {
            n_valid_die++;
            if (n_valid_die == 1) {
                ZAlign();
                delay(500);
            }
            
            // 유효한 wafer 영역 위에 광원이 위치하는 경우 Scan 수행
            Serial.print("Valid wafer area detected at X: ");
            Serial.print(State::GetX());
            Serial.print(", Y: ");
            Serial.println(State::GetY());
            Scan();
        }

        if (State::GetX() < Parameter::DIES - 1)
        {
            // x Direction 끝까지 가지 않은 경우
            Serial.println("Moving to next position in X direction.");
            first_floor.Move(Floor::X, -1 * Parameter::DIE_X);
            State::IncreaseX();
        }
        else
        {
            // x Direction 끝까지 간 경우
            if (State::GetY() < Parameter::DIES - 1)
            {
                // y Direction 끝까지 가지 않은 경우
                Serial.println("Reached end of X direction, moving in Y direction.");
                first_floor.Move(Floor::Y, Parameter::DIE_Y);
                State::IncreaseY();
                first_floor.MoveToInitial(Floor::X);
            }
            else
            {
                // x, y Direction 모두 끝까지 간 경우, 즉 모든 Die가 완료된 경우
                Serial.println("All dies completed.");
                break;
            }
        }
    }
}
