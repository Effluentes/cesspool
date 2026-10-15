# Modern Software Engineering Field Guide

## Architecture, Design, Testing, Observability, Security, Delivery, and Experimental Techniques

**Scope:** A cross-language reference for embedded C++ (e.g.,
ESP32/ESP-IDF/QP), Rust services, Go services, and supporting
applications.\
**Purpose:** A learning backlog and experiment catalogue---not a
recommendation to adopt every technique in production.

> **Adoption rule:** Start with the smallest useful mechanism, define
> the problem and success criteria, measure the result, and keep or
> remove the experiment based on evidence. Novelty is not a substitute
> for reliability.

## 1. How to Use This Catalogue

  -----------------------------------------------------------------------
  Practice                            How to use it
  ----------------------------------- -----------------------------------
  Establish a baseline                Record build time, binary size,
                                      RAM/flash use, latency, failure
                                      rate, recovery time, and test
                                      duration before changing anything.

  Run bounded experiments             Give each experiment a hypothesis,
                                      scope, time limit, success metric,
                                      risks, and rollback plan.

  Separate learning from production   Put experimental implementations
                                      behind interfaces, feature flags,
                                      compile-time options, or separate
                                      branches.

  Prefer evidence                     Use benchmarks, traces, fault
                                      injection, test results, and
                                      operational data rather than
                                      intuition alone.

  Match technique to risk             Formal methods and exhaustive
                                      testing are especially valuable for
                                      protocol parsers, state
                                      transitions, persistence,
                                      safety-relevant behavior, and
                                      distributed consistency.

  Avoid architecture astronautics     Do not introduce CQRS, event
                                      sourcing, service meshes, or a
                                      custom runtime merely because they
                                      are interesting.
  -----------------------------------------------------------------------

## 2. Architecture and System Decomposition

  ----------------------------------------------------------------------------------------------
  Concept           What it is       Why use it         Example             Useful tools /
                                                                            references
  ----------------- ---------------- ------------------ ------------------- --------------------
  Layered           Organizes code   Makes dependencies Domain logic does   CMake targets, Rust
  architecture      into layers with understandable and not import ESP-IDF, crates, Go packages;
                    controlled       testable.          FreeRTOS, HTTP, or  *Clean Architecture*
                    dependencies.                       database packages.  

  Hexagonal         Core logic       Enables hardware   `Radio` port        C++ abstract
  architecture      depends on       and backend        implemented by      interfaces, Rust
  (Ports and        interfaces;      substitutions.     CC1101 and a fake   traits, Go
  Adapters)         infrastructure                      radio in tests.     interfaces
                    implements them.                                        

  Clean             Dependency       Keeps business     Application use     Dependency rules,
  Architecture      direction points rules independent  case calls a        architecture tests
                    inward toward    of frameworks.     `Storage` port, not 
                    policy/domain.                      NVS directly.       

  Modular monolith  One deployable   Avoids premature   One Go server with  Go modules/packages,
                    application with microservice       separate telemetry, Rust workspace,
                    strong internal  complexity.        identity, and       CMake targets
                    module                              device modules.     
                    boundaries.                                             

  Bounded Contexts  Defines where a  Prevents one       Device              Context maps, ADRs
  (DDD)             domain model and ambiguous model    provisioning,       
                    its language are from spreading     telemetry           
                    valid.           everywhere.        ingestion, and user 
                                                        accounts use        
                                                        distinct models.    

  Domain-Driven     Models software  Useful when domain `Device`,           Value objects,
  Design (DDD)      around domain    complexity is      `Measurement`,      aggregates, domain
                    concepts and     substantial.       `AlarmPolicy`, and  services
                    rules.                              their invariants.   

  Vertical slices   Organizes code   Makes a feature    "Provision device"  Feature folders,
                    around           easier to deliver  includes API,       integration tests
                    end-to-end       and test.          application logic,  
                    capabilities.                       persistence, and    
                                                        tests.              

  Component         Splits the       Supports           `RadioDriver`,      CMake targets, Rust
  architecture      system into      replacement and    `RadioProtocol`,    crates, Go packages
                    components with  parallel           `SlaveRegistry`,    
                    explicit         development.       `TelemetryStore`.   
                    contracts.                                              

  Dependency        High-level       Improves           Runtime depends on  Interfaces/traits,
  inversion         policy does not  portability and    a clock interface,  compile-time
                    depend on        testability.       not a hardware      dependency checks
                    low-level                           timer API.          
                    details.                                                

  Composition root  The place where  Makes wiring       `master_main`       Constructors,
                    concrete         explicit and       creates the         dependency injection
                    dependencies are avoids hidden      platform, actors,   
                    assembled.       globals.           storage, and        
                                                        services.           

  Dependency        Supplies         Makes              Inject a fake clock Constructor
  injection         dependencies     substitutions and  or radio into a     injection; avoid
                    from outside a   tests              state machine.      unnecessary DI
                    component.       straightforward.                       frameworks

  Plugin            Extensions       Useful when        Select a transport  Interfaces, dynamic
  architecture      implement a      implementations    adapter at build    loading where
                    stable contract. vary at runtime or configuration time. appropriate
                                     build time.                            

  Anti-corruption   Translates       Keeps vendor or    Translate a BLE     Mappers, adapters
  layer             between external legacy concepts    GATT payload into a 
                    and internal     out of the domain. domain command.     
                    models.                                                 

  Strangler pattern Replaces a       Reduces risk       Route selected API  Routing,
                    legacy subsystem compared with a    operations to a new compatibility tests
                    incrementally.   big-bang rewrite.  service.            

  Architecture      Short record of  Preserves          ADR: why QP/C++     MADR, log4brains,
  Decision Record   context,         reasoning and      owns embedded state Markdown
  (ADR)             decision,        makes reversals    machines.           
                    alternatives,    deliberate.                            
                    and                                                     
                    consequences.                                           

  C4 model          Context,         Communicates       Draw device,        Structurizr,
                    container,       architecture at    gateway, cloud, and Mermaid, PlantUML
                    component, and   multiple zoom      mobile boundaries.  
                    code-level       levels.                                
                    diagrams.                                               

  Fitness functions Automated checks Stops dependency   CI fails if domain  Deptrac, custom
                    that guard       rules from         code imports        scripts,
                    architectural    eroding.           platform headers.   import-linter, tests
                    properties.                                             

  Threat modeling   Systematically   Finds security     STRIDE review of    OWASP Threat Dragon,
                    identifies       issues before      BLE provisioning    STRIDE, attack trees
                    threats and      implementation.    and firmware        
                    mitigations.                        updates.            
  ----------------------------------------------------------------------------------------------

## 3. State, Concurrency, and Event-Driven Design

  --------------------------------------------------------------------------------------------------------------------------------------
  Concept             What it is        Why use it       Example                                               Useful tools / references
  ------------------- ----------------- ---------------- ----------------------------------------------------- -------------------------
  Finite State        A system with     Makes lifecycle  `Disconnected -> Connecting -> Connected -> Fault`.   QP/C++, Boost.SML, Rust
  Machine (FSM)       explicit states   logic explicit.                                                        enums, Go state logic
                      and transitions.                                                                         

  Hierarchical State  States can        Controls state   `Operational` contains `Idle`, `Sampling`, and        QP/C++, statecharts
  Machine (HSM)       contain substates explosion in     `Transmitting`; all operational substates share fault 
                      and share parent  complex          handling.                                             
                      behavior.         lifecycles.                                                            

  UML Statecharts     FSMs with         Useful for       Define radio startup, retries, sleep, and recovery.   PlantUML, Mermaid
                      hierarchy,        complex reactive                                                       (limited), statechart
                      concurrency, and  behavior.                                                              tools
                      event semantics.                                                                         

  State pattern       Encapsulates      Useful when      Different connection states handle commands           OO design; compare with
                      state-dependent   state logic is   differently.                                          enum-based FSM
                      behavior.         large and                                                              
                                        polymorphic.                                                           

  Event-driven        Components react  Reduces coupling `MeasurementReceived` triggers aggregation and        QP, NATS, MQTT, channels
  architecture (EDA)  to events instead and supports     display updates.                                      
                      of synchronous    asynchronous                                                           
                      call chains.      devices.                                                               

  Actor model         Isolated actors   Helps avoid      Each radio manager owns its radio state and queue.    QP/C++, Tokio
                      own state and     shared-memory                                                          actors/libraries, Go
                      communicate by    races.                                                                 goroutines + channels
                      messages.                                                                                

  Active Object       Owns a            Serializes state `RadioAO` handles one event at a time.                QP/C++, custom event loop
                      thread/task and   changes while                                                          only if justified
                      processes events  remaining                                                              
                      through a queue.  asynchronous.                                                          

  Message passing     Components        Makes            Send a typed `SetSamplingPeriod` command.             QP events, Rust channels,
                      exchange          concurrency                                                            Go channels
                      immutable or      boundaries                                                             
                      ownership-safe    explicit.                                                              
                      messages.                                                                                

  Publish/subscribe   Publishers emit   Decouples        One sensor event feeds display, storage, and          QP publish-subscribe,
                      events to         producers from   telemetry.                                            MQTT, NATS
                      multiple          consumers.                                                             
                      subscribers.                                                                             

  Command vs event    A command asks    Clarifies intent `SetOutput(true)` command vs `OutputChanged(true)`    Naming conventions, API
                      for an action; an and failure      event.                                                contracts
                      event reports     semantics.                                                             
                      something that                                                                           
                      happened.                                                                                

  Event envelope      Metadata wrapper  Enables          `event_id`, `device_id`, `timestamp`,                 Protobuf, CBOR, JSON
                      around a domain   correlation,     `schema_version`, payload.                            Schema
                      event.            versioning, and                                                        
                                        diagnostics.                                                           

  CQRS                Separates write   Useful when read Commands update device configuration; dashboard reads Separate handlers/read
                      models from read  and write needs  a materialized status view.                           projections
                      models.           differ                                                                 
                                        significantly.                                                         

  Event sourcing      Stores state      Supports audit   Record configuration changes for traceability.        Append-only log, event
                      changes as an     and replay but                                                         store; not a default for
                      event history.    adds operational                                                       embedded systems
                                        complexity.                                                            

  Blackboard          Shared state or   Useful when many A telemetry snapshot contains latest values and       Explicit ownership,
                      knowledge store   consumers need a health status.                                        mutex/versioning,
                      accessed by       common snapshot.                                                       immutable snapshots
                      components.                                                                              

  Single-writer       One component     Simplifies       `SensorAggregator` is the only writer of aggregate    Actor ownership, message
  principle           owns mutation of  concurrency and  sensor state.                                         queues
                      a state object.   invariants.                                                            

  Structured          Child tasks have  Prevents         A telemetry operation cancels its timeout and retry   Rust
  concurrency         clear lifetimes   orphaned tasks   tasks together.                                       `tokio::task::JoinSet`,
                      tied to a parent  and unclear                                                            Go `context`, C++ task
                      scope.            cancellation.                                                          scopes/libraries

  Backpressure        Slows or rejects  Prevents         Drop low-priority periodic samples when a telemetry   Bounded queues,
                      producers when    unbounded queues queue is full.                                        semaphores, flow control
                      consumers cannot  and memory                                                             
                      keep up.          exhaustion.                                                            

  Queue policy        Defines queue     Makes overload   Alarm events are retained; stale display refreshes    Bounded queues, priority
                      capacity and      behavior         are coalesced.                                        queues
                      behavior under    deliberate.                                                            
                      overload.                                                                                

  Cancellation and    Operations can    Avoids work      Stop a cloud request when its deadline expires.       Rust cancellation tokens,
  deadlines           stop and have a   continuing after                                                       Go `context.Context`, C++
                      time budget.      it is useless.                                                         stop tokens

  Retry with backoff  Retries failures  Avoids           Reconnect to a gateway after a radio or network       Exponential backoff,
  and jitter          with increasing,  synchronized     outage.                                               jitter
                      randomized delay. retry storms.                                                          

  Circuit breaker     Temporarily       Prevents         Pause cloud uploads while the endpoint is             Resilience libraries;
                      blocks calls to a repeated work    unavailable.                                          simple explicit state
                      failing           against a broken                                                       machine
                      dependency.       service.                                                               

  Bulkhead            Isolates          Stops one        Separate queues for alarms and bulk telemetry.        Dedicated queues, worker
                      resources between failure from                                                           pools
                      workloads.        exhausting the                                                         
                                        whole system.                                                          

  Idempotency         Repeating an      Essential for    Repeated `SetOutput(true)` does not toggle output     Idempotency keys,
                      operation has the retries and      twice.                                                desired-state commands
                      same intended     unreliable                                                             
                      effect.           networks.                                                              

  Desired state vs    Separates         Makes            Desired sample period is 10 s; observed device        Reconciliation loop
  observed state      intended          reconciliation   reports 12 s.                                         
                      configuration     and drift                                                              
                      from measured     visible.                                                               
                      reality.                                                                                 

  Timeouts            Bound how long a  Prevents         Radio acknowledgement deadline expires after a        Deadline timers,
                      call or state     indefinite       bounded interval.                                     watchdogs
                      transition can    stalls.                                                                
                      wait.                                                                                    

  Deterministic state Same state and    Makes replay and Feed recorded events into the state machine and       Pure transition
  transitions         event produce the testing easier.  compare outputs.                                      functions, model-based
                      same transition.                                                                         tests
  --------------------------------------------------------------------------------------------------------------------------------------

