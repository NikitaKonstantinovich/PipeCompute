#pragma once
#include "PipeCompute/ThermoProperties.hpp"
#include "PipeCompute/Params.hpp" // ��� Segment
#include "PipeCompute/Results.hpp" // ��� PipeResult
#include <memory>
#include <vector>

namespace PipeCompute {
	struct PipeSettings {
		double initialPressure; // ��������� ��������, ��
		double initialTemperature; // ��������� �����������, �
		double massFlowRate; // �������� ������, ��/�
		double ambientTemperature; // ����������� ���������� �����, �
		double step; // ��� ������������� ����� �����, �
		std::shared_ptr<ThermoProperties> thermo; // ������ ����������������� �������
	};

	class Pipe {
	public:
		Pipe(const std::vector<Segment>& segments, const PipeSettings& settings);
		const std::vector<PointResult>& simulate();

	private:
		std::vector<Segment> segments_; // �������� �����
		PipeSettings settings_; // ��������� �����
		std::vector<PointResult> result_; // ��������� ���������

		void simulateSegment(const Segment& seg, double& currentPressure, double& currentTemperature, double cumulativeLength);

		double computeVelocity(double rho, double area) const;
		double computeReynolds(double rho, double velocity, double diameter, double viscosity) const;
		double computeNusselt(double Re, double Pr) const;
	};
}