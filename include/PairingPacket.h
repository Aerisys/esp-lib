#include <cstdint>

typedef struct {
    char magic[20];
    uint8_t mac[6];
} PairingPacket;
