# Runtime resource budgets

These are configured capacities, not measured worst-case guarantees.

| Resource | Configured capacity | Current evidence / gap |
|---|---:|---|
| `SystemAO` task stack | 1024 `StackType_t` words | Static storage; no stack high-water measurement yet |
| `DisplayAO` task stack | 1024 `StackType_t` words | Static storage; no stack high-water measurement yet |
| Controls task stack | 768 `StackType_t` words | Static storage; logs its minimum free stack in words at runtime |
| Each QP active-object event queue | 10 event pointers | Fixed `std::array`; queue-depth/high-water telemetry is not implemented |
| Application command result | At most 1 domain event | Fixed `std::array<DomainEvent, 1>` |
| Application command-ID history | 16 IDs | Fixed storage; duplicates older than this window can be accepted again |
| Configuration record | At most 107 bytes | Fixed-size encoded record; CRC-32 and schema version validated |
| Firmware image | 838,860 bytes (819.2 KiB) | CI checks against the generated application binary; partition capacity is 1 MiB |
| DRAM | 256,000 bytes | CI checks IDF map-file usage; current observed usage was 59,740 bytes |

On ESP32-C3, `StackType_t` is 32 bits, making the configured stack storage 4096 bytes per QP
active object and 3072 bytes for Controls. This excludes TCBs, queues, and other static objects.
The actual compiler/linker memory report must be reviewed on the production toolchain.

There is currently no dynamically allocated QP event pool: the implemented control signals are
static events with no payload, and application facts are returned by value. Consequently there is
no payload-derived event-pool size to assert for `CONFIGURATION_READY` or
`CONFIGURATION_CHANGED`; those event types are not present. If they are introduced, define their
payload sizes and a pool-capacity `static_assert` alongside the events before publishing them.

The host test `application-runtime-no-allocations` intercepts C++ `new` and C `malloc`, `calloc`,
and `realloc` while exercising application command dispatch and queries. This does not yet cover
the ESP-IDF/QP platform runtime or startup and shutdown paths.

The versioned codec and dual-slot repository are platform-neutral and covered by host tests,
including interrupted writes at every byte offset. They are not yet connected to NVS: credentials
are currently loaded from legacy NVS strings, and saving any new credential record is blocked on
NVS encryption and its manufacturing/key-provisioning process.

Only the Controls task currently enrolls with and resets the Task Watchdog. Watchdog enrollment,
stack high-water instrumentation, and queue-pressure tests for both active objects remain required
before production.
