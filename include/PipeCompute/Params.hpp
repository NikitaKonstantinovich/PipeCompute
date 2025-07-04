#pragma once
#include <vector>
#include <cmath>             // для sqrt()
#include "PipeCompute/Types.hpp"

namespace PipeCompute {
	/** Описание одного сегмента трубы */
	struct Segment {
		double x0, y0, z0; // координаты начала
		double x1, y1, z1; // координаты конца
		double diameter;   // диаметр, м
		double wallThickness; // толщина стенки, м

		double length() const {
			// расстояние между (x0,y0,z0) и (x1,y1,z1)
			double dx = x1 - x0;
			double dy = y1 - y0;
			double dz = z1 - z0;
			return std::sqrt(dx * dx + dy * dy + dz * dz);
		}
	};

	/** Параметры для CalculatePipe */
	struct PipeParams {
		std::vector<Segment> segments; // сегменты трубы
		MediumType medium; // тип среды
		double temperature; // температура среды начальная, К
		double pressure;    // давление начальное, Па
		double massFlowRate; // массовый расход, кг/с
		double ambientTemp; // температура окружающей среды, К
		double step; // шаг дискретизации вдоль трубы, м
		UnitSystem units; // система единиц измерения
	};

	/** Параметры для CalculateBend */
	struct BendParams : PipeParams {
		double bendAngle; // угол поворота, рад
		double bendRadius; // радиус изгиба, м
	};

	/** Параметры для calculateTee (разветвление) */
	struct TeeParams : PipeParams {
		double branchAngle; // угол ветви, рад
		double branchDiameter; // диаметр ветви, м
	};
}