## 4. Domain Modeling, Types, and API Design

  ---------------------------------------------------------------------------------------------------------------------------
  Concept             What it is         Why use it         Example                                     Useful tools /
                                                                                                        references
  ------------------- ------------------ ------------------ ------------------------------------------- ---------------------
  Type-driven design  Encodes valid      Makes invalid      Rust enum distinguishes `Connected` from    Rust enums/newtypes,
                      states and         states harder to   `Disconnected`; C++ uses strong types.      C++ strong typedefs,
                      constraints in     represent.                                                     Go defined types
                      types.                                                                            

  Value object        Immutable value    Encapsulates       `TemperatureCelsius`, `DeviceAddress`,      Newtypes,
                      defined by its     validation and     `SamplingPeriod`.                           constructors
                      value rather than  domain meaning.                                                
                      identity.                                                                         

  Units of measure    Types or naming    Prevents unit      `DurationMs` cannot be confused with        Strong types, Rust
                      encode physical    conversion         `FrequencyHz`.                              units crates
                      units.             mistakes.                                                      

  Parse, don't        Convert untrusted  Reduces repeated   Parse JSON into `ValidDeviceConfig`.        Smart constructors,
  validate            input into a       checks and invalid                                             typed DTOs
                      validated domain   intermediate                                                   
                      type once.         values.                                                        

  Algebraic data      Sum and product    Makes handling     `RadioResult = Ack | Timeout | CrcError`.   Rust enums,
  types               types express      exhaustive.                                                    `std::variant`,
                      alternatives and                                                                  tagged unions
                      combinations.                                                                     

  Error taxonomy      Separates expected Improves recovery  `InvalidConfig` is not treated like         `Result<T,E>`, error
                      domain failures    and observability. `SpiBusFault`.                              codes, Go error
                      from                                                                              wrapping
                      infrastructure                                                                    
                      faults and bugs.                                                                  

  Result-based error  Errors are         Reduces hidden     Rust `Result<T, E>`; C++ expected-like      Rust standard
  handling            explicit in        control flow.      type.                                       library, C++23
                      function                                                                          `std::expected` where
                      signatures.                                                                       supported

  Design by Contract  Defines            Clarifies          A queue API guarantees FIFO ordering up to  Assertions,
                      preconditions,     component          capacity.                                   contracts, tests
                      postconditions,    guarantees.                                                    
                      and invariants.                                                                   

  Invariants          Conditions that    Protects domain    A slave ID must be unique within a network. Assertions, property
                      must always remain correctness.                                                   tests
                      true.                                                                             

  API-first /         Define interfaces  Enables parallel   Define command/event schemas before         OpenAPI, AsyncAPI,
  contract-first      and schemas before work and           implementing device and cloud sides.        Protobuf, JSON Schema
                      implementation.    compatibility                                                  
                                         checks.                                                        

  Schema evolution    Changes message    Devices may update Add optional telemetry fields while old     Protobuf, CBOR,
                      formats compatibly at different       firmware ignores them.                      explicit schema
                      over time.         times.                                                         versions

  Semantic versioning Communicates       Helps manage       Major version for breaking protocol         SemVer, changelogs
                      compatibility      libraries and      changes.                                    
                      expectations.      APIs.                                                          

  Capability-based    Grants specific    Reduces accidental A component receives read-only telemetry    Narrow interfaces,
  design              capabilities       misuse and         access, not all storage access.             capability tokens
                      instead of broad   security risk.                                                 
                      ambient authority.                                                                

  Functional core,    Pure domain logic  Makes logic easy   Pure alarm-threshold calculation; adapter   Pure functions,
  imperative shell    is surrounded by   to test across     reads sensor and drives output.             ports/adapters
                      side-effecting     languages.                                                     
                      adapters.                                                                         

  Immutability        Avoids in-place    Simplifies         Publish immutable telemetry snapshots.      `const`, Rust
                      changes where      reasoning and                                                  ownership,
                      practical.         concurrency.                                                   copy-on-write

  Ownership and       Rust compile-time  Prevents many      Move a message into a task rather than      Rust compiler,
  borrowing           model for memory   classes of memory  share mutable state.                        Clippy, Miri
                      and aliasing.      bugs.                                                          

  RAII /              Resource lifetime  Avoids leaks and   SPI device lock or file handle releases     C++ RAII, Rust Drop
  deterministic       follows object     forgotten cleanup. automatically.                              
  cleanup             lifetime.                                                                         

  Generics and        Express reusable   Enables reuse      Generic retry policy over a transport       C++
  traits/interfaces   behavior behind    without copying    interface.                                  templates/concepts,
                      compile-time or    logic.                                                         Rust traits, Go
                      runtime contracts.                                                                interfaces/generics

  Code generation     Generates          Reduces drift      Generate C++, Rust, and Go protocol types   Protobuf, OpenAPI
                      repetitive code    between            from Protobuf.                              Generator
                      from a schema or   implementations.                                               
                      model.                                                                            

  Serialization       Converts domain    Prevents transport Domain `Measurement` maps to compact CBOR   Protobuf, CBOR,
  boundary            types to           formats from       payload.                                    MessagePack, JSON
                      wire/storage       becoming domain                                                
                      representations.   models.                                                        

  Zero-copy design    Avoids unnecessary Can reduce latency Borrow a packet buffer during parsing.      Rust slices, C++
                      data copying.      and memory use.                                                spans; profile before
                                                                                                        adopting

  Const correctness   Makes mutation     Improves API       `const Config&` for read-only               Compiler warnings,
                      permissions        clarity and        configuration.                              static analysis
                      explicit in C++.   optimization                                                   
                                         opportunities.                                                 
  ---------------------------------------------------------------------------------------------------------------------------

