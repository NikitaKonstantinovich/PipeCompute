#pragma once

namespace PipeCompute {
	/**
	* Тип среды в трубопроводе
	*/
	enum class MediumType {
		Water,        // Вода
		Steam,       // Пар
		Air,         // Воздух
		Oil,         // Нефть
	};

	/** Еденицы измерения вывода */
	enum UnitSystem {
		SI,      // метрическая система (СИ)
		Imperial,    // имперская система
	};

	struct Point3D {
		double x{ 0 }, y{ 0 }, z{ 0 };
	};
}