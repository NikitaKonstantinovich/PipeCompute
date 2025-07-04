#pragma once
#include "PipeCompute/ThermoProperties.hpp"
#include "PipeCompute/Params.hpp" // для Segment
#include "PipeCompute/Results.hpp" // для PipeResult
#include <memory>
#include <vector>

namespace PipeCompute {
	struct PipeSettings {
		double initialPressure; // начальное давление, Па
		double initialTemperature; // начальная температура, К
		double massFlowRate; // массовый расход, кг/с
		double ambientTemperature; // температура окружающей среды, К
		double step; // шаг дискретизации вдоль трубы, м
		std::shared_ptr<ThermoProperties> thermo; // модель термодинамических свойств

		double heatTransferCoeff; // коэффициент теплоотдачи U, Вт/(м²·К)
	};

	class Pipe {
	public:
		Pipe(const std::vector<Segment>& segments, const PipeSettings& settings);
		const std::vector<PointResult>& simulate();

	private:
		std::vector<Segment> segments_; // сегменты трубы
		PipeSettings settings_; // настройки трубы
		std::vector<PointResult> result_; // результат симуляции

		void simulateSegment(const Segment& seg, double& currentPressure, double& currentTemperature, double cumulativeLength);

		double computeVelocity(double rho, double area) const;
		double computeReynolds(double rho, double velocity, double diameter, double viscosity) const;
		double computeNusselt(double Re, double Pr) const;
		double computeFrictionFactor(double Re) const;
		double computeHeatFlux(double localT) const;
	};
}