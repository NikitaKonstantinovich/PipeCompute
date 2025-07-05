#include "PipeCompute/ResultsExporter.hpp"
#include <fstream>

namespace PipeCompute {

    void writeJson(const nlohmann::json& j, const std::string& path) {
        std::ofstream ofs(path);
        if (!ofs) {
            throw std::runtime_error("Cannot open file for writing: " + path);
        }
        ofs << j.dump(4) << std::endl;
    }

}
