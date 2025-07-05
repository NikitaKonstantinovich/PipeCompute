#include <iostream>
#include <string>
#include <memory>

#include <nlohmann/json.hpp>
#include "PipeCompute/ResultsExporter.hpp"
#include "PipeCompute/ConfigParser.hpp"

#include "PipeCompute/PipeSimulator.hpp"
#include "PipeCompute/BendSimulator.hpp"
#include "PipeCompute/TeeSimulator.hpp"

#include "PipeCompute/IdealGasThermo.hpp"
#include "PipeCompute/ConstantFluidThermo.hpp"

using namespace PipeCompute;

//class MockThermo : public PipeCompute::ThermoProperties {
//public:
//    bool compute(const PipeCompute::ThermoInput& in, PipeCompute::ThermoOutput& out) const override {
//        double T = in.temperature;
//        out.density = 1.2 * (273.15 / T);       // Примерная модель для воздуха
//        out.viscosity = 1.8e-5;                   // Вязкость воздуха
//        out.heatCapacity = 1005;                    // Теплоёмкость воздуха
//        out.zFactor = 1.0;                     // Фактор сжимаемости
//        out.enthalpy = out.heatCapacity * (T - 273.15);
//        out.entropy = out.heatCapacity * std::log(T / 273.15);
//        return true;
//    }
//};

int main(int argc, char** argv) {
    // 1) Выбор конфига
    std::string cfgPath = (argc >= 2 ? argv[1] : "configs/test.json");
    std::cout << "Loading config from: " << cfgPath << "\n";

    // 2) Читаем JSON в Config, где thermo — unique_ptr<ThermoConfig>
    Config cfg = ConfigParser::load(cfgPath);

    // 3) Строим конкретную модель ThermoProperties на основании cfg.thermo->model
    std::shared_ptr<ThermoProperties> thermo;
    if (cfg.thermo->model == "ideal_gas") {
        // приведение к IdealGasConfig
        auto& ig = static_cast<IdealGasConfig&>(*cfg.thermo);
        thermo = std::make_shared<IdealGasThermo>(
            ig.molecularMass,
            ig.universalGasConst,
            ig.Cp,
            ig.viscosity,
            ig.thermalConductivity
        );
    }
    else if (cfg.thermo->model == "liquid") {
        auto& lf = static_cast<LiquidConfig&>(*cfg.thermo);
        thermo = std::make_shared<ConstantFluidThermo>(
            lf.density,
            lf.Cp,
            lf.viscosity,
            lf.thermalConductivity,
            lf.T0
        );
    }
    else {
        std::cerr << "Unsupported thermo.model: " << cfg.thermo->model << "\n";
        return 1;
    }

    // 4) Подготовка остальных настроек
    nlohmann::json steps = nlohmann::json::array();
    StreamState st{ cfg.global.initialPressure, cfg.global.initialTemperature };

    PipeCompute::PipeSettings pipeSettings;
    pipeSettings.massFlowRate = cfg.global.massFlowRate;
    pipeSettings.ambientTemperature = cfg.global.ambientTemperature;
    pipeSettings.step = cfg.global.step;
    pipeSettings.heatTransferCoeff = cfg.global.heatTransferCoeff;
    pipeSettings.thermo = thermo;

    // Последовательное выполнение элементов из конфига
    for (auto const& e : cfg.elements) {
        if (e.type == "pipe") {
            simulatePipe(e, st, pipeSettings);
            steps.push_back({
                {"type", "pipe"},
                {"length", e.length},
                {"pressure", st.pressure},
                {"temperature", st.temperature}
             });
        }
        else if (e.type == "bend") {
            simulateBend(e, st, pipeSettings, thermo);
            steps.push_back({
                {"type",        "bend"},
                {"angle_rad",   e.bendAngle},
                {"pressure_Pa", st.pressure}
            });
        }
        else if (e.type == "tee") {
            TeeResult teeResult = simulateTee(e, st, pipeSettings, thermo);

            steps.push_back({
                {"type", "tee"},
                {"diameter_m", e.diameter},
                {"mainBranchDiameter_m", e.mainBranchDiameter},
                {"sideBranchDiameter_m", e.sideBranchDiameter},
                {"dP_Tee_Pa",      teeResult.dP}
            });
            steps.push_back({
                {"type",          "main_branch"},
                {"pressure_Pa",   teeResult.main.pressure},
                {"temperature_K", teeResult.main.temperature}
            });
            steps.push_back({
                {"type",          "side_branch"},
                {"pressure_Pa",   teeResult.side.pressure},
                {"temperature_K", teeResult.side.temperature},
                {"flowRate_kg_s", teeResult.sideFlow}
            });
        }
        else {
            std::cerr << "Unknown element type: " << e.type << "\n";
        }
    }

    nlohmann::json root;
    root["global"] = {
        {"massFlowRate",       cfg.global.massFlowRate},
        {"ambientTemperature", cfg.global.ambientTemperature},
        {"step",               cfg.global.step},
        {"heatTransferCoeff",  cfg.global.heatTransferCoeff},
        {"initialPressure",    cfg.global.initialPressure},  
        {"initialTemperature", cfg.global.initialTemperature}
    };
    root["steps"] = std::move(steps);

    const std::string jsonOut = "results.json";
    writeJson(root, jsonOut);
    std::cout << "Results exported to " << jsonOut << "\n";

    const std::string csvOut = "results.csv";
    writeCsv(root, csvOut);
    std::cout << "Results exported to " << csvOut << "\n";

    return 0;
}
