# Command and event catalog

The application core currently implements an in-memory CQRS boundary. Commands change the
application state and return zero or one domain event; queries read the current `View`.

| Name | Kind / direction | Payload | Version | Sender → receiver |
|---|---|---|---:|---|
| `BootCompleted` | Command | `ConfigurationStatus` (`loaded`, `missing`, `failed`) | 1 | `SystemAO` → application |
| `Rotate` | Command | `Direction` (`left`, `right`) | 1 | `SystemAO` → application |
| `Select` | Command | empty | 1 | `SystemAO` → application |
| `Confirm` | Command | empty | 1 | `SystemAO` → application |
| `Back` | Command | empty | 1 | `SystemAO` → application |
| `PageChanged` | Fact | previous and next `View` | 1 | application → caller / display refresh |
| `SelectionChanged` | Fact | page, previous index, next index | 1 | application → caller / display refresh |
| `GetCurrentView` | Query | empty | n/a | display → application |

Commands sent through `CommandEnvelope` carry a `CommandId` and `schemaVersion`. Unsupported
versions are rejected. The application remembers the most recent 16 non-zero IDs in fixed storage;
repeated IDs in that window are rejected. ID `0` is reserved for untracked, local dispatch. This is
process-local duplicate suppression, not durable idempotency across reboot or a persistent event
store.

Events carry the originating command ID and event schema version. `rebuildStatusView` replays the
current event types into a `StatusView`; no durable journal or event migration exists yet. Any
payload change must increment its schema version and add compatibility/replay tests before external
serialization is introduced.
