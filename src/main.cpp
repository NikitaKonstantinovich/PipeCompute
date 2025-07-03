#include <iostream>
#include "PipeCompute/ThermoProperties.hpp"

namespace PipeCompute {
	// ��������� ���������� ������ main.cpp ��� ������������
	class MockThermo : public ThermoProperties {
	public:
		bool compute(const ThermoInput& in, ThermoOutput& out) const override {
			const double T = in.temperature;
			out.density = 1.2 * (273.15 / T); // ��������� ������� ��� 20�C � 1 ���
			out.viscosity = 1.81e-5; // �������� �������
			out.heatCapacity = 1005; // �������� ������������ ������� ��� ���������� ��������
			out.zFactor = 1.0; // ������ ����������� ��� ���������� ����
			out.enthalpy = out.heatCapacity * (T - 273.15); // ���������
			out.entropy = out.heatCapacity * std::log(T / 273.15); // ��������
			return true;
		}
	};
}

int main() {
	PipeCompute::ThermoInput in;
	in.temperature = 300; // 27�C
	in.pressure = 1e5; // 1 ���

	PipeCompute::ThermoOutput out;
	PipeCompute::MockThermo thermo;

	if (thermo.compute(in, out)) {
		std::cout << "Density: " << out.density << " kg/m^3\n";
		std::cout << "Viscosity: " << out.viscosity << " Pa�s\n";
		std::cout << "Heat Capacity: " << out.heatCapacity << " J/(kg�K)\n";
		std::cout << "Z Factor: " << out.zFactor << "\n";
		std::cout << "Enthalpy: " << out.enthalpy << " J/kg\n";
		std::cout << "Entropy: " << out.entropy << " J/(kg�K)\n";
	} else {
		std::cerr << "Failed to compute thermodynamic properties.\n";
	}
	return 0;
}