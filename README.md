# cesspool
IoT part of project


## Architecture
### 1. Ports & Adapters (Hexagonal Architecture)

Each HAL interface (`IGpio`, `IUart`, `ISpi`) is a **Port** — a pure abstraction with no reference to STM32.
Concrete BSP implementations act as **Adapters**.

Unit test mocking takes about **5 lines of code**.

---

### 2. Active Objects (Miro Samek / QP Framework)

Instead of mutexes and shared state, each subsystem
(`Sensor`, `Comms`, `OTA`, `Storage`) runs as its own **thread with an event queue**.

Race conditions are eliminated **by design**.

This approach is now a **de facto standard in safety-critical embedded systems**
(automotive, medical devices).

---

### 3. Hierarchical State Machines (HSM)

Each Active Object owns its own **HSM**, e.g.:

`CommsHsm: Idle → Connecting → Connected → Transmitting → Reconnecting`

Features include:

- nested states
- history states
- guaranteed entry/exit actions

This eliminates **switch/case spaghetti logic**.

---

### 4. Type-Safe Event Bus (Typed Signals)

Instead of callbacks and `void*`, the system uses **discriminated union signals** derived from `Signal`.

- ISR-safe `publishFromIsr()`
- Memory allocated from a **static pool**
- **No malloc after system startup**

---

### 5. Static Memory Everything

Examples:

- `MessageQueue<T, N>`
- `EventPool<N>`
- `Thread<TStack>`

All sizes are defined as **template parameters**.

The linker knows the exact memory footprint **at compile time**.

Result:

- no heap after startup
- no fragmentation
- **deterministic execution time**

---

### 6. Composition Root (Manual DI)

The **bootloader** is the only place where concrete types exist.

The rest of the system operates **only on interfaces**.

Switching from **STM32 → ESP32** requires changing only the **BSP layer**,
without touching application logic.

---

### 7. CRTP for Zero-Cost Abstractions

(Used in `Thread`, `Queue`, etc.)

Instead of `virtual` calls in hot paths, the system uses
**static polymorphism via templates (CRTP)**.

On **ARM Cortex-M**, this can be the difference between **~1 cycle vs. dozens of cycles**.