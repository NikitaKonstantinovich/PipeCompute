#pragma once

#include "PipeCompute/Params.hpp"         // Для структуры TeeParams
#include "PipeCompute/Results.hpp"        // Для структуры TeeResults и PointResult
#include "PipeCompute/ThermoProperties.hpp"
#include <memory>                         // std::shared_ptr
#include <vector>                         // std::vector

namespace PipeCompute {

    /**
     * @brief Класс моделирует тройник — точку, где поток разделяется на два направления.
     */
    class Tee {
    public:
        /**
         * @param params  Все входные параметры для тройника:
         *                геометрия, давление, расход, температура и т.п.
         * @param thermo  Реализация ThermoProperties для расчёта ρ, μ, cₚ и других свойств среды
         */
        Tee(const TeeParams& params, std::shared_ptr<ThermoProperties> thermo);

        /**
         * @brief Выполняет вычисления для тройника.
         * @return Заполненная структура TeeResults:
         *         - точка выхода (pressure, temperature, velocity, Re),
         *         - branchFlowRate (расход в ответвлении),
         *         - totalPressureDrop (падение давления).
         */
        TeeResults simulate() const;

    private:
        TeeParams params_;   // Сохранённые входные параметры
        std::shared_ptr<ThermoProperties> thermo_;   // Модель термодинамики

        /**
         * @brief Рассчитывает скорость потока по формуле v = ṁ/(ρ·A)
         * @param rho       Плотность среды, кг/м³
         * @param area      Площадь сечения, м²
         * @param massFlow  Массовый расход, кг/с
         */
        double computeVelocity(double rho, double area, double massFlow) const;

        /**
         * @brief Локальные потери давления Δp = ξ·(ρ·v²/2)
         * @param xi  Коэффициент местного сопротивления
         * @param rho Плотность, кг/м³
         * @param v   Скорость, м/с
         */
        double computeLocalLoss(double xi, double rho, double v) const;
    };

    /**
     * @brief Утилита для API: создаёт объект Tee и заполняет out.
     * @param in  Входные параметры для расчёта
     * @param out Выходная структура, в которую запишется результат
     */
    //void calculateTee(const TeeParams& in, TeeResults& out);

}