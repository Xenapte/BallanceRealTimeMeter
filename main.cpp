#include "main.h"

IMod* BMLEntry(IBML* bml) {
  return new RealTimeMeter(bml);
}

void RealTimeMeter::OnLoad() {
  time_manager = m_bml->GetTimeManager();
  GetConfig()->SetCategoryComment("Main", "Settings for Real Time Meter.");
  prop_enable = GetConfig()->GetProperty("Main", "Enabled");
  prop_enable->SetComment("Whether to show the timer when ingame.");
  prop_enable->SetDefaultBoolean(true);
  enabled = prop_enable->GetBoolean();
  prop_streak = GetConfig()->GetProperty("Main", "StreakMode");
  prop_streak->SetComment("Only reset the timer after the level set here is finished. 0 to disable. Example: 13 = Timer will only stop after finishing level 13.");
  prop_streak->SetDefaultInteger(0);
  streak_level = prop_streak->GetInteger();
}

void RealTimeMeter::OnModifyConfig(CKSTRING category, CKSTRING key, IProperty* prop) {
  if (prop == prop_enable) {
    enabled = prop_enable->GetBoolean();
    if (enabled) {
      if (m_bml->IsIngame() && !status)
        init_status_label();
    }
    else
      status.reset();
  }
  if (prop == prop_streak) {
    streak_level = prop_streak->GetInteger();
  }
}

void RealTimeMeter::OnLoadObject(CKSTRING filename, BOOL isMap, CKSTRING masterName,
																 CK_CLASSID filterClass, BOOL addtoscene, BOOL reuseMeshes, BOOL reuseMaterials,
																 BOOL dynamic, XObjectArray* objArray, CKObject* masterObj) {
	if (!enabled || !isMap)
    return;
  if (!status)
    init_status_label();
  counting = false;
  update_status(0.0f, 0.0f);
}

void RealTimeMeter::OnPostResetLevel() {
  if (!enabled)
    return;
  if (!status)
    init_status_label();
  counting = false;
  update_status(0.0f, 0.0f);
  streak_completed = false;
}

void RealTimeMeter::OnPreLoadLevel() {
  if (!enabled)
    return;
  if (streak_level > 0 && level_exited) {
    bml_time = 0;
    sys_time = std::chrono::steady_clock::now().time_since_epoch().count()
      - static_cast<double>(time_manager->GetLastDeltaTime()) * 1000000;
    level_exited = false;
  }
}

void RealTimeMeter::OnPreExitLevel() {
  if (!enabled)
    return;
  status.reset();
  level_exited = true;
  level_finished = false;
  counting = false;
  streak_completed = false;
}

void RealTimeMeter::OnStartLevel() {
  if (!enabled)
    return;
  // update current level
  m_bml->GetArrayByName("CurrentLevel")->GetElementValue(0, 0, &current_level);
  counting = false;
  level_finished = false;
}

void RealTimeMeter::OnLevelFinish() {
  if (!enabled)
    return;
  level_finished = true;
  if (current_level == streak_level)
    streak_completed = true;
}

void RealTimeMeter::OnCounterInactive() {
  if (!enabled || !level_finished)
    return;
  counting = false;
  last_finish_sys_time = std::chrono::steady_clock::now().time_since_epoch().count();
}

void RealTimeMeter::OnCounterActive() {
  if (!enabled || counting)
    return;
  if (streak_level <= 0 || level_exited) {
    bml_time = 0;
    // SR Timer is in fact counting 1 more frame than the acutal SR time
    sys_time = std::chrono::steady_clock::now().time_since_epoch().count()
      - static_cast<double>(time_manager->GetLastDeltaTime()) * 1000000;
    level_exited = false;
  }
  status->SetVisible(true);
  counting = true;
}

void RealTimeMeter::OnProcess() {
  if (!enabled)
    return;
  auto current_time = std::chrono::steady_clock::now().time_since_epoch().count();
  if (counting) {
    bml_time += time_manager->GetLastDeltaTime();
    update_status(bml_time, static_cast<float>((current_time - sys_time) / 1000000));
  }
  else if (!level_exited && streak_level > 0 && !streak_completed && current_time - last_finish_sys_time > 1610612735) {
    update_status(bml_time, static_cast<float>((current_time - sys_time) / 1000000));
  }
  if (status)
    status->Process();
}

void RealTimeMeter::init_status_label() {
  if (status)
    status.reset();
  status = std::make_unique<BGui::Label>("RealTimeMeter");
  status->SetAlignment(ALIGN_BOTTOM);
  status->SetPosition({ 0.03f, 0.732f });
  status->SetSize({ 0.20f, 0.03f });
  status->SetText("");
  status->SetFont(ExecuteBB::GAMEFONT_03);
  status->SetAlignment(ALIGN_LEFT);
  // status->SetZOrder(20);
  status->SetVisible(true);
}

void RealTimeMeter::format_time(float duration, char* buf) {
  int counter = int(duration);
  int ms = counter % 1000;
  counter /= 1000;
  int s = counter % 60;
  counter /= 60;
  int m = counter % 60;
  counter /= 60;
  int h = counter % 100;
  snprintf(buf, 16, "%02d:%02d:%02d.%03d", h, m, s, ms);
}

void RealTimeMeter::update_status(float bml_time_ms, float sys_time_ms) {
  if (!status)
    return;
  char text[64], ingame_time_text[16], world_time_text[16];
  format_time(bml_time_ms, ingame_time_text);
  format_time(sys_time_ms, world_time_text);
  snprintf(text, 64, "Real Time\n Ingame  %s\n World   %s", ingame_time_text, world_time_text);
  status->SetText(text);
}
