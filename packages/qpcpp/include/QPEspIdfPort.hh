#pragma once
#include "qpcpp.hpp"

extern "C" {
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
}

/* ========================================================================== */
/* ESP-IDF requires spinlock (portMUX)                                        */
/* ========================================================================== */

static portMUX_TYPE qf_mux = portMUX_INITIALIZER_UNLOCKED;

/* critical sections */
#define QF_CRIT_STAT
#define QF_CRIT_ENTRY()    taskENTER_CRITICAL(&qf_mux)
#define QF_CRIT_EXIT()     taskEXIT_CRITICAL(&qf_mux)

/* ISR critical sections */
#define QF_CRIT_ENTRY_FROM_ISR()    taskENTER_CRITICAL_FROM_ISR(&qf_mux)
#define QF_CRIT_EXIT_FROM_ISR(x)    taskEXIT_CRITICAL_FROM_ISR(&qf_mux)

/* tick type */
#define QF_TICK_TYPE uint32_t
