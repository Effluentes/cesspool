# Telemetry & Observability

## Cel

Ten moduł definiuje wspólne abstrakcje, formaty danych i mechanizmy integracji dla obserwowalności aplikacji. Ma umożliwiać diagnozowanie zachowania systemu bez uzależniania logiki domenowej od konkretnego dostawcy telemetrii.

Projekt obejmuje ESP32-C3 z ESP-IDF i QP/C++, urządzenia komunikujące się przez CC1101, backend w Ruście oraz aplikację Flutter. Architektura powinna uwzględniać zarówno ograniczenia mikrokontrolera, jak i możliwości środowiska serwerowego.

## Założenia projektowe

- **Modułowość:** sygnały obserwowalności są rozdzielone, ale współdzielą stabilne typy i kontekst.
- **Niski narzut:** instrumentacja nie może blokować krytycznych ścieżek ani powodować niekontrolowanych alokacji.
- **Ograniczone zasoby:** bufory i kolejki mają określoną pojemność oraz jawną politykę przepełnienia.
- **Niezależność od dostawcy:** kod domenowy korzysta z abstrakcji, a integracje z bibliotekami i backendami pozostają w adapterach.
- **Korelacja:** logi, zdarzenia i ślady powinny dawać się powiązać z tą samą operacją, gdy ma to sens.
- **Kontrolowana szczegółowość:** sampling, poziomy logowania i retencja ograniczają koszty.
- **Samodiagnostyka:** system mierzy także utratę i opóźnienia własnej telemetrii.
- **Prywatność:** nie należy bez potrzeby umieszczać danych osobowych, sekretów ani dużych ładunków w logach, etykietach metryk czy baggage.

## Taksonomia danych

Poniższe kategorie tworzą szerszą taksonomię diagnostyczną. Nie są wszystkie równorzędnymi sygnałami jednego standardu. W terminologii OpenTelemetry podstawowe sygnały obejmują metrics, logs i traces; profiles są rozwijanym sygnałem, baggage służy do propagacji kontekstu, a events można reprezentować jako logi.

| Kategoria | Co opisuje | Przykłady |
|---|---|---|
| **Logs** | Zapis konkretnego zdarzenia wraz z czasem i polami kontekstowymi | `radio.ack_timeout`, `config.accepted` |
| **Metrics** | Agregaty liczbowe opisujące zachowanie i trendy | liczba retransmisji, czas odpowiedzi, zajętość kolejki |
| **Traces** | Przebieg pojedynczej operacji przez komponenty | BLE → `SystemAO` → `RadioAO` → slave → backend |
| **Profiles** | Próbki pokazujące wykorzystanie zasobów przez kod | CPU time, stosy wywołań, alokacje pamięci |
| **Events** | Znaczące zdarzenia domenowe lub systemowe | `SLAVE_CONNECTED`, `SYSTEM_DEGRADED` |
| **Context / correlation IDs** | Identyfikatory wiążące dane z jedną operacją | `trace_id`, `span_id`, `operation_id` |
| **Baggage** | Niewielkie metadane propagowane wraz z kontekstem | typ operacji, identyfikator urządzenia, jeśli bezpieczny i potrzebny |
| **Exceptions / errors** | Informacje o błędach i ich klasyfikacji | timeout, błąd walidacji, nieobsłużony wyjątek |
| **Crash reports / dumps** | Dane zebrane przy awarii lub resecie | przyczyna resetu, backtrace, zrzut pamięci, jeśli dostępny |
| **Audit logs** | Ślad działań wymagających rozliczalności | zmiana konfiguracji, aktualizacja uprawnień |
| **Diagnostic snapshots** | Stan systemu uchwycony w danym momencie | stan HSM, pamięć, głębokość kolejek |
| **Flight recorder** | Ograniczony zapis ostatnich zdarzeń przed awarią | ostatnie przejścia stanów i błędy przed resetem |
| **Resource telemetry** | Wykorzystanie zasobów i presję na zasoby | heap, stosy, CPU, kolejki, task watchdog |
| **Network / protocol telemetry** | Stan i jakość komunikacji | RSSI, ACK, retransmisje, utracone ramki |
| **Health / readiness** | Czy komponent działa i jest gotowy do pracy | `RadioAO=ready`, backend niedostępny |
| **Exemplars** | Odniesienia z wybranych próbek metryk do śladów | próbka opóźnienia wskazująca trace |
| **Sampling metadata** | Informacja o zasadach wyboru danych | czy trace został zachowany, czy odrzucony |

