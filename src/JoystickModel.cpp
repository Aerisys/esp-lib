#include "../include/JoystickModel.h"

JoystickModel::JoystickModel(int x, int y, int roomForManeuver)
    : x(x), y(y), roomForManeuver(roomForManeuver) {}

JoystickModel::JoystickModel(const JoystickModel& joystickModel)
    : x(joystickModel.x), y(joystickModel.y), roomForManeuver(joystickModel.roomForManeuver) {}

bool JoystickModel::operator==(const JoystickModel& other) const {
    return (x >= other.x - roomForManeuver && x <= other.x + roomForManeuver &&
            y >= other.y - roomForManeuver && y <= other.y + roomForManeuver);
}

cJSON* JoystickModel::toJson() const {
    cJSON* json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "x", x);
    cJSON_AddNumberToObject(json, "y", y);
    return json;
}

JoystickModel JoystickModel::fromJson(cJSON* json) {
    if (!json) return JoystickModel(); // Vérification du pointeur

    cJSON* xJson = cJSON_GetObjectItemCaseSensitive(json, "x");
    cJSON* yJson = cJSON_GetObjectItemCaseSensitive(json, "y");

    if (!cJSON_IsNumber(xJson) || !cJSON_IsNumber(yJson)) {
        return JoystickModel(); // Retourne un objet par défaut si parsing invalide
    }

    return JoystickModel(xJson->valueint, yJson->valueint);
}
