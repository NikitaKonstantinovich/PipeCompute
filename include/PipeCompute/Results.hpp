#pragma once
#include<vector>
#include "Types.hpp"

namespace PipeCompute {
	/** Результаты в одной точке вдоль трубы */
	struct PointResult {
		double position; // позиция вдоль трубы, м
		double pressure; // давление, Па
		double temperature; // температура, К
		double velocity; // скорость, м/с
		double reynolds; // число Рейнольдса
		double nusselt; // число Нуссельта
	};

	/** Выход для CalculatePipe */
	struct PipeResults {
		std::vector<PointResult> points;
		double totalPressureDrop; // суммарное падение давления, Па
		double totalHeatTransfer; // суммарный теплообмен, Вт
	};

	/** Выход для CalculateBend */
	struct BendResults : PipeResulats {
		double localLoss; // локальные потери давления в изгибе, Па
	};

	/** Выход для CalculateTee */
	struct TeeResults : PipeResults {
		ddouble branchFlowRate; // расход в ответвлении, кг/с
	};
}