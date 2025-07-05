#pragma once

#include "PipeCompute/PipeSimulator.hpp"      // для StreamState
#include "PipeCompute/Params.hpp"             // для ElementConfig
#include "PipeCompute/Tee.hpp"                // для TeeParams и Tee
#include <memory>

namespace PipeCompute {

    /**
     * @brief Симуляция тройника (Tee) и последующих двух ветвей
     * @param e         Описание элемента из конфига (diameter, branchDiameter, wallThickness)
     * @param st        Состояние потока (давление и температура) – обновляется
     * @param settings  Общие настройки для трубы (massFlowRate, step и др.)
     * @param thermo    Модель термодинамики
     */
    void simulateTee(const ElementConfig& e, StreamState& st,const PipeSettings& settings,
        std::shared_ptr<ThermoProperties> thermo);
} 
