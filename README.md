# esp-lib

> DTOs partagés Aerisys pour le lien ESP-NOW **contrôleur ↔ drone** (ESP-IDF / PlatformIO).

Définit les paquets sérialisables échangés entre les deux firmwares :

- **`ControllerRequestDTO`** — commandes pilote (flightController + boutons + compteur)
- **`PingRequestDTO`** — keepalive radio
- **`PairingPacket`** — handshake d'appairage
- **`TelemetryDTO`** — *(v1.2.0)* snapshot IMU + motorSpeeds, drone → controller
- **`FlightController`** / **`JoystickModel`** — modèles utilitaires

## Quick start (consommateur)

```ini
; platformio.ini
lib_deps =
    https://github.com/Aerisys/esp-lib
```

```cpp
#include "ControllerRequestDTO.h"

ControllerRequestDTO dto;
dto.has_flightController = true;
dto.flightController     = FlightController(0.0f, 0.0f, 0.0f, 0.5f); // pitch, roll, yaw, throttle
dto.initCounter();

ControllerRequestData wire = dto.toStruct();
esp_now_send(peer_mac, (uint8_t*)&wire, sizeof(wire));
```

## v1.2.1 — Hotfix : TelemetryDTO autonome

Le `TelemetryDTO` (introduit en v1.2.0) utilise désormais des types POD
locaux (`TelemetryVector3` etc.) au lieu de pulled `<imu_sensor.h>` depuis
imu-lib. **esp-lib n'a plus aucune dépendance externe** — un consommateur
sans IMU (controller, ground station) n'a plus à embarquer imu-lib.

Le drone, qui utilise imu-lib pour son capteur, convertit ses types
`IMUSensor::*` vers `Telemetry*` côté émission (aggregate init trivial,
même layout / mêmes noms de champs).

## v1.2.0 — Additive

Ajout de [`TelemetryDTO`](include/TelemetryDTO.h) : un seul ESP-NOW packet
drone → controller qui contient un snapshot IMU atomique (accel, gyro, mag,
orientation, **quaternion**, temperature, **timestamp**) + `motorSpeeds[4]`.

Non-breaking : l'API v1.1.0 reste identique. Voir [AGENTS.md](docs/AGENTS.md#nouveau-en-v120).

## v1.1.0 — Breaking changes

Refonte `ControllerRequestDTO` en **POD (zero heap)** — élimine 6 mallocs par
paquet RC reçu dans le callback Wi-Fi ESP-NOW. Le wire format est inchangé,
seule l'API C++ change : `dto.flightController->x` → `dto.flightController.x`,
avec un flag `dto.has_flightController`.

Voir [AGENTS.md](AGENTS.md#breaking-changes-v110) pour le guide de migration
complet.

## Doc

- [AGENTS.md](AGENTS.md) / [CLAUDE.md](CLAUDE.md) — contexte technique pour agents IA + dev
- [examples/basic/main.cpp](examples/basic/main.cpp) — sanity-check standalone

## Licence

Voir LICENCE (à ajouter le cas échéant).
