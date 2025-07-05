#include "PipeCompute/BendSimulator.hpp"
#include <iostream>

namespace PipeCompute {

    void simulateBend(const ElementConfig& e, StreamState& st, const PipeSettings& settings,
        std::shared_ptr<ThermoProperties> thermo) {
        // 1) Формируем «заглушечный» сегмент для диаметра и толщины
        Segment stub{ 0,0,0, 0,0,0, e.diameter, e.wallThickness };

        // 2) Собираем параметры колена
        BendParams bp;
        bp.segments = { stub };
        bp.bendAngle = e.bendAngle;
        bp.bendRadius = e.bendRadius;
        bp.pressure = st.pressure;
        bp.temperature = st.temperature;
        bp.massFlowRate = settings.massFlowRate;
        bp.step = settings.step;

        // 3) Прогоняем симуляцию
        Bend bend(bp, thermo);
        auto br = bend.simulate();

        // 4) Обновляем состояние потока
        st.pressure = br.points.back().pressure;

        // 5) Выводим результат
        std::cout << "[bend] angle=" << e.bendAngle
            << "  Δp=" << br.localLoss / 1e5 << " bar"
            << "  p=" << st.pressure / 1e5 << " bar\n";
    }
} 
