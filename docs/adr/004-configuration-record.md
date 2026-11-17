# 4. Configuration persistence will use one atomic record

- Status: Accepted for the platform-neutral repository
- Decision: Serialize the complete configuration into one versioned, checksummed record and keep
  two generations in alternating slots. The storage adapter must make each slot replacement
  durable/atomic; reads select the newest valid generation.
- Context: Partial multi-key updates can leave mixed configuration after power loss.
- Consequences: The codec, dual-slot recovery algorithm, round-trip/property checks, and simulated
  interruption tests exist. No ESP32 NVS adapter or encrypted credential save path is wired yet;
  that work requires an approved NVS encryption/key-provisioning process.
