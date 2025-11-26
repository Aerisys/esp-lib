#include <cstdint>

typedef struct {
    uint8_t droneMac[6];
} AssociationRequestDTO;

typedef struct {
    uint8_t controllerMac[6];
} AssociationResponseDTO;
