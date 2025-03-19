#include "../include/ControllerRequestDTO.h"
#include "ControllerRequestDTO.h"

uint64_t ControllerRequestDTO::nmbInstanciation = 0;
ControllerRequestDTO::ControllerRequestDTO()
{
}
ControllerRequestDTO::ControllerRequestDTO(const ControllerRequestDTO &other)
{
    if (other.joystickLeft)
        joystickLeft = new JoystickModel(*other.joystickLeft);
    if (other.joystickRight)
        joystickRight = new JoystickModel(*other.joystickRight);
    counter = other.counter;
}
ControllerRequestDTO::~ControllerRequestDTO() {
    delete joystickLeft;
    delete joystickRight;
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
    if (joystickLeft == nullptr && other.joystickLeft == nullptr) {
    } else if (joystickLeft == nullptr || other.joystickLeft == nullptr) {
        return false; // Un pointeur est nul, l'autre ne l'est pas
    } else if (!(*joystickLeft == *(other.joystickLeft))) {
        return false;
    }

    // Vérifie si les deux pointeurs `joystickRight` sont nuls
    if (joystickRight == nullptr && other.joystickRight == nullptr) {
    } else if (joystickRight == nullptr || other.joystickRight == nullptr) {
        return false; // Un pointeur est nul, l'autre ne l'est pas
    } else if (!(*joystickRight == *(other.joystickRight))) {
        return false;
    }

    // Si tout est égal
    return true;
}
ControllerRequestDTO &ControllerRequestDTO::operator=(const ControllerRequestDTO &other)
{
    if (this != &other) {
        delete joystickLeft;
        delete joystickRight;
        joystickLeft = (other.joystickLeft) ? new JoystickModel(*other.joystickLeft) : nullptr;
        joystickRight = (other.joystickRight) ? new JoystickModel(*other.joystickRight) : nullptr;
        counter = other.counter;
    }
    return *this;
}
cJSON *ControllerRequestDTO::toJson() const
{
    cJSON* json = cJSON_CreateObject();

    // Ajout conditionnel des joysticks
    if (joystickLeft != nullptr) {cJSON_AddItemToObject(json, "joystickLeft", joystickLeft->toJson());} 
    else {cJSON_AddNullToObject(json, "joystickLeft");}

    if (joystickRight != nullptr) {cJSON_AddItemToObject(json, "joystickRight", joystickRight->toJson());} 
    else {cJSON_AddNullToObject(json, "joystickRight");}

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
    cJSON* joystickLeftJson = cJSON_GetObjectItemCaseSensitive(json, "joystickLeft");
    if (cJSON_IsObject(joystickLeftJson)) {dto.joystickLeft = new JoystickModel(JoystickModel::fromJson(joystickLeftJson));} 
    else {dto.joystickLeft = nullptr;}

    // Récupération de joystickRight
    cJSON* joystickRightJson = cJSON_GetObjectItemCaseSensitive(json, "joystickRight");
    if (cJSON_IsObject(joystickRightJson)) {dto.joystickRight = new JoystickModel(JoystickModel::fromJson(joystickRightJson));} 
    else {dto.joystickRight = nullptr;}

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