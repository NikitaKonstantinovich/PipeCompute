#include "PipeCompute/ResultsExporter.hpp"
#include <fstream>

namespace PipeCompute {

    void writeJson(const nlohmann::json& root, const std::string& path) {
        std::ofstream ofs(path);
        if (!ofs) {
            throw std::runtime_error("Cannot open file for writing: " + path);
        }
        ofs << root.dump(4) << std::endl;
    }

    void writeCsv(const nlohmann::json& root, const std::string& path) {
        // Проверяем, что есть массив steps
        if (!root.contains("steps") || !root["steps"].is_array()) {
            throw std::runtime_error("JSON has no array \"steps\" for CSV export");
        }
        const auto& steps = root["steps"];
        if (steps.empty()) {
            std::ofstream ofs(path); // просто создадим пустой файл
            return;
        }

        // Собираем набор всех ключей из первого элемента (чтобы выровнять колонки)
        const auto& first = steps[0];
        std::vector<std::string> headers;
        headers.reserve(first.size());
        for (auto it = first.begin(); it != first.end(); ++it) {
            headers.push_back(it.key());
        }

        std::ofstream ofs(path);
        if (!ofs) throw std::runtime_error("Cannot open file for CSV export: " + path);

        // Записываем заголовок
        for (size_t i = 0; i < headers.size(); ++i) {
            if (i) ofs << ",";
            ofs << headers[i];
        }
        ofs << "\n";

        // Записываем строки
        for (const auto& elem : steps) {
            for (size_t i = 0; i < headers.size(); ++i) {
                if (i) ofs << ",";
                const auto& key = headers[i];
                if (elem.contains(key)) {
                    const auto& v = elem[key];
                    if (v.is_string()) {
                        // Оборачиваем кавычками на всякий случай
                        ofs << "\"" << v.get<std::string>() << "\"";
                    }
                    else {
                        ofs << v;
                    }
                }
            }
            ofs << "\n";
        }
    }

}
