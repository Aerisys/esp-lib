#ifndef Controller_Request_DTO_H
#define Controller_Request_DTO_H
#include <FlightController.h>
#include <esp_log.h>
#include <JoystickModel.h>

typedef struct
{
    uint64_t counter;

    bool has_buttonMotorState; // Indicateur de présence
    bool buttonMotorState;

    bool has_buttonMotorArming; // Indicateur de présence
    bool buttonMotorArming;

    bool has_flightController; // Indicateur de présence
    FlightControllerData flightController;
} ControllerRequestData;

class ControllerRequestDTO
{
public:
    ControllerRequestDTO();
    ControllerRequestDTO(const ControllerRequestDTO &other);
    ~ControllerRequestDTO(); // Destructeur pour libérer la mémoire

    void initCounter();
    void addInControllerRequestDTO(const ControllerRequestDTO &other);
    float deadZone = 0.17f;

    FlightController *flightController = nullptr;

    bool *buttonMotorState = nullptr;
    bool *buttonMotorArming = nullptr;

    uint64_t getCounter() const;
    bool operator==(const ControllerRequestDTO &) const;
    ControllerRequestDTO &operator=(const ControllerRequestDTO &other);

    void ConvertJoyStickToFlightController(JoystickModel joystickModelLeft, JoystickModel joystickModelRight);
    ControllerRequestData toStruct() const;
    static ControllerRequestDTO fromStruct(const ControllerRequestData &data);
    std::string toString() const;

private:
    static uint64_t nmbInstanciation;
    uint64_t counter = 0;
};
#endif