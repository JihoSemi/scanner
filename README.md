# 2024년 2학기 조원주 교수님 3학년 실험 스캐너 프로젝트

2024년 2학기 광운대학교 전자재료공학과 조원주 교수님 전자재료물성실험및설계2 수업의 스캐너 설계를 위한 아두이노 프로젝트입니다. 이 프로젝트는 리소그라피 장비인 스캐너의 정확한 동작을 구현하기 위해 모터와 센서, 릴레이 등의 구성요소를 제어합니다.

## Project Structure

프로젝트는 아래의 구조로 이루어진 파일으로 구성되어 있습니다:

```bash
scanner
├── README.md
├── circuit_data               
│   └── main_board.diy          - 메인보드의 PCB 회로도
├── include
│   ├── general_control.h       - 일반 설정, 제어에 관련된 클래스와 변수 선언
│   ├── motor_control.h         - 모터 설정, 제어에 관련된 클래스와 변수 선언
│   ├── parameter.h             - 시스템 매개변수와 아두이노 구성 설정
│   └── scanner.h               
├── scanner.ino                 - 메인 .ino 파일
└── src
    ├── general_control.cpp     - 일반 설정, 제어에 관련된 클래스와 변수 정의
    └── motor_control.cpp       - 모터 설정, 제어에 관련된 클래스와 변수 정의
```

## Namespaces and Classes

프로젝트의 각 네임스페이스와 클래스에 관한 설명입니다. 

### Namespace: `Control`
제어와 관련된 모든 기능이 이 네임스페이스에 포함됩니다.

#### Class: `MotorControl`
모터 설정, 제어를 담당하는 클래스입니다. 

- **Methods:**
  - `static void SetMotor(A4988 &motor, uint8_t rpm, uint8_t step=1)`: 모터의 RPM과 마이크로스테핑을 설정
  - `static void _SetMicroStep(const uint8_t* kMs, uint8_t step=1)`: 모터의 마이크로스테핑을 설정, 더이상 사용되지 않음         
  - `static void Rotate(A4988 &motor, const double angle)`: 모터를 지정한 각도만큼 회전
  - `static void Move(const char axis, const double distance)`: 모터에 연결된 물체를 지정한 거리만큼 이동

#### Class: `GeneralControl`
초기화, 동작, 입력 대기 등의 일반적인 시스템 제어를 담당하는 클래스입니다.

- **Methods:**
  - `void InitializeSystem()`: 시스템 초기화; 모터와 아두이노를 설정하고 초기 위치로 이동 
  - `void WaitForStart()`: 시작 버튼이 눌릴 때까지 작동을 대기
  - `void InitializePosition()`: 웨이퍼와 광원의 상대 위치를 초기화
  - `void Operate()`: 웨이퍼 전체에 걸쳐 공정을 1회 진행
  - `void WaitTillPressed(uint8_t button_pin)`: 특정한 버튼이 눌릴 때까지 작동을 대기

### Class: `Position`
웨이퍼와 광원의 상대적인 위치를 좌표 형태로 처리하는 클래스입니다.

- **Members:**
  - `uint16_t x`: 위치의 x 좌표
  - `uint16_t y`: 위치의 y 좌표

- **Methods:**
  - `uint16_t GetX() const`: x 좌표를 리턴
  - `uint16_t GetY() const`: y 좌표를 리턴
  - `void SetX(uint16_t x_value)`: x 좌표를 설정
  - `void SetY(uint16_t y_value)`: y 좌표를 설정
  - `void IncreaseX()`: x 좌표를 1 증가
  - `void IncreaseY()`: y 좌표를 1 증가

#### Class: `State`
현재 웨이퍼의 위치, UV LED 제어를 위한 정보 등을 포함하는 시스템의 현재 상태를 다루는 클래스입니다.

- **Members:**
  - `Position current_pos`: 웨이퍼의 현재 위치
  - `uint8_t num_excuse`: 실행한 횟수
  - `unsigned long exposure_start_time`: Expose가 시작된 시각
  - `bool exposing`: UV LED의 ON/OFF 상태
  - `bool toggle_up_down`: 다음 `scan()`의 이동 방향

