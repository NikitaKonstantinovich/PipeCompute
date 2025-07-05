#include "PipeCompute/ConfigParser.hpp"
#include <fstream>
#include <nlohmann/json.hpp>
#include <iostream>

using json = nlohmann::json;

namespace PipeCompute {

    Config ConfigParser::load(const std::string& jsonPath) {
        std::ifstream in(jsonPath);
        if (!in.is_open()) {
            throw std::runtime_error("Cannot open config file: " + jsonPath);
        }
        nlohmann::json j;
        try {
            in >> j;
        }
        catch (nlohmann::json::parse_error& ex) {
            // ¬ыведем, на каком байте и почему парсер упал:
            std::cerr << "JSON parse error at byte " << ex.byte
                << ": " << ex.what() << "\n";
            throw;  // пробрасываем дальше
        }

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
                e.wallThickness = je.value("wallThickness", 0.0);  // необ€зательное поле
            }
            else if (e.type == "bend") {
                e.bendAngle = je.at("angle").get<double>();
                e.bendRadius = je.at("radius").get<double>();

                // геометри€ сегмента дл€ диаметра и толщи стенки
                e.diameter = je.value("diameter", 0.0);
                e.wallThickness = je.value("wallThickness", 0.0);
            }
            else if (e.type == "tee") {
                e.diameter = je.at("diameter").get<double>();
                e.mainBranchDiameter = je.at("mainBranchDiameter").get<double>();
                e.sideBranchDiameter = je.at("sideBranchDiameter").get<double>();
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
