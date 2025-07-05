#pragma once
#include "PipeCompute/ThermoProperties.hpp"

namespace PipeCompute {

    /**
     * Простая модель идеального газа с константными Cp, μ, λ.
     *   Формулы:
     *     ρ = p·M / (R_u·T)
     *     H = Cp·(T − T0),    S = Cp·ln(T/T0)
     */
    struct IdealGasThermo : ThermoProperties {
        double molecularMass;     // M, кг/моль
        double universalGasConst; // R_u, Дж/(моль·К)
        double Cp;                // Дж/(кг·К)
        double viscosity;         // Па·с
        double thermalConductivity; // Вт/(м·К)
        double T0;                // опорная температура, K (для H,S), e.g. 273.15

        IdealGasThermo(double M,
            double Ru,
            double Cp_,
            double mu,
            double lambda_,
            double T0_ = 273.15)
            : molecularMass(M)
            , universalGasConst(Ru)
            , Cp(Cp_)
            , viscosity(mu)
            , thermalConductivity(lambda_)
            , T0(T0_)
        {
        }

        bool compute(const ThermoInput& in,
            ThermoOutput& out) const override
        {
            double T = in.temperature;
            double p = in.pressure;
            // ρ = p·M / (R_u·T) / молярная масса / масса
            out.density = p * molecularMass / (universalGasConst * T);
            out.heatCapacity = Cp;
            out.viscosity = viscosity;
            out.thermalConductivity = thermalConductivity;
            out.enthalpy = Cp * (T - T0);
            out.entropy = Cp * std::log(T / T0);
            return true;
        }
    };
} 
