#ifndef Joystick_Model_H
#define Joystick_Model_H

#include <string>
#include <sstream>
#include <iostream>

class JoystickModel
{
public:
    static const int JOYSTICK_MAX = 2047;
    int x;
    int y;

    JoystickModel(int x, int y);
    JoystickModel(const JoystickModel &joystickModel);
    JoystickModel() : x(0), y(0) {}

    // Explicitly defaulted to silence -Wdeprecated-copy: with a
    // user-provided copy ctor and no operator=, GCC warns that the
    // implicit operator= is deprecated.
    JoystickModel& operator=(const JoystickModel &) = default;

    bool operator==(const JoystickModel &) const;
    bool operator!=(const JoystickModel &other) const { return !(*this == other); }

    std::string toString() const;

private:
    int roomForManeuver = 50;
};

#endif