#pragma once
#include "pch.h"
#include "config.h"
#include <nlohmann/json.hpp>

using json = nlohmann::ordered_json;

namespace d2gl {

int Config::GetInt(std::string sectionKey, std::string key, int def, int min = INT_MIN, int max = INT_MAX)
{
	int val = def;
	if (jsonConfig.contains(sectionKey) && jsonConfig[sectionKey].contains(key)) {
		try {
			val = jsonConfig[sectionKey][key].template get<int>();
		} 
		catch (const std::exception& e) {
			// Handle conversion error here
			error_log("Error converting int config key '%s' in section '%s', check your values! Exception: %s", key.c_str(), sectionKey.c_str(), e.what());
		}
	}

	if (min != INT_MIN && val < min) {
		val = min;
	}

	if (max != INT_MAX && val > max) {
		val = max;
	}

	return val;
}

float Config::GetFloat(std::string sectionKey, std::string key, float def, float min, float max)
{
	float val = def;
	if (jsonConfig.contains(sectionKey) && jsonConfig[sectionKey].contains(key)) {
		try {
			val = jsonConfig[sectionKey][key].template get<float>();
		} 
		catch (const std::exception& e) {
			// Handle conversion error here
			error_log("Error converting float config key '%s' in section '%s', check your values! Exception: %s", key.c_str(), sectionKey.c_str(), e.what());
		}
	}

	if (val < min) {
		val = min;
	}

	if (val > max) {
		val = max;
	}

	return val;
}

bool Config::GetBool(std::string sectionKey, std::string key, bool def)
{
	bool val = def;
	if (jsonConfig.contains(sectionKey) && jsonConfig[sectionKey].contains(key)) {
		try {
			val = jsonConfig[sectionKey][key].template get<bool>();
		} 
		catch (const std::exception& e) {
			// Handle conversion error here
			error_log("Error converting bool config key '%s' in section '%s', check your values! Exception: %s", key.c_str(), sectionKey.c_str(), e.what());
		}
	}
	return val;
}

std::string Config::GetString(std::string sectionKey, std::string key, const std::string& def)
{
	std::string val = def;
	if (jsonConfig.contains(sectionKey) && jsonConfig[sectionKey].contains(key)) {
		try {
			val = jsonConfig[sectionKey][key].template get<std::string>();

		} catch (const std::exception& e) {
			// Handle conversion error here
			error_log("Error converting string config key '%s' in section '%s', check your values! Exception: %s", key.c_str(), sectionKey.c_str(), e.what());
		}
	}
	return val;
}

void Config::SaveConfig()
{
	// Screen Tab
	json jsonScreen;
	jsonScreen["window_size_width"] = App.window.size.x;
	jsonScreen["window_size_height"] = App.window.size.y;
	jsonScreen["window_centered"] = App.window.centered;
	jsonScreen["window_position_x"] = App.window.position.x;
	jsonScreen["window_position_y"] = App.window.position.y;
	jsonScreen["unlock_cursor"] = App.cursor.unlock;
	jsonScreen["window_fullscreen"] = App.window.fullscreen;
	jsonScreen["window_vsync"] = App.vsync;
	jsonScreen["foreground_fps_active"] = App.foreground_fps.active;
	jsonScreen["foreground_fps_value"] = App.foreground_fps.range.value;
	jsonScreen["background_fps_active"] = App.background_fps.active;
	jsonScreen["background_fps_value"] = App.background_fps.range.value;
	jsonScreen["auto_minimize"] = App.window.auto_minimize;
	jsonScreen["window_dark_mode"] = App.window.dark_mode;
	jsonConfig["screen"] = jsonScreen;

	// Graphics Tab
	json jsonGraphics;
	jsonGraphics["shader_preset"] = App.shader.preset;
	jsonGraphics["sharpen"] = App.sharpen.active;
	jsonGraphics["sharpen_strength"] = App.sharpen.strength.value;
	jsonGraphics["sharpen_clamp"] = App.sharpen.clamp.value;
	jsonGraphics["sharpen_radius"] = App.sharpen.radius.value;
	jsonGraphics["fxaa"] = App.fxaa.active;
	jsonGraphics["fxaa_preset"] = App.fxaa.presets.selected;
	jsonGraphics["lut"] = App.lut.selected;
	jsonGraphics["bloom"] = App.bloom.active;
	jsonGraphics["bloom_exposure"] = App.bloom.exposure.value;
	jsonGraphics["bloom_gamma"] = App.bloom.gamma.value;
	jsonGraphics["stretched_horizontal"] = App.viewport.stretched.x;
	jsonGraphics["stretched_vertical"] = App.viewport.stretched.y;
	jsonConfig["graphics"] = jsonGraphics;

	// Features Tab
	json jsonFeatures;
	jsonFeatures["hd_cursor"] = App.hd_cursor;
	jsonFeatures["hd_text"] = App.hd_text.active;
	jsonFeatures["hd_text_scale"] = App.hd_text.scale.value;
	jsonFeatures["mini_map"] = App.mini_map.active;
	jsonFeatures["mini_map_text_over"] = App.mini_map.text_over;
	jsonFeatures["mini_map_width"] = App.mini_map.width.value;
	jsonFeatures["mini_map_height"] = App.mini_map.height.value;
	jsonFeatures["motion_prediction"] = App.motion_prediction;
	jsonFeatures["skip_intro"] = App.skip_intro;
	jsonFeatures["no_pickup"] = App.no_pickup;
	jsonFeatures["show_item_quantity"] = App.show_item_quantity;
	jsonFeatures["show_fps"] = App.show_fps;
	// jsonFeatures["hd_orbs"] = App.hd_orbs.active;
	// jsonFeatures["hd_orbs_centered"] = App.hd_orbs.centered;
	jsonConfig["features"] = jsonFeatures;

	// Other
	json jsonOther;
	jsonOther["gl_ver_major"] = (int)App.gl_ver.x;
	jsonOther["gl_ver_minor"] = (int)App.gl_ver.y;
	jsonOther["use_compute_shader"] = App.use_compute_shader;
	jsonOther["frame_latency"] = App.frame_latency;
	jsonOther["load_dlls_early"] = App.dlls_early;
	jsonOther["load_dlls_late"] = App.dlls_late;
	jsonConfig["other"] = jsonOther;

	std::ofstream fout;
	fout.open(App.json_file, std::ofstream::out | std::ofstream::trunc);
	fout << std::setw(4) << jsonConfig << std::endl;
	fout.close();
}

void Config::LoadConfig()
{
	bool bCreateFile = false;
	std::ifstream ifile(App.json_file);
	if (!ifile) {
		std::ofstream output(App.json_file);
		output << "{}";
		bCreateFile = true;
	}

	jsonConfig = json::parse(std::ifstream{ App.json_file });

	// Setup Color Grading profiles
	App.lut.items.push_back({ "Game Default" });
	for (int i = 1; i <= 14; i++) {
		char label[50] = { 0 };
		sprintf_s(label, 50, "Color #%d", i);
		App.lut.items.push_back({ label });
	}

	// Resolution
	App.desktop_resolution = {
		GetSystemMetrics(SM_XVIRTUALSCREEN),
		GetSystemMetrics(SM_YVIRTUALSCREEN),
		GetSystemMetrics(SM_CXVIRTUALSCREEN),
		GetSystemMetrics(SM_CYVIRTUALSCREEN)
	};

	// Screen Tab
	App.window.size = App.desktop_resolution;
	App.window.size.x = d2gl::Config::GetInt("screen", "window_size_width", App.window.size.x, 800, App.desktop_resolution.z);
	App.window.size.y = d2gl::Config::GetInt("screen", "window_size_height", App.window.size.y, 600, App.desktop_resolution.w);
	App.window.size_save = App.window.size;
	App.window.centered = d2gl::Config::GetBool("screen", "window_centered", true);
	App.window.position.x = d2gl::Config::GetInt("screen", "window_position_x", App.window.position.x, App.desktop_resolution.x, App.desktop_resolution.z);
	App.window.position.y = d2gl::Config::GetInt("screen", "window_position_y", App.window.position.y, App.desktop_resolution.y, App.desktop_resolution.w);
	App.cursor.unlock = d2gl::Config::GetBool("screen", "unlock_cursor", App.cursor.unlock);

	App.window.fullscreen = d2gl::Config::GetBool("screen", "window_fullscreen", true);
	App.vsync = d2gl::Config::GetBool("screen", "window_vsync", true);
	App.foreground_fps.active = d2gl::Config::GetBool("screen", "foreground_fps_active", true);
	App.foreground_fps.range.value = d2gl::Config::GetInt("screen", "foreground_fps_value", App.foreground_fps.range.value, App.foreground_fps.range.min, App.foreground_fps.range.max);
	App.background_fps.active = d2gl::Config::GetBool("screen", "background_fps_active", true);
	App.background_fps.range.value = d2gl::Config::GetInt("screen", "background_fps_value", App.background_fps.range.value, App.background_fps.range.min, App.background_fps.range.max);
	App.window.auto_minimize = d2gl::Config::GetBool("screen", "auto_minimize", App.window.auto_minimize);
	App.window.dark_mode = d2gl::Config::GetBool("screen", "window_dark_mode", true);

	// Graphics Tab
	App.shader.preset = d2gl::Config::GetString("graphics", "shader_preset", App.shader.preset);

	App.sharpen.active = d2gl::Config::GetBool("graphics", "sharpen", App.sharpen.active);
	App.sharpen.strength.value = d2gl::Config::GetFloat("graphics", "sharpen_strength", App.sharpen.strength.value, App.sharpen.strength.min, App.sharpen.strength.max);
	App.sharpen.clamp.value = d2gl::Config::GetFloat("graphics", "sharpen_clamp", App.sharpen.clamp.value, App.sharpen.clamp.min, App.sharpen.clamp.max);
	App.sharpen.radius.value = d2gl::Config::GetFloat("graphics", "sharpen_radius", App.sharpen.radius.value, App.sharpen.radius.min, App.sharpen.radius.max);
	App.fxaa.active = d2gl::Config::GetBool("graphics", "fxaa", App.fxaa.active);
	App.fxaa.presets.selected = d2gl::Config::GetInt("graphics", "fxaa_preset", App.fxaa.presets.selected, 0, 2);

	App.lut.selected = d2gl::Config::GetInt("graphics", "lut", App.lut.selected, 0, App.lut.items.size() - 1);
	App.bloom.active = d2gl::Config::GetBool("graphics", "bloom", App.bloom.active);
	App.bloom.exposure.value = d2gl::Config::GetFloat("graphics", "bloom_exposure", App.bloom.exposure.value, App.bloom.exposure.min, App.bloom.exposure.max);
	App.bloom.gamma.value = d2gl::Config::GetFloat("graphics", "bloom_gamma", App.bloom.gamma.value, App.bloom.gamma.min, App.bloom.gamma.max);
	App.viewport.stretched.x = d2gl::Config::GetBool("graphics", "stretched_horizontal", App.viewport.stretched.x);
	App.viewport.stretched.y = d2gl::Config::GetBool("graphics", "stretched_vertical", App.viewport.stretched.y);

	// Features Tab
	App.hd_cursor = d2gl::Config::GetBool("features", "hd_cursor", App.hd_cursor);
	App.hd_text.active = d2gl::Config::GetBool("features", "hd_text", App.hd_text.active);
	App.hd_text.scale.value = ISHDTEXT() ? 1.0f : d2gl::Config::GetFloat("features", "hd_text_scale", App.hd_text.scale.value, 0.8f, 1.2f);
	App.mini_map.active = d2gl::Config::GetBool("features", "mini_map", App.mini_map.active) && ISGLIDE3X();
	App.mini_map.text_over = d2gl::Config::GetBool("features", "mini_map_text_over", App.mini_map.text_over) && ISGLIDE3X();
	App.mini_map.width.value = d2gl::Config::GetInt("features", "mini_map_width", App.mini_map.width.value, App.mini_map.width.min, App.mini_map.width.max);
	App.mini_map.height.value = d2gl::Config::GetInt("features", "mini_map_height", App.mini_map.height.value, App.mini_map.height.min, App.mini_map.height.max);
	//App.hd_orbs.active = d2gl::Config::GetBool("features", "hd_orbs", App.hd_orbs.active);
	//App.hd_orbs.centered = d2gl::Config::GetBool("features", "hd_orbs_centered", App.hd_orbs.centered);

	App.motion_prediction = d2gl::Config::GetBool("features", "motion_prediction", App.motion_prediction);
	App.skip_intro = d2gl::Config::GetBool("features", "skip_intro", App.skip_intro);
	App.no_pickup = d2gl::Config::GetBool("features", "no_pickup", App.no_pickup);
	App.show_item_quantity = d2gl::Config::GetBool("features", "show_item_quantity", App.show_item_quantity);
	App.show_fps = d2gl::Config::GetBool("features", "show_fps", App.show_fps);

	// Other
	App.gl_ver.x = d2gl::Config::GetInt("other", "gl_ver_major", App.gl_ver.x, 3, 4);
	App.gl_ver.y = d2gl::Config::GetInt("other", "gl_ver_minor", App.gl_ver.y, 0, 6);
	if (App.gl_ver.x == 3) {
		App.gl_ver.y = 3;
	}

	App.use_compute_shader = d2gl::Config::GetBool("other", "use_compute_shader", App.use_compute_shader);
	App.frame_latency = d2gl::Config::GetInt("other", "frame_latency", App.frame_latency, 1, 5);
	App.dlls_early = d2gl::Config::GetString("other", "load_dlls_early", App.dlls_early);
	App.dlls_late = d2gl::Config::GetString("other", "load_dlls_late", App.dlls_late);


	// Setup resolutions
	std::vector<std::pair<uint32_t, uint32_t>> window_sizes = {
		{ 800, 600 },
		{ 960, 720 },
		{ 1024, 768 },
		{ 1200, 900 },
		{ 1280, 960 },
		{ 1440, 1080 },
		{ 1600, 1200 },
		{ 1920, 1440 },
		{ 2560, 1920 },
		{ 2732, 2048 },
		{ 1068, 600 },
		{ 1280, 720 },
		{ 1600, 900 },
		{ 1920, 1080 },
		{ 2048, 1152 },
		{ 2560, 1440 },
		{ 3200, 1800 },
		{ 3840, 2160 },
	};

	int index = 1;
	App.resolutions.items.push_back({ "Custom Size", glm::uvec2(0, 0) });
	for (size_t i = 0; i < window_sizes.size(); i++) {
		const auto& p = window_sizes[i];
		if (App.desktop_resolution.z < p.first || App.desktop_resolution.w < p.second)
			continue;

		char label[50] = { 0 };
		bool is_4x3 = p.first / 4 == p.second / 3;
		sprintf_s(label, 50, "%d x %d (%s)", p.first, p.second, (is_4x3 ? "4:3" : "16:9"));
		App.resolutions.items.push_back({ label, glm::uvec2(p.first, p.second) });

		if (App.window.size.x == p.first && App.window.size.y == p.second)
			App.resolutions.selected = index;
		index++;
	}

	App.presets.items.push_back({ "Ultra", options_preset("Ultra", true, true, 2, true, true, true) });
	App.presets.items.push_back({ "High", options_preset("High", true, true, 1, true, true, true) });
	App.presets.items.push_back({ "Medium", options_preset("Medium", false, true, 0, false, false, true) });
	App.presets.items.push_back({ "Low", options_preset("Low", false, false, 0, false, false, false) });
	App.presets.items.push_back({ "Custom", options_preset("Custom", true, true, 2, true, true, true) });

	if (bCreateFile) {
		SaveConfig();
	}
}
}
