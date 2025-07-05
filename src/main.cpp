#include <iostream>
#include <string>

#include "PipeCompute/ConfigParser.hpp"
#include "PipeCompute/Params.hpp"
#include "PipeCompute/Pipe.hpp"
#include "PipeCompute/Bend.hpp"
#include "PipeCompute/Tee.hpp"
#include "PipeCompute/ThermoProperties.hpp"

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
            PipeCompute::Segment seg;
            seg.x0 = seg.y0 = seg.z0 = 0.0;
            seg.x1 = e.length; seg.y1 = seg.z1 = 0.0;
            seg.diameter = e.diameter;
            seg.wallThickness = e.wallThickness;

            pipeSettings.initialPressure = currentP;
            pipeSettings.initialTemperature = currentT;
            PipeCompute::Pipe pipe({ seg }, pipeSettings);
            auto res = pipe.simulate();

            currentP = res.back().pressure;
            currentT = res.back().temperature;
            std::cout << "[pipe] length=" << e.length
                << "  p=" << currentP / 1e5 << " bar"
                << "  T=" << currentT - 273.15 << " °C\n";
        }
        else if (e.type == "bend") {
            PipeCompute::BendParams bp;
            PipeCompute::Segment stub{ 0,0,0,0,0,0, e.diameter, e.wallThickness };
            bp.segments = { stub };
            bp.bendAngle = e.bendAngle;
            bp.bendRadius = e.bendRadius;
            bp.pressure = currentP;
            bp.temperature = currentT;
            bp.massFlowRate = pipeSettings.massFlowRate;
            bp.step = pipeSettings.step;

            PipeCompute::Bend bend(bp, thermo);
            auto br = bend.simulate();
            currentP = br.points.back().pressure;
            std::cout << "[bend] angle=" << e.bendAngle
                << "  Δp=" << br.localLoss / 1e5 << " bar"
                << "  p=" << currentP / 1e5 << " bar\n";
        }
        else if (e.type == "tee") {
            PipeCompute::TeeParams tp;
            PipeCompute::Segment trunk{ 0,0,0,0,0,0, e.diameter, e.wallThickness };
            tp.segments = { trunk };
            tp.branchDiameter = e.branchDiameter;
            tp.pressure = currentP;
            tp.temperature = currentT;
            tp.massFlowRate = pipeSettings.massFlowRate;
            tp.step = pipeSettings.step;

            PipeCompute::Tee tee(tp, thermo);
            auto tr = tee.simulate();
            currentP = tr.points[0].pressure;
            std::cout << "[tee] branchD=" << e.branchDiameter
                << "  Δp=" << tr.totalPressureDrop / 1e5 << " bar"
                << "  m_branch=" << tr.branchFlowRate << " kg/s"
                << "  p_main=" << currentP / 1e5 << " bar\n";

            for (int i = 0; i < 2; ++i) {
                PipeCompute::Segment branch{ 0,0,0,0,0,0,
                                           (i == 0 ? e.diameter : e.branchDiameter),
                                           e.wallThickness };
                branch.x1 = 2.0;

                pipeSettings.initialPressure = currentP;
                pipeSettings.initialTemperature = currentT;
                PipeCompute::Pipe branchPipe({ branch }, pipeSettings);
                auto brRes = branchPipe.simulate();

                std::cout << "[branch" << (i + 1) << "] length=2"
                    << "  p=" << brRes.back().pressure / 1e5 << " bar\n";
            }
        }
        else {
            std::cerr << "Unknown element type: " << e.type << "\n";
        }
    }

    return 0;
}
