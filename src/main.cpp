#include <iostream>
#include <string>
#include <memory>

#include <nlohmann/json.hpp>
#include "PipeCompute/ResultsExporter.hpp"

#include "PipeCompute/ConfigParser.hpp"
#include "PipeCompute/PipeSimulator.hpp"
#include "PipeCompute/BendSimulator.hpp"
#include "PipeCompute/TeeSimulator.hpp"
#include "PipeCompute/ThermoProperties.hpp"

using namespace PipeCompute;

class MockThermo : public PipeCompute::ThermoProperties {
public:
    bool compute(const PipeCompute::ThermoInput& in, PipeCompute::ThermoOutput& out) const override {
        double T = in.temperature;
        out.density = 1.2 * (273.15 / T);       // Примерная модель для воздуха
        out.viscosity = 1.8e-5;                   // Вязкость воздуха
        out.heatCapacity = 1005;                    // Теплоёмкость воздуха
        out.zFactor = 1.0;                     // Фактор сжимаемости
        out.enthalpy = out.heatCapacity * (T - 273.15);
        out.entropy = out.heatCapacity * std::log(T / 273.15);
        return true;
    }
};

int main(int argc, char** argv) {
    // Выбираем конфиг: аргумент или дефолт
    std::string cfgPath;
    if (argc >= 2) {
        cfgPath = argv[1];
    }
    else {
        cfgPath = "configs/test.json";
        std::cout << "No config argument provided; using default: " << cfgPath << "\n";
    }
    std::cout << "Loading config from: " << cfgPath << "\n";

    // Загружаем конфигурацию
    PipeCompute::Config cfg = PipeCompute::ConfigParser::load(cfgPath);

    nlohmann::json steps = nlohmann::json::array();

    // Модель термодинамики
    auto thermo = std::make_shared<MockThermo>();

    // Начальные условия (можно вынести в JSON)
    double currentP = 2e5;   // Па
    double currentT = 300.0; // K

    StreamState st{ currentP, currentT };  // 2 bar, 27°C

    // Общие настройки для Pipe
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
        {"heatTransferCoeff",  cfg.global.heatTransferCoeff}
    };
    root["steps"] = std::move(steps);

    const std::string outPath = "results.json";
    writeJson(root, outPath);
    std::cout << "Results exported to " << outPath << "\n";

    return 0;
}
