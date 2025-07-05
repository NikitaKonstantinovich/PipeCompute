#pragma once

#include "PipeCompute/PipeSimulator.hpp"  // ��� StreamState
#include "PipeCompute/Params.hpp"         // ��� ElementConfig
#include "PipeCompute/Bend.hpp"           // ��� BendParams � Bend
#include <memory>

namespace PipeCompute {

    /**
     * @brief ��������� ������ (Bend)
     * @param e         �������� �������� �� ������� (bendAngle, bendRadius, diameter, wallThickness)
     * @param st        ������� ��������� ������ (�������� � �����������): �����������
     * @param settings  ����� ��������� (massFlowRate, step)
     * @param thermo    ������ �������������
     */
    void simulateBend(const ElementConfig& e, StreamState& st, const PipeSettings& settings, 
        std::shared_ptr<ThermoProperties> thermo);
} 
