#pragma once
#include <string>
#include <nlohmann/json.hpp>

namespace PipeCompute {

    /** Базовый класс для конфигурации термомодели */
    struct ThermoConfig {
        std::string model;  // "ideal_gas" или "liquid"
        virtual ~ThermoConfig() = default;

        /** Заполнить себя из JSON-объекта */
        virtual void load(const nlohmann::json& j) = 0;
    };

    /** Конфиг для идеального газа */
    struct IdealGasConfig : ThermoConfig {
        double molecularMass;
        double universalGasConst;
        double Cp;
        double viscosity;
        double thermalConductivity;

        IdealGasConfig() { model = "ideal_gas"; }

        void load(const nlohmann::json& j) override {
            molecularMass = j.at("molecularMass").get<double>();
            universalGasConst = j.at("universalGasConst").get<double>();
            Cp = j.at("Cp").get<double>();
            viscosity = j.at("viscosity").get<double>();
            thermalConductivity = j.at("thermalConductivity").get<double>();
        }
    };

    /** Конфиг для жидкости */
    struct LiquidConfig : ThermoConfig {
        double density;
        double Cp;
        double viscosity;
        double thermalConductivity;
        double T0;

        LiquidConfig() { model = "liquid"; }

        void load(const nlohmann::json& j) override {
            density = j.at("density").get<double>();
            Cp = j.at("Cp").get<double>();
            viscosity = j.at("viscosity").get<double>();
            thermalConductivity = j.at("thermalConductivity").get<double>();
            T0 = j.value("T0", 273.15);
        }
    };
}
