#ifndef Joystick_Model_H
#define Joystick_Model_H

#include <string>
#include <sstream>
#include <iostream>

class JoystickModel {
    public:
        static const int JOYSTICK_MAX = 4095;
        int deadZone = 400;
        int x;
        int y;
    
        JoystickModel(int x, int y);
        JoystickModel(const JoystickModel& joystickModel);
        JoystickModel() : x(0), y(0) {}
    
        bool operator==(const JoystickModel&) const;

        std::string toString() const;
    
    private:
        int roomForManeuver = 50;
    };

#endif