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
}