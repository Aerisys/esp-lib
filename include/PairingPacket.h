#include <cstdint>

#define REQ_MAGIC "AERISYS_DRONE_PAIR"
#define RESP_MAGIC "PAIR_CONFIRM"

typedef struct {
    char magic[20];
} PairingPacket;