`Resource telemetry` i `Network / protocol telemetry` są przede wszystkim kategoriami danych, a nie wymaganymi osobnymi modułami implementacyjnymi. Podobnie flight recorder może być mechanizmem opartym na buforze zdarzeń i logów, a nie niezależnym formatem sygnału.

## Proponowana struktura katalogów

```text
telemetry/
├── README.md
├── api/                  # stabilne publiczne interfejsy
├── core/                 # wspólne typy, zegar, identyfikatory, zasoby
├── logs/                 # API i formatowanie logów
├── metrics/              # counters, gauges, histograms
├── traces/               # spans i relacje między operacjami
├── profiles/             # integracja z profilowaniem, gdy dostępna
├── events/                # typowane zdarzenia diagnostyczne/domenowe
├── context/               # kontekst operacji i correlation IDs
├── sampling/              # polityki próbkowania
├── buffering/             # ograniczone bufory i polityka przepełnienia
├── exporters/             # konsola, pamięć, storage, sieć
├── exceptions/            # klasyfikacja i raportowanie błędów
├── crash_reports/         # integracja z panic/crash diagnostics
├── audit/                 # audyt zmian i operacji
├── diagnostics/           # snapshots i flight recorder
├── health/                # health/readiness i diagnostyka komponentów
└── CMakeLists.txt         # jeśli katalog jest modułem CMake
```

Struktura jest propozycją, nie wymogiem tworzenia wszystkich modułów od razu. Zacznij od minimalnego zestawu i dodawaj katalogi, gdy pojawi się konkretna potrzeba.

## Znaczenie poszczególnych modułów

### `api/`

Publiczne kontrakty używane przez resztę systemu, np. logger, rejestrator metryk, tracer i kontekst. Interfejsy powinny być małe i nie ujawniać typów ESP-IDF, QP, Rust ani konkretnego dostawcy telemetrii.

### `core/`

Wspólne prymitywy: typy identyfikatorów, zegar monotoniczny, identyfikacja komponentu/urządzenia, wspólny model rekordu i wersja schematu. Czas monotoniczny służy do pomiarów czasu trwania; czas ścienny może służyć do korelacji między urządzeniami, jeśli jest wiarygodnie zsynchronizowany.

### `logs/`

Ustrukturyzowane zapisy z poziomem ważności, nazwą zdarzenia i polami. Na MCU unikaj kosztownego formatowania w ścieżkach czasu rzeczywistego. Rozważ rejestrowanie kodów zdarzeń i argumentów, a formatowanie wykonuj później.

### `metrics/`

- **Counter:** wartość monotonicznie rosnąca, np. liczba timeoutów.
- **Gauge:** wartość chwilowa, np. wolny heap lub głębokość kolejki.
- **Histogram:** rozkład pomiarów, np. czas odpowiedzi radiowej.

Używaj ograniczonej liczby etykiet. Nie umieszczaj `trace_id`, numerów pakietów ani nieograniczonej liczby identyfikatorów urządzeń jako etykiet metryk o wysokiej kardynalności.

### `traces/`

Spany opisujące operacje i ich relacje. Ślady są szczególnie przydatne do operacji przechodzących przez wiele komponentów. Nie musisz tworzyć pełnego trace dla każdego odczytu czujnika; określ politykę próbkowania.

### `profiles/`

