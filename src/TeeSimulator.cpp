#include "PipeCompute/TeeSimulator.hpp"
#include <iostream>

namespace PipeCompute {

    void simulateTee(const ElementConfig& e, StreamState& st, const PipeSettings& settings,
        std::shared_ptr<ThermoProperties> thermo) {

        // 1) Настраиваем параметры тройника
        Segment trunk{ 0,0,0, 0,0,0, e.diameter, e.wallThickness };
        TeeParams tp;
        tp.segments = { trunk };
        tp.branchDiameter = e.branchDiameter;
        tp.pressure = st.pressure;
        tp.temperature = st.temperature;
        tp.massFlowRate = settings.massFlowRate;
        tp.step = settings.step;

        // 2) Прогоняем Tee
        Tee tee(tp, thermo);
        auto tr = tee.simulate();

        // 3) Обновляем главное давление
        st.pressure = tr.points[0].pressure;

        std::cout << "[tee] branchD=" << e.branchDiameter
            << "  Δp=" << tr.totalPressureDrop / 1e5 << " bar"
            << "  m_branch=" << tr.branchFlowRate << " kg/s"
            << "  p_main=" << st.pressure / 1e5 << " bar\n";

        // 4) Симуляция двух ветвей по 2 м каждая
        for (int i = 0; i < 2; ++i) {
            Segment branch{ 0,0,0, 0,0,0,
                           (i == 0 ? e.diameter : e.branchDiameter),
                           e.wallThickness };
            branch.x1 = 2.0;

            PipeSettings bs = settings;
            bs.initialPressure = st.pressure;
            bs.initialTemperature = st.temperature;
            Pipe branchPipe({ branch }, bs);
            auto res = branchPipe.simulate();

            std::cout << "[branch" << (i + 1) << "] length=2"
                << "  p=" << res.back().pressure / 1e5 << " bar\n";
        }
    }
}
