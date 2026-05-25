// -----------------------------------------------------------------------------
// esp-lib — standalone sanity-check entry point.
// -----------------------------------------------------------------------------
// This file builds an `app_main()` so the lib can be flashed on its own as a
// PlatformIO project to verify the DTOs serialize correctly. It is moved to
// `examples/basic/` and excluded from consumer builds in v1.1.0 (task E3) —
// kept here in src/ for the E1 commit boundary only.
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
