#pragma once

#include <esp_err.h>
#include <qpcpp.hpp>

namespace esp32::human_machine_interface::controls
{
    class Controls
    {
    public:
        esp_err_t start(QP::QActive &systemAO);

    private:
        static void taskEntry(void *arg);
        void run();
        void postSignal(QP::QSignal signal);

        QP::QActive *systemAO_{nullptr};
        bool started_{false};
    };
}
