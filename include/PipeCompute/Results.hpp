#pragma once
#include<vector>
#include "Types.hpp"

namespace PipeCompute {
	/** ���������� � ����� ����� ����� ����� */
	struct PointResult {
		double position; // ������� ����� �����, �
		double pressure; // ��������, ��
		double temperature; // �����������, �
		double velocity; // ��������, �/�
		double reynolds; // ����� ����������
		double nusselt; // ����� ���������
	};

	/** ����� ��� CalculatePipe */
	struct PipeResults {
		std::vector<PointResult> points;
		double totalPressureDrop; // ��������� ������� ��������, ��
		double totalHeatTransfer; // ��������� ����������, ��
	};

	/** ����� ��� CalculateBend */
	struct BendResults : PipeResulats {
		double localLoss; // ��������� ������ �������� � ������, ��
	};

	/** ����� ��� CalculateTee */
	struct TeeResults : PipeResults {
		ddouble branchFlowRate; // ������ � �����������, ��/�
	};
}