Integracja z profilowaniem CPU, stosów wywołań i alokacji. Możliwości będą inne na serwerze Rust i na ESP32-C3. Profilowanie na MCU powinno być opcjonalne i dobrane do dostępnych narzędzi oraz budżetu CPU/pamięci.

### `events/`

Typowane zdarzenia o znaczeniu dla systemu, np. zmiana stanu HSM, utrata łączności ze slave'em czy przejście do trybu zdegradowanego. Zdarzenia mogą być emitowane do logów, śladów lub lokalnego dziennika; nie muszą stanowić oddzielnego backendu.

### `context/`

Jawny kontekst operacji z identyfikatorami korelacyjnymi. W systemie embedded unikaj ukrytego globalnego kontekstu, jeśli współbieżne Active Objects mogłyby go nadpisywać. Przekazuj kontekst w zdarzeniu lub komunikacie, gdy jest potrzebny.

### `baggage`

Metadane propagowane wzdłuż operacji. Stosuj oszczędnie: nie przenoś sekretów, danych osobowych ani dużych struktur. Dla komunikacji radiowej lepszy może być mały, wersjonowany nagłówek protokołu niż przenoszenie całego kontekstu OpenTelemetry.

### `sampling/`

Reguły wyboru śladów i zdarzeń do zachowania. Na urządzeniu wybieraj proste, deterministyczne reguły i ogranicz liczbę rekordów; tail-based sampling zwykle lepiej realizować na backendzie lub collectorze, który widzi zakończony ślad.

### `buffering/`

Ograniczone bufory/queues oraz jawna polityka przepełnienia. Określ, które dane można odrzucać, agregować albo nadpisywać. Telemetria nie może blokować krytycznej ścieżki komunikacji.

### `exporters/`

Adaptery wysyłające dane do konsoli, pamięci, trwałego storage lub sieci. Logika domenowa nie powinna zależeć od transportu ani backendu telemetrii.

### `exceptions/` i `crash_reports/`

Rozdziel błędy operacyjne od awarii procesu/urządzenia. Nie zakładaj, że po panicu zawsze będzie możliwe zapisanie pełnego backtrace; zależy to od konfiguracji, rodzaju awarii i dostępnej pamięci.

### `audit/`

Dziennik zmian konfiguracji i działań wymagających rozliczalności. Audyt powinien mieć określoną retencję i ochronę przed nieautoryzowaną zmianą; nie jest zamiennikiem zwykłych logów.

### `diagnostics/`

Snapshots i flight recorder. Snapshot pokazuje stan w danym momencie, a flight recorder utrzymuje ograniczony bufor ostatnich zdarzeń, który można zachować po wykryciu problemu lub przed kontrolowanym restartem.

### `health/`

Stan komponentu, np. `starting`, `ready`, `degraded`, `failed`, oraz przyczyna braku gotowości. Health status powinien być oparty na rzeczywistych warunkach działania, a nie tylko na tym, że zadanie nadal istnieje.

## Zasady implementacyjne dla ESP32-C3 i QP/C++

1. **Bez blokowania:** `RadioAO` nie może czekać na eksport logu przez Wi-Fi. W miarę możliwości rekord trafia do ograniczonego bufora.
2. **Jawne przepełnienia:** licznik utraconych rekordów, przepełnień bufora i błędów eksportu jest częścią diagnostyki.
3. **ISR-safe:** w przerwaniach nie wykonuj blokującego I/O, nie alokuj dynamicznie i nie formatuj ciężkich komunikatów.
4. **Oddziel priorytety:** zwykłe logi można próbkować lub odrzucać; zdarzenia krytyczne powinny mieć inną politykę, z uwzględnieniem ograniczeń pamięci.
5. **Mierz czas właściwym zegarem:** czas monotoniczny do opóźnień; czas ścienny do korelacji, jeśli synchronizacja jest dostępna.
6. **Nie mieszaj metryk z identyfikatorami:** metryki powinny mieć niską kardynalność; identyfikatory konkretnych operacji umieszczaj w logach i śladach.
7. **Wersjonuj formaty:** rekordy przechowywane trwale lub wysyłane między urządzeniami powinny mieć wersję schematu.
8. **Nie zakładaj pełnego OTel na MCU:** wspólny model danych może być zgodny z konwencjami OpenTelemetry, ale implementacja na ESP32 może być znacznie lżejsza.
9. **Obserwuj telemetrię:** monitoruj opóźnienie eksportu, liczbę odrzuconych rekordów, wykorzystanie buforów i awarie samego pipeline'u.
10. **Dbaj o prywatność:** filtruj sekrety i dane wrażliwe, szczególnie w logach, baggage i raportach awarii.

