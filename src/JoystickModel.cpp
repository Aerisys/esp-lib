#include "../include/JoystickModel.h"
#include "JoystickModel.h"

JoystickModel::JoystickModel(int x, int y)
    : x((x >= (JOYSTICK_MAX/2-deadZone) && x <= (JOYSTICK_MAX/2+deadZone)) ? JOYSTICK_MAX/2 : x), 
      y((y >= (JOYSTICK_MAX/2-deadZone) && y <= (JOYSTICK_MAX/2+deadZone)) ? JOYSTICK_MAX/2 : y) {}


JoystickModel::JoystickModel(const JoystickModel& joystickModel)
    : x(joystickModel.x), y(joystickModel.y), roomForManeuver(joystickModel.roomForManeuver) {}

bool JoystickModel::operator==(const JoystickModel& other) const {
    return (x >= other.x - roomForManeuver && x <= other.x + roomForManeuver &&
            y >= other.y - roomForManeuver && y <= other.y + roomForManeuver);
}

std::string JoystickModel::toString() const
{
    std::ostringstream oss;
    oss << "(x=" << x << ", y=" << y << ")";
    return oss.str();
}
