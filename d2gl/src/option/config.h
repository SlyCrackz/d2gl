#pragma once
#include <nlohmann/json.hpp>

using json = nlohmann::ordered_json;

namespace d2gl {
class Config {
public:
	void SaveConfig();
	void LoadConfig();

private:
	json jsonConfig;

	int GetInt(std::string sectionKey, std::string key, int def, int min, int max);
	float GetFloat(std::string sectionKey, std::string key, float def, float min, float max);
	bool GetBool(std::string sectionKey, std::string key, bool def);
	std::string GetString(std::string sectionKey, std::string key, const std::string& def);
};
}
