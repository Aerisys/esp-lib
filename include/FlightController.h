#ifndef Flight_Controller_H
#define Flight_Controller_H

#include <string>
#include <sstream>
#include <iostream>

typedef struct {
    float pitch, roll, yaw, throttle;
} FlightControllerData;

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

        bool isFullZero();

        bool operator==(const FlightController&) const;
    
        FlightControllerData toStruct() const;
        static FlightController fromStruct(const FlightControllerData& data);

        std::string toString() const;
};

#endif