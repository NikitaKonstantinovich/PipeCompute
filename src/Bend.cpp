#include "PipeCompute/Bend.hpp"
#include <cmath>
#include <numbers>

namespace PipeCompute {
    // простая корреляция для ξ_bend по углу (рад)
    // можно потом уточнить формулу
    static double xiForAngle(double angle) {
        // пример: ξ = 0.1 * (angle / π)
        return 0.1 * (angle / std::numbers::pi);
    }

    Bend::Bend(const BendParams& params,
        std::shared_ptr<ThermoProperties> thermo)
        : params_(params), thermo_(thermo)
    {}

    BendResults Bend::simulate() const {
        BendResults res;
        // шапка рез-а: единственный сегмент, длина — угол*радиус
        double L = params_.bendRadius * params_.bendAngle;
        double D = params_.segments.front().diameter;
        double area = std::numbers::pi * D * D / 4.0;

        // свойства в входе
        double p = params_.pressure;
        double T = params_.temperature;

        // считаем скорость и rho по входу
        ThermoInput tin{ T, p, {} };
        ThermoOutput tout;
        thermo_->compute(tin, tout);
        double rho = tout.density;
        double v = computeVelocity(rho, area);

        // локальные потери на изгиб
        double xi = xiForAngle(params_.bendAngle);
        double dp_bend = computeLocalLoss(xi, rho, v);
        p -= dp_bend;

        // заполняем одну точку (можно разбить на шаги аналогично трубе)
        res.points.push_back( PointResult{ 0.0, p, T, v, computeReynolds(rho,v,D,tout.viscosity), 0.0 } );
        res.localLoss = dp_bend;
        res.totalPressureDrop = dp_bend;
        res.totalHeatTransfer = 0.0;

        return res;
    }

    double Bend::computeVelocity(double rho, double area) const {
        return params_.massFlowRate / (rho * area);
    }

    double Bend::computeReynolds(double rho, double velocity, double diameter, double viscosity) const {
        return (viscosity == 0 ? 0 : rho * velocity * diameter / viscosity);
    }

    double Bend::computeFrictionFactor(double Re) const {
        if (Re < 2300) return 64.0 / Re;
        return 0.3164 * std::pow(Re, -0.25);
    }

    double Bend::computeLocalLoss(double xi, double rho, double v) const {
        return xi * (rho * v * v / 2.0);
    }

    //void calculateBend(const BendParams& in, BendResults& out) {
    //    auto thermo = std::make_shared<MockThermo>(); // или ваша CoolProp-реализация
    //    Bend bend(in, thermo);
    //    out = bend.simulate();
    //}
}