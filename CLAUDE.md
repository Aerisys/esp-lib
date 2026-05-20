# esp-lib — Bibliothèque partagée Aerisys (DTOs ESP-NOW)

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

## Contenu (`include/` + `src/`)

| Header | Rôle |
| ------ | ---- |
| [include/ControllerRequestDTO.h](include/ControllerRequestDTO.h) | Paquet principal manette → drone. Contient un `FlightController` + boutons `arming` / `motor state` + compteur. Conversion `toStruct()` / `fromStruct()` pour transport binaire ESP-NOW. |
| [include/FlightController.h](include/FlightController.h)         | Consigne de vol pure : `pitch`, `roll`, `yaw`, `throttle` (floats). Méthode `isFullZero()` utile pour détecter neutre. |
| [include/JoystickModel.h](include/JoystickModel.h)               | Couple `(x, y)` brut joystick. `JOYSTICK_MAX = 2047` (ADC 12 bits). Champ privé `roomForManeuver = 50` = deadzone en LSB. |
| [include/PairingPacket.h](include/PairingPacket.h)               | Paquet appairage : `char magic[20]`. Constantes `REQ_MAGIC = "AERISYS_DRONE_PAIR"`, `RESP_MAGIC = "PAIR_CONFIRM"`. |
| [include/PingRequestDTO.h](include/PingRequestDTO.h)             | Keepalive : un seul booléen `pingState`. Sert au failsafe drone si la manette disparaît. |

`src/main.cpp` n'est utile que pour les tests autonomes de la lib
(PlatformIO oblige un point d'entrée si on construit la lib seule).

## Commandes utiles

```powershell
pio run                       # build standalone (sanity check)
pio run -t clean
```

Les firmwares ne build pas cette lib avec `pio run` ; PlatformIO la
clone depuis GitHub vers `.pio/libdeps/esp32dev/esp-lib/`.

## Conventions / pièges

- **Versioning** : à chaque modif publiée, bumper `version` dans
  [library.json](library.json) (`1.0.27` actuellement) puis pousser
  un tag/commit sur GitHub. PlatformIO résout par version, pas par
  branche par défaut.
- **Mutation des structs** : les `typedef struct` (`ControllerRequestData`,
  `FlightControllerData`) sont **transportés tels quels** via
  `esp_now_send`. **Tout changement de layout casse la compatibilité
  binaire** manette ↔ drone. Re-flasher les deux côtés en cohérence.
- **`has_*` flags** : `ControllerRequestData` utilise des flags
  `has_buttonMotorState`, `has_buttonMotorArming`, `has_flightController`
  pour signaler les champs valides (style protobuf optional). Bien
  les set quand on remplit un paquet.
- **Conversion** : passer par `toStruct()` / `fromStruct()` plutôt
  qu'accéder aux membres `nullptr`-able directement (cf. les pointeurs
  `flightController`, `buttonMotorState`, `buttonMotorArming` dans
  `ControllerRequestDTO`).
- **Compteur** : `counter` est incrémenté automatiquement via
  `initCounter()` (statique `nmbInstanciation`). Permet de détecter
  des paquets dupliqués/perdus côté drone.
- **`JOYSTICK_MAX = 2047`** : si tu passes en ADC 8 bits ou 11 bits,
  adapter cette constante (et la code manette qui mappe l'ADC).

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

Pas de tests unitaires. La validation se fait via les firmwares
consommateurs (sérialisation/désérialisation des paquets ESP-NOW).
