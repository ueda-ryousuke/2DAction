#include "ModelLibrary.h"

using namespace KamataEngine;

ModelLibrary& ModelLibrary::Get() {
	static ModelLibrary instance;
	return instance;
}

void ModelLibrary::LoadAll() {
	if (!models_.empty()) {
		return;
	}
	// Tools/generate_assets.py が出力するモデル名と一致させる
	static const char* kNames[] = {
	    "player",         "enemy_shooter",   "enemy_flyer", "enemy_shield",   "enemy_split",    "bullet_straight", "bullet_homing",
	    "block",          "block_crumble",   "block_ice",   "platform_moving", "door",          "switch",          "ceiling_spike",
	    "crusher",        "chase_wall",      "goal_flag",   "bg_sky",         "bg_hills_far",   "bg_hills_near",   "bg_cloud",
	};
	for (const char* name : kNames) {
		models_[name] = Model::CreateFromOBJ(name, true);
	}
}

Model* ModelLibrary::Find(const std::string& name) const {
	auto it = models_.find(name);
	return it == models_.end() ? nullptr : it->second;
}

void ModelLibrary::Release() {
	for (auto& kv : models_) {
		delete kv.second;
	}
	models_.clear();
}
