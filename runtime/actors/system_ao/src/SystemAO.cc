#include "runtime/actors/system_ao/SystemAO.hh"

namespace runtime::actors
{
    // Konstruktor korzysta z poprawnej klasy bazowej QP::QActive
    SystemAO::SystemAO() noexcept
        : QP::QActive(Q_STATE_CAST(&SystemAO::initial))
    {
        // Implementacja inicjalizacji aktora
    }
}