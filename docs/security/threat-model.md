# STRIDE threat model

## Scope and assumptions

This assessment covers the current ESP32-C3 firmware skeleton, local controls, display, configuration
loader, application commands, and the planned remote-service boundary. Physical possession of the
device is in scope. No server API or production key-management procedure has been supplied.

## Threats and current controls

| STRIDE | Threat | Current control / status |
|---|---|---|
| Spoofing | A nearby party sends commands or changes local settings | Inputs are local GPIO events. There is no network command API or identity model yet. |
| Tampering | Configuration or firmware is modified | Configuration uses bounded application buffers. Authenticated storage, signed OTA, Secure Boot v2, and Flash Encryption are not enabled/qualified. |
| Repudiation | A state change cannot be attributed | Commands/events carry an in-memory command ID. Persistent audit logging and correlation across reboot are absent. |
| Information disclosure | Wi-Fi credentials leak through logs or storage | Secret buffers clear on destruction/move. The current legacy loader reads separate Wi-Fi SSID/password strings from NVS without an established encryption configuration; treat existing provisioned devices as storing credentials without confidentiality until encryption is enabled and verified. New record persistence is deliberately not wired to ESP NVS yet. |
| Denial of service | Malformed input, queue exhaustion, or blocking work prevents operation | Fixed-size queues and input buffers limit some resource use. The configuration-record decoder is fuzzed on the host, and dual-slot recovery is tested with interrupted writes. Other decoder fuzzing, NVS power-loss testing, queue-pressure tests, and end-to-end watchdog coverage remain incomplete. |
| Elevation of privilege | Unauthorized firmware or configuration gains control | No remote service is integrated. Secure boot, authenticated OTA, and privileged-action authorization remain open requirements. |

## Required before production

1. Define the server protocol and trust anchors before implementing TLS, certificate/public-key
   pinning, or remote command authorization.
2. Define manufacturing key injection, Secure Boot v2, Flash Encryption, NVS encryption, recovery,
   and revocation procedures; do not enable irreversible eFuse settings without a reviewed process.
3. Implement signed OTA with rollback and a verified post-boot health check.
4. Extend decoder fuzzing and storage power-loss testing to all external decoders and the actual NVS adapter; add secret-log checks and resource-exhaustion tests.
5. Review and update this model when network, provisioning, persistence, or OTA code is introduced.
