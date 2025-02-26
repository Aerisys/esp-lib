#ifndef Controller_Request_DTO_H
#define Controller_Request_DTO_H
#include <JoystickModel.h>
#include <esp_log.h>

class ControllerRequestDTO {
    public:
        ControllerRequestDTO();
        ControllerRequestDTO(const ControllerRequestDTO& other);
        ~ControllerRequestDTO();  // Destructeur pour libérer la mémoire
        
        JoystickModel* joystickLeft = nullptr;
        JoystickModel* joystickRight = nullptr;

        uint64_t getCounter() const;
        bool operator==(const ControllerRequestDTO&) const;
        ControllerRequestDTO& operator=(const ControllerRequestDTO&);

        cJSON* toJson() const;
        static ControllerRequestDTO fromJson(cJSON* json);

    private:

        static uint64_t nmbInstanciation;
        uint64_t counter = 0;
};
#endif