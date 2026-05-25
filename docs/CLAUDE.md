# esp-lib — Bibliothèque partagée Aerisys (DTOs ESP-NOW)

> **v1.2.0 — additive.** Ajout de [`TelemetryDTO`](../include/TelemetryDTO.h)
> pour le sens drone → controller (snapshot IMU + motorSpeeds, single ESP-NOW
> packet). Non-breaking : l'API v1.1.0 reste inchangée. Voir
> [Nouveau en v1.2.0](#nouveau-en-v120) ci-dessous.
>
> **v1.1.0 — refonte majeure.** DTOs en POD (zero heap), structure de paquet
> conforme aux conventions ESP-IDF, `library.json` export rules pour ne pas
> polluer les consommateurs. Voir [Breaking changes v1.1.0](#breaking-changes-v110)
> avant migration depuis v1.0.x.

Bibliothèque PlatformIO / ESP-IDF utilisée par les deux firmwares
(`controller` et `drone`). Elle définit les **DTOs** échangés via
ESP-NOW et quelques modèles utilitaires.

> Cette lib est consommée **depuis GitHub** par les firmwares :
> `lib_deps = https://github.com/Aerisys/esp-lib`.
> Tout changement doit être **versionné** dans [library.json](library.json)
> puis poussé sur GitHub pour être tiré par `pio pkg update`.

## Stack

- **Type** : library PlatformIO
- **Framework** : `espidf`
- **Plateforme** : `espressif32`
- **Langage** : C++

## Contenu

```
include/   - headers publics (DTOs)
src/       - implémentations des DTOs (.cpp uniquement)
examples/  - point d'entrée standalone (app_main) pour sanity-check; NON
             distribué aux consommateurs via library.json export.exclude
```

| Header | Rôle |
| ------ | ---- |
| [include/ControllerRequestDTO.h](include/ControllerRequestDTO.h) | Paquet principal manette → drone. Contient un `FlightController` + boutons `arming` / `motor state` + compteur. **POD depuis v1.1.0**, plus de raw pointers (cf. breaking changes). |
| [include/FlightController.h](include/FlightController.h)         | Consigne de vol pure : `pitch`, `roll`, `yaw`, `throttle` (floats). Méthode `isFullZero()`. **Ctor par défaut initialise désormais `throttle = 0`** (était UB en v1.0). |
| [include/JoystickModel.h](include/JoystickModel.h)               | Couple `(x, y)` brut joystick. `JOYSTICK_MAX = 2047` (ADC 12 bits). Champ privé `roomForManeuver = 50` = deadzone en LSB. |
| [include/PairingPacket.h](include/PairingPacket.h)               | Paquet appairage : `char magic[20]`. Constantes `REQ_MAGIC = "AERISYS_DRONE_PAIR"`, `RESP_MAGIC = "PAIR_CONFIRM"`. |
| [include/PingRequestDTO.h](include/PingRequestDTO.h)             | Keepalive : un seul booléen `pingState`. Sert au failsafe drone si la manette disparaît. |
| [include/TelemetryDTO.h](../include/TelemetryDTO.h)              | **Nouveau v1.2.0.** Paquet drone → controller : snapshot IMU complet (accel/gyro/mag/orientation/quaternion/temperature/timestamp) + `motorSpeeds[NUM_MOTORS]`. Dépend des types `IMUSensor::*` (imu-lib). |

[examples/basic/main.cpp](examples/basic/main.cpp) est un sanity-check
autonome (round-trip `toStruct` / `fromStruct`) ; il est **exclu** du paquet
distribué aux consommateurs (cf. `library.json` `export.exclude`) afin de
ne pas créer de conflit avec leur propre `app_main`.

## Nouveau en v1.2.0

### `TelemetryDTO` — paquet drone → controller unifié

Avant v1.2.0, le drone émettait un `mpuDTO` (issu d'imu-lib) avec un champ
`motorSpeeds[]` ad-hoc et ne contenait ni quaternion, ni temperature, ni
timestamp. Le controller le consommait via un dispatch `len == sizeof(mpuDTO)`
sans souscripteur réel.

[`TelemetryDTO`](../include/TelemetryDTO.h) (v1.2.0) :
- Snapshot IMU **atomique** issu de `imu->getSnapshot()` (seqlock côté
  imu-lib v1.1) — accel/gyro/mag/orientation/quaternion/temperature
  cohérents entre eux ;
- `motorSpeeds[NUM_MOTORS]` (`NUM_MOTORS=4` par défaut, défini dans le même
  header) — wire-format-locked ;
- `timestampUs` = `esp_timer_get_time()` au moment de la lecture I²C, permet
  au receveur de calculer son propre `dt` sans re-poll FreeRTOS ticks ;
- Taille totale : **92 octets** (< 250 B max ESP-NOW).

Dépend de `imu_sensor.h` (imu-lib). Les deux consommateurs (`drone` et
`controller`) ont déjà imu-lib dans leur `lib_deps` — pas de changement
nécessaire de leur côté.

### `NUM_MOTORS` centralisé

Désormais défini dans `TelemetryDTO.h` (la valeur conditionne le wire
layout). Le drone v2 retire son propre `include/DroneConstants.h` local
(cf. tâche H1). Un consommateur peut overrider la valeur avec un
`#define NUM_MOTORS X` avant l'include, **à condition** de recompiler
les deux ends en lockstep.

## Breaking changes v1.1.0

### `ControllerRequestDTO` en POD (zero heap)

Auparavant, les 3 champs optionnels étaient exposés via raw pointers gérés
par `new`/`delete`. Conséquences :
- **6 allocations heap par paquet RC reçu** dans le callback ESP-NOW
  (contexte ISR-like Wi-Fi) → fragmentation + latence imprévisible ;
- race `operator=` ↔ callback (UAF possible) ;
- memory leak dans `ConvertJoyStickToFlightController`.

En v1.1.0 : valeurs par valeur + flags `has_X`. Compat binaire `ControllerRequestData`
(POD wire) **préservée**.

**Migration consommateur :**

| Avant (v1.0.x) | Après (v1.1.0) |
|---|---|
| `if (dto.flightController)` | `if (dto.has_flightController)` |
| `dto.flightController->throttle` | `dto.flightController.throttle` |
| `dto.flightController = new FlightController(p, r, y, t);` | `dto.flightController = FlightController(p, r, y, t);`<br>`dto.has_flightController = true;` |
| `dto.buttonMotorArming = new bool(true);` | `dto.buttonMotorArming = true;`<br>`dto.has_buttonMotorArming = true;` |
| `delete dto.flightController;` | (rien — destruction automatique par valeur) |

### `FlightController()` initialise `throttle = 0`

Le ctor par défaut laissait `throttle` indéterminé. Lecture avant
assignation = UB. Maintenant tous les axes sont à 0 explicitement.

### Structure de paquet conforme

`src/main.cpp` parasite déplacé vers `examples/basic/main.cpp` et **exclu
du paquet** via `library.json` `export.exclude`. Les consommateurs ne
tireront plus que `src/*.cpp` + `include/*.h`. Cela règle un conflit
latent de redéfinition d'`app_main` qui n'avait pas surface en v1.0 que
par chance du linker.

## Format wire ESP-NOW

| Sens | Type | Taille | Depuis |
|---|---|---|---|
| Manette → drone | `ControllerRequestData` (POD) | ~36 B | v1.0 (inchangé en v1.1+) |
| Manette → drone | `PingRequestDTO` | 1 B | v1.0 |
| Manette ↔ drone | `PairingPacket` | 20 B | v1.0 |
| Drone → manette | `TelemetryDTO` | **92 B** | **v1.2.0** |

Le receveur dispatche par `sizeof()`. Si tu modifies un layout, bumper la
version et migrer les deux ends en cohérence.

`ControllerRequestDTO` (la classe C++) est sérialisée via `toStruct()`
en `ControllerRequestData` (POD plat) pour le transport, puis re-hydratée
côté drone via `fromStruct()`. **Aucun changement de layout binaire** en
v1.1.0 — les contrôleurs v1.0 et drones v1.1 communiquent encore (le
contrôleur a juste les bugs heap si non migré).

## Commandes utiles

```powershell
pio run             # build standalone (compile la lib + examples/basic/)
pio run -t upload   # flash le sanity-check
pio run -t clean
```

Les firmwares ne build pas cette lib avec `pio run` ; PlatformIO la
clone depuis GitHub vers `.pio/libdeps/esp32dev/esp-lib/`.

## Conventions / pièges

- **Versioning** : bumper `version` dans [library.json](library.json) **et**
  [package.json](package.json) puis pousser un tag sur GitHub. PlatformIO
  résout par version, pas par branche par défaut.
- **Wire format = compat binaire** : changer le layout de
  `ControllerRequestData`, `FlightControllerData`, `PingRequestDTO` ou
  `PairingPacket` casse le lien manette ↔ drone. Migrer les deux côtés
  en cohérence.
- **`has_*` flags** : `ControllerRequestData` utilise des flags
  `has_buttonMotorState`, `has_buttonMotorArming`, `has_flightController`
  pour signaler les champs valides (style protobuf optional). Le DTO
  C++ duplique ces flags par symétrie (`dto.has_flightController` etc.).
- **Conversion** : passer par `toStruct()` / `fromStruct()` pour le wire.
  L'API DTO C++ se manipule directement (valeurs, pas pointeurs).
- **Compteur** : `counter` est incrémenté automatiquement via
  `initCounter()` (statique `nmbInstanciation`). Permet de détecter
  des paquets dupliqués/perdus côté drone.
  **Limite connue** : un reboot du contrôleur reset le counter à 0 →
  le drone refuse les paquets jusqu'à dépasser l'ancien max. À gérer
  côté drone (timeout / heuristique), prévu pour une future release.
- **`JOYSTICK_MAX = 2047`** : si tu passes en ADC 8 bits ou 11 bits,
  adapter cette constante (et la code manette qui mappe l'ADC).
- **`fromStruct` callsafe ISR** : depuis v1.1.0, `fromStruct` n'alloue
  rien sur le heap. Appel direct depuis le callback `esp_now_register_recv_cb`
  est désormais sans risque de fragmentation / latence imprévisible.
- **Dep imu-lib (depuis v1.2.0)** : `TelemetryDTO.h` inclut `<imu_sensor.h>`
  de imu-lib. esp-lib ne déclare PAS imu-lib dans ses `library.json`
  `dependencies` (PlatformIO ne gère pas bien les deps transitives sur
  des libs git-only). Tout consommateur de `TelemetryDTO` doit donc
  ajouter `imu-lib` à ses propres `lib_deps`. Drone et controller le
  font déjà ; documenter pour tout futur consommateur.

## Itération locale (sans pousser sur GitHub à chaque test)

Côté firmware (`controller` ou `drone`), remplacer temporairement le
`lib_deps` URL par un chemin local dans `platformio.ini` :

```ini
lib_deps =
    file://../esp-lib
    https://github.com/Aerisys/imu-lib
```

ou ajouter :
```ini
lib_extra_dirs = ../
```

et garder la lib dans `lib_deps` mais sans URL. Penser à rétablir
l'URL GitHub avant de commit.

## Tests

Pas de tests unitaires. La validation se fait via :
- le sanity-check `examples/basic/main.cpp` (round-trip sérialisation)
- les firmwares consommateurs (`controller`, `drone`).
