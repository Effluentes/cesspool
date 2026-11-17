# Architecture

## C4 context

```mermaid
flowchart LR
    User[Device operator] -->|rotary encoder and buttons| Device[Cesspool controller]
    Device -->|status and menu| Display[SSD1306 display]
    Device -. "future: provisioned API; not implemented" .-> Server[Remote service]
```

## C4 containers

```mermaid
flowchart TB
    subgraph Firmware["ESP32-C3 firmware"]
        Platform["platforms/esp32\nGPIO, display, configuration, logging"]
        Runtime["runtime\nQP active objects and composition"]
        Application["runtime/application\nC++23 application core, commands, events, views"]
        Packages["packages\nplatform-neutral interfaces and configuration types"]
        Platform --> Runtime
        Runtime --> Application
        Runtime --> Packages
        Application --> Packages
    end
    User[Operator] --> Platform
    Display[SSD1306] <--> Platform
```

`App` initializes QP and delegates startup to `SystemOrchestrator`. The orchestrator owns the
application core and wires static ports and active objects. Application decisions live in
`runtime/application`; platform implementations stay under `platforms/`.

The application core uses fixed-size values and buffers. This does not establish a whole-firmware
no-allocation guarantee: IDF and third-party component internals still need measurement.

## Current limitations

- The menu and in-memory event/read-model foundation exist; persistence, remote provisioning, and
  event storage/replay across reboot do not.
- The STM32 target is compile-only and does not provide platform adapters.
- ESP-IDF local installation is `v6.1-dev-7308-g0276131402f`, while `dependencies.lock` records
  IDF `6.2.0`. Resolve this mismatch before treating either version as the production baseline.
- Wokwi CI, signed OTA, certificate pinning, secure boot, flash/NVS encryption, core-dump
  collection, and production size/stack budgets are not yet qualified.
