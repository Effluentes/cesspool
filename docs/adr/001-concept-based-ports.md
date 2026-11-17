# 1. Platform ports use concepts

- Status: Accepted
- Decision: Application-facing ports are C++ concepts with statically bound function-pointer
  adapters; platform implementations live under `platforms/`.
- Context: The firmware targets constrained systems and should avoid virtual dispatch and heap-backed
  callable wrappers at the application boundary.
- Consequences: Compile-time checking is strong and runtime allocation is avoidable. Adapters must
  be small, explicit, and tested at their platform boundary.
