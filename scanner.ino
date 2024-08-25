#include "include/scanner.h"

void setup() {
    Control::GeneralControl::InitializeSystem();
}

void loop() {
    Control::GeneralControl::WaitForStart();
    Control::GeneralControl::InitializePosition();
    Control::GeneralControl::Operate();
    Control::GeneralControl::WaitTillPressed(ArduinoMega::kRestartButton);
}
