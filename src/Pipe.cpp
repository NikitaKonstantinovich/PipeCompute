#include "PipeCompute/Pipe.hpp"
#include <cmath>

namespace PipeCompute {

	Pipe::Pipe(const std::vector<Segment>& segments, const PipeSettings& settings)
		: segments_(segments), settings_(settings) {}

	const std::vector<PointResult>& Pipe::simulate() {
		result_.clear();

		double currentPressure = settings_.initialPressure;
		double currentTemperature = settings_.initialTemperature;
		double cumulativeLength = 0.0;

		for (const auto& seg : segments_) {
			simulateSegment(seg, currentPressure, currentTemperature, cumulativeLength);
			cumulativeLength += seg.length();
		}

		return result_;
	}

	void Pipe::simulateSegment(const Segment& seg, double& currentPressure, double& currentTemperature, double cumulativeLength) {

		const double L = seg.length(); // длина сегмента
		const double D = seg.diameter; // диаметр сегмента
		const double area = M_PI * D * D / 4.0; // площадь поперечного сечения

		ThermoInput tin{ currentTemperature, currentPressure, {} };
		ThermoOutput tout;
		settings_.thermo->compute(tin, tout);

		double rho = tout.density;
		double v = computeVelocity(rho, area);
		double Re = computeReynolds(rho, v, D, tout.viscosity);
		double Pr = tout.heatCapacity * tout.viscosity / 0.026;
		double Nu = computeNusselt(Re, Pr);

		result_.push_back(PointResult{ pos, currentPressure, currentTemperature, velocity, Re, Nu });
	}

	double Pipe::computeVelocity(double rho, double area) const {
		if (rho == 0 || area == 0) {
			return 0.0; // защита от деления на ноль
		};
		return settings_.massFlowRate / (rho * area); // V = m / (rho * A)
	}

	double Pipe::computeReynolds(double rho, double velocity, double diameter, double viscosity) const {
		if (viscosity == 0) {
			return 0.0; // защита от деления на ноль
		};
		return rho * velocity * diameter / viscosity;
	}

	double Pipe::computeNusselt(double Re, double Pr) const {
		return 0.023 * std::pow(Re, 0.8) * std::pow(Pr, 0.33);
	}
}