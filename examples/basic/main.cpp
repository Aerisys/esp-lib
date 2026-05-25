// -----------------------------------------------------------------------------
// esp-lib — Basic standalone example (sanity check).
// -----------------------------------------------------------------------------
// This file is NOT part of the library payload. It exists only to allow
// building and flashing the lib as a standalone PlatformIO / ESP-IDF project,
// to verify that the DTOs serialize/deserialize correctly after a change.
//
// When `esp-lib` is consumed as a dependency by another project (drone,
// controller), this file is NOT compiled: PlatformIO's library manager only
// pulls `src/*.cpp` + `include/*.h` via the rules in `library.json`, and the
// standalone CMakeLists only adds this example when present.
// -----------------------------------------------------------------------------

#include <esp_log.h>
#include "JoystickModel.h"
#include "FlightController.h"
#include "ControllerRequestDTO.h"

extern "C" void app_main()
{
    // Construction d'un DTO avec la nouvelle API POD (v1.1.0).
    ControllerRequestDTO dto;
    dto.has_buttonMotorArming = true;
    dto.buttonMotorArming     = true;
    dto.initCounter();

    ESP_LOGI("TEST", "dto: %s", dto.toString().c_str());

    // Round-trip wire (toStruct -> fromStruct).
    ControllerRequestData dtodata = dto.toStruct();
    ControllerRequestDTO dto2 = ControllerRequestDTO::fromStruct(dtodata);
    ESP_LOGI("TEST", "dto2: %s", dto2.toString().c_str());

    // Copie par valeur (zero heap, plus de fuite possible).
    ControllerRequestDTO dto3 = dto2;
    ESP_LOGI("TEST", "dto3: %s", dto3.toString().c_str());
}
