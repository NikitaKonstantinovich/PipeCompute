#include <iostream>
#include <string>
#include <memory>

#include "PipeCompute/ConfigParser.hpp"
#include "PipeCompute/PipeSimulator.hpp"
#include "PipeCompute/BendSimulator.hpp"
#include "PipeCompute/TeeSimulator.hpp"
#include "PipeCompute/Params.hpp"
#include "PipeCompute/Pipe.hpp"
#include "PipeCompute/Bend.hpp"
#include "PipeCompute/Tee.hpp"
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
        }
        else if (e.type == "bend") {
            simulateBend(e, st, pipeSettings, thermo);
        }
        else if (e.type == "tee") {
            simulateTee(e, st, pipeSettings, thermo);
        }
        else {
            std::cerr << "Unknown element type: " << e.type << "\n";
        }
    }

    return 0;
}
