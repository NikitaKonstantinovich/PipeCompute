#include "PipeCompute/ConfigParser.hpp"
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace PipeCompute {

    Config ConfigParser::load(const std::string& jsonPath) {
        std::ifstream in(jsonPath);
        if (!in.is_open()) {
            throw std::runtime_error("Cannot open config file: " + jsonPath);
        }
        json j; in >> j;

        Config cfg;
        auto jg = j.at("global");
        cfg.global.massFlowRate = jg.at("massFlowRate").get<double>();
        cfg.global.ambientTemperature = jg.at("ambientTemperature").get<double>();
        cfg.global.heatTransferCoeff = jg.at("heatTransferCoeff").get<double>();
        cfg.global.step = jg.at("step").get<double>();

        for (const auto& je : j.at("elements")) {
            ElementConfig e;
            e.type = je.at("type").get<std::string>();

            if (e.type == "pipe") {
                e.length = je.at("length").get<double>();
                e.diameter = je.at("diameter").get<double>();
                e.wallThickness = je.value("wallThickness", 0.0);  // необязательное поле
            }
            else if (e.type == "bend") {
                e.bendAngle = je.at("angle").get<double>();
                e.bendRadius = je.at("radius").get<double>();

                // геометрия сегмента для диаметра и толщи стенки
                e.diameter = je.value("diameter", 0.0);
                e.wallThickness = je.value("wallThickness", 0.0);
            }
            else if (e.type == "tee") {
                e.branchDiameter = je.at("branchDiameter").get<double>();
                // тоже имеет основное сечение трубы:
                e.diameter = je.value("diameter", 0.0);
                e.wallThickness = je.value("wallThickness", 0.0);
            }
            else {
                throw std::runtime_error("Unknown element type: " + e.type);
            }

            cfg.elements.push_back(e);
        }
        return cfg;
    }

} 
