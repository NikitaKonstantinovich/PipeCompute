#include "PipeCompute/PipeSimulator.hpp"
#include <iostream>

namespace PipeCompute {

    Segment makeSegment(const ElementConfig& e) {
        Segment s{};
        s.x0 = s.y0 = s.z0 = 0.0;
        s.x1 = e.length;
        s.y1 = s.z1 = 0.0;
        s.diameter = e.diameter;
        s.wallThickness = e.wallThickness;
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