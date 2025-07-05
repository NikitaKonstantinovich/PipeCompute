#pragma once

#include "PipeCompute/ConfigParser.hpp"  // ��� ElementConfig
#include "PipeCompute/Pipe.hpp"          // ��� Segment, PipeSettings
#include "PipeCompute/Results.hpp"       // ��� PointResult

namespace PipeCompute {

    /** ��������� ������: �������� (��) � ����������� (K) */
    struct StreamState {
        double pressure;
        double temperature;
    };

    /** �������� Segment �� JSON-������� */
    Segment makeSegment(const ElementConfig& e);

    /** ��������� Pipe � ���������� ��������� ����� */
    PointResult runPipe(const Segment& seg, PipeSettings& settings);

    /** �������� ��������� ������ ������� */
    void logPipeResult(double length, const PointResult& pr);

    /** ������� ������ ��� main: ��������� st � �������� */
    inline void simulatePipe(const ElementConfig& e,
        StreamState& st,
        PipeSettings& settings)
    {
        // 1) ��������� �������
        Segment seg = makeSegment(e);

        // 2) ����������� ������� �������
        settings.initialPressure = st.pressure;
        settings.initialTemperature = st.temperature;

        // 3) ��������� Pipe � ����� ��������� �����
        PointResult pr = runPipe(seg, settings);

        // 4) �������� ��������� ������
        st.pressure = pr.pressure;
        st.temperature = pr.temperature;

        // 5) ������� ���������
        logPipeResult(e.length, pr);
    }

} // namespace PipeCompute