## 5. Testing Strategy and Verification

  ----------------------------------------------------------------------------------------------------------------
  Test / method          What it checks       Why use it         Example                      Tools
  ---------------------- -------------------- ------------------ ---------------------------- --------------------
  Unit tests             Small units in       Fast feedback on   Test alarm threshold logic   GoogleTest/Catch2,
                         isolation.           rules and edge     with fixed inputs.           Rust `cargo test`,
                                              cases.                                          Go `testing`

  Component tests        A module plus its    Validates module   Test `RadioProtocol` against Test doubles,
                         immediate            contracts without  a fake transport.            component harnesses
                         collaborators.       the whole system.                               

  Integration tests      Real interactions    Finds wiring,      Device service writes and    Testcontainers,
                         between modules or   serialization, and reads configuration from a   Docker Compose,
                         services.            persistence        test database.               hardware rigs
                                              issues.                                         

  System / end-to-end    User-visible         Verifies the       Phone provisions device,     Playwright, device
  tests                  workflows across the complete path.     device stores config, cloud  farms, HIL rigs
                         full system.                            sees status.                 

  Contract tests         Two independently    Detects            Firmware and server both     Pact, schema
                         developed sides      incompatible API   accept the same protocol     compatibility checks
                         agree on an          or message         fixtures.                    
                         interface.           changes.                                        

  Regression tests       Previously fixed     Prevents recurring A regression test for a BLE  Any test framework
                         failures stay fixed. bugs.              write that was not           
                                                                 processed.                   

  Smoke tests            Critical basics work Fast gate before   Firmware boots, service      CI scripts, health
                         after a              expensive testing. health endpoint responds.    checks
                         build/deploy.                                                        

  Acceptance tests       Requirements are     Connects behavior  "A lost acknowledgement must Gherkin optionally,
                         satisfied from a     to explicit        not cause a second physical  test plans
                         user or domain       requirements.      toggle."                     
                         perspective.                                                         

  Table-driven tests     Many cases share one Makes edge cases   Test parser inputs: valid,   Go table tests,
                         test structure.      concise and        truncated, oversized,        parameterized C++
                                              visible.           invalid CRC.                 tests

  Property-based testing Checks general       Finds combinations Encoding then decoding a     RapidCheck,
                         properties over      humans may miss.   valid message preserves      Hypothesis,
                         generated inputs.                       fields.                      `proptest`, Go
                                                                                              fuzzing

  Fuzz testing           Feeds malformed or   Finds crashes,     Fuzz BLE JSON and CC1101     libFuzzer, AFL++,
                         random inputs to     hangs, and memory  packet parsers.              Honggfuzz,
                         parsers and APIs.    errors.                                         `cargo fuzz`, Go
                                                                                              fuzzing

  Coverage-guided        Uses execution       Improves           Fuzz binary protocol decoder libFuzzer, AFL++
  fuzzing                feedback to discover exploration of     with sanitizers.             
                         new paths.           complex parsers.                                

  Mutation testing       Deliberately alters  Measures           Flip a comparison in alarm   Mull, mutmut,
                         code to see whether  test-suite         logic; tests should fail.    `cargo-mutants`, Go
                         tests detect it.     sensitivity, not                                mutation tools
                                              just coverage.                                  

  Branch / condition     Measures             Reveals untested   Verify every HSM transition  gcov/lcov, llvm-cov,
  coverage               control-flow         branches.          and error path.              `go tool cover`
                         coverage.                                                            

  MC/DC coverage         Checks whether each  Useful in          Verify alarm activation      Specialized coverage
                         condition            high-assurance     logic with multiple          tools
                         independently        contexts; costly.  thresholds.                  
                         affects a decision.                                                  

  Model-based testing    Generates tests from Checks sequences   Generate connect, timeout,   QP model/statechart
                         a state or behavior  and transitions    retry, disconnect sequences. tooling, Hypothesis
                         model.               systematically.                                 stateful testing

  State transition       Ensures every        Particularly       Test                         Transition tables,
  coverage               transition is        valuable for HSMs. `Starting -> Operational`,   model-based tests
                         exercised.                              `Starting -> Fault`,         
                                                                 `Fault -> Starting`.         

  Temporal logic / model Verifies properties  Finds deadlocks    "Every accepted command      TLA+, TLC, PlusCal,
  checking               across possible      and forbidden      eventually completes or      Spin
                         execution sequences. states before      times out."                  
                                              deployment.                                     

  Formal specification   Describes behavior   Removes ambiguity  Specify sequence numbers,    TLA+, Alloy, state
                         mathematically or    in protocol and    retries, and duplicate       machines
                         with precise         concurrency        handling.                    
                         contracts.           requirements.                                   

  Differential testing   Compares independent Finds              C++, Rust, and Go decoders   Shared fixtures,
                         implementations on   cross-language     process identical packet     property tests
                         the same inputs.     inconsistencies.   corpus.                      

  Golden / snapshot      Compares output to   Detects unintended Verify canonical JSON or     Snapshot testing
  tests                  approved fixtures.   serialization or   diagnostic output.           libraries
                                              CLI changes.                                    

  Deterministic replay   Replays captured     Reproduces         Replay a recorded radio      Event logs,
                         inputs/events.       timing-sensitive   event sequence through the   deterministic clocks
                                              or field failures. HSM.                         

  Fault injection        Intentionally        Tests recovery     Drop acknowledgements,       Fakes, proxies,
                         introduces failures. paths.             corrupt CRC, fail storage    hardware switches
                                                                 writes.                      

  Chaos testing          Randomly disrupts a  Evaluates          Restart a service or         Chaos Mesh, Litmus;
                         running distributed  resilience under   interrupt connectivity       use cautiously
                         system.              combined failures. during uploads.              

  Stress testing         Pushes load beyond   Finds bottlenecks  Flood telemetry queue at 10× k6, custom load
                         normal operating     and resource       expected rate.               generators
                         levels.              exhaustion.                                     

  Load testing           Measures behavior    Establishes        Simulate thousands of        k6, Locust, Vegeta
                         under expected load. throughput and     connected devices.           
                                              latency envelopes.                              

  Soak / endurance       Runs for long        Finds leaks,       Run firmware for days while  HIL rigs, metrics,
  testing                periods.             drift, wear, and   sampling and reconnecting.   watchdog logs
                                              rare failures.                                  

  Performance regression Compares performance Prevents silent    Track boot time, packet      Criterion, Google
  testing                against a baseline.  slowdowns or       latency, flash size,         Benchmark,
                                              binary growth.     allocations.                 `benchstat`

  Hardware-in-the-loop   Tests real firmware  Captures timing    Test SPI radio interactions  pytest, lab
  (HIL)                  against real or      and peripheral     with RF attenuator or signal instruments, relay
                         simulated hardware.  behavior mocks     generator.                   boards
                                              cannot.                                         

  Software-in-the-loop   Runs system logic in Enables repeatable Simulate radio loss and      Fakes, simulators
  (SIL)                  a simulated          tests without      sensor readings on a host    
                         environment.         hardware.          machine.                     

  Test doubles           Fake, stub, spy, or  Isolates behavior  Fake clock triggers a        Hand-written fakes,
                         mock dependencies.   and controls       timeout instantly.           mocking frameworks
                                              failure                                         
                                              conditions.                                     

  Golden hardware /      Uses a known-good    Helps separate     Keep a stable ESP32-C3 board Lab inventory,
  reference device       physical setup.      software defects   and calibrated sensor.       calibration records
                                              from lab drift.                                 

  Static analysis        Finds suspicious     Catches defects    Detect nullability,          clang-tidy, Clippy,
                         code without running early.             ownership, unsafe patterns,  `go vet`,
                         it.                                     and style issues.            staticcheck

  Sanitizers             Detect runtime       Finds classes of   Run host tests under         AddressSanitizer,
                         memory and           C/C++ defects.     ASan/UBSan/TSan.             UBSan, TSan
                         undefined-behavior                                                   
                         errors.                                                              

  Miri                   Interprets Rust code Useful for unsafe  Test unsafe buffer           `cargo miri`
                         to detect certain    Rust and low-level manipulation.                
                         undefined behavior.  abstractions.                                   

  Race detection         Detects data races   Helps validate     Run Go tests with `-race`;   Go race detector,
                         in supported runtime concurrent code.   C++ host tests with TSan.    TSan
                         environments.                                                        

  Compile-time tests     Verify type-level    Catches invalid    Static assertions for packet `static_assert`,
                         contracts and API    usage before       layout or trait bounds.      compile-fail tests
                         availability.        runtime.                                        

  Architecture tests     Check dependency and Keeps the design   Domain packages cannot       Custom scripts,
                         layering rules.      from regressing.   depend on platform packages. import-linter,
                                                                                              deptry

  Test pyramid / testing Balances fast        Avoids slow,       Many unit/property tests,    CI test stages
  portfolio              isolated tests with  brittle suites.    fewer HIL end-to-end tests.  
                         fewer expensive                                                      
                         system tests.                                                        

  Risk-based testing     Allocates effort     Focuses resources  Prioritize firmware update,  FMEA, risk matrix
                         according to failure where defects      output control, and packet   
                         impact and           matter most.       parsing.                     
                         likelihood.                                                          

  Requirements           Links requirements   Helps demonstrate  Requirement ID maps to state Markdown IDs, issue
  traceability           to design and tests. coverage and       transition and test case.    tracker, test
                                              diagnose gaps.                                  management
  ----------------------------------------------------------------------------------------------------------------

## 6. Embedded and Hardware-Specific Techniques

  ------------------------------------------------------------------------------------------------------------------------------------
  Concept                What it is            Why use it        Example                                              Tools /
                                                                                                                      references
  ---------------------- --------------------- ----------------- ---------------------------------------------------- ----------------
  Hardware Abstraction   Stable interfaces     Makes domain      `ISpi`, `IGpio`, `IClock`, `IRadio`.                 ESP-IDF drivers,
  Layer (HAL)            over                  logic portable.                                                        platform
                         peripheral-specific                                                                          adapters
                         code.                                                                                        

  Board Support Package  Board-specific pin,   Keeps board       ESP32-C3 pin mapping and radio wiring.               ESP-IDF, board
  (BSP)                  clock, and peripheral details                                                                configuration
                         setup.                localized.                                                             

  RTOS abstraction       Keeps OS-specific     Enables a         Only platform/runtime adapter uses FreeRTOS task     C++ interfaces,
  boundary               APIs out of portable  different RTOS or APIs.                                                compile-time
                         policy code.          host build.                                                            adapters

  Bare-metal / RTOS      Separates scheduling  Makes logic       Host-test HSM transitions with a fake event queue.   Host builds,
  portability            assumptions from      reusable across                                                        portability CI
                         domain logic.         targets.                                                               

  Hardware-in-the-loop   Automates tests on    Finds             CI flashes a board, runs a radio test, collects      pytest, OpenOCD,
  farm                   physical boards.      board-specific    logs.                                                serial control
                                               regressions                                                            
                                               continuously.                                                          

  Watchdog strategy      Defines how progress  Recovers from     Feed watchdog only after critical subsystems make    Task watchdog,
                         is monitored and      hangs without     progress.                                            independent
                         recovered.            hiding root                                                            watchdog
                                               causes.                                                                

  Brownout and           Tests unstable supply Protects          Interrupt power during configuration writes.         Programmable
  power-failure testing  and reset behavior.   persistence and                                                        supply, power
                                               safe outputs.                                                          switch

  Fault containment      Limits impact of a    Prevents one      Radio failure does not block alarm sampling.         Separate actors,
                         component failure.    peripheral                                                             bounded queues,
                                               failure from                                                           watchdogs
                                               corrupting the                                                         
                                               whole system.                                                          

  Memory budgeting       Tracks stack, heap,   Prevents field    CI fails if firmware exceeds a flash/RAM budget.     ESP-IDF size
                         static RAM, and       failures on                                                            tools, map files
                         flash.                constrained                                                            
                                               devices.                                                               

  Stack high-water       Measures remaining    Finds latent      Record high-water mark during worst-case packet      FreeRTOS APIs,
  monitoring             task stack headroom.  stack exhaustion. parsing.                                             diagnostics

  WCET analysis          Estimates worst-case  Helps meet        Bound the maximum processing time of an alarm event. Static analysis,
                         execution time.       real-time                                                              cycle counters,
                                               deadlines.                                                             measurements

  Priority inversion     Finds high-priority   Protects          Avoid long mutex holds in a high-priority path.      RTOS tracing,
  analysis               work blocked by       real-time                                                              lock analysis
                         lower-priority tasks. responsiveness.                                                        

  DMA and zero-copy      Moves data with fewer Can improve       DMA SPI buffers with explicit ownership.             ESP-IDF drivers;
  buffers                CPU copies.           throughput and                                                         benchmark first
                                               power use.                                                             

  Power-aware scheduling Adapts activity to    Extends battery   Batch telemetry and sleep between radio windows.     Current
                         energy constraints.   life.                                                                  analyzer, energy
                                                                                                                      profiling

  Store-and-forward      Persists messages     Supports          Buffer telemetry locally and upload after reconnect. Flash journal,
                         until a link is       disconnected                                                           bounded ring
                         available.            operation.                                                             buffer

  Time synchronization   Defines clock source, Makes distributed Distinguish monotonic time from UTC wall time.       SNTP, RTC, drift
                         drift, and timestamp  measurements                                                           measurements
                         meaning.              comparable.                                                            

  Protocol state machine Models link-level     Avoids ad hoc     `Idle -> TxPending -> AwaitAck -> Retry/Complete`.   HSM, model-based
                         exchange explicitly.  retries and                                                            tests
                                               ambiguous packet                                                       
                                               flows.                                                                 

  Sequence numbers and   Identify repeats and  Handles lost ACKs Duplicate `SetRelay(true)` is safely acknowledged    Protocol design,
  deduplication          order messages.       and               without reapplying a toggle.                         property tests
                                               retransmission.                                                        

  CRC and framing        Detects corruption    Protects          Length, version, sequence, payload, CRC.             Fuzz tests,
                         and defines packet    unreliable radio                                                       golden packet
                         boundaries.           links.                                                                 corpus

  OTA update safety      Makes firmware        Prevents bricked  Signed image, rollback slot, post-boot health        ESP-IDF OTA,
                         updates recoverable   or maliciously    confirmation.                                        secure boot,
                         and authentic.        modified devices.                                                      flash encryption

  Secure boot and key    Verifies firmware and Establishes       Per-device credentials and signed firmware.          ESP-IDF security
  management             protects secrets.     device trust.                                                          features,
                                                                                                                      provisioning
                                                                                                                      process

  Calibration and        Records sensor        Makes             Store calibration version with measurements.         Versioned
  provenance             calibration and data  measurements                                                           configuration,
                         origin.               interpretable.                                                         test fixtures
  ------------------------------------------------------------------------------------------------------------------------------------

