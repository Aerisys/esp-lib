#include "../include/ControllerRequestDTO.h"
#include "ControllerRequestDTO.h"

uint64_t ControllerRequestDTO::nmbInstanciation = 0;
ControllerRequestDTO::ControllerRequestDTO()
{
}
ControllerRequestDTO::ControllerRequestDTO(const ControllerRequestDTO &other) {
    if (this != &other) {  // Protection auto-référence
        delete flightController; flightController=nullptr;
        delete buttonMotorArming; buttonMotorArming=nullptr;
        delete buttonMotorState; buttonMotorState=nullptr;

        flightController = other.flightController ? new FlightController(*other.flightController) : nullptr;
        buttonMotorArming = other.buttonMotorArming ? new bool(*other.buttonMotorArming) : nullptr;
        buttonMotorState = other.buttonMotorState ? new bool(*other.buttonMotorState) : nullptr;

        counter = other.counter;
    }
}


ControllerRequestDTO::~ControllerRequestDTO() {
    delete flightController; flightController = nullptr;
    delete buttonMotorState; buttonMotorState = nullptr;
    delete buttonMotorArming; buttonMotorArming = nullptr;
}
void ControllerRequestDTO::initCounter()
{
    counter = ++ControllerRequestDTO::nmbInstanciation;
}
void ControllerRequestDTO::addInControllerRequestDTO(const ControllerRequestDTO &other)
{
    if(getCounter()>=other.getCounter()){return;}

    counter = other.getCounter();
    if(other.flightController){
        delete flightController; flightController = nullptr;
        flightController = new FlightController(*other.flightController);
    }
    if (other.buttonMotorArming && *other.buttonMotorArming){
        if (buttonMotorArming) {
            *buttonMotorArming = !*buttonMotorArming;
        } else {
            buttonMotorArming = new bool(!*other.buttonMotorArming); // toggle à false
        }
    }
    if(other.buttonMotorState){
        delete buttonMotorState; buttonMotorState = nullptr;
        buttonMotorState = new bool(*other.buttonMotorState);
    }
}

uint64_t ControllerRequestDTO::getCounter() const
{
    return counter;
}
ControllerRequestDTO& ControllerRequestDTO::operator=(const ControllerRequestDTO &other) {
    if (this != &other) { // Protection auto-affectation
        delete flightController;
        delete buttonMotorArming;
        delete buttonMotorState;

        flightController = other.flightController ? new FlightController(*other.flightController) : nullptr;
        buttonMotorArming = other.buttonMotorArming ? new bool(*other.buttonMotorArming) : nullptr;
        buttonMotorState = other.buttonMotorState ? new bool(*other.buttonMotorState) : nullptr;

        counter = other.counter;
    }
    return *this;
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

    // Vérifie si les deux pointeurs `buttonMotorArming` sont nuls
    if (buttonMotorArming == nullptr && other.buttonMotorArming == nullptr) {
    } else if (buttonMotorArming == nullptr || other.buttonMotorArming == nullptr) {
        return false; // Un pointeur est nul, l'autre ne l'est pas
    } else if (!(*buttonMotorArming == *(other.buttonMotorArming))) {
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
void ControllerRequestDTO::ConvertJoyStickToFlightController(JoystickModel joystickModelLeft, JoystickModel joystickModelRight)
{
    // Valeurs maximales des joysticks
    
    const float JOYSTICK_MID = JoystickModel::JOYSTICK_MAX / 2.0f; // Centre des joysticks (2000)

    // Normalisation des valeurs (-1.0 à 1.0 pour pitch, roll et yaw)
    float pitch    = (joystickModelLeft.y  - JOYSTICK_MID) / JOYSTICK_MID;   // Avant / arrière
    float roll     = (joystickModelLeft.x  - JOYSTICK_MID) / JOYSTICK_MID;   // Gauche / droite
    float yaw      = (joystickModelRight.x - JOYSTICK_MID) / JOYSTICK_MID;   // Rotation
    float throttle = (joystickModelRight.y - JOYSTICK_MID) / JOYSTICK_MID;   // Monter / descendre

    if( pitch>= -deadZone && pitch <= deadZone){pitch=0;}
    if( roll>= -deadZone && roll <= deadZone){roll=0;}
    if( yaw>= -deadZone && yaw <= deadZone){yaw=0;}
    if( throttle>= -deadZone && throttle <= deadZone){throttle=0;}

    flightController = new FlightController(pitch, roll, yaw, throttle);
}
ControllerRequestData ControllerRequestDTO::toStruct() const {
    ControllerRequestData data = {};
    data.counter = counter;

    data.has_buttonMotorState = (buttonMotorState != nullptr);
    if (buttonMotorState) data.buttonMotorState = *buttonMotorState;

    data.buttonMotorArming = (buttonMotorArming != nullptr);
    if (buttonMotorArming) data.buttonMotorArming = *buttonMotorArming;

    data.has_flightController = (flightController != nullptr);
    if (flightController) data.flightController = flightController->toStruct();

    return data;
}

ControllerRequestDTO ControllerRequestDTO::fromStruct(const ControllerRequestData& data) {
    ControllerRequestDTO dto;
    dto.counter = data.counter;

    if (data.has_buttonMotorState) dto.buttonMotorState = new bool(data.buttonMotorState);
    if (data.has_buttonMotorArming) dto.buttonMotorArming = new bool(data.buttonMotorArming);
    if (data.has_flightController) dto.flightController = new FlightController(FlightController::fromStruct(data.flightController));

    return dto;
}

std::string ControllerRequestDTO::toString() const
{
    std::ostringstream oss;
    oss << "(counter=" << counter 
        << ", buttonMotorState=" << (buttonMotorState ? std::to_string(*buttonMotorState) : "null")
        << ", buttonMotorArming=" << (buttonMotorArming ? std::to_string(*buttonMotorArming) : "null")
        << ", flightController=" << (flightController ? flightController->toString() : "null") 
        << ")";
    return oss.str();
}