- **Methods:**
  - `void SetX(uint16_t x_value)`: 현재 위치의 x 좌표를 설정
  - `void SetY(uint16_t y_value)`: 현재 위치의 y 좌표를 설정
  - `void IncreaseX()`: 현재 위치의 x 좌표를 1 증가
  - `void IncreaseY()`: 현재 위치의 y 좌표를 1 증가
  - `void InvertDirection()`: 이동 방향을 반전
  - `void SetExposing(bool state)`: UV LED의 상태를 설정
  - `void IncreaseNumExcuse()`: 실행 횟수를 1 증가
  - `void InitState()`: 시스템 상태를 초기화
  - `void UpdateExposureStartTime()`: Expose가 시작된 시각을 업데이트

### Namespace: `ArduinoMega`
시스템에 사용되는 아두이노 메가의 핀 할당 정보가 이 네임스페이스에 포함됩니다.

- **Constants:**
  - `constexpr uint8_t kStartButton`: 시작 버튼 (Digital 50)
  - `constexpr uint8_t kRestartButton`: 재시작 버튼 (Digital 51)
  - `constexpr uint8_t kExposureUnit`: UV LED ON/OFF를 담당하는 릴레이 제어 신호 (Digital 48)
  - `constexpr uint8_t kFanPwr`: 쿨링팬 ON/OFF를 담당하는 릴레이 제어 신호 (Digital 49)
  - `constexpr uint8_t kIrX`: x축 적외선 센서 신호 (Analog 15)
  - `constexpr uint8_t kIrY`: y축 적외선 센서 신호 (Analog 14)

#### Namespace: `MotorPin`
아두이노 메가의 핀 할당 정보 중 모터 제어를 위한 핀 정보는 이 네임스페이스(`ArduinoMega::MotorPin`)에 포함됩니다.

- **Constants:**
  - `constexpr uint8_t kEnableX`: x축 모터 활성화 신호 (Digital 22)
  - `constexpr uint8_t kEnableY`: y축 모터 활성화 신호 (Digital 23)
  - `constexpr uint8_t kEnableM`: 마스크(레티클) 모터 활성화 신호 (Digital 24)
  - `constexpr uint8_t kStepX`: x축 모터 Step 제어 신호 (Digital 2)
  - `constexpr uint8_t kStepY`: y축 모터 Step 제어 신호 (Digital 4)
  - `constexpr uint8_t kStepM`: 마스크(레티클) 모터 Step 제어 신호 (Digital 6)
  - `constexpr uint8_t kDirX`: x축 모터 방향 제어 신호 (Digital 3)
  - `constexpr uint8_t kDirY`: y축 모터 방향 제어 신호 (Digital 5)
  - `constexpr uint8_t kDirM`: 마스크(레티클) 모터 방향 제어 신호 (Digital 7)
  - `constexpr uint8_t kMsX[]`: x축 모터 마이크로스테핑 설정 신호 (Digital 25, 26, 27)
  - `constexpr uint8_t kMsY[]`: y축 모터 마이크로스테핑 설정 신호 (Digital 28, 29, 30)
  - `constexpr uint8_t kMsM[]`: 마스크(레티클) 모터 마이크로스테핑 설정 신호 (Digital 31, 32, 33)

## Getting Started

1. **Hardware Setup**: 아두이노와 모터, 센서, 버튼, 릴레이 등 모든 구성요소를 연결하고 `parameter.h` 파일의 `ArduinoMega`에 연결 정보를 업데이트 합니다. 이후 전원을 켭니다.
2. **Upload the Code**: `scanner.ino` 파일을 아두이노에 업로드합니다.
3. **Start Operation**: 시작 버튼을 눌러 작동을 시작합니다.

## Dependencies

- Arduino IDE
- Stepper motor drivers & library: A4988
- Scanner Hardware

## Team Members

광운대학교 전자재료공학과

- 2020734003 정지호
- 2020734003 조민규
- 2020734041 양승민
- 2020734047 정윤철
- 2022734025 김다인
- 2022734037 김유민

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
