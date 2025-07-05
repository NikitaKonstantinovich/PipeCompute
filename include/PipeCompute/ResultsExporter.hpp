#pragma once

#include <nlohmann/json.hpp>
#include <string>


namespace PipeCompute {
	/**
	 * @brief Сериализует JSON-объект в файл
	 * @param j      JSON-объект
	 * @param path   Путь к выходному файлу
	 */
	void writeJson(const nlohmann::json& j, const std::string& path);
}