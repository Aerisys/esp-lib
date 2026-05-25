#ifndef Flight_Controller_H
#define Flight_Controller_H

#include <string>
#include <sstream>
#include <iostream>

typedef struct
{
    float pitch, roll, yaw, throttle;
} FlightControllerData;

class FlightController
{
private:
public:
    FlightController();
    FlightController(const FlightController &flightController);
    FlightController(float pitch, float rool, float yaw, float throttle);

    // Explicitly defaulted to silence -Wdeprecated-copy: with a user-provided
    // copy ctor and no operator=, GCC warns that the implicit operator= is
    // deprecated. Same pattern as JoystickModel.
    FlightController& operator=(const FlightController&) = default;

    float pitch; // Tangage
    float roll;  // Roulis
    float yaw;   // Lacet
    float throttle;

    bool isFullZero();

    bool operator==(const FlightController &) const;

    FlightControllerData toStruct() const;
    static FlightController fromStruct(const FlightControllerData &data);

    std::string toString() const;
};

#endif