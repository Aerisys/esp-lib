#include <esp_log.h>
#include "JoystickModel.h"
#include "FlightController.h"
#include "ControllerRequestDTO.h"

extern "C" void app_main()
{
    // Création du DTO
    ControllerRequestDTO dto;

    // Création et assignation dynamique du FlightController
    dto.flightController = nullptr;
    dto.buttonMotorState = nullptr;
    dto.buttonMotorArming = new bool(true);
    dto.initCounter();

    // Affichage des logs avec `toString()`
    ESP_LOGI("TEST", "dto: %s", dto.toString().c_str());

    ControllerRequestData dtodata = dto.toStruct();

    ControllerRequestDTO dto2 = ControllerRequestDTO::fromStruct(dtodata);

    ESP_LOGI("TEST", "dto2: %s", dto2.toString().c_str());

    ControllerRequestDTO dto3 = ControllerRequestDTO(dto2);
    dto3.~ControllerRequestDTO();

    ESP_LOGI("TEST", "dto3: %s", dto3.toString().c_str());

    // Pas besoin de delete, le destructeur de `dto` s'en charge
}
