#pragma once
#include "qpcpp.hpp"


enum Signals : QP::QSignal {
    RADIO_INIT_SIG = QP::Q_USER_SIG,
    RADIO_DATA_RX_SIG,
    RADIO_SEND_SIG,
    RADIO_TX_TIMEOUT_SIG,
    SENSOR_DATA_SIG,       // publikowany dalej na bus
    // ...
};

struct RadioDataEvt : QP::QEvt {
    uint8_t slaveId;
    uint8_t sensorType;
    float   value;
};

struct RadioSendEvt : QP::QEvt {
    uint8_t slaveId;
    uint8_t command;
    uint8_t payload[8];
    uint8_t len;
};

struct SensorDataEvt : QP::QEvt {
    uint8_t slaveId;
    uint8_t sensorType;
    float   value;
};