## Przykładowe metryki dla systemu radiowego

| Nazwa | Typ | Znaczenie |
|---|---|---|
| `radio_tx_total` | Counter | Łączna liczba transmisji |
| `radio_tx_retries_total` | Counter | Liczba retransmisji |
| `radio_ack_timeout_total` | Counter | Liczba timeoutów ACK |
| `radio_ack_duration_ms` | Histogram | Rozkład czasu oczekiwania na ACK |
| `radio_rssi_dbm` | Gauge | Ostatni pomiar RSSI |
| `runtime_queue_depth` | Gauge | Aktualna głębokość kolejki |
| `runtime_queue_overflow_total` | Counter | Liczba przepełnień kolejek |
| `telemetry_dropped_records_total` | Counter | Liczba odrzuconych rekordów telemetrii |
| `sensor_sample_age_ms` | Gauge | Wiek najstarszej/aktualnej próbki według zdefiniowanej semantyki |
| `system_state_changes_total` | Counter | Liczba zmian stanów systemu |

Wybierz nazwy i jednostki konsekwentnie; przed wystawieniem metryk do Prometheusa lub OpenTelemetry ustal też konwencję nazewnictwa dla całego systemu.

## Proponowana kolejność wdrażania

1. `api/`, `core/`, `logs/` i `metrics/`.
2. `buffering/` oraz prosty `exporters/console`.
3. `context/` i `traces/` dla operacji przekraczających granice komponentów.
4. `health/`, `diagnostics/` i `crash_reports/`.
5. `sampling/`, `audit/` oraz `profiles/` według rzeczywistych potrzeb.
6. Integracja backendu z OpenTelemetry Collector i narzędziami do wizualizacji.

## Pojęcia powiązane, ale niebędące osobnymi sygnałami

- **Context propagation:** przenoszenie kontekstu między komponentami.
- **Correlation IDs:** wiązanie danych z tą samą operacją.
- **Sampling:** wybór danych do zachowania.
- **Exemplars:** powiązanie próbki metryki ze śladem.
- **Telemetry pipeline:** zbieranie, przetwarzanie i eksport danych.
- **Semantic conventions:** ujednolicone nazwy atrybutów.
- **SLO / error budget:** cele niezawodności i budżet błędów; korzystają z telemetrii, ale nie są sygnałami.
- **Fault injection:** celowe wywoływanie awarii w celu sprawdzenia odporności; to technika testowa, a nie rodzaj danych.

## Źródła i standardy

- [OpenTelemetry — Signals](https://opentelemetry.io/docs/concepts/signals/)
- [OpenTelemetry — Context propagation](https://opentelemetry.io/docs/concepts/context-propagation/)
- [OpenTelemetry — Baggage](https://opentelemetry.io/docs/concepts/signals/baggage/)
- [OpenTelemetry — Semantic conventions](https://opentelemetry.io/docs/concepts/semantic-conventions/)
- [ESP-IDF Programming Guide](https://docs.espressif.com/projects/esp-idf/en/stable/esp32c3/)
- [ESP-IDF — Heap memory debugging](https://docs.espressif.com/projects/esp-idf/en/stable/esp32c3/api-reference/system/heap_debug.html)
