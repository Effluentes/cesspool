# Definition of done

A change is done when:

- Host build and focused tests pass; sanitizer tests pass for changed application logic.
- The ESP32-C3 build passes for firmware changes, and the size report is attached to CI.
- The STM32 compile-only target passes when shared application code changes.
- `pre-commit run --all-files` passes; clang-format and clang-tidy findings are resolved or have
  documented, reviewed exceptions.
- New commands/events update `docs/events.md`, schema versions, and replay tests as applicable.
- Platform-only code remains in `platforms/`; application policy remains platform-neutral.
- Resource, security, and failure-path impacts are documented; no secrets or unverified production
  endpoints are added.
- Any newly introduced limitation, migration, or user-visible behavior is recorded in
  `CHANGELOG`.

Production release additionally requires a qualified ESP-IDF baseline, an approved security/key
provisioning process, storage/OTA recovery tests, measured memory budgets, and hardware or Wokwi
acceptance evidence.
