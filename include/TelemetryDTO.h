#ifndef TELEMETRY_DTO_H
#define TELEMETRY_DTO_H

// -----------------------------------------------------------------------------
// TelemetryDTO — unified payload sent drone -> controller over ESP-NOW.
// -----------------------------------------------------------------------------
// Bundles a full IMU snapshot (taken atomically by the imu-lib seqlock) with
// the current motor speeds, so a single ESP-NOW packet gives the controller
// (and any ground station / logger downstream) everything it needs to render
// attitude + thrust state at the same instant.
//
// Depends on the abstract IMUSensor interface from imu-lib rather than a
// specific chip type — swapping MPU9250 for an ICM-* in the future does not
// change the wire layout.
//
// ----------------------- Dependencies ----------------------------------------
// This header pulls `imu_sensor.h` from the imu-lib library. Any consumer of
// esp-lib that wants to use TelemetryDTO must also have imu-lib in its
// `lib_deps` (already the case for both `drone` and `controller`).
//
// ----------------------- Wire format / size ----------------------------------
// The receiver (controller / ground station) dispatches incoming ESP-NOW
// packets BY SIZE (see EspNowLink::dispatchRecv on the controller side).
// Any field change here is a BREAKING CHANGE — both ends must be updated
// in lockstep and esp-lib version bumped accordingly.
//
// Current layout (no padding on ESP32, all members 4-byte aligned):
//   accel        12  (Vector3)
//   gyro         12
//   mag          12
//   orientation  12  (Orientation)
//   quaternion   16  (Quaternion)
//   temperature   4
//   motorSpeeds  16  (4 floats, see NUM_MOTORS below)
//   timestampUs   8
//   -------------
//   total:       92 bytes  (well below ESP-NOW 250-byte payload limit)
// -----------------------------------------------------------------------------

#include "imu_sensor.h"
#include <stdint.h>

// Number of motors in the airframe carried by this DTO. Fixed at compile
// time because the wire format depends on it. Currently 4 (X quadcopter).
// If a different airframe (hexa, octo, ...) is introduced, the wire format
// MUST be versioned and both ends recompiled in lockstep.
#ifndef NUM_MOTORS
#define NUM_MOTORS 4
#endif

struct TelemetryDTO
{
    IMUSensor::Vector3     accel;                   // g
    IMUSensor::Vector3     gyro;                    // deg/s
    IMUSensor::Vector3     mag;                     // uT (zero on sensors without mag)
    IMUSensor::Orientation orientation;             // roll/pitch/yaw, deg
    IMUSensor::Quaternion  quaternion;              // body-to-world, identity if filter inactive
    float                  temperature;             // degC
    float                  motorSpeeds[NUM_MOTORS]; // pulse ticks (0..PULSE_RANGE on drone side)
    uint64_t               timestampUs;             // esp_timer_get_time() at IMU sample
};

#endif // TELEMETRY_DTO_H
