#pragma once

#include "PipeCompute/PipeSimulator.hpp"      // дл€ StreamState
#include "PipeCompute/Params.hpp"             // дл€ ElementConfig
#include "PipeCompute/Tee.hpp"                // дл€ TeeParams и Tee
#include <memory>
#include <vector>

namespace PipeCompute {
    /** –езультат симул€ции тройника и его ветвей */
    struct TeeResult {
        StreamState main;  // давление и температура в магистрали после Tee
        StreamState side;  // давление и температура в ответвлении сразу после Tee
        double sideFlow;  // массовый расход в ответвлении
        double dP; // падение давлени€ в Tee
    };

    /**
     * @brief —имул€ци€ тройника (Tee) и последующих двух ветвей
     * @param e         ќписание элемента из конфига (diameter, branchDiameter, wallThickness)
     * @param st        —осто€ние потока (давление и температура) Ц обновл€етс€
     * @param settings  ќбщие настройки дл€ трубы (massFlowRate, step и др.)
     * @param thermo    ћодель термодинамики
     */
    TeeResult simulateTee(const ElementConfig& e, StreamState& st,const PipeSettings& settings,
        std::shared_ptr<ThermoProperties> thermo);
} 
