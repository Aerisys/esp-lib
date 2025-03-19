#include "../include/JoystickModel.h"
#include "JoystickModel.h"

JoystickModel::JoystickModel(int x, int y)
    : x(x), y(y) {}

JoystickModel::JoystickModel(const JoystickModel& joystickModel)
    : x(joystickModel.x), y(joystickModel.y), roomForManeuver(joystickModel.roomForManeuver) {}

bool JoystickModel::operator==(const JoystickModel& other) const {
    return (x >= other.x - roomForManeuver && x <= other.x + roomForManeuver &&
            y >= other.y - roomForManeuver && y <= other.y + roomForManeuver);
}

std::string JoystickModel::toString() const
{
    std::ostringstream oss;
    oss << "JoystickModel(x=" << x << ", y=" << y << ")";
    return oss.str();
}