## 7. Distributed Systems and Communication

  -------------------------------------------------------------------------------------------
  Concept             What it is         Why use it           Example         Tools /
                                                                              references
  ------------------- ------------------ -------------------- --------------- ---------------
  MQTT                Lightweight        Fits constrained     Device          Mosquitto,
                      pub/sub messaging  telemetry and device publishes       EMQX, HiveMQ
                      protocol.          communication.       measurements    
                                                              and receives    
                                                              configuration   
                                                              commands.       

  NATS                Lightweight        Useful for           Ingest service  NATS, JetStream
                      messaging system   service-to-service   emits device    
                      with request/reply events.              events to       
                      and streaming                           downstream      
                      options.                                processors.     

  gRPC / Protobuf     Typed RPC and      Useful for service   Go backend and  gRPC, Protobuf
                      compact            APIs and             Rust agent      
                      schema-based       cross-language       share generated 
                      serialization.     contracts.           message types.  

  REST / OpenAPI      HTTP resource APIs Broad ecosystem and  Mobile app      OpenAPI,
                      with               easy integration.    updates device  Swagger UI
                      machine-readable                        configuration   
                      contracts.                              through an API. 

  AsyncAPI            Describes          Documents            Define MQTT     AsyncAPI
                      asynchronous APIs  event-driven         topics and      
                      and message        contracts.           message         
                      channels.                               schemas.        

  Message broker      Routes messages    Decouples services   Telemetry       MQTT broker,
                      between producers  and smooths bursts.  ingestion       NATS, Kafka
                      and consumers.                          queues events   where justified
                                                              for processing. 

  At-least-once       Messages may be    Common reliability   Retry an upload Idempotency
  delivery            delivered more     trade-off.           and deduplicate keys, durable
                      than once.                              by message ID.  queues

  At-most-once        A message is       Lower overhead where Periodic        UDP,
  delivery            delivered zero or  loss is acceptable.  display refresh non-retained
                      one times.                              event.          events

  Exactly-once claims Attempts to avoid  Important to         Use idempotent  Broker docs,
                      duplicate effects  scrutinize because   writes and      database
                      across a defined   end-to-end           transactional   transactions
                      boundary.          exactly-once is      deduplication   
                                         difficult.           instead of      
                                                              assuming        
                                                              transport       
                                                              guarantees.     

  Eventual            Replicas converge  Fits many            Dashboard may   Version
  consistency         after updates      telemetry/status     briefly lag the numbers,
                      propagate.         views.               device's latest timestamps
                                                              measurement.    

  Reconciliation loop Compares desired   Supports             Server notices  Controllers,
                      and observed state self-healing.        a device has    idempotent
                      and corrects                            stale           commands
                      drift.                                  configuration   
                                                              and retries     
                                                              delivery.       

  Offline-first       Core workflows     Important for remote Cache settings  Local
  design              continue without   devices and mobile   and queue       persistence,
                      connectivity.      apps.                telemetry while sync engine
                                                              offline.        

  Store-and-forward   Buffers outbound   Avoids data loss     Flash-backed    Durable queue,
  delivery            data until         during outages.      telemetry queue retention
                      transport                               on gateway.     policy
                      recovers.                                               

  Timeouts, retries,  Bound waiting and  Prevents hung        End-to-end      Context
  deadlines           control repeated   requests and retry   command has a   propagation,
                      attempts.          storms.              deadline across retry budgets
                                                              service hops.   

  Retry budget        Caps retry work    Protects an          No more than a  Metrics, rate
                      over a time        overloaded system.   fixed fraction  limits
                      window.                                 of requests may 
                                                              be retries.     

  Rate limiting       Restricts request  Protects services    Limit           Token bucket,
                      rate.              and devices.         provisioning    leaky bucket
                                                              attempts or     
                                                              per-device      
                                                              commands.       

  Load shedding       Drops low-priority Preserves critical   Keep alarm      Priority
                      work under         functionality.       events; discard queues,
                      overload.                               stale dashboard admission
                                                              refreshes.      control

  Distributed tracing Follows one        Shows where latency  Trace device    OpenTelemetry
                      operation across   and failures occur.  upload through  
                      services.                               gateway, API,   
                                                              and database.   

  Correlation IDs     Link logs and      Helps diagnose       `command_id`    Trace context,
                      events for one     asynchronous flows.  appears in      structured logs
                      operation.                              device,         
                                                              gateway, and    
                                                              server logs.    

  Clock skew handling Accounts for       Prevents incorrect   Use sequence    Monotonic
                      imperfect clocks.  ordering             numbers for     clocks, NTP
                                         assumptions.         order;          
                                                              timestamps for  
                                                              approximate     
                                                              time.           

  Partition tolerance Defines behavior   Clarifies            Device          Failure-mode
  strategy            during network     availability vs      continues local documentation
                      separation.        consistency          alarms when     
                                         trade-offs.          cloud is        
                                                              unreachable.    

  Bulkheads and       Separate resource  Prevents one         Separate        Queues, worker
  isolation           pools and failure  workload from        command         pools
                      domains.           starving another.    processing from 
                                                              bulk telemetry  
                                                              ingestion.      

  Backward            New and old        Supports staged      Server accepts  Compatibility
  compatibility       components         rollout.             current and     tests
                      interoperate.                           previous device 
                                                              schema          
                                                              versions.       

  Device identity and Establishes        Prevents             Unique identity PKI, secure
  provisioning        per-device         unauthorized devices and rotated     elements where
                      identity and       from joining.        credentials per available
                      credentials.                            board.          
  -------------------------------------------------------------------------------------------

## 8. Observability: Logs, Metrics, Traces, Profiles, and Events

  ---------------------------------------------------------------------------------------------------------------------------------
  Concept         What it is          Why use it           Example                                       Tools
  --------------- ------------------- -------------------- --------------------------------------------- --------------------------
  Observability   Ability to infer    Makes failures       Determine why a slave stopped reporting       OpenTelemetry ecosystem
                  internal behavior   diagnosable in       without attaching a debugger.                 
                  from outputs.       environments you                                                   
                                      cannot inspect                                                     
                                      directly.                                                          

  Structured      Logs are            Supports search,     `event=radio_timeout device_id=17 retry=2`.   zap, slog, tracing, spdlog
  logging         machine-readable    aggregation, and                                                   
                  fields, not only    correlation.                                                       
                  free text.                                                                             

  Log levels and  Defines severity    Prevents noise and   Debug logs disabled by default on constrained `log` crates, slog, ESP
  policy          and retention.      protects storage.    firmware.                                     logging

  Metrics         Numeric             Good for rates,      Radio retries, queue depth, BLE connection    Prometheus, OpenTelemetry
                  measurements over   counts, and          count.                                        Metrics
                  time.               distributions.                                                     

  Counters        Monotonically       Quantifies           `radio_tx_retries_total`.                     Prometheus counters
                  increasing event    occurrences.                                                       
                  counts.                                                                                

  Gauges          Current values that Tracks current       `active_ble_connections`, `queue_depth`.      Prometheus gauges
                  rise and fall.      state.                                                             

  Histograms      Bucketed            Shows latency and    Radio round-trip time and command processing  Prometheus histograms,
                  distributions of    size distributions.  latency.                                      OTel
                  observations.                                                                          

  Percentiles /   Summarize tail      Averages can hide    p95/p99 command latency.                      Histogram quantiles,
  quantiles       latency.            slow outliers.                                                     dashboards

  RED method      Rate, Errors,       Compact service      Request rate, error ratio, latency.           Prometheus, Grafana
                  Duration for        health framework.                                                  
                  services.                                                                              

  USE method      Utilization,        Useful for CPU,      CPU use, queue saturation, radio errors.      Metrics dashboards
                  Saturation, Errors  queues, memory, and                                                
                  for resources.      hardware.                                                          

  Distributed     Span graph of a     Finds latency and    API request -\> queue -\> device command -\>  OpenTelemetry, Tempo,
  tracing         request across      causal paths.        acknowledgement.                              Jaeger
                  components.                                                                            

  Span events and Structured details  Adds context without `retry_number`, `device_id`,                  OpenTelemetry SDKs
  attributes      attached to spans.  huge logs.           `transport=cc1101`.                           

  Context         Carries             Links asynchronous   Put trace or command ID in a queued message.  W3C Trace Context, OTel
  propagation     trace/correlation   work.                                                              
                  identity across                                                                        
                  boundaries.                                                                            

  Continuous      Samples CPU,        Finds production     Identify a hot parser or excessive            Pyroscope, pprof, `perf`
  profiling       memory, allocation, performance          allocations.                                  
                  or lock behavior    regressions.                                                       
                  over time.                                                                             

  eBPF            Kernel-level        Helps inspect Linux  Diagnose packet drops or syscall latency on a Cilium, Beyla, bpftrace,
  observability   instrumentation     services and         Linux gateway.                                Tetragon
                  with low overhead   networking.                                                        
                  in suitable                                                                            
                  environments.                                                                          

  OpenTelemetry   Receives,           Centralizes          Batch, filter, enrich, and export             OTel Collector
  Collector       processes, and      telemetry pipelines. traces/metrics/logs.                          
                  exports telemetry.                                                                     

  Log aggregation Centralizes         Helps cross-service  Query all errors for one device ID.           Loki,
                  searchable logs.    investigations.                                                    Elasticsearch/OpenSearch

  Metrics storage Stores time-series  Supports alerts and  Fleet-wide retry-rate dashboard.              Prometheus, Mimir,
                  measurements.       historical analysis.                                               VictoriaMetrics

  Trace storage   Stores distributed  Enables latency      Inspect a slow provisioning request.          Tempo, Jaeger
                  spans.              investigations.                                                    

  Dashboards      Visualize health    Makes operational    Device fleet overview with stale-device and   Grafana
                  and trends.         state legible.       alarm panels.                                 

  Alerting        Notifies on         Reduces time to      Alert when no telemetry arrives for a         Alertmanager, Grafana
                  actionable          detect failures.     critical device.                              Alerting
                  conditions.                                                                            

  SLO / SLI /     Defines service     Makes reliability    99.9% successful command completion within a  Prometheus, SLO tooling
  error budget    goals and           measurable.          defined deadline.                             
                  acceptable failure                                                                     
                  budget.                                                                                

  Synthetic       Periodically        Detects failures     Test provisioning endpoint every few minutes. Blackbox Exporter,
  monitoring      exercises a         before users report                                                Playwright
                  critical workflow.  them.                                                              

  Heartbeats and  Track liveness and  Distinguishes alive  `last_measurement_age_seconds`, not just      Metrics, watchdog logic
  freshness       age of last useful  from useful.         connection status.                            
                  update.                                                                                

  Telemetry       Reduces volume      Controls cost and    Always retain errors; sample repetitive       OTel sampling
  sampling        while preserving    constrained-device   successful traces.                            
                  useful signals.     overhead.                                                          

  Cardinality     Limits unique label Prevents metrics     Use device class as a metric label; avoid     Prometheus guidance
  control         combinations.       storage explosion.   unbounded command IDs.                        

  Privacy-aware   Minimizes sensitive Reduces data         Never log credentials or raw provisioning     Redaction, allowlists
  telemetry       data in             exposure and         secrets.                                      
                  diagnostics.        retention risk.                                                    

  Embedded        Explicitly budgets  Observability itself Ring-buffer diagnostics and sampled counters  Compile-time levels, ring
  observability   CPU, RAM, flash,    can disrupt          on ESP32.                                     buffers
  budget          radio airtime, and  real-time behavior.                                                
                  bandwidth for                                                                          
                  diagnostics.                                                                           

  Tracy /         Captures detailed   Useful for           Trace radio transaction phases and queue      Tracy profiler; evaluate
  timeline        event timelines and understanding        waits.                                        target/transport overhead
  tracing         zones, often for    scheduling and                                                     
                  local performance   latency.                                                           
                  analysis.                                                                              

  Flight recorder Keeps a rolling     Preserves evidence   Retain the last N state transitions and radio Ring buffer, persistent
                  buffer of recent    before a             errors.                                       crash log
                  diagnostic events.  crash/reset.                                                       

  Crash dumps and Preserve state      Improves root-cause  Store reset reason, build ID, stack           Core dumps, symbol files
  postmortems     needed to           analysis.            watermark, last events.                       
                  investigate                                                                            
                  failures.                                                                              

  Semantic        Standard names for  Improves             Consistent service name, device ID, protocol, OpenTelemetry semantic
  conventions     common telemetry    cross-service        and error type.                               conventions
                  attributes.         consistency.                                                       
  ---------------------------------------------------------------------------------------------------------------------------------

