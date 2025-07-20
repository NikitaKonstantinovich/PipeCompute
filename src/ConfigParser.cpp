#include "PipeCompute/ConfigParser.hpp"
#include <fstream>
#include <nlohmann/json.hpp>
#include <iostream>
#include <stdexcept>

using json = nlohmann::json;

namespace PipeCompute {

    Config ConfigParser::load(const std::string& jsonPath) {
        std::ifstream in(jsonPath);
        if (!in.is_open()) {
            throw std::runtime_error("Cannot open config file: " + jsonPath);
        }
        nlohmann::json j;
        try {
            // читаем из потока, игнорируя комментарии (ignore_comments = true)
            j = nlohmann::json::parse(
                in,
                /*callback*/ nullptr,
                /*allow_exceptions*/ true,
                /*ignore_comments*/ true
            );
        }
        catch (nlohmann::json::parse_error& ex) {
            // Выведем, на каком байте и почему парсер упал:
            std::cerr << "JSON parse error at byte " << ex.byte
                << ": " << ex.what() << "\n";
            throw;  // пробрасываем дальше
        }

        Config cfg;
        auto jg = j.at("global");
        cfg.global.massFlowRate = jg.at("massFlowRate").get<double>();
        cfg.global.ambientTemperature = jg.at("ambientTemperature").get<double>();
        cfg.global.step = jg.at("step").get<double>();
        cfg.global.initialPressure = jg.at("initialPressure").get<double>();
        cfg.global.initialTemperature = jg.at("initialTemperature").get<double>();

        auto jt = j.at("thermo");
        std::string model = jt.value("model", "");
        if (model == "ideal_gas") {
            auto ig = std::make_unique<IdealGasConfig>();
            ig->load(jt);
            cfg.thermo = std::move(ig);
        }
        else if (model == "liquid") {
            auto lf = std::make_unique<LiquidConfig>();
            lf->load(jt);
            cfg.thermo = std::move(lf);
        }
        else {
            throw std::runtime_error("Unknown thermo.model: " + model);
        }

        for (const auto& je : j.at("elements")) {
            ElementConfig e;
            e.type = je.at("type").get<std::string>();

            if (e.type == "pipe") {
                e.length = je.at("length").get<double>();
                e.diameter = je.at("diameter").get<double>();
                e.wallThickness = je.value("wallThickness", 0.0); 
				e.roughness = je.value("roughness", 0.0);
                
				e.wallConductivity = je.value("wallConductivity", 50.0); // вт/(м·К)
                e.hOuter = je.value("hOuter", 10.0); // Вт/(м?·К)
            }
            else if (e.type == "bend") {
                e.bendAngle = je.at("angle").get<double>();
                e.bendRadius = je.at("radius").get<double>();

                // геометрия сегмента для диаметра и толщи стенки
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
