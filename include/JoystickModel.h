#ifndef Joystick_Model_H
#define Joystick_Model_H

#include <string>
#include <sstream>
#include <iostream>
#include "cJSON.h"

class JoystickModel{
    public:
        int x;
        int y;
        JoystickModel(int x, int y);
        JoystickModel(JoystickModel* joystickModel);
        JoystickModel();
        bool operator==(const JoystickModel& )const;
        cJSON* toJson() const;
        static JoystickModel fromJson(cJSON* json);
    private:
        int roomForManeuver = 50;
};

#endif