#pragma once
#include <vector>
#include <cmath>             // для sqrt()
#include "PipeCompute/Types.hpp"

namespace PipeCompute {
	/** Описание одного сегмента трубы */
	struct Segment {
		Point3D start; // начало сегмента
		Point3D end;   // конец сегмента
		double diameter;   // диаметр, м
		double wallThickness; // толщина стенки, м
		double roughness; // шероховатость стенки, м
		double wallConductivity; // теплопроводность стенки, Вт/(м·К)
		double hOuter; // коэффициент теплоотдачи снаружи, Вт/(м²·К)

		double length() const {
			// расстояние между (x0,y0,z0) и (x1,y1,z1)
			double dx = end.x - start.x;
			double dy = end.y - start.y;
			double dz = end.z - start.z;
			return std::sqrt(dx * dx + dy * dy + dz * dz);
		}

		double dz() const {
			return end.z - start.z;
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
		double mainBranchDiameter; // диаметр основной ветви
		double sideBranchDiameter; // диаметр второстепенной ветви
	};
}