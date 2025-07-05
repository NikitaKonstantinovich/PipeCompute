#pragma once

#include <nlohmann/json.hpp>
#include <string>


namespace PipeCompute {
	/**
	 * @param j      JSON-объект
	 * @param path   Путь к выходному файлу
	 */
	void writeJson(const nlohmann::json& root, const std::string& path);

	/**
	 * Формирует CSV со строкой заголовков из всех ключей первого элемента "steps",
	 * а затем по одному ряду на каждый элемент.
	 */
	void writeCsv(const nlohmann::json& root, const std::string& path);
}