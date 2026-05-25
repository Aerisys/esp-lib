#ifndef Controller_Request_DTO_H
#define Controller_Request_DTO_H

#include <FlightController.h>
#include <esp_log.h>
#include <JoystickModel.h>

// -----------------------------------------------------------------------------
// ControllerRequestDTO — payload contrôleur → drone (ESP-NOW).
// -----------------------------------------------------------------------------
// v1.1.0 (breaking) : refonte en POD. Les champs optionnels étaient
// auparavant exposés via raw pointers + `new`/`delete`, ce qui:
//   - causait 6 allocations heap par paquet reçu DANS le callback ESP-NOW
//     (contexte ISR-like Wi-Fi) -> fragmentation + latence imprévisible;
//   - introduisait une race operator= ↔ callback;
//   - fuitait de la mémoire dans ConvertJoyStickToFlightController.
//
// Le wire format `ControllerRequestData` (POD) reste **strictement
// identique** : compat binaire ESP-NOW préservée. Seule l'API C++ change.
// Migration consommateur :
//   AVANT : if (dto.flightController) { ... dto.flightController->pitch ... }
//           dto.flightController = new FlightController(p, r, y, t);
//   APRÈS : if (dto.has_flightController) { ... dto.flightController.pitch ... }
//           dto.flightController     = FlightController(p, r, y, t);
//           dto.has_flightController = true;
// -----------------------------------------------------------------------------

typedef struct
{
    uint64_t counter;

    bool has_buttonMotorState;   // present flag
    bool buttonMotorState;

    bool has_buttonMotorArming;  // present flag
    bool buttonMotorArming;

    bool has_flightController;   // present flag
    FlightControllerData flightController;
} ControllerRequestData;

class ControllerRequestDTO
{
public:
    ControllerRequestDTO() = default;
    ControllerRequestDTO(const ControllerRequestDTO&) = default;
    ControllerRequestDTO& operator=(const ControllerRequestDTO&) = default;
    ~ControllerRequestDTO() = default;

    void initCounter();

    // Merge `other` into this DTO if `other.counter` is strictly more recent.
    // Older or equal counters are ignored (de-dup / out-of-order protection).
    // For each optional field, if `other.has_*` is true the value (and the
    // present flag) overwrites the current one. Stale fields are preserved.
    void addInControllerRequestDTO(const ControllerRequestDTO &other);

    float deadZone = 0.17f;

    // Optional fields (POD, no heap). Inspect `has_*` before reading the value.
    bool             has_flightController  = false;
    FlightController flightController{};

    bool             has_buttonMotorState  = false;
    bool             buttonMotorState      = false;

    bool             has_buttonMotorArming = false;
    bool             buttonMotorArming     = false;

    uint64_t getCounter() const;
    bool operator==(const ControllerRequestDTO&) const;

    void ConvertJoyStickToFlightController(JoystickModel joystickModelLeft, JoystickModel joystickModelRight);
    ControllerRequestData toStruct() const;
    static ControllerRequestDTO fromStruct(const ControllerRequestData &data);
    std::string toString() const;

private:
    static uint64_t nmbInstanciation;
    uint64_t counter = 0;
};
#endif
