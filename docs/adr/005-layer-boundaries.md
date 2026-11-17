# 5. Packages, platforms, and runtime have separate responsibilities

- Status: Accepted
- Decision: `packages/` contains platform-neutral types and contracts, `platforms/` contains
  hardware adapters, and `runtime/` contains actors, application behavior, and composition.
- Context: Sharing domain behavior across hardware targets should not import board-specific code
  into packages.
- Consequences: Dependency direction points inward from adapters and runtime to neutral contracts.
  New hardware code belongs in `platforms/`, never in `packages/`.
