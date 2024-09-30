# 2024년 2학기 조원주 교수님 3학년 실험 스캐너 프로젝트

<img src="img/full_1.jpg" width="50%" height="50%" title="full render 1"><img src="img/full_2.jpg" width="50%" height="50%" title="full render 2">

2024년 2학기 광운대학교 전자재료공학과 조원주 교수님 전자재료물성실험및설계2 수업의 **스캐너 설계를 위한 아두이노 프로젝트**입니다. 이 프로젝트는 리소그라피 장비인 스캐너의 정확한 동작을 구현하기 위해 모터와 센서, 릴레이 등의 구성요소를 제어합니다.

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

프로젝트에서 사용되는 각 네임스페이스와 클래스에 대한 설명입니다.

### Namespace: `Control`

제어와 관련된 모든 기능이 이 네임스페이스에 포함됩니다.

#### Class: `MotorControl`

모터 설정 및 제어를 담당하는 클래스입니다. 모든 메서드가 `static`으로 선언됩니다. 

- **Methods:**
  - `void SetMotor(A4988 &motor, uint8_t rpm, uint8_t step=1)`: 모터의 RPM과 마이크로스테핑을 설정
  - `void Rotate(A4988 &motor, const double angle)`: 모터를 지정한 각도만큼 회전
  - `void Move(const char axis, const double distance)`: 지정한 축을 따라 주어진 거리만큼 모터를 이동

#### Class: `GeneralControl`

시스템 초기화, 동작, 입력 대기 등 일반적인 시스템 제어를 담당하는 클래스입니다. 모든 상수와 메서드가 `static`으로 선언됩니다.

- **Constants:**
  - `uint32_t DBNC_DLY = 50;`: 버튼 디바운싱을 위한 지연 시간 (밀리초)
  - `uint8_t ON = 0x0;`: Active LOW에서의 ON 상태
  - `uint8_t OFF = 0x1;`: Active LOW에서의 OFF 상태

- **Methods:**
  - `void SetPinMode()`: 아두이노의 각종 핀 모드를 설정
  - `void InitializeSystem()`: 시스템을 초기화
    - 모터와 아두이노를 설정하고 초기 위치로 이동
  - `void WaitTillPressed(uint8_t pin)`: 특정 핀이 눌릴 때까지 대기
  - `void WaitForStart()`: 시작 버튼이 눌릴 때까지 대기
  - `void InitializePosition()`: 웨이퍼와 광원의 상대 위치를 초기화
  - `void ControlLED(uint8_t op)`: LED를 제어; `op`에 따라 ON/OFF를 설정
  - `void ControlFan(uint8_t op)`: 팬을 제어; `op`에 따라 ON/OFF를 설정
  - `void StartExposure()`: Exposure 시작
  - `void EndExposure()`: Exposure 종료
  - `void CheckExposure()`: Exposure 종료 조건을 확인
  - `void Scan()`: 스캔 동작을 수행
  - `void Operate()`: 웨이퍼 전체에 걸쳐 공정을 진행

#### Class: `Floor`

각 층의 두 모터를 층별로 한 번에 제어하기 위한 클래스입니다.

- **Members:**
  - `A4988 &left`: 각 층의 왼쪽 모터 객체에 대한 참조
  - `A4988 &right`: 각 층의 오른쪽 모터 객체에 대한 참조
  - `int16_t limit_x`: X축 리미트 스위치 핀 번호
    - 리미트 스위치가 없는 경우(2층) `-1`로 설정
  - `int16_t limit_y`: Y축 리미트 스위치 핀 번호

- **Methods:**
  - `void SetFloor(A4988 &m_left, A4988 &m_right, int16_t lim_y, int16_t lim_x=-1)`: 층의 왼쪽과 오른쪽 모터 및 리미트 스위치를 설정
  - `void Enable()`: 층의 모든 모터를 활성화
  - `void Disable()`: 층의 모든 모터를 비활성화
  - `void Move(char axis, double distance, bool is_exposure=false)`: 
    - 지정한 축을 따라 주어진 거리만큼 웨이퍼 스테이지 또는 레티클을 이동
    - Non-blocking method; 모터 이동 명령을 내리고 프로그램의 다음 라인으로 이동
    - `is_exposure`가 `true`일 경우, Exposure 시간 동안 이동
    - 그렇지 않을 경우 이동 시간은 설정된 모터 RPM에 따름
  - `void MoveBlock(char axis, double distance)`: 
    - 지정한 축을 따라 주어진 거리만큼 웨이퍼 스테이지 또는 레티클을 이동
    - Blocking method; 프로그램의 다음 라인으로 넘어가지 않음
  - `void MoveToInitial(char axis)`: 지정한 축의 초기 위치로 이동

#### Class: `State`

현재 웨이퍼의 위치, UV LED 제어를 위한 정보 등을 포함하는 시스템의 상태를 관리하는 클래스입니다. 모든 멤버 및 메서드가 `static`으로 선언됩니다.

- **Members:**
  - `Position current_pos`: 웨이퍼의 현재 위치
  - `uint8_t num_executions`: 실행 횟수
  - `unsigned long exposure_start_time`: 노출이 시작된 시간
  - `bool exposing`: UV LED의 ON/OFF 상태

