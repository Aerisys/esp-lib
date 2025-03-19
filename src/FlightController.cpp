#include "FlightController.h"

FlightController::FlightController(): pitch(0.0f), roll(0.0f), yaw(0.0f) {}

FlightController::FlightController(const FlightController &flightController): pitch(flightController.pitch), roll(flightController.roll), yaw(flightController.yaw), throttle(flightController.throttle) {}

FlightController::FlightController(float pitch, float roll, float yaw, float throttle): pitch(pitch), roll(roll), yaw(yaw), throttle(throttle) {}

bool FlightController::operator==(const FlightController &other) const {
    return (pitch == other.pitch && roll == other.roll && yaw == other.yaw && throttle == other.throttle);
}

FlightControllerData FlightController::toStruct() const {
    FlightControllerData data = {};
    data.pitch = pitch;
    data.roll = roll;
    data.yaw = yaw;
    data.throttle = throttle;
    return data;
}


FlightController FlightController::fromStruct(const FlightControllerData& data) {
    return FlightController(data.pitch, data.roll, data.yaw, data.throttle);
}

std::string FlightController::toString() const
{
    std::ostringstream oss;
    oss << "FlightController(pitch=" << pitch 
        << ", roll=" << roll 
        << ", yaw=" << yaw 
        << ", throttle=" << throttle << ")";
    return oss.str();
}
