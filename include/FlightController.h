#ifndef Flight_Controller_H
#define Flight_Controller_H

#include <string>
#include <sstream>
#include <iostream>
#include "cJSON.h"

class FlightController {
    private:
    
    public:
        FlightController();
        FlightController(const FlightController& flightController);
        FlightController(float pitch, float rool, float yaw, float throttle);
    
        float pitch; // Tangage
        float roll;  // Roulis
        float yaw;   // Lacet
        float throttle;

        bool operator==(const FlightController&) const;
    
        cJSON* toJson() const;
        static FlightController fromJson(cJSON* json);
};

#endif