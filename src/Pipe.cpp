#include "PipeCompute/Pipe.hpp"
#include <cmath>
#include <numbers>
#include <iostream>

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
		const double area = std::numbers::pi * D * D / 4.0; // площадь поперечного сечения
		const double step = settings_.step; // шаг дискретизации

		int nSteps = static_cast<int>(std::ceil(L / step));  // Сколько шагов по этому сегменту

		for (int i = 0; i <= nSteps; ++i) {
			// локальное смещение вдоль сегмента
			double local_s = std::min(i * step, L);
			double pos = cumulativeLength + local_s;

			ThermoInput tin{ currentTemperature, currentPressure, {} };
			ThermoOutput tout;
			settings_.thermo->compute(tin, tout);

			double rho = tout.density;
			double mu = tout.viscosity;
			double cp = tout.heatCapacity;
			double kf = tout.thermalConductivity;

			//гидравлика
			double v = computeVelocity(rho, area);
			double Re = computeReynolds(rho, v, D, tout.viscosity);
			// расчет f по Хааланду c учетом шероховатости 
			double f = computeFrictionFactor(Re, D, seg.roughness); // коэффициент трения

			// потери давления на этом участке
			double dp = f * (step / D) * (rho * v * v / 2.0);
			currentPressure -= dp;

			//теплообмен
			
				//Внутренний теплоперенос | Nu -> h_i
			double Pr = cp * mu / kf;
			double Nu = computeNusselt(Re, Pr);
			double hi = Nu * kf / D; // коэффициент теплоотдачи внутри трубы

				//геомеетрия стенки
			double Ri = D / 2.0; // внутренний радиус трубы
			double Ro = Ri + seg.wallThickness; // внешний радиус трубы

				//сопротиление на 1 метр
			double Ri_conv = 1.0 / (hi * 2.0 * std::numbers::pi * Ri); // внутреннее сопротивление
			double R_wall = std::log(Ro / Ri) / (2.0 * std::numbers::pi * seg.wallConductivity); // сопротивление стенки
			double Ro_conv = 1.0 / (seg.hOuter * 2.0 * std::numbers::pi * Ro); // внешнее сопротивление
			double R_total = Ri_conv + R_wall + Ro_conv; // общее сопротивление

				//тепловой поток на метр
			double q_per_m = (currentTemperature - settings_.ambientTemperature) / R_total; // Вт/м

				//изменение температуры на этом шаге
			double dT = -q_per_m * step / (settings_.massFlowRate * cp); // изменение температуры
			currentTemperature += dT;

			std::cout << "[pipe] x=" << pos << " m, p=" << currentPressure / 1e5 << " bar"
				 << ", T=" << (currentTemperature - 273.15) << " °C | Re: " << Re << " | dp :" << dp << "\n";

			std::cout << "v: " << computeVelocity(rho, area) << "m/s \n";

			result_.push_back(PointResult{ pos, currentPressure, currentTemperature, v, Re, Nu });
		}
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

	double Pipe::computeFrictionFactor(double Re, double diameter, double roughness) const {
		if (Re < 2300) {
			return 64.0 / Re; // ламинарный режим
		} else {
			// турбулентный режим
			double relR = roughness / diameter;
			double term = std::pow(relR / 3.7, 1.11) + 6.9 / Re;
			double inv_s = -1.8 * std::log10(term);
			return 1.0 / (inv_s * inv_s);
		}
	}

	double Pipe::computeHeatFlux(double localT) const {
		// q' = U * π * D * (T - Tamb)
		// здесь U из настроек, D нужно передать извне или хранить перед вызовом
		// сделаем параметром: settings_.heatTransferCoeff * perimeter * ΔT
		// В simulateSegment будем вызывать с known D и localT
		// поэтому прототип лучше: computeHeatFlux(double D, double localT)
		return 0.0; // заглушка, реализацию чуть ниже перенесём inline
	}
}