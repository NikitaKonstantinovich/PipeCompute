#include "PipeCompute/Tee.hpp"
#include <cmath>     
#include <numbers>    

namespace PipeCompute {

    // Глобальная функция (не метод) для получения ξ тройника.
    // Здесь взяли примерное постоянное значение 1.5.
    // В дальнейшем можно сделать зависимость от угла или конфигурации.
    static double xiForTee() {
        return 1.5;
    }

    Tee::Tee(const TeeParams& params, std::shared_ptr<ThermoProperties> thermo) : params_(params), thermo_(thermo) {}

    TeeResults Tee::simulate() const {
        TeeResults res;

        // --- 1. Исходные данные из params_ ---
        double p_in = params_.pressure;          // Входное давление, Па
        double T_in = params_.temperature;       // Входная температура, K
        double mdot_total = params_.massFlowRate;      // Общий массовый расход, кг/с

        // --- 2. Геометрия сечений ---
        // Площадь основного потока (из первого сегмента параметров)
        double D1 = params_.segments[0].diameter;
        double A1 = std::numbers::pi * D1 * D1 / 4.0;
        // Площадь ответвления
        double D2 = params_.branchDiameter;
        double A2 = std::numbers::pi * D2 * D2 / 4.0;

        // --- 3. Разделяем расход по площадям ---
        // Пропорционально площадям: m1 = m_total * A1/(A1+A2), m2 = остаток
        double mdot1 = mdot_total * (A1 / (A1 + A2));
        double mdot2 = mdot_total - mdot1;

        // --- 4. Рассчитываем свойства среды на входе ---
        ThermoInput  tin{ T_in, p_in, {} };  // пока без композиции
        ThermoOutput tout;
        thermo_->compute(tin, tout);
        double rho = tout.density;           // Плотность, кг/м³

        // --- 5. Скорости потоков в каждом направлении ---
        double v1 = computeVelocity(rho, A1, mdot1);
        double v2 = computeVelocity(rho, A2, mdot2);

        // --- 6. Локальные потери давления в тройнике ---
        double xi = xiForTee();                       // Коэффициент ξ
        double dp = computeLocalLoss(xi, rho, v1);    // Δp = ξ·(ρ·v1²/2)
        double p_out = p_in - dp;                     // Давление после тройника

        // --- 7. Формируем результат ---
        // Единственная точка: position = 0 (тройник не разбивается на длину)
        res.points.push_back(PointResult{
            0.0,                // позиция вдоль "длины" тройника
            p_out,              // выходное давление, Па
            T_in,               // выходная температура, K (без теплопотерь)
            v1,                 // скорость основного потока, м/с
            rho * v1 * D1 / tout.viscosity, // Re = ρ v D / μ
            0.0                 // Nusselt не считаем здесь
            });

        res.branchFlowRate = mdot2;   // расход в ответвлении, кг/с
        res.totalPressureDrop = dp;     // общее падение давления, Pa
        res.totalHeatTransfer = 0.0;    // теплопередача не учитываем

        return res;
    }

    double Tee::computeVelocity(double rho, double area, double massFlow) const {
        // Защита от деления на ноль
        if (rho * area <= 0.0) return 0.0;
        return massFlow / (rho * area);
    }

    double Tee::computeLocalLoss(double xi, double rho, double v) const {
        // Δp = ξ · (ρ · v² / 2)
        return xi * (rho * v * v / 2.0);
    }

    //void calculateTee(const TeeParams& in,TeeResults& out) {
    //    auto thermo = std::make_shared<MockThermo>();
    //    Tee tee(in, thermo);
    //    out = tee.simulate();
    //}

}