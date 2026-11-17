# 3. App delegates startup to the orchestrator

- Status: Accepted
- Decision: `App` initializes QP and delegates construction/wiring/startup to `SystemOrchestrator`;
  it does not own application policy.
- Context: The composition root should remain thin while actors and adapters are composed in one
  place.
- Consequences: Startup failures are returned explicitly from orchestration. Platform-specific
  objects are supplied through statically bound ports.