### Example embedded metrics worth considering

  ---------------------------------------------------------------------------------
  Metric                          Type                    Meaning
  ------------------------------- ----------------------- -------------------------
  `radio_tx_total`                Counter                 Number of radio
                                                          transmissions attempted

  `radio_tx_retries_total`        Counter                 Number of retransmissions

  `radio_ack_timeout_total`       Counter                 Acknowledgement timeouts

  `radio_rtt_ms`                  Histogram               Radio
                                                          request/acknowledgement
                                                          latency

  `radio_rssi_dbm`                Gauge or histogram      Received signal strength,
                                                          depending on sampling
                                                          strategy

  `radio_rx_crc_errors_total`     Counter                 Packets rejected due to
                                                          CRC

  `queue_depth`                   Gauge                   Current queue occupancy

  `queue_overflow_total`          Counter                 Events rejected or
                                                          dropped due to capacity

  `state_transition_total`        Counter                 Number of state
                                                          transitions, optionally
                                                          by bounded transition
                                                          label

  `state_duration_ms`             Histogram               Time spent in selected
                                                          states

  `ble_connections_active`        Gauge                   Current BLE connection
                                                          count

  `ble_disconnects_total`         Counter                 Disconnection count

  `sensor_sample_age_seconds`     Gauge                   Age of the latest sample

  `telemetry_buffered_messages`   Gauge                   Number of messages
                                                          waiting for upload

  `watchdog_reset_total`          Counter after boot      Watchdog resets observed
                                                          across boots

  `firmware_build_info`           Info-style metric       Firmware version/build
                                                          identity
  ---------------------------------------------------------------------------------

**Embedded caution:** do not export every metric over the radio at high
frequency. Maintain cheap local counters, expose snapshots on demand,
and measure the overhead of tracing and logging.

## 9. Security and Supply-Chain Engineering

  -----------------------------------------------------------------------------------------
  Concept         What it is          Why use it         Example          Tools
  --------------- ------------------- ------------------ ---------------- -----------------
  Secure by       Security is part of Reduces systemic   Threat-model     Threat modeling,
  design          architecture, not a vulnerabilities.   provisioning,    security reviews
                  late patch.                            commands, OTA,   
                                                         and cloud APIs.  

  Least privilege Components receive  Limits blast       CI token can     IAM, scoped
                  only required       radius.            publish one      tokens
                  permissions.                           package but      
                                                         cannot           
                                                         administer the   
                                                         organization.    

  Secrets         Keeps credentials   Prevents           Inject           Vault, cloud
  management      out of source and   accidental         deployment       secret managers,
                  logs.               exposure.          credentials from CI secrets
                                                         a secret         
                                                         manager.         

  Encryption in   Protects network    Essential for      TLS for API and  TLS libraries,
  transit         traffic against     remote management. broker           certificate
                  interception.                          connections.     management

  Encryption at   Protects stored     Reduces exposure   Encrypt          Platform security
  rest            data under defined  from lost storage. sensitive local  features, OS
                  threat assumptions.                    configuration.   encryption

  Device identity Gives each device a Prevents           Per-device       PKI, secure
                  unique verifiable   shared-secret      certificate or   provisioning
                  identity.           fleet compromise.  unique           
                                                         credential.      

  Secure boot     Verifies firmware   Reduces risk of    Bootloader       ESP-IDF Secure
                  before execution.   unauthorized       validates signed Boot
                                      firmware.          firmware.        

  Firmware        Verifies updates    Reduces bricking   Signed OTA image ESP-IDF OTA,
  signing and     and recovers from   and supply-chain   plus known-good  signing pipeline
  rollback        bad releases.       risk.              fallback.        

  SBOM            Inventory of        Supports           Attach CycloneDX Syft, cdxgen
                  software components vulnerability      SBOM to every    
                  and versions.       response and       release.         
                                      compliance.                         

  Dependency      Checks dependencies Finds known        Scan Rust        OSV-Scanner,
  vulnerability   against known       vulnerable         crates, Go       Dependabot,
  scanning        advisories.         packages.          modules, and     Renovate, Trivy
                                                         C/C++            
                                                         dependencies.    

  Reproducible    Same inputs yield   Improves trust and Pin toolchains   Nix, containers,
  builds          equivalent          debugging.         and build in a   lockfiles
                  artifacts.                             controlled       
                                                         environment.     

  Artifact        Records how and     Helps verify       CI signs         SLSA,
  provenance /    where an artifact   release integrity. provenance for   Sigstore/cosign
  attestations    was built.                             firmware and     
                                                         containers.      

  Code signing    Verifies artifact   Prevents           Sign firmware    Sigstore, vendor
                  authenticity.       unauthorized       images and       signing workflows
                                      release artifacts. container        
                                                         images.          

  Fuzzing         Tests untrusted     Finds exploitable  Fuzz network     libFuzzer, AFL++,
  security        input handling      parser bugs.       packet and BLE   cargo-fuzz
  boundaries      aggressively.                          configuration    
                                                         parsers.         

  Secure coding   Defines             Improves           C++ Core         clang-tidy,
  standards       language-specific   consistency.       Guidelines, Rust Clippy, gosec
                  safe practices.                        unsafe policy,   
                                                         Go secure coding 
                                                         rules.           

  Unsafe-code     Limits use of       Makes Rust safety  Small unsafe     `unsafe` review,
  minimization    unsafe operations   reasoning          SPI/DMA adapter  Miri
                  to audited          tractable.         with safe public 
                  boundaries.                            API.             

  Static          Scans source for    Finds common flaws Detect           CodeQL, Semgrep,
  application     vulnerability       early.             injection,       gosec
  security        patterns.                              unsafe API       
  testing (SAST)                                         usage, and       
                                                         insecure         
                                                         defaults.        

  Dynamic         Tests a running     Finds              Test API         OWASP ZAP, Burp
  application     system from the     runtime-facing     authentication   Suite where
  security        outside.            security issues.   and input        authorized
  testing (DAST)                                         handling.        

  SBOM and        Tracks dependency   Avoids unreviewed  Check licenses   ScanCode, ORT
  license         licenses.           legal obligations. before adding a  
  compliance                                             library.         

  Vulnerability   Defines how reports Supports           Publish a        Security policy,
  disclosure      are received and    responsible        security contact issue templates
  process         handled.            remediation.       and response     
                                                         policy.          

  Data            Collect only        Reduces privacy    Keep only the    Retention
  minimization    necessary data and  and storage risks. telemetry        policies,
  and retention   delete it when no                      history required redaction
                  longer needed.                         by product       
                                                         needs.           
  -----------------------------------------------------------------------------------------

