#include "ControllerRequestDTO.h"

uint64_t ControllerRequestDTO::nmbInstanciation = 0;

void ControllerRequestDTO::initCounter()
{
    counter = ++ControllerRequestDTO::nmbInstanciation;
}

void ControllerRequestDTO::addInControllerRequestDTO(const ControllerRequestDTO &other)
{
    // De-dup / out-of-order protection: ignore packets whose counter is not
    // strictly more recent than ours. Counter == 0 is treated as "no counter
    // set", so any incoming value can apply.
    //
    // NOTE: A controller reboot resets its counter to 0 and the drone will
    // refuse subsequent packets until the new counter overtakes the old
    // maximum. The drone side is expected to handle the reset (e.g. via a
    // timeout that re-syncs the counter). Tracked separately, not addressed
    // in v1.1.0.
    if (getCounter() >= other.getCounter() && other.getCounter() != 0)
    {
        return;
    }

    counter = other.getCounter();

    if (other.has_flightController)
    {
        flightController     = other.flightController;
        has_flightController = true;
    }
    if (other.has_buttonMotorArming)
    {
        buttonMotorArming     = other.buttonMotorArming;
        has_buttonMotorArming = true;
    }
    if (other.has_buttonMotorState)
    {
        buttonMotorState     = other.buttonMotorState;
        has_buttonMotorState = true;
    }
}

uint64_t ControllerRequestDTO::getCounter() const
{
    return counter;
}

bool ControllerRequestDTO::operator==(const ControllerRequestDTO &other) const
{
    if (has_flightController != other.has_flightController) return false;
    if (has_flightController && !(flightController == other.flightController)) return false;

    if (has_buttonMotorArming != other.has_buttonMotorArming) return false;
    if (has_buttonMotorArming && buttonMotorArming != other.buttonMotorArming) return false;

    if (has_buttonMotorState != other.has_buttonMotorState) return false;
    if (has_buttonMotorState && buttonMotorState != other.buttonMotorState) return false;

    return true;
}

void ControllerRequestDTO::ConvertJoyStickToFlightController(JoystickModel joystickModelLeft, JoystickModel joystickModelRight)
{
    const float JOYSTICK_MIN = 0.0f;
    const float JOYSTICK_MAX = JoystickModel::JOYSTICK_MAX;
    const float JOYSTICK_MID = JOYSTICK_MAX / 2.0f;

    auto NormalizeCentered = [JOYSTICK_MID](float value)
    {
        return (value - JOYSTICK_MID) / JOYSTICK_MID;
    };

    auto NormalizePositive = [JOYSTICK_MIN, JOYSTICK_MAX](float value)
    {
        return (value - JOYSTICK_MIN) / (JOYSTICK_MAX - JOYSTICK_MIN);
    };

    float throttle = 1.0f - NormalizePositive(joystickModelLeft.y);   // 0..1
    float yaw      = -NormalizeCentered(joystickModelLeft.x);         // -1..1
    float pitch    = -NormalizeCentered(joystickModelRight.y);        // -1..1
    float roll     = -NormalizeCentered(joystickModelRight.x);        // -1..1

    if (std::abs(pitch) < deadZone) pitch = 0;
    if (std::abs(roll)  < deadZone) roll  = 0;
    if (std::abs(yaw)   < deadZone) yaw   = 0;

    throttle = std::clamp(throttle, 0.0f, 1.0f);

    // Assignation par valeur — pas de heap, pas de fuite si appelée plusieurs
    // fois consécutives (l'ancien code faisait `new FlightController(...)`
    // sans `delete` préalable et fuitait à chaque appel).
    flightController     = FlightController(pitch, roll, yaw, throttle);
    has_flightController = true;
}

ControllerRequestData ControllerRequestDTO::toStruct() const
{
    ControllerRequestData data = {};
    data.counter = counter;

    data.has_buttonMotorState = has_buttonMotorState;
    if (has_buttonMotorState)
        data.buttonMotorState = buttonMotorState;

    data.has_buttonMotorArming = has_buttonMotorArming;
    if (has_buttonMotorArming)
        data.buttonMotorArming = buttonMotorArming;

    data.has_flightController = has_flightController;
    if (has_flightController)
        data.flightController = flightController.toStruct();

    return data;
}

ControllerRequestDTO ControllerRequestDTO::fromStruct(const ControllerRequestData &data)
{
    // Zero heap allocations: safe to call from the ESP-NOW receive callback
    // (ISR-like Wi-Fi context). The whole DTO is stack/value-allocated.
    ControllerRequestDTO dto;
    dto.counter = data.counter;

    if (data.has_buttonMotorState)
    {
        dto.has_buttonMotorState = true;
        dto.buttonMotorState     = data.buttonMotorState;
    }
    if (data.has_buttonMotorArming)
    {
        dto.has_buttonMotorArming = true;
        dto.buttonMotorArming     = data.buttonMotorArming;
    }
    if (data.has_flightController)
    {
        dto.has_flightController = true;
        dto.flightController     = FlightController::fromStruct(data.flightController);
    }

    return dto;
}

std::string ControllerRequestDTO::toString() const
{
    std::ostringstream oss;
    oss << "(counter=" << counter
        << ", buttonMotorState="  << (has_buttonMotorState  ? std::to_string(buttonMotorState)  : "null")
        << ", buttonMotorArming=" << (has_buttonMotorArming ? std::to_string(buttonMotorArming) : "null")
        << ", flightController="  << (has_flightController  ? flightController.toString()       : "null")
        << ")";
    return oss.str();
}
