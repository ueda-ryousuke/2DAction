#include "GimmickManager.h"
#include "FrustumCuller.h"

using namespace KamataEngine;

void GimmickManager::Clear() {
	gimmicks_.clear();
	solids_.clear();
	switches_.clear();
	goalReached_ = false;
}

void GimmickManager::Add(std::unique_ptr<Gimmick> gimmick) { gimmicks_.push_back(std::move(gimmick)); }

bool GimmickManager::IsSwitchOn(int channel) const {
	auto it = switches_.find(channel);
	return it != switches_.end() && it->second;
}

void GimmickManager::Update(GimmickContext& ctx) {
	ctx.manager = this;
	for (auto& g : gimmicks_) {
		g->Update(ctx);
	}
	solids_.clear();
	for (auto& g : gimmicks_) {
		SolidBox s;
		if (g->GetSolid(s)) {
			s.owner = g.get();
			solids_.push_back(s);
		}
	}
}

void GimmickManager::Draw(const Camera& camera, const FrustumCuller& culler) {
	for (auto& g : gimmicks_) {
		if (culler.IsVisible(g->Position(), g->CullRadius())) {
			g->Draw(camera);
		}
	}
}

bool GimmickManager::CheckHazard(const Aabb& victim, int& damage, Vector3& sourcePos) const {
	for (const auto& g : gimmicks_) {
		Aabb box;
		int dmg = 0;
		if (g->GetHazard(box, dmg) && Intersects(victim, box)) {
			damage = dmg;
			sourcePos = box.Center();
			return true;
		}
	}
	return false;
}
