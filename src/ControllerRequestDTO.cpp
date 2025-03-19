#include "../include/ControllerRequestDTO.h"
#include "ControllerRequestDTO.h"

uint64_t ControllerRequestDTO::nmbInstanciation = 0;
ControllerRequestDTO::ControllerRequestDTO()
{
}
ControllerRequestDTO::ControllerRequestDTO(const ControllerRequestDTO &other)
{
    if (other.flightController)
    flightController = new FlightController(*other.flightController);

    buttonEmergencyStop = other.buttonEmergencyStop;
    buttonMotorState=other.buttonMotorState;
    counter = other.counter;
}
ControllerRequestDTO::~ControllerRequestDTO() {
    delete flightController;
}
void ControllerRequestDTO::initCounter()
{
    counter = ControllerRequestDTO::nmbInstanciation++;
}
uint64_t ControllerRequestDTO::getCounter() const
{
    return counter;
}
bool ControllerRequestDTO::operator==(const ControllerRequestDTO &other) const
{
    // Vérifie si les deux pointeurs `joystickLeft` sont nuls
    if (flightController == nullptr && other.flightController == nullptr) {
    } else if (flightController == nullptr || other.flightController == nullptr) {
        return false; // Un pointeur est nul, l'autre ne l'est pas
    } else if (!(*flightController == *(other.flightController))) {
        return false;
    }

    // Vérifie si les deux pointeurs `buttonEmergencyStop` sont nuls
    if (buttonEmergencyStop == nullptr && other.buttonEmergencyStop == nullptr) {
    } else if (buttonEmergencyStop == nullptr || other.buttonEmergencyStop == nullptr) {
        return false; // Un pointeur est nul, l'autre ne l'est pas
    } else if (!(*buttonEmergencyStop == *(other.buttonEmergencyStop))) {
        return false;
    }

    // Vérifie si les deux pointeurs `buttonMotorState` sont nuls
    if (buttonMotorState == nullptr && other.buttonMotorState == nullptr) {
    } else if (buttonMotorState == nullptr || other.buttonMotorState == nullptr) {
        return false; // Un pointeur est nul, l'autre ne l'est pas
    } else if (!(*buttonMotorState == *(other.buttonMotorState))) {
        return false;
    }

    // Si tout est égal
    return true;
}
ControllerRequestDTO &ControllerRequestDTO::operator=(const ControllerRequestDTO &other)
{
    if (this != &other) {
        delete flightController;
        flightController = (other.flightController) ? new FlightController(*other.flightController) : nullptr;
        counter = other.counter;
    }
    return *this;
}
cJSON *ControllerRequestDTO::toJson() const
{
    cJSON* json = cJSON_CreateObject();

    // Ajout conditionnel des joysticks
    if (flightController != nullptr) {cJSON_AddItemToObject(json, "flightController", flightController->toJson());} 
    else {cJSON_AddNullToObject(json, "flightController");}

    // Ajout conditionnel de buttonMotorState
    if (buttonMotorState != nullptr) {cJSON_AddBoolToObject(json, "buttonMotorState", *buttonMotorState);} 
    else {cJSON_AddNullToObject(json, "buttonMotorState");}

    // Ajout conditionnel de buttonEmergencyStop
    if (buttonEmergencyStop != nullptr) {cJSON_AddBoolToObject(json, "buttonEmergencyStop", *buttonEmergencyStop);} 
    else {cJSON_AddNullToObject(json, "buttonEmergencyStop");}

    // Ajout du compteur
    cJSON_AddNumberToObject(json, "counter", counter);

    return json;
}
ControllerRequestDTO ControllerRequestDTO::fromJson(cJSON* json)
{
    ControllerRequestDTO dto;

    // Récupération de joystickLeft
    cJSON* flightControllerJson = cJSON_GetObjectItemCaseSensitive(json, "flightController");
    if (cJSON_IsObject(flightControllerJson)) {dto.flightController = new FlightController(FlightController::fromJson(flightControllerJson));} 
    else {dto.flightController = nullptr;}


    // Récupération du buttonMotorState
    cJSON* buttonMotorStateJson = cJSON_GetObjectItemCaseSensitive(json, "buttonMotorState");
    if (cJSON_IsBool(buttonMotorStateJson)) {
        *dto.buttonMotorState = buttonMotorStateJson->valueint;
    }

    // Récupération du buttonEmergencyStop
    cJSON* buttonEmergencyStopJson = cJSON_GetObjectItemCaseSensitive(json, "buttonEmergencyStop");
    if (cJSON_IsBool(buttonEmergencyStopJson)) {
        *dto.buttonEmergencyStop = buttonEmergencyStopJson->valueint;
    }

    // Récupération du compteur
    cJSON* counterJson = cJSON_GetObjectItemCaseSensitive(json, "counter");
    if (cJSON_IsNumber(counterJson)) {
        dto.counter = counterJson->valueint;
    }

    return dto;
};