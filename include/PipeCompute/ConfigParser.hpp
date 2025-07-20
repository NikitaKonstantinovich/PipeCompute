#pragma once

#include "PipeCompute/Params.hpp"
#include "PipeCompute/ThermoConfig.hpp"
#include <vector>
#include <string>

namespace PipeCompute {
	/** Глобальные настройки расчета, общие для всех элементов */
	struct GlobalSettings {
		double massFlowRate = 0.0;  // Массовый расход, кг/с
		double ambientTemperature = 0.0; // Температура окружающей среды, K
		double step = 1.0; // Шаг расчета, м

		double initialPressure; // Pa
		double initialTemperature; // K
	};

	/** Описание одного элемента трубопровода из конфига */
	struct ElementConfig {
		std::string type; // Тип элемента: "pipe", "bend", "tee" 

		/** Pipe */
		Point3D start; // Начало сегмента (x0, y0, z0)
		Point3D end; // Конец сегмента (x1, y1, z1)
		double diameter = 0.0; // Диаметр, м
		double wallThickness = 0.0; // Толщина стенки, м
		double roughness = 0.0; // Шероховатость стенки, м
		double wallConductivity = 0.0; // Теплопроводность стенки, Вт/(м·К)
		double hOuter = 0.0; // Коэффициент теплоотдачи снаружи, Вт/(м²·К)

		/** Bend */
		double bendAngle = 0.0; // Угол изгиба для "bend", радианы
		double bendRadius = 0.0; // Радиус изгиба для "bend", м

		/** Tee */
		double mainBranchDiameter = 0.0; // Диаметр main ответвления
		double sideBranchDiameter = 0.0; // Диаметр side ответвления
	};

	/** Общая структура: глобальные настройки + список элементов */
	struct Config {
		GlobalSettings global; // Глобальные настройки
		std::unique_ptr<ThermoConfig> thermo; // термодинамическая модель
		std::vector<ElementConfig> elements; // Список элементов трубопровода
	};

	class ConfigParser {
	public:
		static Config load(const std::string& jsonPath);
	};

}