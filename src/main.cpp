#include <iostream>
#include "PipeCompute/Params.hpp"
#include "PipeCompute/Pipe.hpp"
#include "PipeCompute/ThermoProperties.hpp"

class MockThermo : public PipeCompute::ThermoProperties {
public:
	bool compute(const PipeCompute::ThermoInput& in, PipeCompute::ThermoOutput& out) const override {
		double T = in.temperature;
		out.density = 1.2 * (273.15 / T); // Примерная модель для воздуха
		out.viscosity = 1.8e-5; // Вязкость воздуха
		out.heatCapacity = 1005; // Теплоемкость воздуха
		out.zFactor = 1.0; // Фактор сжимаемости
		out.enthalpy = out.heatCapacity * (T - 273.15); // энтальпия
		out.entropy = out.heatCapacity * std::log(T / 273.15); // энтропия
		return true;
	}
};

int main() {
	using namespace PipeCompute;

	// --- 1. Задаём один сегмент длиной 10 м, D=0.1 м ---
	Segment seg;
	seg.x0 = seg.y0 = seg.z0 = 0.0;
	seg.x1 = 10.0; seg.y1 = seg.z1 = 0.0;
	seg.diameter = 0.1;      // 10 см
	seg.wallThickness = 0.005; // 5 мм

	std::vector<Segment> segments = { seg };

	// --- 2. Настройки симуляции ---
	PipeSettings settings;
	settings.initialPressure = 2e5;    // 2 бар в Па
	settings.initialTemperature = 300.0;  // 27 °C в K
	settings.massFlowRate = 0.1;    // 0.1 кг/с
	settings.ambientTemperature = 290.0;  // 17 °C в K
	settings.step = 1.0;    // шаг 1 метр
	settings.heatTransferCoeff = 10.0;   // 10 Вт/(м²·К), грубо
	settings.thermo = std::make_shared<MockThermo>();

	// --- 3. Запускаем симуляцию ---
	Pipe pipe(segments, settings);
	const auto& results = pipe.simulate();

	// --- 4. Печатаем профиль вдоль трубы ---
	std::cout << "x (m)\t p (bar)\t T (°C)\t v (m/s)\n";
	for (const auto& pt : results) {
		std::cout
			<< pt.position << "\t"
			<< pt.pressure / 1e5 << "\t"
			<< pt.temperature - 273.15 << "\t"
			<< pt.velocity
			<< "\n";
	}

	return 0;
}