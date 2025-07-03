#pragma once
#include <vector>
#include <string>

namespace PipeCompute {
	/** Входные параметры для расчета термодинамики */
	struct ThermoInput {
		double temperature; // Температура, К
		double pressure;    // Давление, Па
		std::vector<std::pair<std::string, double>> composition; // Состав в виде  "название" - "концентрация" {"CH4", 0.95} и тп
	};

	/** Результаты расчета */
	struct ThermoOutput {
		double density; // Плотность, кг/м^3
		double viscosity; // Вязкость, Па·с
		double heatCapacity; // Удельная теплоемкость, Дж/(кг·К)
		double zFactor; // Фактор сжимаемости
		double enthalpy; // Энтальпия, Дж/кг
		double entropy; // Энтропия, Дж/(кг·К)
	};

	/** Абстрактный интерфейс (базовый класс) */
	class ThermoProperties {
	public:
		virtual ~ThermoProperties() = default;
		
		virtual bool compute(const ThermoInput& in, ThermoOutput& out) const = 0;
	};
}