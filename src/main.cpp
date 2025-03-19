#include <esp_log.h>
#include "ControllerRequestDTO.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern "C" void app_main() {
    ESP_LOGI("TEST", "Démarrage du test de sérialisation");
    vTaskDelay(pdMS_TO_TICKS(1000));

    // Création d'un FlightController
    FlightController fc(1.0f, 2.0f, 3.0f, 4.0f);
    ESP_LOGI("TEST", "FlightController créé : Pitch=%.2f, Roll=%.2f, Yaw=%.2f, Throttle=%.2f",
             fc.pitch, fc.roll, fc.yaw, fc.throttle);
    vTaskDelay(pdMS_TO_TICKS(1000));

    // Conversion en structure
    FlightControllerData fcData = fc.toStruct();
    ESP_LOGI("TEST", "FlightController converti en structure : Pitch=%.2f, Roll=%.2f, Yaw=%.2f, Throttle=%.2f",
             fcData.pitch, fcData.roll, fcData.yaw, fcData.throttle);
    vTaskDelay(pdMS_TO_TICKS(1000));

    // Restauration depuis la structure
    FlightController fcRestored = FlightController::fromStruct(fcData);
    ESP_LOGI("TEST", "FlightController restauré : Pitch=%.2f, Roll=%.2f, Yaw=%.2f, Throttle=%.2f",
             fcRestored.pitch, fcRestored.roll, fcRestored.yaw, fcRestored.throttle);
    vTaskDelay(pdMS_TO_TICKS(1000));

    // Création d'un ControllerRequestDTO avec un bouton activé
    ControllerRequestDTO dto;
    dto.flightController = new FlightController(fc);
    dto.buttonMotorState = new bool(true);
    dto.buttonEmergencyStop = new bool(false);
    dto.initCounter();

    ESP_LOGI("TEST", "ControllerRequestDTO créé : Counter=%llu, MotorState=%d, EmergencyStop=%d",
             dto.getCounter(), *dto.buttonMotorState, *dto.buttonEmergencyStop);
    vTaskDelay(pdMS_TO_TICKS(1000));

    // Conversion en structure
    ControllerRequestData dtoData = dto.toStruct();
    ESP_LOGI("TEST", "ControllerRequestDTO converti en structure : Counter=%llu, MotorState=%d, EmergencyStop=%d",
             dtoData.counter, dtoData.buttonMotorState, dtoData.buttonEmergencyStop);
    vTaskDelay(pdMS_TO_TICKS(1000));

    // Restauration depuis la structure
    ControllerRequestDTO dtoRestored = ControllerRequestDTO::fromStruct(dtoData);
    ESP_LOGI("TEST", "ControllerRequestDTO restauré : Counter=%llu, MotorState=%d, EmergencyStop=%d",
             dtoRestored.getCounter(), *dtoRestored.buttonMotorState, *dtoRestored.buttonEmergencyStop);
    vTaskDelay(pdMS_TO_TICKS(1000));
}