- **Methods:**
  - `void SetX(uint16_t x_value)`: 현재 위치의 x 좌표를 설정
  - `void SetY(uint16_t y_value)`: 현재 위치의 y 좌표를 설정
  - `void IncreaseX()`: 현재 위치의 x 좌표를 1 증가
  - `void IncreaseY()`: 현재 위치의 y 좌표를 1 증가
  - `void IncreaseNumExecutions()`: 실행 횟수를 1 증가
  - `void InitState()`: 시스템 상태를 초기화
  - `void SetExposing(bool state)`: UV LED의 상태를 설정
  - `void UpdateExposureStartTime()`: 노출이 시작된 시간을 업데이트
  - `uint16_t GetX()`: 현재 위치의 x 좌표를 리턴
  - `uint16_t GetY()`: 현재 위치의 y 좌표를 리턴
  - `uint8_t GetNumExecutions()`: 실행 횟수를 리턴
  - `bool GetExposing()`: UV LED의 상태를 리턴
  - `unsigned long GetExposureStartTime()`: 노출이 시작된 시간을 리턴

---

### Namespace: `ArduinoMega`

시스템에 사용되는 아두이노 메가의 핀 할당 정보가 이 네임스페이스에 포함됩니다.

- **Constants:**
  - `uint8_t kStartButton = 50;`: 시작 버튼 (Digital 50)
  - `uint8_t kRestartButton = 51;`: 재시작 버튼 (Digital 51)
  - `uint8_t kExposureControl = 48;`: UV LED 제어를 위한 릴레이 제어 신호 (Digital 48)
  - `uint8_t kFanPwr = 49;`: 쿨링 팬 제어를 위한 릴레이 제어 신호 (Digital 49)
  - `uint8_t kLimitX1F = 14;`: 1층 x축 리미트 스위치 (Digital 14)
  - `uint8_t kLimitY1F = 15;`: 1층 y축 리미트 스위치 (Digital 15)
  - `uint8_t kLimitY2F = 16;`: 2층 y축 리미트 스위치 (Digital 16)

#### Namespace: `MotorPin`

모터 제어를 위한 아두이노 메가의 핀 할당 정보가 이 네임스페이스에 포함됩니다.

- **Constants:**
  - `uint8_t kEnable1FR = 22;`: 1층 오른쪽 모터 활성화 신호 (Digital 22)
  - `uint8_t kEnable1FL = 23;`: 1층 왼쪽 모터 활성화 신호 (Digital 23)
  - `uint8_t kEnable2FR = 24;`: 2층 오른쪽 모터 활성화 신호 (Digital 24)
  - `uint8_t kEnable2FL = 25;`: 2층 왼쪽 모터 활성화 신호 (Digital 25)
  - `uint8_t kStep1FR = 2;`: 1층 오른쪽 모터 스텝 신호 (Digital 2)
  - `uint8_t kStep1FL = 4;`: 1층 왼쪽 모터 스텝 신호 (Digital 4)
  - `uint8_t kStep2FR = 6;`: 2층 오른쪽 모터 스텝 신호 (Digital 6)
  - `uint8_t kStep2FL = 8;`: 2층 왼쪽 모터 스텝 신호 (Digital 8)
  - `uint8_t kDir1FR = 3;`: 1층 오른쪽 모터 방향 신호 (Digital 3)
  - `uint8_t kDir1FL = 5;`: 1층 왼쪽 모터 방향 신호 (Digital 5)
  - `uint8_t kDir2FR = 7;`: 2층 오른쪽 모터 방향 신호 (Digital 7)
  - `uint8_t kDir2FL = 9;`: 2층 왼쪽 모터 방향 신호 (Digital 9)
  - `uint8_t kMs1FR[] = {26, 27, 28};`: 1층 오른쪽 모터 마이크로스테핑 신호 (Digital 26, 27, 28)
  - `uint8_t kMs1FL[] = {29, 30, 31};`: 1층 왼쪽 모터 마이크로스테핑 신호 (Digital 29, 30, 31)
  - `uint8_t kMs2FR[] = {32, 33, 34};`: 2층 오른쪽 모터 마이크로스테핑 신호 (Digital 32, 33, 34)
  - `uint8_t kMs2FL[] = {35, 36, 37};`: 2층 왼쪽 모터 마이크로스테핑 신호 (Digital 35, 36, 37)

---

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

## Getting Started

1. **Upload the Code**: 아두이노와 PC를 연결한 뒤 `scanner.ino` 파일을 아두이노에 업로드
2. **Hardware Setup**: 
  - 아두이노와 모터, 스위치, 버튼, 릴레이 등 모든 구성 요소를 연결
  - Systembox를 조립하고 정격 전압 20V의 DC 어댑터를 메인보드에 연결
  - `parameter.h` 파일의 `ArduinoMega` 네임스페이스에 연결 정보를 업데이트
3. **Start Operation**: 전원을 켜고 시작 버튼을 눌러 작동을 시작

## Dependencies

- Arduino IDE
- Stepper motor drivers & library: A4988
- Scanner Hardware

## Team Members

광운대학교 전자재료공학과

- 2020734003 정지호
- 2020734004 조민규
- 2020734041 양승민
- 2020734047 정윤철
- 2022734025 김다인
- 2022734037 김유민

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
