#pragma once
#include <vector>
#include <string>

namespace PipeCompute {
	/** ������� ��������� ��� ������� ������������� */
	struct ThermoInput {
		double temperature; // �����������, �
		double pressure;    // ��������, ��
		std::vector<std::pair<std::string, double>> composition; // ������ � ����  "��������" - "������������" {"CH4", 0.95} � ��
	};

	/** ���������� ������� */
	struct ThermoOutput {
		double density; // ���������, ��/�^3
		double viscosity; // ��������, ���
		double heatCapacity; // �������� ������������, ��/(���)
		double zFactor; // ������ �����������
		double enthalpy; // ���������, ��/��
		double entropy; // ��������, ��/(���)
	};

	/** ����������� ��������� (������� �����) */
	class ThermoProperties {
	public:
		virtual ~ThermoProperties() = default;
		
		virtual bool compute(const ThermoInput& in, ThermoOutput& out) const = 0;
	};
}