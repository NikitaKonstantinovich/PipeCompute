#pragma once
#include "PipeCompute/ThermoProperties.hpp"

namespace PipeCompute {

    /**
     * Модель жидкости с постоянными свойствами.
     * Плотность, Cp, μ и λ задаются константно.
     */
    struct ConstantFluidThermo : ThermoProperties {
        double density;             // ρ, кг/м³
        double heatCapacity;        // Cp, Дж/(кг·К)
        double viscosity;           // μ, Па·с
        double thermalConductivity; // λ, Вт/(м·К)
        double T0;                  // опорная температура, K (для расчёта H, S)

        ConstantFluidThermo(double rho,
            double Cp_,
            double mu,
            double lambda,
            double T0_ = 273.15)
            : density(rho)
            , heatCapacity(Cp_)
            , viscosity(mu)
            , thermalConductivity(lambda)
            , T0(T0_)
        {}

        bool compute(const ThermoInput& in,
            ThermoOutput& out) const override
        {
            out.density = density;
            out.heatCapacity = heatCapacity;
            out.viscosity = viscosity;
            out.thermalConductivity = thermalConductivity;
            // H = Cp·(T–T0), S = Cp·ln(T/T0)
            out.enthalpy = heatCapacity * (in.temperature - T0);
            out.entropy = heatCapacity * std::log(in.temperature / T0);
            return true;
        }
    };
}
