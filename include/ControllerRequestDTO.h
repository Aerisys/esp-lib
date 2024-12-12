#ifndef Controller_Request_DTO_H
#define Controller_Request_DTO_H
#include <JoystickModel.h>
#include <esp_log.h>

class ControllerRequestDTO {
    public:
        JoystickModel* joystickLeft = nullptr;
        JoystickModel* joystickRight = nullptr;
        ControllerRequestDTO();
        int getCounter();
        bool operator==(const ControllerRequestDTO& )const;
        cJSON* toJson() const;
        static ControllerRequestDTO fromJson(cJSON* json);
    private:
        static int nmbInstanciation;
        int counter = 0;
};
#endif