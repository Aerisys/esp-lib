#ifndef Controller_Request_DTO_H
#define Controller_Request_DTO_H
#include <JoystickModel.h>
#include <esp_log.h>

class ControllerRequestDTO {
    public:
        ControllerRequestDTO();
        ~ControllerRequestDTO();  // Destructeur pour libérer la mémoire

        int getCounter() const;
        bool operator==(const ControllerRequestDTO&) const;

        cJSON* toJson() const;
        static ControllerRequestDTO fromJson(cJSON* json);

    private:
        JoystickModel* joystickLeft = nullptr;
        JoystickModel* joystickRight = nullptr;

        static int nmbInstanciation;
        int counter = 0;
};
#endif