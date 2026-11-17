# 2. Commands and facts are distinct

- Status: Accepted
- Decision: Requests are commands; successful state changes are returned as domain facts. Commands
  and facts have schema versions, and commands may carry an ID.
- Context: UI input, application decisions, and display refresh should not be conflated.
- Consequences: Commands can be rejected without publishing a fact. The current implementation
  suppresses duplicate non-zero IDs only within a fixed 16-ID in-memory window; durable idempotency
  is deferred until a persistent command source exists.
