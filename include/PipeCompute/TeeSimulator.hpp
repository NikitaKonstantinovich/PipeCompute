#pragma once

#include "PipeCompute/PipeSimulator.hpp"      // ��� StreamState
#include "PipeCompute/Params.hpp"             // ��� ElementConfig
#include "PipeCompute/Tee.hpp"                // ��� TeeParams � Tee
#include <memory>

namespace PipeCompute {

    /**
     * @brief ��������� �������� (Tee) � ����������� ���� ������
     * @param e         �������� �������� �� ������� (diameter, branchDiameter, wallThickness)
     * @param st        ��������� ������ (�������� � �����������) � �����������
     * @param settings  ����� ��������� ��� ����� (massFlowRate, step � ��.)
     * @param thermo    ������ �������������
     */
    void simulateTee(const ElementConfig& e, StreamState& st,const PipeSettings& settings,
        std::shared_ptr<ThermoProperties> thermo);
} 
