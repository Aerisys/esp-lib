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
    if (joystickLeft == other.joystickLeft && joystickRight == other.joystickRight)
    {
        return true;
    }
    return false;
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