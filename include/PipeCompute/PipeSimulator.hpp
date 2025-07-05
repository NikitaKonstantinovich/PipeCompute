#pragma once

#include "PipeCompute/ConfigParser.hpp"  // для ElementConfig
#include "PipeCompute/Pipe.hpp"          // для Segment, PipeSettings
#include "PipeCompute/Results.hpp"       // для PointResult

namespace PipeCompute {

    /** Состояние потока: давление (Па) и температура (K) */
    struct StreamState {
        double pressure;
        double temperature;
    };

    /** Собирает Segment из JSON-конфига */
    Segment makeSegment(const ElementConfig& e);

    /** Запускает Pipe и возвращает последнюю точку */
    PointResult runPipe(const Segment& seg, PipeSettings& settings);

    /** Печатает результат одного участка */
    void logPipeResult(double length, const PointResult& pr);

    /** Удобная обёртка для main: обновляет st и логирует */
    inline void simulatePipe(const ElementConfig& e,
        StreamState& st,
        PipeSettings& settings)
    {
        // 1) Построить сегмент
        Segment seg = makeSegment(e);

        // 2) Подготовить входные условия
        settings.initialPressure = st.pressure;
        settings.initialTemperature = st.temperature;

        // 3) Запустить Pipe и взять последнюю точку
        PointResult pr = runPipe(seg, settings);

        // 4) Обновить состояние потока
        st.pressure = pr.pressure;
        st.temperature = pr.temperature;

        // 5) Вывести результат
        logPipeResult(e.length, pr);
    }

} // namespace PipeCompute
