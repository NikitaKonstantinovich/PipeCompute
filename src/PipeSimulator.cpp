#include "PipeCompute/PipeSimulator.hpp"
#include <iostream>

namespace PipeCompute {

    Segment makeSegment(const ElementConfig& e) {
        Segment s{};
        s.start = e.start;
        s.end = e.end;
        s.diameter = e.diameter;
        s.wallThickness = e.wallThickness;
        s.roughness = e.roughness;
		s.wallConductivity = e.wallConductivity;
		s.hOuter = e.hOuter;
        s.insulationThickness = e.insulationThickness;
        s.insulationConductivity = e.insulationConductivity;
        return s;
    }

    PointResult runPipe(const Segment& seg, PipeSettings& settings) {
        Pipe pipe({ seg }, settings);
        const auto& res = pipe.simulate();
        return res.back();
    }

    void logPipeResult(double length, const PointResult& pr) {
        std::cout << "[pipe] length=" << length
            << "  p=" << pr.pressure / 1e5 << " bar"
            << "  T=" << pr.temperature - 273.15 << " °C\n";
    }
}