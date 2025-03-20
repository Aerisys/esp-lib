#include "../include/ControllerRequestDTO.h"
#include "ControllerRequestDTO.h"

uint64_t ControllerRequestDTO::nmbInstanciation = 0;
ControllerRequestDTO::ControllerRequestDTO()
{
}
ControllerRequestDTO::ControllerRequestDTO(const ControllerRequestDTO &other) {
    if (other.flightController)
        flightController = new FlightController(*other.flightController);
    else
        flightController = nullptr;

    if (other.buttonEmergencyStop)
        buttonEmergencyStop = new bool(*other.buttonEmergencyStop);
    else
        buttonEmergencyStop = nullptr;

    if (other.buttonMotorState)
        buttonMotorState = new bool(*other.buttonMotorState);
    else
        buttonMotorState = nullptr;

    counter = other.counter;
}

ControllerRequestDTO::~ControllerRequestDTO() {
    delete flightController;
    delete buttonMotorState;
    delete buttonEmergencyStop;
}
void ControllerRequestDTO::initCounter()
{
    counter = ++ControllerRequestDTO::nmbInstanciation;
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
void ControllerRequestDTO::ConvertJoyStickToFlightController(JoystickModel joystickModelLeft, JoystickModel joystickModelRight)
{
    // Valeurs maximales des joysticks
    
    const float JOYSTICK_MID = JoystickModel::JOYSTICK_MAX / 2.0f; // Centre des joysticks (2000)

    // Normalisation des valeurs (-1.0 à 1.0 pour pitch, roll et yaw)
    float pitch    = (joystickModelLeft.y  - JOYSTICK_MID) / JOYSTICK_MID;   // Avant / arrière
    float roll     = (joystickModelLeft.x  - JOYSTICK_MID) / JOYSTICK_MID;   // Gauche / droite
    float yaw      = (joystickModelRight.x - JOYSTICK_MID) / JOYSTICK_MID;   // Rotation
    float throttle = (joystickModelRight.y - JOYSTICK_MID) / JOYSTICK_MID;   // Monter / descendre

    flightController = new FlightController(pitch, roll, yaw, throttle);
}
ControllerRequestData ControllerRequestDTO::toStruct() const {
    ControllerRequestData data = {};
    data.counter = counter;

    data.has_buttonMotorState = (buttonMotorState != nullptr);
    if (buttonMotorState) data.buttonMotorState = *buttonMotorState;

    data.has_buttonEmergencyStop = (buttonEmergencyStop != nullptr);
    if (buttonEmergencyStop) data.buttonEmergencyStop = *buttonEmergencyStop;

    data.has_flightController = (flightController != nullptr);
    if (flightController) data.flightController = flightController->toStruct();

    return data;
}

ControllerRequestDTO ControllerRequestDTO::fromStruct(const ControllerRequestData& data) {
    ControllerRequestDTO dto;
    dto.counter = data.counter;

    if (data.has_buttonMotorState) dto.buttonMotorState = new bool(data.buttonMotorState);
    if (data.has_buttonEmergencyStop) dto.buttonEmergencyStop = new bool(data.buttonEmergencyStop);
    if (data.has_flightController) dto.flightController = new FlightController(FlightController::fromStruct(data.flightController));

    return dto;
}

std::string ControllerRequestDTO::toString() const
{
    std::ostringstream oss;
    oss << "(counter=" << counter 
        << ", buttonMotorState=" << (buttonMotorState ? std::to_string(*buttonMotorState) : "null")
        << ", buttonEmergencyStop=" << (buttonEmergencyStop ? std::to_string(*buttonEmergencyStop) : "null")
        << ", flightController=" << (flightController ? flightController->toString() : "null") 
        << ")";
    return oss.str();
}
