#pragma once

#include "PipeCompute/Params.hpp"   // для BendParams
#include "PipeCompute/Results.hpp"  // для BendResults
#include "PipeCompute/ThermoProperties.hpp"
#include <memory>
#include <vector>

namespace PipeCompute {

    /**
     * Класс для расчёта одного изгиба трубы.
     */
    class Bend {
    public:
        Bend(const BendParams& params,
            std::shared_ptr<ThermoProperties> thermo);

        /**
         * Запускает симуляцию изгиба:
         * - рассчитывает профиль давления и температуры вдоль всего сегмента,
         * - добавляет локальные потери Δp_bend = ξ_bend · (ρ v² / 2).
         */
        BendResults simulate() const;

    private:
        BendParams params_;
        std::shared_ptr<ThermoProperties> thermo_;

        // Функции расчёта
        double computeVelocity(double rho, double area) const;
        double computeReynolds(double rho, double velocity, double diameter, double viscosity) const;
        double computeFrictionFactor(double Re) const;
        double computeLocalLoss(double xi, double rho, double v) const;
    };

    ///  функция из API
    void calculateBend(const BendParams& in, BendResults& out);

}