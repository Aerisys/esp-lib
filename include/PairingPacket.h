#include <cstdint>

typedef struct {
    char magic[12];
    uint8_t mac[6];
} PairingPacket;
