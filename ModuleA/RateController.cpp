#include "Producer.h"

RateController::RateController(QObject *parent) : QObject(parent) { }

void RateController::run()
{
    std::cout << "RateController::run()" << std::endl;
    using Timestep = READ_RATE;
    std::chrono::duration<double> timestamp;
    m_startTime = hrclock::now();
    hrclock::time_point next = m_startTime + Timestep{1};
    while(true) {
        timestamp = hrclock::now() - m_startTime;
        Producer::instance()->readPacket(timestamp.count());
        while (hrclock::now() < next)
            ;
        next += Timestep{1};
    }
}
