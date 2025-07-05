#pragma once

#include "PipeCompute/PipeSimulator.hpp"  // для StreamState
#include "PipeCompute/Params.hpp"         // для ElementConfig
#include "PipeCompute/Bend.hpp"           // для BendParams и Bend
#include <memory>

namespace PipeCompute {

    /**
     * @brief Симуляция колена (Bend)
     * @param e         Описание элемента из конфига (bendAngle, bendRadius, diameter, wallThickness)
     * @param st        Текущее состояние потока (давление и температура): обновляется
     * @param settings  Общие настройки (massFlowRate, step)
     * @param thermo    Модель термодинамики
     */
    void simulateBend(const ElementConfig& e, StreamState& st, const PipeSettings& settings, 
        std::shared_ptr<ThermoProperties> thermo);
} 
