#include "PipeCompute/ThermoProperties.hpp"
#include <cmath>

namespace PipeCompute {

	// Простая реализация: возвращает параметры воздуха (приблизительно)
	class MockThermo : public ThermoProperties {
	public:
		bool compute(const ThermoInput& in, ThermoOutput& out) const override {
			const double T = in.temperature;
			out.density = 1.2 * (273.15 / T); // Плотность воздуха при 20°C и 1 атм
			out.viscosity = 1.81e-5; // Вязкость воздуха
			out.heatCapacity = 1005; // Удельная теплоемкость воздуха при постоянном давлении
			out.zFactor = 1.0; // Фактор сжимаемости для идеального газа
			out.enthalpy = out.heatCapacity * (T - 273.15); // Энтальпия
			out.entropy = out.heatCapacity * std::log(T / 273.15); // Энтропия
			return true;
		};
	};
}