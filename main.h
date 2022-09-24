#pragma once

#include <BML/BMLAll.h>
#include <chrono>
#include <memory>

extern "C" {
	__declspec(dllexport) IMod* BMLEntry(IBML* bml);
}

class RealTimeMeter : public IMod {
public:
	RealTimeMeter(IBML* bml) : IMod(bml) {};

	virtual CKSTRING GetID() override { return "RealTimeMeter"; }
	virtual CKSTRING GetVersion() override { return "0.1.0"; }
	virtual CKSTRING GetName() override { return "Real Time Meter"; }
	virtual CKSTRING GetAuthor() override { return "BallanceBug"; }
	virtual CKSTRING GetDescription() override { return "Measure the actual time elapsed during your gameplays."; }
	DECLARE_BML_VERSION;

	void OnLoad() override;
	void OnModifyConfig(CKSTRING category, CKSTRING key, IProperty* prop) override;
	void OnLoadObject(CKSTRING filename, BOOL isMap, CKSTRING masterName,
										CK_CLASSID filterClass, BOOL addtoscene, BOOL reuseMeshes, BOOL reuseMaterials,
										BOOL dynamic, XObjectArray* objArray, CKObject* masterObj) override;
	void OnPostResetLevel() override;
	void OnPreExitLevel() override;
	void OnStartLevel() override;
	void OnLevelFinish() override;
	void OnCounterInactive() override;
	void OnCounterActive() override; 
	void OnProcess() override;

	void init_status_label();
	void format_time(float duration, char* buf);
	void update_current_level();
	void update_status(float bml_time, float sys_time);

private:
	std::unique_ptr<BGui::Label> status;
	bool enabled = false, counting = false;
	bool level_finished = false, level_exited = true, streak_completed = false;
	float bml_time = 0;
	uint64_t sys_time = 0;
	CKTimeManager *time_manager = nullptr;
	IProperty *prop_enable = nullptr, *prop_streak = nullptr;
	int streak_level = 0, current_level = 0;
};
