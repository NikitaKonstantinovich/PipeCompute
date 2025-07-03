#pragma once
#include "Types.hpp"
#include "Params.hpp"
#include "Results.hpp"

namespace PipeCompute {
	/**
	* �������� ������� ������� ���������� � �����������
	*/
	void calculatePipe(const PipeParams& in, PipeResults& out);
	void calculateBend(const BendParams& in, BendResults& out);
	void calculateTee(const TeeParams& in, TeeResults& out);
}