## 10. CI/CD, Build Reproducibility, and Release Engineering

  -------------------------------------------------------------------------------------------------
  Concept             What it is       Why use it           Example           Tools
  ------------------- ---------------- -------------------- ----------------- ---------------------
  Continuous          Automatically    Finds integration    Build C++, Rust,  GitHub Actions,
  Integration (CI)    builds and tests issues early.        and Go modules on GitLab CI
                      every change.                         each pull         
                                                            request.          

  Continuous Delivery Automates        Reduces manual       Publish signed    CI pipelines
  / Deployment        preparing or     release errors.      firmware and      
                      releasing                             container images  
                      artifacts.                            after gates pass. 

  Build matrix        Tests multiple   Detects portability  Host tests +      CI matrix
                      configurations   issues.              ESP32 build +     
                      and toolchains.                       Rust service + Go 
                                                            service.          

  Cross-compilation   Builds for a     Required for         Build RISC-V      ESP-IDF toolchain,
                      different target embedded and         firmware from a   Cargo targets
                      architecture.    multi-architecture   Linux host.       
                                       software.                              

  Hermetic builds     Build depends    Improves             Pinned compiler   Nix, Bazel,
                      only on declared reproducibility.     and dependency    containers
                      inputs.                               versions.         

  Nix / Dev           Define           Reduces "works on my Pin ESP-IDF,      Nix, Dev Containers
  Containers          repeatable       machine."            Rust, Go,         
                      development                           formatters, and   
                      environments.                         analysis tools.   

  Build caching       Reuses valid     Speeds up feedback.  Cache Cargo       sccache, ccache, CI
                      build outputs.                        artifacts or      caches
                                                            CMake build       
                                                            dependencies.     

  Compiler warnings   Treats selected  Prevents warning     Separate strict   GCC/Clang, Rust
  as errors           warnings as      debt.                host-test flags   lints, Go vet
                      failures.                             from vendor code  
                                                            if necessary.     

  Formatting gates    Enforces         Keeps review focused `clang-format`,   Formatters
                      consistent style on logic.            `rustfmt`,        
                      automatically.                        `gofmt`.          

  Lint gates          Runs static      Catches common       Clippy,           Language linters
                      checks           mistakes.            staticcheck,      
                      automatically.                        clang-tidy.       

  Release channels    Separates        Supports controlled  Internal test     Versioning, artifact
                      experimental,    rollout.             firmware before   repositories
                      beta, and stable                      field deployment. 
                      artifacts.                                              

  Feature flags       Enable/disable   Supports staged      Enable a new      Build flags, runtime
                      behavior without rollout and          retry policy for  flags
                      large code       experiments.         a test cohort.    
                      forks.                                                  

  Canary rollout      Releases to a    Limits impact of     Update a small    Deployment
                      small subset     defects.             device group      orchestration
                      first.                                before the fleet. 

  Rollback plan       Defines how to   Makes deployment     OTA fallback,     OTA slots, release
                      recover from a   failures survivable. previous          automation
                      bad release.                          container image,  
                                                            schema rollback   
                                                            strategy.         

  Database migration  Changes schemas  Avoids               Expand schema,    Migration tooling
  discipline          compatibly and   deployment-order     deploy compatible 
                      safely.          failures.            code, then remove 
                                                            old fields.       

  GitOps              Declarative      Provides auditable   Commit changes to Argo CD, Flux
                      desired          operations for       cluster           
                      deployment state suitable             manifests.        
                      stored in        infrastructure.                        
                      version control.                                        

  Infrastructure as   Defines          Makes environments   Provision         OpenTofu/Terraform,
  Code                infrastructure   repeatable.          telemetry backend Pulumi
                      as versioned                          resources.        
                      code.                                                   

  Containerization    Packages         Improves deployment  Package Go        Docker, Podman
                      services with    consistency.         ingestion service 
                      runtime                               as a container.   
                      dependencies.                                           

  SBOM and provenance Produces         Makes releases       Attach SBOM and   Syft, cosign, SLSA
  in CI               dependency       auditable.           provenance to     
                      inventory and                         each artifact.    
                      build                                                   
                      attestations.                                           

  Automated           Keeps            Reduces stale        Renovate opens    Renovate, Dependabot
  dependency updates  dependencies     dependency risk.     pinned-version    
                      current through                       updates with CI   
                      reviewable PRs.                       results.          

  Release manifest    Records exact    Supports field       Firmware build ID JSON/YAML manifest,
                      versions and     diagnosis.           maps to source    signed metadata
                      hashes of all                         commit and        
                      shipped parts.                        toolchain.        
  -------------------------------------------------------------------------------------------------

## 11. Performance, Capacity, and Efficiency

  ------------------------------------------------------------------------------------------------
  Method              What it measures      Why use it         Example            Tools
  ------------------- --------------------- ------------------ ------------------ ----------------
  Microbenchmarking   Small operation cost. Compares           Compare CRC        Google
                                            implementations of implementations.   Benchmark,
                                            a focused                             Criterion, Go
                                            operation.                            benchmarks

  Macrobenchmarking   End-to-end operation  Captures realistic Measure            Custom harness,
                      cost.                 system overhead.   sensor-to-cloud    k6
                                                               latency.           

  Profiling           Identifies where CPU  Avoids optimizing  Find parser or     `perf`, pprof,
                      time or allocations   guesses.           serialization hot  Tracy,
                      go.                                      paths.             Instruments

  Flame graphs        Visualize sampled     Makes hot paths    Identify expensive FlameGraph,
                      stack costs.          easy to spot.      JSON parsing.      pprof, Pyroscope

  Allocation          Measures allocation   Useful for latency Reduce per-packet  pprof,
  profiling           count and volume.     and                allocations in Go  heaptrack,
                                            memory-sensitive   or Rust.           allocator
                                            systems.                              metrics

  Cache-aware design  Considers memory      Can improve        Avoid              `perf`,
                      locality and cache    throughput on      pointer-heavy      cachegrind
                      behavior.             larger systems.    structures in a    
                                                               hot loop.          

  Memory profiling    Tracks heap, stack,   Finds growth and   Long-running       heaptrack,
                      and leaks.            exhaustion.        telemetry          Valgrind, pprof
                                                               ingestion test.    

  Latency budget      Allocates end-to-end  Prevents each      Radio response     Tracing,
                      latency across        component from     budget includes    histograms
                      stages.               spending the whole queue wait,        
                                            deadline.          transmission, and  
                                                               retry.             

  Tail-latency        Examines slowest      User-visible       Compare p50, p95,  Histograms,
  analysis            observations.         failures often     and p99 command    trace analysis
                                            hide in tails.     latency.           

  Capacity planning   Estimates resources   Prevents           Estimate           Load tests,
                      at target load.       predictable        messages/sec,      spreadsheets
                                            overload.          storage growth,    
                                                               and broker         
                                                               connections.       

  Energy profiling    Measures energy use   Critical for       Compare sleep      Power analyzer,
                      over time.            battery-powered    schedule and radio current shunt
                                            devices.           retry policy.      

  Binary-size         Measures              Embedded flash is  CI records         `idf.py size`,
  tracking            executable/firmware   limited.           firmware sections  `size`, map
                      growth.                                  and fails on       files
                                                               budget overrun.    

  Optimization gates  Require evidence      Prevents premature Keep a zero-copy   Benchmarks, ADR
                      before accepting      optimization.      parser only if     
                      complexity.                              benchmarks show    
                                                               benefit.           
  ------------------------------------------------------------------------------------------------

## 12. Reliability Engineering and Operations

  ---------------------------------------------------------------------------------------
  Concept             What it is       Why use it       Example          Tools /
                                                                         references
  ------------------- ---------------- ---------------- ---------------- ----------------
  SLO / SLI / SLA     Service          Makes            Define           Prometheus,
                      indicator,       reliability      successful       Grafana
                      target, and      expectations     command delivery 
                      formal           measurable.      within a         
                      agreement.                        deadline.        

  Error budget        Allowed          Balances feature Pause risky      SLO dashboards
                      unreliability    velocity and     changes when the 
                      before changing  reliability.     error budget is  
                      priorities.                       exhausted.       

  Failure Mode and    Enumerates       Prioritizes risk Radio stuck,     FMEA worksheet
  Effects Analysis    failures,        reduction.       sensor           
  (FMEA)              causes, effects,                  disconnected,    
                      and mitigations.                  corrupted        
                                                        configuration.   

  Fault tree analysis Models           Helps reason     Determine how an Fault trees,
                      combinations of  about            unsafe output    diagrams
                      failures leading system-level     could be         
                      to an outcome.   hazards.         activated.       

  Incident response   Defines          Reduces          Triage a         Runbooks,
                      detection,       confusion during fleet-wide       on-call process
                      ownership,       outages.         telemetry        
                      mitigation, and                   outage.          
                      communication.                                     

  Blameless           Examines         Turns incidents  Add an alert and Postmortem
  postmortem          systemic causes  into durable     regression test  template
                      without personal improvements.    after an outage. 
                      blame.                                             

  Runbooks            Step-by-step     Makes diagnosis  Recover a device Markdown, wiki
                      operational      repeatable.      stuck in         
                      procedures.                       provisioning.    

  Game days           Planned          Tests recovery   Simulate broker  Staging
                      exercises of     skills and       outage or failed environment
                      operational      assumptions.     OTA rollout.     
                      scenarios.                                         

  Disaster recovery   Tests restore    Verifies backups Restore          Backup/restore
  testing             and recovery     are usable.      telemetry        drills
                      procedures.                       storage and      
                                                        validate data    
                                                        integrity.       

  Graceful            Preserves        Keeps the system Local alarm      Capability
  degradation         critical         useful during    works while      tiers,
                      functions when   partial outages. cloud sync is    failure-mode
                      optional                          down.            design
                      dependencies                                       
                      fail.                                              

  Recovery time /     Defines          Makes recovery   Set acceptable   DR plans,
  recovery point      acceptable       goals explicit.  telemetry        metrics
  objectives          downtime and                      backlog and      
                      data loss.                        restoration      
                                                        time.            

  Run-to-completion   Keep event       Protects         An HSM handler   QP patterns,
  handlers            handlers bounded responsiveness   schedules a      timing tests
                      and              in event-driven  radio operation  
                      non-blocking.    systems.         instead of       
                                                        waiting          
                                                        synchronously.   

  Watchdogs and       Detect lack of   Supports         A subsystem      RTOS watchdogs,
  health checks       progress or      automatic        heartbeat        service probes
                      unhealthy        recovery and     expires if no    
                      components.      alerting.        progress occurs. 

  Operational         Checks whether a Catches missing  Require runbook, Release
  readiness review    system can be    dashboards,      SLO, and OTA     checklist
                      supported.       alerts, and      rollback before  
                                       rollback plans.  release.         
  ---------------------------------------------------------------------------------------

