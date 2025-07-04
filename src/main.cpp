#include <iostream>
#include <numbers>
#include "PipeCompute/Params.hpp"
#include "PipeCompute/Pipe.hpp"
#include "PipeCompute/Bend.hpp"
#include "PipeCompute/ThermoProperties.hpp"
#include "PipeCompute/Tee.hpp"

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
	auto thermo = std::make_shared<MockThermo>();

	// --- 1. Настройки симуляции ---
	PipeSettings settings;
	settings.initialPressure = 2e5;    // 2 бар в Па
	settings.initialTemperature = 300.0;  // 27 °C в K
	settings.massFlowRate = 0.1;    // 0.1 кг/с
	settings.ambientTemperature = 290.0;  // 17 °C в K
	settings.step = 1.0;    // шаг 1 метр
	settings.heatTransferCoeff = 10.0;   // 10 Вт/(м²·К), грубо
	settings.thermo = thermo;

	// --- 2. Задаём один сегмент длиной 10 м, D=0.1 м ---
	Segment seg1{};
	seg1.x0 = seg1.y0 = seg1.z0 = 0.0;
	seg1.x1 = 10.0; seg1.y1 = seg1.z1 = 0.0;
	seg1.diameter = 0.1;      // 10 см
	seg1.wallThickness = 0.005; // 5 мм

	// --- 3. Запускаем симуляцию первого сегмента и выводим результат ---
	Pipe pipe1({seg1}, settings);
	auto res1 = pipe1.simulate();
	double p1_end = res1.back().pressure;
	double T1_end = res1.back().temperature;
	std::cout << "[1] End of 10 m pipe: p=" << p1_end / 1e5 << " bar, T="
		<< (T1_end - 273.15) << " °C\n";

	// --- 4. Второй сегмент отвод вверх 90 град ---
	BendParams bp1;
	bp1.segments = { seg1 };         // для диаметра
	bp1.pressure = p1_end;
	bp1.temperature = T1_end;
	bp1.massFlowRate = settings.massFlowRate;
	bp1.step = settings.step;
	bp1.bendAngle = std::numbers::pi / 2;  // 90°
	bp1.bendRadius = 1.0;                  // радиус 1 м

	Bend bend1(bp1, thermo);
	auto br1 = bend1.simulate();
	double p2 = br1.points.back().pressure;
	double T2 = br1.points.back().temperature;
	std::cout << "[2] After 1st bend: Δp_bend=" << br1.localLoss / 1e5
		<< " bar, p=" << p2 / 1e5 << " bar\n";

	// --- 5. Второй участок: 2 м прямой трубы вверх (по оси Z) ---
	Segment seg2{};
	seg2.x0 = seg1.x1; seg2.y0 = 0; seg2.z0 = 0;
	seg2.x1 = seg1.x1; seg2.y1 = 0; seg2.z1 = 2.0;
	seg2.diameter = seg1.diameter;
	seg2.wallThickness = seg1.wallThickness;

	Pipe pipe2({ seg2 }, settings);
	// обновляем начальные p,T
	settings.initialPressure = p2;
	settings.initialTemperature = T2;
	Pipe pipe2_sim({ seg2 }, settings);
	auto res2 = pipe2_sim.simulate();
	double p3 = res2.back().pressure;
	double T3 = res2.back().temperature;
	std::cout << "[3] After 2 m up-pipe: p=" << p3 / 1e5 << " bar, T="
		<< (T3 - 273.15) << " °C\n";

	// --- 6. Второе колено 90° (вниз в горизонталь) ---
	BendParams bp2 = bp1;  // копируем настройки
	bp2.pressure = p3;
	bp2.temperature = T3;
	Bend bend2(bp2, thermo);
	auto br2 = bend2.simulate();
	double p4 = br2.points.back().pressure;
	std::cout << "[4] After 2nd bend: Δp_bend=" << br2.localLoss / 1e5
		<< " bar, p=" << p4 / 1e5 << " bar\n";

	// --- 7. Ещё 2 м прямой трубы ---
	Segment seg3{};
	seg3.x0 = seg2.x1; seg3.y0 = 0; seg3.z0 = seg2.z1;
	seg3.x1 = seg3.x0 + 2.0; seg3.y1 = 0; seg3.z1 = seg2.z1;
	seg3.diameter = seg1.diameter;
	seg3.wallThickness = seg1.wallThickness;

	settings.initialPressure = p4;
	settings.initialTemperature = br2.points.back().temperature;
	Pipe pipe3({ seg3 }, settings);
	auto res3 = pipe3.simulate();
	double p5 = res3.back().pressure;
	std::cout << "[5] After 2 m pipe: p=" << p5 / 1e5 << " bar\n";

	// --- 8. Тройник ---
	TeeParams tp;
	tp.segments = { seg3 };           // для диаметра главного
	tp.branchDiameter = seg1.diameter;      // ветка того же D
	tp.pressure = p5;
	tp.temperature = br2.points.back().temperature;
	tp.massFlowRate = settings.massFlowRate;
	tp.step = settings.step;

	Tee tee(tp, thermo);
	auto tr = tee.simulate();
	std::cout << "[6] Tee: dp=" << tr.totalPressureDrop / 1e5
		<< " bar, branch flow=" << tr.branchFlowRate
		<< " kg/s, main p=" << tr.points[0].pressure / 1e5 << " bar\n";

	// --- 8. Две ветви по 2 м каждая ---
	// Ветка 1: продолжаем главную линию
	Segment t1_seg{ seg3.x1, 0, seg3.z1, seg3.x1 + 2, 0, seg3.z1,
					tp.segments[0].diameter, seg1.wallThickness };
	settings.initialPressure = tr.points[0].pressure;
	settings.initialTemperature = tp.temperature;
	Pipe t1_pipe({ t1_seg }, settings);
	auto t1_res = t1_pipe.simulate();
	std::cout << "[7a] Branch1 2 m: p=" << t1_res.back().pressure / 1e5 << " bar\n";

	// Ветка 2: ответвление
	Segment t2_seg{ seg3.x1, 0, seg3.z1, seg3.x1, 2.0, seg3.z1,
					tp.branchDiameter, seg1.wallThickness };
	settings.initialPressure = tr.points[0].pressure;
	Pipe t2_pipe({ t2_seg }, settings);
	auto t2_res = t2_pipe.simulate();
	std::cout << "[7b] Branch2 2 m: p=" << t2_res.back().pressure / 1e5 << " bar\n";

	return 0;
}