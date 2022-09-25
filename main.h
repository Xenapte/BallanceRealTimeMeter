#pragma once

#include <BMLPlus/BMLAll.h>
#include <chrono>
#include <memory>

typedef const char* C_CKSTRING;

extern "C" {
	__declspec(dllexport) IMod* BMLEntry(IBML* bml);
}

class RealTimeMeter : public IMod {
public:
	RealTimeMeter(IBML* bml) : IMod(bml) {};

	virtual C_CKSTRING GetID() override { return "RealTimeMeter"; }
	virtual C_CKSTRING GetVersion() override { return "0.1.1"; }
	virtual C_CKSTRING GetName() override { return "Real Time Meter"; }
	virtual C_CKSTRING GetAuthor() override { return "BallanceBug"; }
	virtual C_CKSTRING GetDescription() override { return "Measure the actual time elapsed during your gameplays."; }
	DECLARE_BML_VERSION;

	void OnLoad() override;
	void OnModifyConfig(C_CKSTRING category, C_CKSTRING key, IProperty* prop) override;
	void OnLoadObject(C_CKSTRING filename, CKBOOL isMap, C_CKSTRING masterName,
										CK_CLASSID filterClass, CKBOOL addtoscene, CKBOOL reuseMeshes, CKBOOL reuseMaterials,
										CKBOOL dynamic, XObjectArray* objArray, CKObject* masterObj) override;
	void OnPostResetLevel() override;
	void OnPreLoadLevel() override;
	void OnPreExitLevel() override;
	void OnStartLevel() override;
	void OnLevelFinish() override;
	void OnCounterInactive() override;
	void OnCounterActive() override; 
	void OnProcess() override;

	void init_status_label();
	void format_time(float duration, char* buf);
	void update_status(float bml_time_ms, float sys_time_ms);

private:
	std::unique_ptr<BGui::Label> status;
	bool enabled = false, counting = false;
	bool level_finished = false, level_exited = true, streak_completed = false;
	float bml_time = 0;
	int64_t sys_time = 0, last_finish_sys_time = 0;
	CKTimeManager *time_manager = nullptr;
	IProperty *prop_enable = nullptr, *prop_streak = nullptr;
	int streak_level = 0, current_level = 0;
};