## 13. Programming-Language-Specific Practices

  --------------------------------------------------------------------------------------------------------------
  Language          Practice          Why use it          Example / tools
  ----------------- ----------------- ------------------- ------------------------------------------------------
  C++               C++ Core          Consistent safety   clang-tidy, guideline checks
                    Guidelines        and API design.     

  C++               RAII and value    Deterministic       `std::unique_ptr`, scope guards
                    semantics         resource management 
                                      and predictable     
                                      ownership.          

  C++               Concepts and      Make generic        `template <typename T> concept RadioTransport = ...`
                    constrained       requirements        
                    templates         explicit.           

  C++               `std::span` and   Express non-owning  Packet parser accepts a span rather than raw
                    views             buffer access       pointer/length pair.
                                      safely.             

  C++               `std::expected` / Explicit error      Return either parsed packet or a typed parse error.
                    result types      paths where         
                                      supported.          

  C++               Sanitizers and    Catch memory and    ASan, UBSan, TSan, clang-tidy
                    static analysis   concurrency         
                                      defects.            

  C++               Compile-time      Keep                CMake options choose host fake vs ESP-IDF adapter.
                    configuration     platform-specific   
                                      choices explicit.   

  C++               Avoid exceptions  Keeps failure       Document exceptions policy per target.
                    where unsupported semantics and       
                    by the target     binary/runtime      
                    policy            assumptions clear.  

  Rust              Ownership and     Prevents many       Pass slices and owned messages rather than shared
                    borrowing         memory-safety bugs. mutable globals.

  Rust              `Result` and      Makes failure and   Return `Result<Packet, ParseError>`.
                    `Option`          absence explicit.   

  Rust              Traits and        Express contracts   `DeviceId(u16)`, `Radio` trait.
                    newtypes          and prevent         
                                      unit/domain         
                                      confusion.          

  Rust              Clippy and        Static feedback and Run in CI with `cargo test`.
                    rustfmt           consistent style.   

  Rust              Property testing  Exercises parser    `proptest`, `cargo-fuzz`, `cargo-mutants`.
                    and fuzzing       and state-machine   
                                      edge cases.         

  Rust              Miri              Checks certain      Test a buffer abstraction with `cargo miri test`.
                                      unsafe-code errors. 

  Rust              `no_std`          Enables libraries   Keep domain/protocol crate independent of OS APIs.
                    boundaries        on constrained      
                                      targets.            

  Rust              Tokio and         Async runtime for   `tokio`, `JoinSet`, cancellation and deadlines.
                    structured        suitable            
                    concurrency       host/server         
                                      workloads.          

  Go                Small interfaces  Keeps dependencies  `type Radio interface { Send(...) error }`.
                                      easy to substitute. 

  Go                Context           Carries             `context.Context` on network and database operations.
                    propagation       cancellation and    
                                      deadlines.          

  Go                Error wrapping    Preserves error     `%w`, `errors.Is`, `errors.As`.
                                      identity and        
                                      context.            

  Go                Table-driven      Efficiently covers  `testing`, native fuzz tests.
                    tests and fuzzing many inputs.        

  Go                Race detector     Detects data races  `go test -race ./...`.
                                      in tested paths.    

  Go                `go vet` and      Finds common        Run in CI.
                    staticcheck       correctness and     
                                      maintainability     
                                      issues.             

  Go                pprof             Profiles CPU, heap, `net/http/pprof` in protected diagnostic environments.
                                      goroutines, and     
                                      blocking.           

  Go                Context-aware     Avoids goroutine    Parent context cancels workers on shutdown.
                    goroutine         leaks.              
                    lifecycle                             

  Cross-language    Shared protocol   Prevents C++, Rust, Protobuf/CBOR schema plus shared golden fixtures.
                    schema            and Go models       
                                      drifting.           

  Cross-language    Differential      Compares            All language decoders must produce the same canonical
                    tests             independent         result.
                                      implementations.    

  Cross-language    Common error      Makes diagnostics   Shared error codes mapped to language-specific error
                    taxonomy          comparable.         types.

  Cross-language    ABI and           Avoids assuming     Serialize fields explicitly; do not transmit raw C++
                    wire-format       in-memory layouts   structs.
                    discipline        are portable.       
  --------------------------------------------------------------------------------------------------------------

## 14. Developer Experience and Tooling

  ------------------------------------------------------------------------------------------------------
  Tool / approach   What it is          Why use it         Example
  ----------------- ------------------- ------------------ ---------------------------------------------
  Git               Version control.    Tracks changes and Small commits with explicit intent.
                                        supports review.   

  Pull-request      Peer review before  Catches design and Review checklist for API, tests, security,
  reviews           integration.        correctness        and observability.
                                        problems.          

  Conventional      Consistent commit   Helps automate     `feat(radio): add acknowledgement timeout`.
  Commits           message format.     changelogs and     
                                        releases.          

  ADR tooling       Stores and indexes  Preserves design   ADRs in `docs/decisions/`.
                    decisions.          rationale.         

  Mermaid /         Text-based          Keeps diagrams     State diagrams, sequence diagrams, C4 views.
  PlantUML          diagrams.           versioned with     
                                        code.              

  Doxygen / rustdoc Generates API       Makes interfaces   CI builds documentation and checks examples.
  / Go doc          documentation.      discoverable.      

  Dev Containers    Standardized        Lowers setup       Pin compiler, ESP-IDF tools, Rust, Go, and
                    editor/container    friction.          linters.
                    environment.                           

  Nix               Declarative         Supports           Flake for toolchain and host test
                    environment and     reproducibility.   dependencies.
                    package management.                    

  CMake presets     Named build         Makes host,        `host-debug`, `esp32-release`.
                    configurations.     embedded, debug,   
                                        and release builds 
                                        repeatable.        

  `clang-format` /  Language            Avoids style       Enforced in CI.
  `rustfmt` /       formatters.         debates.           
  `gofmt`                                                  

  `clang-tidy` /    Language-specific   Finds common       Run on changed code and periodically on the
  Clippy /          analysis.           defects.           full codebase.
  staticcheck                                              

  Renovate /        Dependency update   Makes updates      Group patch updates; run tests before
  Dependabot        automation.         visible and        merging.
                                        reviewable.        

  pre-commit hooks  Fast local quality  Gives feedback     Format and lint staged files.
                    checks.             before pushing.    

  Task runner       Standard commands   Avoids             `just test`, `just lint`, `just firmware`.
                    for common          undocumented       
                    workflows.          command sequences. 

  Local service     Reproducible        Lets contributors  Docker Compose for broker, API, database, and
  stack             dependencies for    run services       telemetry stack.
                    development.        locally.           

  AI coding         Tools for code      Can speed routine  Require tests, diff review, and security
  assistants        search, drafting,   work but may       review for generated changes.
                    and refactoring.    introduce          
                                        plausible errors.  

  Code search       Fast navigation     Helps understand   ripgrep, clangd, IDE indexing.
                    across a            dependencies and   
                    repository.         call paths.        

  Build and test    Makes CI outcomes   Reduces time to    Publish test results, coverage, binary size,
  reports           inspectable.        diagnose failures. and benchmark deltas.
  ------------------------------------------------------------------------------------------------------

## 15. Experimental and Emerging Techniques

These are learning candidates. Their value depends on the problem and
the maturity of the toolchain.

  --------------------------------------------------------------------------------------------------------
  Technique           What it is           Why experiment      Example                 Cautions
  ------------------- -------------------- ------------------- ----------------------- -------------------
  TLA+ / PlusCal      Formal modeling of   Finds protocol      Model radio             Model the smallest
                      concurrent and       deadlocks, races in retry/acknowledgement   critical protocol
                      distributed          logical behavior,   and configuration       first.
                      behavior.            and liveness        reconciliation.         
                                           failures.                                   

  Alloy               Relational modeling  Useful for data     Model device membership Bounded scope can
                      and bounded          relationships and   and unique IDs.         miss larger-scale
                      exploration.         access-control                              behavior.
                                           rules.                                      

  Property-based      Generates action     Finds rare          Randomly connect,       Keep failing seeds
  state-machine       sequences against a  lifecycle           disconnect, time out,   and minimize
  testing             model.               sequences.          and reset a device.     counterexamples.

  Differential        Implement a small    Exposes ambiguous   C++ firmware and Rust   Shared bugs in the
  implementation      protocol twice       specs and parser    host parser process the spec remain
                      independently.       differences.        same corpus.            possible.

  eBPF                Programmable         Deep visibility     Trace TCP latency or    Primarily Linux;
                      instrumentation in   into Linux gateway  process-level network   not a general ESP32
                      the Linux kernel.    behavior.           issues.                 technique.

  WebAssembly (Wasm)  Portable sandboxed   Can run portable    Execute user-defined    Often inappropriate
                      bytecode/runtime.    plugins or rules in alarm rules in a server for tiny MCU
                                           suitable host       sandbox.                firmware; measure
                                           environments.                               runtime cost.

  Component model /   Standardized         Explore portable,   Package a protocol      Ecosystem and
  WASI                component interfaces isolated            analysis plugin.        target constraints
                      and WASI             server-side                                 vary.
                      capabilities.        modules.                                    

  Rust `no_std`       Rust libraries       Reuse               Share packet parsing    Allocator, panic,
  libraries           without a standard   protocol/domain     between firmware and    and HAL choices
                      OS environment.      logic on            host tools.             need explicit
                                           microcontrollers.                           policy.

  Fuzzing in CI       Continuous fuzz      Finds new parser    Nightly fuzzing of      Set runtime budgets
                      campaigns and        bugs over time.     packet decoders.        and retain
                      regression corpus.                                               minimized
                                                                                       reproducers.

  Sanitizer-enabled   Run embedded logic   Finds defects       Compile domain and      Hardware-specific
  host simulation     on a desktop under   cheaply before      protocol code with      behavior still
                      sanitizers.          hardware testing.   ASan/UBSan.             needs HIL.

  Deterministic       Simulates time,      Makes race-like     Simulate dropped radio  Simulation model
  simulation          messages, and        event sequences     packets and timer       itself must be
                      failures under a     reproducible.       expiration.             validated.
                      controllable                                                     
                      scheduler.                                                       

  Digital twin /      Software model of    Enables backend and Simulated slaves emit   Avoid pretending
  device simulator    device state and     mobile development  realistic measurements  simulation is proof
                      protocol.            before hardware is  and faults.             of hardware
                                           ready.                                      behavior.

  OpenTelemetry on    Instrumentation      Explore             Export selected         Measure RAM, CPU,
  constrained targets adapted to tight     standardized        counters from a         packet volume, and
                      resource budgets.    telemetry without   gateway; buffer events  timing impact.
                                           uncontrolled        on firmware.            
                                           overhead.                                   

  Continuous          Persistent           Detects performance Track allocation hot    Control overhead
  profiling           low-overhead         changes in real     spots after a release.  and data retention.
                      sampling on host     workloads.                                  
                      services.                                                        

  eBPF-based security Runtime visibility   Detects suspicious  Monitor unexpected      Requires careful
  observability       and policy           process/network     outbound connections    policy tuning and
                      enforcement on       behavior.           from a gateway.         Linux support.
                      Linux.                                                           

  Supply-chain        Machine-verifiable   Improves artifact   Verify signed           Define trust roots
  attestations        evidence about build trust and           provenance before       and enforcement,
                      inputs and process.  auditability.       deployment.             not just metadata
                                                                                       generation.

  Reproducible        Independent builds   Improves release    Build the same commit   Toolchain
  firmware builds     yield matching       confidence and      in two clean            timestamps and
                      artifacts or         forensic analysis.  environments and        signing can affect
                      explainable                              compare hashes.         byte identity.
                      differences.                                                     

  AI-assisted test    Uses AI to propose   Can expand test     Generate malformed      Treat generated
  generation          edge cases and test  ideas quickly.      packet cases, then      tests as untrusted;
                      scaffolding.                             validate each assertion tests can encode
                                                               manually.               incorrect
                                                                                       assumptions.

  Mutation-guided     Uses surviving       Turns coverage into Strengthen tests for    Can be
  test improvement    mutants to identify  actionable test     retry boundary          computationally
                      weak tests.          work.               conditions.             expensive.

  Architecture        Automated checks for Keeps architecture  Fail CI when platform   Avoid brittle
  fitness functions   dependency and       intentional as the  headers leak into       checks that block
                      operational rules.   project grows.      domain modules.         legitimate changes.

  Progressive         Gradually exposes a  Limits blast        Deploy to lab devices,  Requires
  delivery            new version to       radius.             then a small field      version/health
                      users/devices.                           cohort, then the fleet. telemetry and
                                                                                       rollback
                                                                                       capability.
  --------------------------------------------------------------------------------------------------------

