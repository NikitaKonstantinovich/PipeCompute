#include <iostream>
#include <numbers>
#include "PipeCompute/Params.hpp"
#include "PipeCompute/Pipe.hpp"
#include "PipeCompute/Bend.hpp"
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

	// --- 1. Настройки симуляции ---
	PipeSettings settings;
	settings.initialPressure = 2e5;    // 2 бар в Па
	settings.initialTemperature = 300.0;  // 27 °C в K
	settings.massFlowRate = 0.1;    // 0.1 кг/с
	settings.ambientTemperature = 290.0;  // 17 °C в K
	settings.step = 1.0;    // шаг 1 метр
	settings.heatTransferCoeff = 10.0;   // 10 Вт/(м²·К), грубо
	settings.thermo = std::make_shared<MockThermo>();

	// --- 2. Задаём один сегмент длиной 10 м, D=0.1 м ---
	Segment seg1{};
	seg1.x0 = seg1.y0 = seg1.z0 = 0.0;
	seg1.x1 = 10.0; seg1.y1 = seg1.z1 = 0.0;
	seg1.diameter = 0.1;      // 10 см
	seg1.wallThickness = 0.005; // 5 мм

	// --- 3. Запускаем симуляцию первого сегмента и выводим результат ---
	Pipe pipe1({seg1}, settings);
	auto res1 = pipe1.simulate();
	std::cout << "=== After first straight 10m ===\n"
		<< "Pressure at end: " << res1.back().pressure / 1e5 << " bar\n"
		<< "Temperature at end: " << res1.back().temperature - 273.15 << " °C\n\n";

	// --- 4. Второй сегмент отвод вверх 90 град ---
	BendParams bp;
	// возьмём те же координаты и диаметр для ориентира (из seg1):
	bp.segments = { seg1 };           // для доступа к диаметру
	bp.pressure = res1.back().pressure;
	bp.temperature = res1.back().temperature;
	bp.massFlowRate = settings.massFlowRate;
	bp.ambientTemp = settings.ambientTemperature;
	bp.step = settings.step;
	//bp.heatTransferCoeff = settings.heatTransferCoeff;
	//bp.composition = {};              // пустой состав
	bp.bendAngle = std::numbers::pi / 2.0;        // 90°
	bp.bendRadius = 1.0;             // радиус изгиба 1 м

	Bend bend(bp, settings.thermo);
	BendResults br = bend.simulate();

	std::cout << "Loss in bend: " << br.localLoss / 1e5 << " bar\n";

	// 4) Второй участок: 2 м прямой трубы вверх (по оси Z)
	Segment seg2{};
	seg2.x0 = 10.0; seg2.y0 = 0.0; seg2.z0 = 0.0;
	seg2.x1 = 10.0; seg2.y1 = 0.0; seg2.z1 = 2.0;
	seg2.diameter = 0.1;
	seg2.wallThickness = 0.005;

	// Новый PipeSettings, входящее давление и температура — выход колена
	settings.initialPressure = br.points.back().pressure;
	settings.initialTemperature = br.points.back().temperature;

	Pipe pipe2({ seg2 }, settings);
	auto res2 = pipe2.simulate();
	std::cout << "=== After second straight 2m ===\n"
		<< "Pressure at end: " << res2.back().pressure / 1e5 << " bar\n"
		<< "Temperature at end: " << res2.back().temperature - 273.15 << " °C\n\n";

	return 0;
}