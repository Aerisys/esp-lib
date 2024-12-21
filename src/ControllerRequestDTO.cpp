#include "../include/ControllerRequestDTO.h"

int ControllerRequestDTO::nmbInstanciation = 0;
ControllerRequestDTO::ControllerRequestDTO()
{
    counter = ControllerRequestDTO::nmbInstanciation++;
}
int ControllerRequestDTO::getCounter()
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
cJSON* ControllerRequestDTO::toJson() const
{
    cJSON* json = cJSON_CreateObject();

    // Ajout conditionnel des joysticks
    if (joystickLeft != nullptr) {cJSON_AddItemToObject(json, "joystickLeft", joystickLeft->toJson());} 
    else {cJSON_AddNullToObject(json, "joystickLeft");}

    if (joystickRight != nullptr) {cJSON_AddItemToObject(json, "joystickRight", joystickRight->toJson());} 
    else {cJSON_AddNullToObject(json, "joystickRight");}

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

    // Récupération du compteur
    cJSON* counterJson = cJSON_GetObjectItemCaseSensitive(json, "counter");
    if (cJSON_IsNumber(counterJson)) {
        dto.counter = counterJson->valueint;
    }

    return dto;
};