## 16. Suggested Reference Stack by System Area

  ---------------------------------------------------------------------------------------------------
  Area              Embedded C++ / ESP32 Rust host service     Go host service       Cross-language /
                                                                                     platform
  ----------------- -------------------- --------------------- --------------------- ----------------
  Build             ESP-IDF + CMake      Cargo + workspace     Go modules            CI build matrix

  Unit tests        GoogleTest/Catch2 on `cargo test`          `go test`             Shared fixtures
                    host; target tests                                               
                    as needed                                                        

  Static analysis   clang-tidy, compiler Clippy                `go vet`, staticcheck Semgrep/CodeQL
                    warnings                                                         where useful

  Memory / UB       ASan/UBSan/TSan on   Ownership checks,     Runtime checks, race  Fuzzing
                    host builds          Miri for selected     detector              
                                         unsafe code                                 

  State machines    QP/C++ HSM or        Enums + pure          Explicit state types  Shared
                    explicit transition  transition logic, or  and transition        behavioral
                    tables               a suitable state      functions             specification
                                         machine crate                               

  Protocol          Explicit             Shared schema/decoder Shared schema/decoder Protobuf, CBOR,
                    serialization, CRC,                                              JSON Schema
                    versioning                                                       

  Messaging         QP events, CC1101    Tokio/channels,       Channels, MQTT/NATS   MQTT, NATS, gRPC
                    protocol, BLE GATT   MQTT/NATS as needed   as needed             

  Observability     Counters, bounded    `tracing`,            `slog`/`slog`-style   OTel Collector,
                    logs, flight         OpenTelemetry         structured logs,      Prometheus,
                    recorder                                   OTel, pprof           Grafana, Loki,
                                                                                     Tempo

  Profiling         Tracy where          `cargo flamegraph`,   pprof                 Pyroscope
                    practical, cycle     `perf`, profiling                           
                    counters, ESP-IDF    tools                                       
                    metrics                                                          

  Security          Secure boot, signed  `cargo audit`,        govulncheck, gosec    SBOM, SLSA,
                    OTA, input fuzzing   `cargo deny`, fuzzing                       Sigstore

  Reproducibility   Pinned               `Cargo.lock`, pinned  `go.sum`, pinned      Nix/Dev
                    ESP-IDF/toolchain,   toolchain             toolchain             Containers
                    container/Nix                                                    

  HIL               ESP32 board +        Integration against   Integration against   Automated lab
                    serial +             device simulator or   device simulator or   harness
                    radio/sensor         hardware              hardware              
                    fixtures                                                         
  ---------------------------------------------------------------------------------------------------

## 17. A Practical Adoption Roadmap

  --------------------------------------------------------------------------
  Stage             Focus             Concrete             Exit criteria
                                      deliverables         
  ----------------- ----------------- -------------------- -----------------
  0\. Baseline      Understand the    Architecture         Another developer
                    current system    diagram, build       can build and run
                                      instructions, known  the project
                                      limitations,         
                                      baseline metrics     

  1\. Minimal       Boundaries and    One HSM, one second  State transitions
  architecture      state             component, explicit  and message flow
                                      messages,            have tests
                                      CMake/module         
                                      boundaries           

  2\. Portable core Separate policy   Domain types,        Core logic tests
                    from hardware     protocol codec,      run without ESP
                                      platform interfaces, hardware or
                                      host build           FreeRTOS

  3\. Test          Unit,             Test harness, golden Critical parsers
  foundations       integration,      fixtures, parser     and transitions
                    property, fuzz    fuzz target          are covered

  4\. Embedded      Timeouts,         Explicit failure     Fault-injection
  reliability       retries, queue    states, bounded      tests demonstrate
                    policy, watchdog  queues, recovery     expected recovery
                                      behavior             

  5\. Observability Logs, counters,   Structured           Field failure can
  baseline          snapshots         diagnostics, key     be investigated
                                      counters, build ID,  without a
                                      reset reason         debugger

  6\. Contracts     Versioned schemas Protocol schema,     C++, Rust, and Go
                    and compatibility compatibility        agree on shared
                                      policy, differential fixtures
                                      tests                

  7\. Host services API and ingestion API contract,        End-to-end path
                                      service tests, local works with
                                      Compose environment  simulated devices

  8\. CI quality    Reproducible      Build matrix,        Every change runs
  gates             checks            format, lint, tests, automated checks
                                      size budget, SBOM    

  9\. Resilience    Offline-first,    Store-and-forward,   Network loss and
                    idempotency,      duplicate handling,  recovery are
                    backpressure      retry budget         tested

  10\. Formal       Model checking    TLA+ model of one    Important
  experiments       and stateful      critical protocol;   safety/liveness
                    testing           stateful property    properties are
                                      tests                checked

  11\. Production   Metrics, traces,  OTel pipeline,       A simulated
  observability     dashboards, SLOs  dashboards,          incident can be
                                      actionable alerts    detected and
                                                           diagnosed

  12\. Supply-chain Signing and       Signed releases,     Release artifacts
  maturity          provenance        SBOM, dependency     are traceable and
                                      scanning, rollback   recoverable

  13\. Advanced     eBPF, continuous  Small isolated       Keep/drop
  experiments       profiling, Wasm,  prototypes with      decision recorded
                    AI-assisted tests measured results     in an ADR
  --------------------------------------------------------------------------

## 18. Experiment Template

Use this template for every "wild" or novel experiment.

``` markdown
# Experiment: <name>

## Hypothesis
What measurable improvement or new capability do I expect?

## Problem
What concrete limitation does this address?

## Scope
Which module, language, target, and environment are included?

## Baseline
What are the current measurements?

## Success criteria
- Metric:
- Target:
- Measurement method:

## Risks
- Functional:
- Security:
- Performance / memory:
- Maintenance:

## Implementation
What is the smallest experiment that can test the hypothesis?

## Tests
Which unit, property, fuzz, integration, HIL, or model-based tests are required?

## Results
What changed in measured terms?

## Decision
Keep / iterate / reject.

## Follow-up
What should be documented, generalized, or removed?
```

## 19. Architecture and Release Checklists

### Architecture review

-   [ ] Are module boundaries explicit?
-   [ ] Does domain logic avoid direct dependencies on RTOS, drivers,
    database, and transport APIs?
-   [ ] Is mutable state owned by one component where practical?
-   [ ] Are commands distinguished from events?
-   [ ] Are queues bounded and overflow policies defined?
-   [ ] Are timeouts, cancellation, retries, and idempotency specified?
-   [ ] Are wire formats versioned and serialized explicitly?
-   [ ] Are failure modes and recovery behavior documented?
-   [ ] Are dependencies justified by a concrete need?
-   [ ] Is there an ADR for decisions with meaningful long-term
    consequences?

### Test review

-   [ ] Are critical state transitions tested?
-   [ ] Are parser and protocol boundaries fuzzed?
-   [ ] Are property tests checking invariants?
-   [ ] Are cross-language golden fixtures shared?
-   [ ] Are failures injected for timeout, storage, network, and
    hardware paths?
-   [ ] Are race detectors and sanitizers used where supported?
-   [ ] Are long-run and HIL tests included for hardware-sensitive
    behavior?
-   [ ] Does every bug fix add a regression test where feasible?

### Observability review

-   [ ] Can a device/service be identified by build and version?
-   [ ] Are logs structured and free of secrets?
-   [ ] Are counters and latency distributions defined?
-   [ ] Are queue depth, dropped events, retries, and stale data
    visible?
-   [ ] Is there a bounded diagnostic buffer for recent events?
-   [ ] Are trace/metric labels bounded in cardinality?
-   [ ] Has instrumentation overhead been measured on constrained
    targets?
-   [ ] Are alerts actionable and linked to runbooks?

### Release review

-   [ ] Are builds reproducible enough for the target?
-   [ ] Are dependencies pinned and scanned?
-   [ ] Is an SBOM generated?
-   [ ] Are artifacts signed where appropriate?
-   [ ] Is firmware rollback or recovery tested?
-   [ ] Is the release linked to a source commit, toolchain, and build
    configuration?
-   [ ] Is staged rollout possible where the deployment model supports
    it?

## 20. Core Principles to Keep in Mind

1.  **Make invalid states difficult to represent.**
2.  **Keep policy independent from hardware and frameworks.**
3.  **Prefer explicit ownership over shared mutable global state.**
4.  **Bound every resource that can grow: queues, retries, buffers,
    logs, and telemetry.**
5.  **Treat timeouts, duplicate messages, and network loss as normal
    operating conditions.**
6.  **Test state transitions and failure paths---not only happy paths.**
7.  **Use fuzzing for untrusted parsers and property tests for
    invariants.**
8.  **Use formal methods selectively for critical protocols and
    concurrency behavior.**
9.  **Measure before optimizing; benchmark before adopting clever
    techniques.**
10. **Make observability part of the design, while budgeting its cost on
    embedded targets.**
11. **Share schemas and fixtures across C++, Rust, and Go rather than
    assuming memory layouts match.**
12. **Automate reproducibility, dependency checks, and release
    evidence.**
13. **Keep experiments reversible and document what was learned.**
14. **Do not adopt every pattern at once. A small, well-tested system is
    a better foundation for advanced experiments than a framework built
    before the problem exists.**

## 21. Selected Primary References

Prefer official documentation for language and tool behavior; verify
versions against the actual project toolchain.

-   C++ Core Guidelines: https://isocpp.github.io/CppCoreGuidelines/
-   CMake documentation: https://cmake.org/cmake/help/latest/
-   ESP-IDF Programming Guide:
    https://docs.espressif.com/projects/esp-idf/en/stable/esp32c3/
-   QP/C++ documentation: https://www.state-machine.com/qpcpp/
-   Rust Book: https://doc.rust-lang.org/book/
-   Rust standard library: https://doc.rust-lang.org/std/
-   The Rustonomicon (unsafe Rust): https://doc.rust-lang.org/nomicon/
-   Go documentation: https://go.dev/doc/
-   Go memory model: https://go.dev/ref/mem
-   OpenTelemetry documentation: https://opentelemetry.io/docs/
-   Prometheus documentation: https://prometheus.io/docs/
-   Grafana documentation: https://grafana.com/docs/
-   TLA+: https://lamport.azurewebsites.net/tla/tla.html
-   Alloy: https://alloytools.org/
-   OWASP Threat Modeling:
    https://owasp.org/www-community/Threat_Modeling
-   OWASP ASVS:
    https://owasp.org/www-project-application-security-verification-standard/
-   SLSA: https://slsa.dev/
-   Sigstore: https://www.sigstore.dev/
-   CycloneDX: https://cyclonedx.org/
-   Renovate: https://docs.renovatebot.com/
-   Nix: https://nixos.org/
-   Docker Compose: https://docs.docker.com/compose/
-   OpenTofu: https://opentofu.org/docs/

**Version note:** Tool capabilities and names evolve. Check official
documentation and compatibility with the project's pinned versions
before integrating a tool.
