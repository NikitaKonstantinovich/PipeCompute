#pragma once

#include "PipeCompute/Params.hpp"
#include <vector>
#include <string>

namespace PipeCompute {
	/** Глобальные настройки расчета, общие для всех элементов */
	struct GlobalSettings {
		double massFlowRate = 0.0;  // Массовый расход, кг/с
		double heatTransferCoeff = 0.0; // Коэффициент теплоотдачи, Вт/(м²·К)
		double ambientTemperature = 0.0; // Температура окружающей среды, K
		double step = 1.0; // Шаг расчета, м
	};

	/** Описание одного элемента трубопровода из конфига */
	struct ElementConfig {
		std::string type; // Тип элемента: "pipe", "bend", "tee" 

		/** Pipe */
		double length = 0.0; // Длина элемента, м
		double diameter = 0.0; // Диаметр, м
		double wallThickness = 0.0; // Толщина стенки, м

		/** Bend */
		double bendAngle = 0.0; // Угол изгиба для "bend", радианы
		double bendRadius = 0.0; // Радиус изгиба для "bend", м

		/** Tee */
		double branchDiameter = 0.0; // Диаметр ответвления
	};

	/** Общая структура: глобальные настройки + список элементов */
	struct Config {
		GlobalSettings global; // Глобальные настройки
		std::vector<ElementConfig> elements; // Список элементов трубопровода
	};

	class ConfigParser {
	public:
		static Config load(const std::string& jsonPath);
	};

}