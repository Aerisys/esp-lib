# esp-lib — Bibliothèque partagée Aerisys (DTOs ESP-NOW)

> **v1.1.0 — refonte majeure.** DTOs en POD (zero heap), structure de paquet
> conforme aux conventions ESP-IDF, `library.json` export rules pour ne pas
> polluer les consommateurs. Voir [Breaking changes v1.1.0](#breaking-changes-v110)
> ci-dessous avant migration.

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

[examples/basic/main.cpp](examples/basic/main.cpp) est un sanity-check
autonome (round-trip `toStruct` / `fromStruct`) ; il est **exclu** du paquet
distribué aux consommateurs (cf. `library.json` `export.exclude`) afin de
ne pas créer de conflit avec leur propre `app_main`.

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

## Format wire ESP-NOW (inchangé)

| Sens | Type | Taille |
|---|---|---|
| Manette → drone | `ControllerRequestData` (POD) | ~36 B |
| Manette → drone | `PingRequestDTO` | 1 B |
| Manette ↔ drone | `PairingPacket` | 20 B |

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
