#ifndef TELEMETRY_DTO_H
#define TELEMETRY_DTO_H

// -----------------------------------------------------------------------------
// TelemetryDTO — unified payload sent drone -> controller over ESP-NOW.
// -----------------------------------------------------------------------------
// Bundles an IMU snapshot + the current motor speeds, so a single ESP-NOW
// packet gives the controller (and any ground station / logger downstream)
// everything it needs to render attitude + thrust state at the same instant.
//
// ----------------------- Self-contained, no external dep ---------------------
// v1.2.1: the payload uses LOCAL POD types (TelemetryVector3 / ...) instead
// of pulling `<imu_sensor.h>` from imu-lib. Consumers that don't run an IMU
// (the controller, future ground stations, ...) therefore don't have to
// embark imu-lib just to read this DTO. The drone, which DOES use imu-lib,
// converts its IMUSensor::Vector3 / Orientation / Quaternion values to the
// telemetry types field-by-field when building the packet (trivial — same
// layout, same field names).
//
// ----------------------- Wire format / size ----------------------------------
// The receiver dispatches incoming ESP-NOW packets BY SIZE (see
// EspNowLink::dispatchRecv on the controller side). Any field change here
// is a BREAKING CHANGE — both ends must be updated in lockstep and esp-lib
// version bumped accordingly.
//
// Current layout (no padding on ESP32, all members 4-byte aligned):
//   accel        12  (TelemetryVector3)
//   gyro         12
//   mag          12
//   orientation  12  (TelemetryOrientation)
//   quaternion   16  (TelemetryQuaternion)
//   temperature   4
//   motorSpeeds  16  (4 floats, see NUM_MOTORS below)
//   timestampUs   8
//   -------------
//   total:       92 bytes  (well below ESP-NOW 250-byte payload limit)
// -----------------------------------------------------------------------------

#include <stdint.h>

// Number of motors in the airframe carried by this DTO. Fixed at compile
// time because the wire format depends on it. Currently 4 (X quadcopter).
// If a different airframe is introduced, version the wire format and
// recompile both ends in lockstep.
#ifndef NUM_MOTORS
#define NUM_MOTORS 4
#endif

// Plain POD types — same layout as imu-lib's IMUSensor::Vector3 etc., but
// kept local to esp-lib so consumers without an IMU need no extra dep.
struct TelemetryVector3
{
    float x;
    float y;
    float z;
};

struct TelemetryOrientation
{
    float roll;
    float pitch;
    float yaw;
};

struct TelemetryQuaternion
{
    float w;
    float x;
    float y;
    float z;
};

struct TelemetryDTO
{
    TelemetryVector3     accel;                   // g
    TelemetryVector3     gyro;                    // deg/s
    TelemetryVector3     mag;                     // uT (zero on sensors without mag)
    TelemetryOrientation orientation;             // roll/pitch/yaw, deg
    TelemetryQuaternion  quaternion;              // body-to-world, identity if filter inactive
    float                temperature;             // degC
    float                motorSpeeds[NUM_MOTORS]; // pulse ticks (0..PULSE_RANGE on drone side)
    uint64_t             timestampUs;             // esp_timer_get_time() at IMU sample
};

#endif // TELEMETRY_DTO_H
