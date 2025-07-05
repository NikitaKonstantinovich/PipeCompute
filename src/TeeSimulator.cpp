#include "PipeCompute/TeeSimulator.hpp"
#include <iostream>

namespace PipeCompute {

    TeeResult simulateTee(const ElementConfig& e, StreamState& st, const PipeSettings& settings,
        std::shared_ptr<ThermoProperties> thermo) {

        // 1) Настраиваем параметры тройника
        Segment trunkStub{ 0,0,0, 0,0,0, e.mainBranchDiameter, e.wallThickness };
        Segment sideStub{ 0,0,0, 0,0,0, e.sideBranchDiameter, e.wallThickness };

        TeeParams tp;
        tp.segments = { trunkStub, sideStub };
        tp.sideBranchDiameter = e.sideBranchDiameter;
        tp.pressure = st.pressure;
        tp.temperature = st.temperature;
        tp.massFlowRate = settings.massFlowRate;
        tp.step = settings.step;

        // 2) Прогоняем Tee
        auto tr = Tee(tp, thermo).simulate();

        // 3) Собираем результат:
        //    - главная ветвь: первая точка tr.points[0]
        //    - побочная: используем tr.branchFlowRate и tr.points[1] (если есть)
        StreamState mainSt{ tr.points[0].pressure, tr.points[0].temperature };
        StreamState sideSt{ tr.points[1].pressure, tr.points[1].temperature };

        double sideFlow = tr.branchFlowRate;      // расход в побочной
        double dP = tr.totalPressureDrop;   // падение в Tee

        // 4) Логируем в консоль
        std::cout << "[tee] main→ D=" << e.mainBranchDiameter
            << "  p_main=" << mainSt.pressure / 1e5 << " bar"
            << "  T_main=" << mainSt.temperature - 273.15 << " °C\n";
        std::cout << "[tee] side→ D=" << e.sideBranchDiameter
            << "  p_side=" << sideSt.pressure / 1e5 << " bar"
            << "  T_side=" << sideSt.temperature - 273.15 << " °C"
            << "  m_side=" << sideFlow << " kg/s\n";

        // 5) Обновляем главное состояние, побочная «замораживается»
        st = mainSt;

        return { mainSt, sideSt, sideFlow, dP };
    }
}