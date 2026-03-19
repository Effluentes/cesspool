#include "../include/QPEspIdfPort.hh"

namespace QP {

void QF::init(void) {
}

int_t QF::run(void) {
    /* ESP-IDF już ma scheduler uruchomiony */
    return 0;
}

void QF::stop(void) {
    taskDISABLE_INTERRUPTS();
    for (;;) {}
}

}