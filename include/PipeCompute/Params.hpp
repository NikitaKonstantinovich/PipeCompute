#pragma once
#include <vector>
#include "Types.hpp"

namespace PipeCompute {
	/** �������� ������ �������� ����� */
	struct Segment {
		double x0, y0, z0; // ���������� ������
		double x1, y1, z1; // ���������� �����
		double diameter;   // �������, �
		double wallThickness; // ������� ������, �
	};
}

/** ��������� ��� CalculatePipe */
struct PipeParams {
	std::vector<Segment> segments; // �������� �����
	MediumType medium; // ��� �����
	double temperature; // ����������� ����� ���������, �
	double pressure;    // �������� ���������, ��
	double massFlowRate; // �������� ������, ��/�
	double ambientTemp; // ����������� ���������� �����, �
	double step; // ��� ������������� ����� �����, �
	UnitSystem units; // ������� ������ ���������
};

/** ��������� ��� CalculateBend */
struct BendParams : PipeParams {
	double bendAngle; // ���� ��������, ���
	double bendRadius; // ������ ������, �
};

/** ��������� ��� calculateTee (������������) */
struct TeeParams : PipeParams {
	double branchAngle; // ���� �����, ���
	double branchDiameter; // ������� �����, �
};;