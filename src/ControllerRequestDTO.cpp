#include "../include/ControllerRequestDTO.h"
#include "ControllerRequestDTO.h"

int ControllerRequestDTO::nmbInstanciation = 0;
ControllerRequestDTO::ControllerRequestDTO()
{
    counter = ControllerRequestDTO::nmbInstanciation++;
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
int ControllerRequestDTO::getCounter() const
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