#include "FlightController.h"

FlightController::FlightController(): pitch(0.0f), roll(0.0f), yaw(0.0f) {}

FlightController::FlightController(const FlightController &flightController): pitch(flightController.pitch), roll(flightController.roll), yaw(flightController.yaw), throttle(flightController.throttle) {}

FlightController::FlightController(float pitch, float roll, float yaw, float throttle): pitch(pitch), roll(roll), yaw(yaw), throttle(throttle) {}

bool FlightController::operator==(const FlightController &other) const {
    return (pitch == other.pitch && roll == other.roll && yaw == other.yaw && throttle == other.throttle);
}

cJSON *FlightController::toJson() const
{
    cJSON* json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "pitch", pitch);
    cJSON_AddNumberToObject(json, "roll", roll);
    cJSON_AddNumberToObject(json, "yaw", yaw);
    cJSON_AddNumberToObject(json, "throttle", throttle);
    return json;
}

FlightController FlightController::fromJson(cJSON *json) {
    if (!json) return FlightController(); // Vérification du pointeur

    cJSON* pitchJson = cJSON_GetObjectItemCaseSensitive(json, "pitch");
    cJSON* rollJson = cJSON_GetObjectItemCaseSensitive(json, "roll");
    cJSON* yawJson = cJSON_GetObjectItemCaseSensitive(json, "yaw");
    cJSON* throttleJson = cJSON_GetObjectItemCaseSensitive(json, "throttle");

    if (!cJSON_IsNumber(pitchJson) || !cJSON_IsNumber(rollJson) || !cJSON_IsNumber(yawJson)|| !cJSON_IsNumber(throttleJson)) {
        return FlightController(); // Retourne un objet par défaut si parsing invalide
    }

    return FlightController(
        static_cast<float>(pitchJson->valuedouble),
        static_cast<float>(rollJson->valuedouble),
        static_cast<float>(yawJson->valuedouble),
        static_cast<float>(throttleJson->valuedouble)
    );
}
