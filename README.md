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

## Driver Architecture and Naming Conventions

## 1. Ports vs. Adapters (Folder Structure)
To clearly distinguish between hardware-specific implementations and abstract interfaces, the following naming rules apply:

- **`components/drivers/` (Ports)**:
  Named using the pattern `{technology}_{chip}` (e.g., `display_ili9341`, `bluetooth_nimble`).
  These folders contain the **concrete drivers** that are tightly coupled to the specific hardware (MCU peripheral or external chip).

- **`packages/drivers/` (Adapters)**:
  Named using the abstract technology name only (e.g., `display`, `bluetooth`).
  These folders contain the **generic abstraction layers** (interfaces, virtual classes, or adapter logic) that the application logic relies upon.

This structure makes it immediately obvious which elements are **ports** (concrete implementations) and which are **adapters** (abstract contracts), simplifying maintenance and hardware porting.

---

## 2. Subcomponents
If a component requires additional auxiliary logic (e.g., internal state machines, protocol parsers, or helper utilities), it must reside in a **nested subfolder** within its parent component (e.g., `domain/slave/protocol/`).

Such extensions **must not** be promoted to new top-level packages; they remain encapsulated inside the parent directory. This policy enforces high cohesion, keeps the project root clean, and makes dependencies between closely related modules explicit.

---

## 3. CRTP Boundary and Low-Level Hardware Dependencies
A strict architectural boundary is enforced regarding Curiously Recurring Template Pattern (CRTP) base classes and low-level hardware abstractions:

- Files such as **`SpiBase.hpp`** and **`GpioBase.hpp`** are located in **`components/`**, **not** in `packages/`.

### Rationale
This decision is made because these base classes either:
- Directly utilise **ESP-IDF specific types** (e.g., `spi_device_handle_t`, `gpio_num_t`), or
- Are tightly coupled to the **register-level configuration** or peripheral internals of the target chip.

Consequently, despite being named "Base" or appearing abstract in design, they are **inherently hardware-dependent**. Placing them in `components/` (alongside the concrete ports) rather than in the platform-agnostic `packages/` layer avoids false abstractions and ensures that the adapter layer remains truly portable across different platforms.

### [Example of folder structure](./cesspool_folder_structure.html)