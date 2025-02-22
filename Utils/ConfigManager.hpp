#pragma once
#include <iostream>
#include <string>
#include <unistd.h>
#include <atomic>
#include <vector>
#include <chrono>
#include <thread>
#include <GLFW/glfw3.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XInput2.h>

#include "../Utils/Memory.hpp"
#include "../Utils/XDisplay.hpp"
#include "../Utils/Conversion.hpp"
#include "../Utils/Config.hpp"
#include "../Utils/Features.hpp"
#include "../Utils/HitboxType.hpp"
#include "../Utils/InputManager.hpp"
#include "../Utils/InputTypes.hpp"

#include "Features/Legitbot.hpp"
#include "Features/Ragebot.hpp"
#include "Features/Sense.hpp"
#include "Features/Radar.hpp"
#include "Features/Flickbot.hpp"
#include "Features/Triggerbot.hpp"
#include "Features/Misc.hpp"
#include "Features/Glow.hpp"

// UI //
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"

#define WriteSection(key) \
	conf << "[" #key "]"  \
		 << "\n";
#define WritePair(section, key) \
	conf << #key " = " << Config::section::key << "\n";
#define WriteSectionEnd() conf << "\n";
#define WriteComment(msg) conf << "; " << msg << '\n';

#define ReadBool(section, key) \
	Config::section::key = reader.GetBoolean(#section, #key, Config::section::key);
#define ReadFloat(section, key) \
	Config::section::key = reader.GetFloat(#section, #key, Config::section::key);
#define ReadInt(section, key) \
	Config::section::key = reader.GetInteger(#section, #key, Config::section::key);

struct ConfigManager
{
	std::vector<std::string> configFiles;
	char configName[64] = {0};
	int selectedConfig = 0;

	Legitbot* Legit;
	Ragebot* Rage;
	Flickbot* Flick;
	Triggerbot* Trigger;
	Glow* GlowESP;
	Sense* ESP;
	Radar* MapRadar;
	Misc* MiscTab;

	ConfigManager(Legitbot* Legit, Ragebot* Rage, Flickbot* Flick, Triggerbot* Trigger, Glow* GlowESP, Sense* ESP, Radar* MapRadar, Misc* MiscTab)
	{
		this->Legit = Legit;
		this->Rage = Rage;
		this->Flick = Flick;
		this->Trigger = Trigger;
		this->GlowESP = GlowESP;
		this->ESP = ESP;
		this->MapRadar = MapRadar;
		this->MiscTab = MiscTab;
	}

	void RenderConfigs()
	{
		ImGui::InputText("Config", configName, 64);

		ImGui::SameLine();
		if (ImGui::Button("Save"))
		{
			SaveConfig();
			LoadConfigs();
		}

		ImGui::SameLine();

		if (ImGui::Button("Load"))
		{
			LoadConfig();
		}

		ImGui::BeginListBox("##Configs");
		int n = 0;
		for (auto config : configFiles)
		{
			const bool is_selected = (selectedConfig == n);
			if (ImGui::Selectable(config.c_str(), is_selected))
			{
				selectedConfig = n;
				strcpy(configName, config.c_str());
			}

			if (is_selected)
			{
				ImGui::SetItemDefaultFocus();
			}
			n++;
		}
		ImGui::EndListBox();
	}

	void LoadConfigs() // List Configs For The ListBox
	{
		std::string configDirectory = "Configs";
		configFiles.clear();
		for (const auto &entry : std::filesystem::directory_iterator(configDirectory))
		{
			if (entry.is_regular_file() && entry.path().extension() == ".ini")
			{
				configFiles.push_back(entry.path().stem().string());
			}
		}
	}

	void UpdateConfig()
	{
		std::string ConfigName = "Configs/" + std::string(configName) + ".ini";
		std::ofstream conf(ConfigName);
    	if (conf.is_open()) {
			WriteSection(Aimbot);
			WritePair(Aimbot, AimbotEnabled);
			WritePair(Aimbot, BindMethod);
			WritePair(Aimbot, AimbotMode);	 // Cubic Beizer (xap-client) or Grinder (Possibly linear?)
			WritePair(Aimbot, InputMethod); // MoveMouse or Controller (Write To ViewAngles)

			WritePair(Aimbot, ClosestHitbox);

			WritePair(Aimbot, OnFire);
			WritePair(Aimbot, OnADS);
			WritePair(Aimbot, VisCheck);
			WritePair(Aimbot, TeamCheck);
			WritePair(Aimbot, TargetSwitching);
			WritePair(Aimbot, Priority);

			WritePair(Aimbot, PredictMovement);
			WritePair(Aimbot, PredictBulletDrop);

			WritePair(Aimbot, FinalDistance);
			WritePair(Aimbot, Smooth);

			WritePair(Aimbot, Speed);
			WritePair(Aimbot, HipfireSmooth);
			WritePair(Aimbot, ADSSmooth);
			WritePair(Aimbot, SmoothDistance);
			WritePair(Aimbot, Delay);
			WritePair(Aimbot, FOV);
			WritePair(Aimbot, ZoomScale);
			WritePair(Aimbot, MinDistance);
			WritePair(Aimbot, HipfireDistance);
			WritePair(Aimbot, ZoomDistance);

			// AimMode 2
			WritePair(Aimbot, HipfireSmooth1);
			WritePair(Aimbot, ADSSmooth1);
			WritePair(Aimbot, ExtraSmoothing);
			WritePair(Aimbot, Deadzone);
			WritePair(Aimbot, FOV1);
			WritePair(Aimbot, MinDistance2);
			WritePair(Aimbot, MaxDistance2);

			// Weapon Toggles
			// Light
			WritePair(Aimbot, P2020);
			WritePair(Aimbot, RE45);
			WritePair(Aimbot, Alternator);
			WritePair(Aimbot, R99);
			WritePair(Aimbot, R301);
			WritePair(Aimbot, Spitfire);
			WritePair(Aimbot, G7);

			// Heavy
			WritePair(Aimbot, Flatline);
			WritePair(Aimbot, Hemlock);
			WritePair(Aimbot, Repeater);
			WritePair(Aimbot, Rampage);
			WritePair(Aimbot, CARSMG);

			// Energy
			WritePair(Aimbot, Havoc);
			WritePair(Aimbot, Devotion);
			WritePair(Aimbot, LSTAR);
			WritePair(Aimbot, TripleTake);
			WritePair(Aimbot, Volt);
			WritePair(Aimbot, Nemesis);

			// Shotgun
			WritePair(Aimbot, Mozambique);
			WritePair(Aimbot, EVA8);
			WritePair(Aimbot, Peacekeeper);
			WritePair(Aimbot, Mastiff);

			// Snipers
			WritePair(Aimbot, Longbow);
			WritePair(Aimbot, ChargeRifle);
			WritePair(Aimbot, Sentinel);

			// Legendary
			WritePair(Aimbot, Wingman);
			WritePair(Aimbot, Prowler);
			WritePair(Aimbot, Bocek);
			WritePair(Aimbot, Kraber);
			WritePair(Aimbot, Knife);

			//---------------Advanced---------------//
			WritePair(Aimbot, AdvancedAim);
			WritePair(Aimbot, AdvancedFire);
			WritePair(Aimbot, AdvancedADS);
			// Aimbot Mode 0 - xap-client
			WritePair(Aimbot, AdvancedClosestHitbox);
			WritePair(Aimbot, AdvancedHitbox);
			WritePair(Aimbot, AdvancedSpeed);
			WritePair(Aimbot, AdvancedSmooth);
			WritePair(Aimbot, AdvancedHipfireSmooth);
			WritePair(Aimbot, AdvancedADSSmooth);
			// Aimbot Mode 1 - Grinder
			WritePair(Aimbot, AdvancedHipfireSmooth1);
			WritePair(Aimbot, AdvancedADSSmooth1);
			WritePair(Aimbot, AdvancedExtraSmooth1);
			WritePair(Aimbot, AdvancedFOV1);
			WritePair(Aimbot, AdvancedDeadzone);
			WritePair(Aimbot, AdvancedMinDistance1);
			WritePair(Aimbot, AdvancedMaxDistance1);

			// Advanced OnFire & OnADS - Aimbot Mode 0 & 1 - xap-client & grinder
			WritePair(Aimbot, P2020Fire);
			WritePair(Aimbot, P2020ADS);
			WritePair(Aimbot, RE45Fire);
			WritePair(Aimbot, RE45ADS);
			WritePair(Aimbot, AlternatorFire);
			WritePair(Aimbot, AlternatorADS);
			WritePair(Aimbot, R99Fire);
			WritePair(Aimbot, R99ADS);
			WritePair(Aimbot, R301Fire);
			WritePair(Aimbot, R301ADS);
			WritePair(Aimbot, SpitfireFire);
			WritePair(Aimbot, SpitfireADS);
			WritePair(Aimbot, G7Fire);
			WritePair(Aimbot, G7ADS);
			WritePair(Aimbot, FlatlineFire);
			WritePair(Aimbot, FlatlineADS);
			WritePair(Aimbot, HemlockFire);
			WritePair(Aimbot, HemlockADS);
			WritePair(Aimbot, RepeaterFire);
			WritePair(Aimbot, RepeaterADS);
			WritePair(Aimbot, RampageFire);
			WritePair(Aimbot, RampageADS);
			WritePair(Aimbot, CARSMGFire);
			WritePair(Aimbot, CARSMGADS);
			WritePair(Aimbot, HavocFire);
			WritePair(Aimbot, HavocADS);
			WritePair(Aimbot, DevotionFire);
			WritePair(Aimbot, DevotionADS);
			WritePair(Aimbot, LSTARFire);
			WritePair(Aimbot, LSTARADS);
			WritePair(Aimbot, TripleTakeFire);
			WritePair(Aimbot, TripleTakeADS);
			WritePair(Aimbot, VoltFire);
			WritePair(Aimbot, VoltADS);
			WritePair(Aimbot, NemesisFire);
			WritePair(Aimbot, NemesisADS);
			WritePair(Aimbot, MozambiqueFire);
			WritePair(Aimbot, MozambiqueADS);
			WritePair(Aimbot, EVA8Fire);
			WritePair(Aimbot, EVA8ADS);
			WritePair(Aimbot, PeacekeeperFire);
			WritePair(Aimbot, PeacekeeperADS);
			WritePair(Aimbot, MastiffFire);
			WritePair(Aimbot, MastiffADS);
			WritePair(Aimbot, LongbowFire);
			WritePair(Aimbot, LongbowADS);
			WritePair(Aimbot, ChargeRifleFire);
			WritePair(Aimbot, ChargeRifleADS);
			WritePair(Aimbot, SentinelFire);
			WritePair(Aimbot, SentinelADS);
			WritePair(Aimbot, WingmanFire);
			WritePair(Aimbot, WingmanADS);
			WritePair(Aimbot, ProwlerFire);
			WritePair(Aimbot, ProwlerADS);
			WritePair(Aimbot, KraberFire);
			WritePair(Aimbot, KraberADS);
			WritePair(Aimbot, BocekFire);
			WritePair(Aimbot, BocekADS);
			WritePair(Aimbot, ThrowingKnifeFire);
			WritePair(Aimbot, ThrowingKnifeADS);

			// Advanced Speed, Smooth + Hitbox - Aimbot Mode 0 - xap-client
			WritePair(Aimbot, P2020ClosestHitbox);
			WritePair(Aimbot, P2020Hitbox);
			WritePair(Aimbot, P2020Speed);
			WritePair(Aimbot, P2020HipfireSmooth);
			WritePair(Aimbot, P2020ADSSmooth);
			WritePair(Aimbot, P2020FOV);
			WritePair(Aimbot, P2020ZoomScale);
			WritePair(Aimbot, RE45ClosestHitbox);
			WritePair(Aimbot, RE45Hitbox);
			WritePair(Aimbot, RE45Speed);
			WritePair(Aimbot, RE45HipfireSmooth);
			WritePair(Aimbot, RE45ADSSmooth);
			WritePair(Aimbot, RE45FOV);
			WritePair(Aimbot, RE45ZoomScale);
			WritePair(Aimbot, AlternatorClosestHitbox);
			WritePair(Aimbot, AlternatorHitbox);
			WritePair(Aimbot, AlternatorSpeed);
			WritePair(Aimbot, AlternatorHipfireSmooth);
			WritePair(Aimbot, AlternatorADSSmooth);
			WritePair(Aimbot, AlternatorFOV);
			WritePair(Aimbot, AlternatorZoomScale);
			WritePair(Aimbot, R99ClosestHitbox);
			WritePair(Aimbot, R99Hitbox);
			WritePair(Aimbot, R99Speed);
			WritePair(Aimbot, R99HipfireSmooth);
			WritePair(Aimbot, R99ADSSmooth);
			WritePair(Aimbot, R99FOV);
			WritePair(Aimbot, R99ZoomScale);
			WritePair(Aimbot, R301ClosestHitbox);
			WritePair(Aimbot, R301Hitbox);
			WritePair(Aimbot, R301Speed);
			WritePair(Aimbot, R301HipfireSmooth);
			WritePair(Aimbot, R301ADSSmooth);
			WritePair(Aimbot, R301FOV);
			WritePair(Aimbot, R301ZoomScale);
			WritePair(Aimbot, SpitfireClosestHitbox);
			WritePair(Aimbot, SpitfireHitbox);
			WritePair(Aimbot, SpitfireSpeed);
			WritePair(Aimbot, SpitfireHipfireSmooth);
			WritePair(Aimbot, SpitfireADSSmooth);
			WritePair(Aimbot, SpitfireFOV);
			WritePair(Aimbot, SpitfireZoomScale);
			WritePair(Aimbot, G7ClosestHitbox);
			WritePair(Aimbot, G7Hitbox);
			WritePair(Aimbot, G7Speed);
			WritePair(Aimbot, G7HipfireSmooth);
			WritePair(Aimbot, G7ADSSmooth);
			WritePair(Aimbot, G7FOV);
			WritePair(Aimbot, G7ZoomScale);
			// Heavy
			WritePair(Aimbot, FlatlineClosestHitbox);
			WritePair(Aimbot, FlatlineHitbox);
			WritePair(Aimbot, FlatlineSpeed);
			WritePair(Aimbot, FlatlineHipfireSmooth);
			WritePair(Aimbot, FlatlineADSSmooth);
			WritePair(Aimbot, FlatlineFOV);
			WritePair(Aimbot, FlatlineZoomScale);
			WritePair(Aimbot, HemlockClosestHitbox);
			WritePair(Aimbot, HemlockHitbox);
			WritePair(Aimbot, HemlockSpeed);
			WritePair(Aimbot, HemlockHipfireSmooth);
			WritePair(Aimbot, HemlockADSSmooth);
			WritePair(Aimbot, HemlockFOV);
			WritePair(Aimbot, HemlockZoomScale);
			WritePair(Aimbot, RepeaterClosestHitbox);
			WritePair(Aimbot, RepeaterHitbox);
			WritePair(Aimbot, RepeaterSpeed);
			WritePair(Aimbot, RepeaterHipfireSmooth);
			WritePair(Aimbot, RepeaterADSSmooth);
			WritePair(Aimbot, RepeaterFOV);
			WritePair(Aimbot, RepeaterZoomScale);
			WritePair(Aimbot, RampageClosestHitbox);
			WritePair(Aimbot, RampageHitbox);
			WritePair(Aimbot, RampageSpeed);
			WritePair(Aimbot, RampageHipfireSmooth);
			WritePair(Aimbot, RampageADSSmooth);
			WritePair(Aimbot, RampageFOV);
			WritePair(Aimbot, RampageZoomScale);
			WritePair(Aimbot, CARSMGClosestHitbox);
			WritePair(Aimbot, CARSMGHitbox);
			WritePair(Aimbot, CARSMGSpeed);
			WritePair(Aimbot, CARSMGHipfireSmooth);
			WritePair(Aimbot, CARSMGADSSmooth);
			WritePair(Aimbot, CARSMGFOV);
			WritePair(Aimbot, CARSMGZoomScale);
			// Energy
			WritePair(Aimbot, HavocClosestHitbox);
			WritePair(Aimbot, HavocHitbox);
			WritePair(Aimbot, HavocSpeed);
			WritePair(Aimbot, HavocHipfireSmooth);
			WritePair(Aimbot, HavocADSSmooth);
			WritePair(Aimbot, HavocFOV);
			WritePair(Aimbot, HavocZoomScale);
			WritePair(Aimbot, DevotionClosestHitbox);
			WritePair(Aimbot, DevotionHitbox);
			WritePair(Aimbot, DevotionSpeed);
			WritePair(Aimbot, DevotionHipfireSmooth);
			WritePair(Aimbot, DevotionADSSmooth);
			WritePair(Aimbot, DevotionFOV);
			WritePair(Aimbot, DevotionZoomScale);
			WritePair(Aimbot, LSTARClosestHitbox);
			WritePair(Aimbot, LSTARHitbox);
			WritePair(Aimbot, LSTARSpeed);
			WritePair(Aimbot, LSTARHipfireSmooth);
			WritePair(Aimbot, LSTARADSSmooth);
			WritePair(Aimbot, LSTARFOV);
			WritePair(Aimbot, LSTARZoomScale);
			WritePair(Aimbot, TripleTakeClosestHitbox);
			WritePair(Aimbot, TripleTakeHitbox);
			WritePair(Aimbot, TripleTakeSpeed);
			WritePair(Aimbot, TripleTakeHipfireSmooth);
			WritePair(Aimbot, TripleTakeADSSmooth);
			WritePair(Aimbot, TripleTakeFOV);
			WritePair(Aimbot, TripleTakeZoomScale);
			WritePair(Aimbot, VoltClosestHitbox);
			WritePair(Aimbot, VoltHitbox);
			WritePair(Aimbot, VoltSpeed);
			WritePair(Aimbot, VoltHipfireSmooth);
			WritePair(Aimbot, VoltADSSmooth);
			WritePair(Aimbot, VoltFOV);
			WritePair(Aimbot, VoltZoomScale);
			WritePair(Aimbot, NemesisClosestHitbox);
			WritePair(Aimbot, NemesisHitbox);
			WritePair(Aimbot, NemesisSpeed);
			WritePair(Aimbot, NemesisHipfireSmooth);
			WritePair(Aimbot, NemesisADSSmooth);
			WritePair(Aimbot, NemesisFOV);
			WritePair(Aimbot, NemesisZoomScale);
			// Shotguns
			WritePair(Aimbot, MozambiqueClosestHitbox);
			WritePair(Aimbot, MozambiqueHitbox);
			WritePair(Aimbot, MozambiqueSpeed);
			WritePair(Aimbot, MozambiqueHipfireSmooth);
			WritePair(Aimbot, MozambiqueADSSmooth);
			WritePair(Aimbot, MozambiqueFOV);
			WritePair(Aimbot, MozambiqueZoomScale);
			WritePair(Aimbot, EVA8ClosestHitbox);
			WritePair(Aimbot, EVA8Hitbox);
			WritePair(Aimbot, EVA8Speed);
			WritePair(Aimbot, EVA8HipfireSmooth);
			WritePair(Aimbot, EVA8ADSSmooth);
			WritePair(Aimbot, EVA8FOV);
			WritePair(Aimbot, EVA8ZoomScale);
			WritePair(Aimbot, PeacekeeperClosestHitbox);
			WritePair(Aimbot, PeacekeeperHitbox);
			WritePair(Aimbot, PeacekeeperSpeed);
			WritePair(Aimbot, PeacekeeperHipfireSmooth);
			WritePair(Aimbot, PeacekeeperADSSmooth);
			WritePair(Aimbot, PeacekeeperFOV);
			WritePair(Aimbot, PeacekeeperZoomScale);
			WritePair(Aimbot, MastiffClosestHitbox);
			WritePair(Aimbot, MastiffHitbox);
			WritePair(Aimbot, MastiffSpeed);
			WritePair(Aimbot, MastiffHipfireSmooth);
			WritePair(Aimbot, MastiffADSSmooth);
			WritePair(Aimbot, MastiffFOV);
			WritePair(Aimbot, MastiffZoomScale);
			// Snipers
			WritePair(Aimbot, LongbowClosestHitbox);
			WritePair(Aimbot, LongbowHitbox);
			WritePair(Aimbot, LongbowSpeed);
			WritePair(Aimbot, LongbowHipfireSmooth);
			WritePair(Aimbot, LongbowADSSmooth);
			WritePair(Aimbot, LongbowFOV);
			WritePair(Aimbot, LongbowZoomScale);
			WritePair(Aimbot, ChargeRifleClosestHitbox);
			WritePair(Aimbot, ChargeRifleHitbox);
			WritePair(Aimbot, ChargeRifleSpeed);
			WritePair(Aimbot, ChargeRifleHipfireSmooth);
			WritePair(Aimbot, ChargeRifleADSSmooth);
			WritePair(Aimbot, ChargeRifleFOV);
			WritePair(Aimbot, ChargeRifleZoomScale);
			WritePair(Aimbot, SentinelClosestHitbox);
			WritePair(Aimbot, SentinelHitbox);
			WritePair(Aimbot, SentinelSpeed);
			WritePair(Aimbot, SentinelHipfireSmooth);
			WritePair(Aimbot, SentinelADSSmooth);
			WritePair(Aimbot, SentinelFOV);
			WritePair(Aimbot, SentinelZoomScale);
			// Legendary
			WritePair(Aimbot, WingmanClosestHitbox);
			WritePair(Aimbot, WingmanHitbox);
			WritePair(Aimbot, WingmanSpeed);
			WritePair(Aimbot, WingmanHipfireSmooth);
			WritePair(Aimbot, WingmanADSSmooth);
			WritePair(Aimbot, WingmanFOV);
			WritePair(Aimbot, WingmanZoomScale);
			WritePair(Aimbot, ProwlerClosestHitbox);
			WritePair(Aimbot, ProwlerHitbox);
			WritePair(Aimbot, ProwlerSpeed);
			WritePair(Aimbot, ProwlerHipfireSmooth);
			WritePair(Aimbot, ProwlerADSSmooth);
			WritePair(Aimbot, ProwlerFOV);
			WritePair(Aimbot, ProwlerZoomScale);
			WritePair(Aimbot, KraberClosestHitbox);
			WritePair(Aimbot, KraberHitbox);
			WritePair(Aimbot, KraberSpeed);
			WritePair(Aimbot, KraberHipfireSmooth);
			WritePair(Aimbot, KraberADSSmooth);
			WritePair(Aimbot, KraberFOV);
			WritePair(Aimbot, KraberZoomScale);
			WritePair(Aimbot, BocekClosestHitbox);
			WritePair(Aimbot, BocekHitbox);
			WritePair(Aimbot, BocekSpeed);
			WritePair(Aimbot, BocekHipfireSmooth);
			WritePair(Aimbot, BocekADSSmooth);
			WritePair(Aimbot, BocekFOV);
			WritePair(Aimbot, BocekZoomScale);
			WritePair(Aimbot, ThrowingKnifeClosestHitbox);
			WritePair(Aimbot, ThrowingKnifeHitbox);
			WritePair(Aimbot, ThrowingKnifeSpeed);
			WritePair(Aimbot, ThrowingKnifeHipfireSmooth);
			WritePair(Aimbot, ThrowingKnifeADSSmooth);
			WritePair(Aimbot, ThrowingKnifeFOV);
			WritePair(Aimbot, ThrowingKnifeZoomScale);

			// Advanced Smooth - Aimbot Mode 1 - Grinder
			WritePair(Aimbot, P2020HipfireSmooth1);
			WritePair(Aimbot, P2020ADSSmooth1);
			WritePair(Aimbot, P2020ExtraSmooth1);
			WritePair(Aimbot, P2020Deadzone);
			WritePair(Aimbot, P2020FOV1);
			WritePair(Aimbot, P2020MinDistance1);
			WritePair(Aimbot, P2020MaxDistance1);
			WritePair(Aimbot, RE45HipfireSmooth1);
			WritePair(Aimbot, RE45ADSSmooth1);
			WritePair(Aimbot, RE45ExtraSmooth1);
			WritePair(Aimbot, RE45Deadzone);
			WritePair(Aimbot, RE45FOV1);
			WritePair(Aimbot, RE45MinDistance1);
			WritePair(Aimbot, RE45MaxDistance1);
			WritePair(Aimbot, AlternatorHipfireSmooth1);
			WritePair(Aimbot, AlternatorADSSmooth1);
			WritePair(Aimbot, AlternatorExtraSmooth1);
			WritePair(Aimbot, AlternatorDeadzone);
			WritePair(Aimbot, AlternatorFOV1);
			WritePair(Aimbot, AlternatorMinDistance1);
			WritePair(Aimbot, AlternatorMaxDistance1);
			WritePair(Aimbot, R99HipfireSmooth1);
			WritePair(Aimbot, R99ADSSmooth1);
			WritePair(Aimbot, R99ExtraSmooth1);
			WritePair(Aimbot, R99Deadzone);
			WritePair(Aimbot, R99FOV1);
			WritePair(Aimbot, R99MinDistance1);
			WritePair(Aimbot, R99MaxDistance1);
			WritePair(Aimbot, R301HipfireSmooth1);
			WritePair(Aimbot, R301ADSSmooth1);
			WritePair(Aimbot, R301ExtraSmooth1);
			WritePair(Aimbot, R301Deadzone);
			WritePair(Aimbot, R301FOV1);
			WritePair(Aimbot, R301MinDistance1);
			WritePair(Aimbot, R301MaxDistance1);
			WritePair(Aimbot, SpitfireHipfireSmooth1);
			WritePair(Aimbot, SpitfireADSSmooth1);
			WritePair(Aimbot, SpitfireExtraSmooth1);
			WritePair(Aimbot, SpitfireDeadzone);
			WritePair(Aimbot, SpitfireFOV1);
			WritePair(Aimbot, SpitfireMinDistance1);
			WritePair(Aimbot, SpitfireMaxDistance1);
			WritePair(Aimbot, G7HipfireSmooth1);
			WritePair(Aimbot, G7ADSSmooth1);
			WritePair(Aimbot, G7ExtraSmooth1);
			WritePair(Aimbot, G7Deadzone);
			WritePair(Aimbot, G7FOV1);
			WritePair(Aimbot, G7MinDistance1);
			WritePair(Aimbot, G7MaxDistance1);

			WritePair(Aimbot, FlatlineHipfireSmooth1);
			WritePair(Aimbot, FlatlineADSSmooth1);
			WritePair(Aimbot, FlatlineExtraSmooth1);
			WritePair(Aimbot, FlatlineDeadzone);
			WritePair(Aimbot, FlatlineFOV1);
			WritePair(Aimbot, FlatlineMinDistance1);
			WritePair(Aimbot, FlatlineMaxDistance1);
			WritePair(Aimbot, HemlockHipfireSmooth1);
			WritePair(Aimbot, HemlockADSSmooth1);
			WritePair(Aimbot, HemlockExtraSmooth1);
			WritePair(Aimbot, HemlockDeadzone);
			WritePair(Aimbot, HemlockFOV1);
			WritePair(Aimbot, HemlockMinDistance1);
			WritePair(Aimbot, HemlockMaxDistance1);
			WritePair(Aimbot, RepeaterHipfireSmooth1);
			WritePair(Aimbot, RepeaterADSSmooth1);
			WritePair(Aimbot, RepeaterExtraSmooth1);
			WritePair(Aimbot, RepeaterDeadzone);
			WritePair(Aimbot, RepeaterFOV1);
			WritePair(Aimbot, RepeaterMinDistance1);
			WritePair(Aimbot, RepeaterMaxDistance1);
			WritePair(Aimbot, RampageHipfireSmooth1);
			WritePair(Aimbot, RampageADSSmooth1);
			WritePair(Aimbot, RampageExtraSmooth1);
			WritePair(Aimbot, RampageDeadzone);
			WritePair(Aimbot, RampageFOV1);
			WritePair(Aimbot, RampageMinDistance1);
			WritePair(Aimbot, RampageMaxDistance1);
			WritePair(Aimbot, CARSMGHipfireSmooth1);
			WritePair(Aimbot, CARSMGADSSmooth1);
			WritePair(Aimbot, CARSMGExtraSmooth1);
			WritePair(Aimbot, CARSMGDeadzone);
			WritePair(Aimbot, CARSMGFOV1);
			WritePair(Aimbot, CARSMGMinDistance1);
			WritePair(Aimbot, CARSMGMaxDistance1);

			WritePair(Aimbot, HavocHipfireSmooth1);
			WritePair(Aimbot, HavocADSSmooth1);
			WritePair(Aimbot, HavocExtraSmooth1);
			WritePair(Aimbot, HavocDeadzone);
			WritePair(Aimbot, HavocFOV1);
			WritePair(Aimbot, HavocMinDistance1);
			WritePair(Aimbot, HavocMaxDistance1);
			WritePair(Aimbot, DevotionHipfireSmooth1);
			WritePair(Aimbot, DevotionADSSmooth1);
			WritePair(Aimbot, DevotionExtraSmooth1);
			WritePair(Aimbot, DevotionDeadzone);
			WritePair(Aimbot, DevotionFOV1);
			WritePair(Aimbot, DevotionMinDistance1);
			WritePair(Aimbot, DevotionMaxDistance1);
			WritePair(Aimbot, LSTARHipfireSmooth1);
			WritePair(Aimbot, LSTARADSSmooth1);
			WritePair(Aimbot, LSTARExtraSmooth1);
			WritePair(Aimbot, LSTARDeadzone);
			WritePair(Aimbot, LSTARFOV1);
			WritePair(Aimbot, LSTARMinDistance1);
			WritePair(Aimbot, LSTARMaxDistance1);
			WritePair(Aimbot, TripleTakeHipfireSmooth1);
			WritePair(Aimbot, TripleTakeADSSmooth1);
			WritePair(Aimbot, TripleTakeExtraSmooth1);
			WritePair(Aimbot, TripleTakeDeadzone);
			WritePair(Aimbot, TripleTakeFOV1);
			WritePair(Aimbot, TripleTakeMinDistance1);
			WritePair(Aimbot, TripleTakeMaxDistance1);
			WritePair(Aimbot, VoltHipfireSmooth1);
			WritePair(Aimbot, VoltADSSmooth1);
			WritePair(Aimbot, VoltExtraSmooth1);
			WritePair(Aimbot, VoltDeadzone);
			WritePair(Aimbot, VoltFOV1);
			WritePair(Aimbot, VoltMinDistance1);
			WritePair(Aimbot, VoltMaxDistance1);
			WritePair(Aimbot, NemesisHipfireSmooth1);
			WritePair(Aimbot, NemesisADSSmooth1);
			WritePair(Aimbot, NemesisExtraSmooth1);
			WritePair(Aimbot, NemesisDeadzone);
			WritePair(Aimbot, NemesisFOV1);
			WritePair(Aimbot, NemesisMinDistance1);
			WritePair(Aimbot, NemesisMaxDistance1);

			WritePair(Aimbot, MozambiqueHipfireSmooth1);
			WritePair(Aimbot, MozambiqueADSSmooth1);
			WritePair(Aimbot, MozambiqueExtraSmooth1);
			WritePair(Aimbot, MozambiqueDeadzone);
			WritePair(Aimbot, MozambiqueFOV1);
			WritePair(Aimbot, MozambiqueMinDistance1);
			WritePair(Aimbot, MozambiqueMaxDistance1);
			WritePair(Aimbot, EVA8HipfireSmooth1);
			WritePair(Aimbot, EVA8ADSSmooth1);
			WritePair(Aimbot, EVA8ExtraSmooth1);
			WritePair(Aimbot, EVA8Deadzone);
			WritePair(Aimbot, EVA8FOV1);
			WritePair(Aimbot, EVA8MinDistance1);
			WritePair(Aimbot, EVA8MaxDistance1);
			WritePair(Aimbot, PeacekeeperHipfireSmooth1);
			WritePair(Aimbot, PeacekeeperADSSmooth1);
			WritePair(Aimbot, PeacekeeperExtraSmooth1);
			WritePair(Aimbot, PeacekeeperDeadzone);
			WritePair(Aimbot, PeacekeeperFOV1);
			WritePair(Aimbot, PeacekeeperMinDistance1);
			WritePair(Aimbot, PeacekeeperMaxDistance1);
			WritePair(Aimbot, MastiffHipfireSmooth1);
			WritePair(Aimbot, MastiffADSSmooth1);
			WritePair(Aimbot, MastiffExtraSmooth1);
			WritePair(Aimbot, MastiffDeadzone);
			WritePair(Aimbot, MastiffFOV1);
			WritePair(Aimbot, MastiffMinDistance1);
			WritePair(Aimbot, MastiffMaxDistance1);

			WritePair(Aimbot, LongbowHipfireSmooth1);
			WritePair(Aimbot, LongbowADSSmooth1);
			WritePair(Aimbot, LongbowExtraSmooth1);
			WritePair(Aimbot, LongbowDeadzone);
			WritePair(Aimbot, LongbowFOV1);
			WritePair(Aimbot, LongbowMinDistance1);
			WritePair(Aimbot, LongbowMaxDistance1);
			WritePair(Aimbot, ChargeRifleHipfireSmooth1);
			WritePair(Aimbot, ChargeRifleADSSmooth1);
			WritePair(Aimbot, ChargeRifleExtraSmooth1);
			WritePair(Aimbot, ChargeRifleDeadzone);
			WritePair(Aimbot, ChargeRifleFOV1);
			WritePair(Aimbot, ChargeRifleMinDistance1);
			WritePair(Aimbot, ChargeRifleMaxDistance1);
			WritePair(Aimbot, SentinelHipfireSmooth1);
			WritePair(Aimbot, SentinelADSSmooth1);
			WritePair(Aimbot, SentinelExtraSmooth1);
			WritePair(Aimbot, SentinelDeadzone);
			WritePair(Aimbot, SentinelFOV1);
			WritePair(Aimbot, SentinelMinDistance1);
			WritePair(Aimbot, SentinelMaxDistance1);

			WritePair(Aimbot, WingmanHipfireSmooth1);
			WritePair(Aimbot, WingmanADSSmooth1);
			WritePair(Aimbot, WingmanExtraSmooth1);
			WritePair(Aimbot, WingmanDeadzone);
			WritePair(Aimbot, WingmanFOV1);
			WritePair(Aimbot, WingmanMinDistance1);
			WritePair(Aimbot, WingmanMaxDistance1);
			WritePair(Aimbot, ProwlerHipfireSmooth1);
			WritePair(Aimbot, ProwlerADSSmooth1);
			WritePair(Aimbot, ProwlerExtraSmooth1);
			WritePair(Aimbot, ProwlerDeadzone);
			WritePair(Aimbot, ProwlerFOV1);
			WritePair(Aimbot, ProwlerMinDistance1);
			WritePair(Aimbot, ProwlerMaxDistance1);
			WritePair(Aimbot, BocekHipfireSmooth1);
			WritePair(Aimbot, BocekADSSmooth1);
			WritePair(Aimbot, BocekExtraSmooth1);
			WritePair(Aimbot, BocekDeadzone);
			WritePair(Aimbot, BocekFOV1);
			WritePair(Aimbot, BocekMinDistance1);
			WritePair(Aimbot, BocekMaxDistance1);
			WritePair(Aimbot, KraberHipfireSmooth1);
			WritePair(Aimbot, KraberADSSmooth1);
			WritePair(Aimbot, KraberExtraSmooth1);
			WritePair(Aimbot, KraberDeadzone);
			WritePair(Aimbot, KraberFOV1);
			WritePair(Aimbot, KraberMinDistance1);
			WritePair(Aimbot, KraberMaxDistance1);
			WritePair(Aimbot, ThrowingKnifeHipfireSmooth1);
			WritePair(Aimbot, ThrowingKnifeADSSmooth1);
			WritePair(Aimbot, ThrowingKnifeExtraSmooth1);
			WritePair(Aimbot, ThrowingKnifeDeadzone);
			WritePair(Aimbot, ThrowingKnifeFOV1);
			WritePair(Aimbot, ThrowingKnifeMinDistance1);
			WritePair(Aimbot, ThrowingKnifeMaxDistance1);
			WriteSectionEnd();

			WriteSection(AimbotBinds);
			WritePair(AimbotBinds, AimBind);
			WritePair(AimbotBinds, ExtraBind);
			// Advanced Binds
			WritePair(AimbotBinds, P2020AimBind);
			WritePair(AimbotBinds, P2020ExtraBind);
			WritePair(AimbotBinds, RE45AimBind);
			WritePair(AimbotBinds, RE45ExtraBind);
			WritePair(AimbotBinds, AlternatorAimBind);
			WritePair(AimbotBinds, AlternatorExtraBind);
			WritePair(AimbotBinds, R99AimBind);
			WritePair(AimbotBinds, R99ExtraBind);
			WritePair(AimbotBinds, R301AimBind);
			WritePair(AimbotBinds, R301ExtraBind);
			WritePair(AimbotBinds, SpitfireAimBind);
			WritePair(AimbotBinds, SpitfireExtraBind);
			WritePair(AimbotBinds, G7AimBind);
			WritePair(AimbotBinds, G7ExtraBind);
			WritePair(AimbotBinds, FlatlineAimBind);
			WritePair(AimbotBinds, FlatlineExtraBind);
			WritePair(AimbotBinds, HemlockAimBind);
			WritePair(AimbotBinds, HemlockExtraBind);
			WritePair(AimbotBinds, RepeaterAimBind);
			WritePair(AimbotBinds, RepeaterExtraBind);
			WritePair(AimbotBinds, RampageAimBind);
			WritePair(AimbotBinds, RampageExtraBind);
			WritePair(AimbotBinds, CARSMGAimBind);
			WritePair(AimbotBinds, CARSMGExtraBind);
			WritePair(AimbotBinds, HavocAimBind);
			WritePair(AimbotBinds, HavocExtraBind);
			WritePair(AimbotBinds, DevotionAimBind);
			WritePair(AimbotBinds, DevotionExtraBind);
			WritePair(AimbotBinds, LSTARAimBind);
			WritePair(AimbotBinds, LSTARExtraBind);
			WritePair(AimbotBinds, TripleTakeAimBind);
			WritePair(AimbotBinds, TripleTakeExtraBind);
			WritePair(AimbotBinds, VoltAimBind);
			WritePair(AimbotBinds, VoltExtraBind);
			WritePair(AimbotBinds, NemesisAimBind);
			WritePair(AimbotBinds, NemesisExtraBind);
			WritePair(AimbotBinds, MozambiqueAimBind);
			WritePair(AimbotBinds, MozambiqueExtraBind);
			WritePair(AimbotBinds, EVA8AimBind);
			WritePair(AimbotBinds, EVA8ExtraBind);
			WritePair(AimbotBinds, PeacekeeperAimBind);
			WritePair(AimbotBinds, PeacekeeperExtraBind);
			WritePair(AimbotBinds, MastiffAimBind);
			WritePair(AimbotBinds, MastiffExtraBind);
			WritePair(AimbotBinds, LongbowAimBind);
			WritePair(AimbotBinds, LongbowExtraBind);
			WritePair(AimbotBinds, ChargeRifleAimBind);
			WritePair(AimbotBinds, ChargeRifleExtraBind);
			WritePair(AimbotBinds, SentinelAimBind);
			WritePair(AimbotBinds, SentinelExtraBind);
			WritePair(AimbotBinds, WingmanAimBind);
			WritePair(AimbotBinds, WingmanExtraBind);
			WritePair(AimbotBinds, ProwlerAimBind);
			WritePair(AimbotBinds, ProwlerExtraBind);
			WritePair(AimbotBinds, BocekAimBind);
			WritePair(AimbotBinds, BocekExtraBind);
			WritePair(AimbotBinds, KraberAimBind);
			WritePair(AimbotBinds, KraberExtraBind);
			WritePair(AimbotBinds, ThrowingKnifeAimBind);
			WritePair(AimbotBinds, ThrowingKnifeExtraBind);
			WriteSectionEnd();

			WriteSection(AimbotHitboxes);
			WritePair(AimbotHitboxes, Hitbox);
			// Advanced Hitboxes
			WritePair(AimbotHitboxes, P2020Hitbox);
			WritePair(AimbotHitboxes, RE45Hitbox);
			WritePair(AimbotHitboxes, AlternatorHitbox);
			WritePair(AimbotHitboxes, R99Hitbox);
			WritePair(AimbotHitboxes, R301Hitbox);
			WritePair(AimbotHitboxes, SpitfireHitbox);
			WritePair(AimbotHitboxes, G7Hitbox);
			WritePair(AimbotHitboxes, FlatlineHitbox);
			WritePair(AimbotHitboxes, HemlockHitbox);
			WritePair(AimbotHitboxes, RepeaterHitbox);
			WritePair(AimbotHitboxes, RampageHitbox);
			WritePair(AimbotHitboxes, CARSMGHitbox);
			WritePair(AimbotHitboxes, HavocHitbox);
			WritePair(AimbotHitboxes, DevotionHitbox);
			WritePair(AimbotHitboxes, LSTARHitbox);
			WritePair(AimbotHitboxes, TripleTakeHitbox);
			WritePair(AimbotHitboxes, VoltHitbox);
			WritePair(AimbotHitboxes, NemesisHitbox);
			WritePair(AimbotHitboxes, MozambiqueHitbox);
			WritePair(AimbotHitboxes, EVA8Hitbox);
			WritePair(AimbotHitboxes, PeacekeeperHitbox);
			WritePair(AimbotHitboxes, MastiffHitbox);
			WritePair(AimbotHitboxes, LongbowHitbox);
			WritePair(AimbotHitboxes, ChargeRifleHitbox);
			WritePair(AimbotHitboxes, SentinelHitbox);
			WritePair(AimbotHitboxes, WingmanHitbox);
			WritePair(AimbotHitboxes, ProwlerHitbox);
			WritePair(AimbotHitboxes, BocekHitbox);
			WritePair(AimbotHitboxes, KraberHitbox);
			WritePair(AimbotHitboxes, ThrowingKnifeHitbox);
			WriteSectionEnd();

			WriteSection(Ragebot);
			WritePair(Ragebot, RagebotEnabled);
			WritePair(Ragebot, RageAimbot);
			WritePair(Ragebot, AimMethod);
			WritePair(Ragebot, ClosestHitbox);
			WritePair(Ragebot, Hitbox);
			WritePair(Ragebot, BindMethod);
			WritePair(Ragebot, OnFire);
			WritePair(Ragebot, OnADS);
			WritePair(Ragebot, AimBind);
			WritePair(Ragebot, ExtraBind);
			WritePair(Ragebot, VisCheck);
			WritePair(Ragebot, TeamCheck);
			WritePair(Ragebot, Priority);
			WritePair(Ragebot, AutoShoot);
			WritePair(Ragebot, PredictMovement);
			WritePair(Ragebot, PredictBulletDrop);
			WritePair(Ragebot, PreditcionAmount);
			WritePair(Ragebot, FinalDistance);
			WritePair(Ragebot, Speed);
			WritePair(Ragebot, Smooth);
			WritePair(Ragebot, HipfireSmooth);
			WritePair(Ragebot, ADSSmooth);
			WritePair(Ragebot, SmoothDistance);
			WritePair(Ragebot, Delay);
			WritePair(Ragebot, FOV);
			WritePair(Ragebot, ZoomScale);
			WritePair(Ragebot, MinDistance);
			WritePair(Ragebot, HipfireDistance);
			WritePair(Ragebot, ZoomDistance);
			// Weapon Toggles
			// Light
			WritePair(Ragebot, P2020);
			WritePair(Ragebot, RE45);
			WritePair(Ragebot, Alternator);
			WritePair(Ragebot, R99);
			WritePair(Ragebot, R301);
			WritePair(Ragebot, Spitfire);
			WritePair(Ragebot, G7);
			// Heavy
			WritePair(Ragebot, Flatline);
			WritePair(Ragebot, Hemlock);
			WritePair(Ragebot, Repeater);
			WritePair(Ragebot, Rampage);
			WritePair(Ragebot, CARSMG);
			// Energy
			WritePair(Ragebot, Havoc);
			WritePair(Ragebot, Devotion);
			WritePair(Ragebot, LSTAR);
			WritePair(Ragebot, TripleTake);
			WritePair(Ragebot, Volt);
			WritePair(Ragebot, Nemesis);
			// Shotgun
			WritePair(Ragebot, Mozambique);
			WritePair(Ragebot, EVA8);
			WritePair(Ragebot, Peacekeeper);
			WritePair(Ragebot, Mastiff);
			// Snipers
			WritePair(Ragebot, Longbow);
			WritePair(Ragebot, ChargeRifle);
			WritePair(Ragebot, Sentinel);
			// Legendary
			WritePair(Ragebot, Wingman);
			WritePair(Ragebot, Prowler);
			WritePair(Ragebot, Bocek);
			WritePair(Ragebot, Kraber);
			WritePair(Ragebot, Knife);

			WritePair(Ragebot, RageRCS);
			WritePair(Ragebot, RecoilRate);

			WriteSectionEnd();

			WriteSection(Flickbot);
			WritePair(Flickbot, Flickbot);
			WritePair(Flickbot, FlickbotMethod); // 0 = Mouse, 1 = Memory
			WritePair(Flickbot, FlickBind);
			WritePair(Flickbot, ClosestHitbox);
			WritePair(Flickbot, Hitbox);
			WritePair(Flickbot, Priority);
			WritePair(Flickbot, AutoShoot);
			WritePair(Flickbot, AutoShootDelay);
			WritePair(Flickbot, FlickBack);
			WritePair(Flickbot, FlickBackDelay);
			WritePair(Flickbot, FlickBackSmoothing);
			WritePair(Flickbot, PredictMovement);
			WritePair(Flickbot, PredictBulletDrop);
			WritePair(Flickbot, TeamCheck);
			WritePair(Flickbot, PreditcionAmount);
			WritePair(Flickbot, FinalDistance);
			WritePair(Flickbot, HipfireSmooth);
			WritePair(Flickbot, ADSSmooth);
			WritePair(Flickbot, SmoothDistance);
			WritePair(Flickbot, Delay);
			WritePair(Flickbot, FOV);
			WritePair(Flickbot, ZoomScale);
			WritePair(Flickbot, MinDistance);
			WritePair(Flickbot, HipfireDistance);
			WritePair(Flickbot, ZoomDistance);

			WritePair(Flickbot, Smooth);
			WritePair(Flickbot, FlickBackSmooth);

			// Weapon Toggles
			// Light
			WritePair(Flickbot, P2020);
			WritePair(Flickbot, RE45);
			WritePair(Flickbot, Alternator);
			WritePair(Flickbot, R99);
			WritePair(Flickbot, R301);
			WritePair(Flickbot, Spitfire);
			WritePair(Flickbot, G7);

			// Heavy
			WritePair(Flickbot, Flatline);
			WritePair(Flickbot, Hemlock);
			WritePair(Flickbot, Repeater);
			WritePair(Flickbot, Rampage);
			WritePair(Flickbot, CARSMG);

			// Energy
			WritePair(Flickbot, Havoc);
			WritePair(Flickbot, Devotion);
			WritePair(Flickbot, LSTAR);
			WritePair(Flickbot, TripleTake);
			WritePair(Flickbot, Volt);
			WritePair(Flickbot, Nemesis);

			// Shotgun
			WritePair(Flickbot, Mozambique);
			WritePair(Flickbot, EVA8);
			WritePair(Flickbot, Peacekeeper);
			WritePair(Flickbot, Mastiff);

			// Snipers
			WritePair(Flickbot, Longbow);
			WritePair(Flickbot, ChargeRifle);
			WritePair(Flickbot, Sentinel);

			// Legendary
			WritePair(Flickbot, Wingman);
			WritePair(Flickbot, Prowler);
			WritePair(Flickbot, Bocek);
			WritePair(Flickbot, Kraber);
			WritePair(Flickbot, Knife);

			WriteSectionEnd();

			WriteSection(RCS);
			WritePair(RCS, RCSEnabled);
			WritePair(RCS, RCSMode);

			WritePair(RCS, OnADS);

			WritePair(RCS, PitchPower);
			WritePair(RCS, YawPower);
			WritePair(RCS, PitchReduction);
			WritePair(RCS, YawReduction);

			// Weapon Toggles
			// Light
			WritePair(RCS, P2020);
			WritePair(RCS, RE45);
			WritePair(RCS, Alternator);
			WritePair(RCS, R99);
			WritePair(RCS, R301);
			WritePair(RCS, Spitfire);
			WritePair(RCS, G7);

			// Heavy
			WritePair(RCS, Flatline);
			WritePair(RCS, Hemlock);
			WritePair(RCS, Repeater);
			WritePair(RCS, Rampage);
			WritePair(RCS, CARSMG);

			// Energy
			WritePair(RCS, Havoc);
			WritePair(RCS, Devotion);
			WritePair(RCS, LSTAR);
			WritePair(RCS, TripleTake);
			WritePair(RCS, Volt);
			WritePair(RCS, Nemesis);

			// Shotgun
			WritePair(RCS, Mozambique);
			WritePair(RCS, EVA8);
			WritePair(RCS, Peacekeeper);
			WritePair(RCS, Mastiff);

			// Snipers
			WritePair(RCS, Longbow);
			WritePair(RCS, ChargeRifle);
			WritePair(RCS, Sentinel);

			// Legendary
			WritePair(RCS, Wingman); // Emotional damage!
			WritePair(RCS, Prowler);
			WritePair(RCS, Kraber);

			//---------------Advanced---------------//
			WritePair(RCS, AdvancedRCS);
			WritePair(RCS, AdvancedPitchPower);
			WritePair(RCS, AdvancedYawPower);
			WritePair(RCS, AdvancedPitchReduction);
			WritePair(RCS, AdvancedYawReduction);

			// Weapons
			// Light
			WritePair(RCS, P2020Pitch);
			WritePair(RCS, P2020Yaw);
			WritePair(RCS, P2020PitchReduction);
			WritePair(RCS, P2020YawReduction);
			WritePair(RCS, RE45Pitch);
			WritePair(RCS, RE45Yaw);
			WritePair(RCS, RE45PitchReduction);
			WritePair(RCS, RE45YawReduction);
			WritePair(RCS, AlternatorPitch);
			WritePair(RCS, AlternatorYaw);
			WritePair(RCS, AlternatorPitchReduction);
			WritePair(RCS, AlternatorYawReduction);
			WritePair(RCS, R99Pitch);
			WritePair(RCS, R99Yaw);
			WritePair(RCS, R99PitchReduction);
			WritePair(RCS, R99YawReduction);
			WritePair(RCS, R301Pitch);
			WritePair(RCS, R301Yaw);
			WritePair(RCS, R301PitchReduction);
			WritePair(RCS, R301YawReduction);
			WritePair(RCS, SpitfirePitch);
			WritePair(RCS, SpitfireYaw);
			WritePair(RCS, SpitfirePitchReduction);
			WritePair(RCS, SpitfireYawReduction);
			WritePair(RCS, G7Pitch);
			WritePair(RCS, G7Yaw);
			WritePair(RCS, G7PitchReduction);
			WritePair(RCS, G7YawReduction);
			// Heavy
			WritePair(RCS, FlatlinePitch);
			WritePair(RCS, FlatlineYaw);
			WritePair(RCS, FlatlinePitchReduction);
			WritePair(RCS, FlatlineYawReduction);
			WritePair(RCS, HemlockPitch);
			WritePair(RCS, HemlockYaw);
			WritePair(RCS, HemlockPitchReduction);
			WritePair(RCS, HemlockYawReduction);
			WritePair(RCS, RepeaterPitch);
			WritePair(RCS, RepeaterYaw);
			WritePair(RCS, RepeaterPitchReduction);
			WritePair(RCS, RepeaterYawReduction);
			WritePair(RCS, RampagePitch);
			WritePair(RCS, RampageYaw);
			WritePair(RCS, RampagePitchReduction);
			WritePair(RCS, RampageYawReduction);
			WritePair(RCS, CARSMGPitch);
			WritePair(RCS, CARSMGYaw);
			WritePair(RCS, CARSMGPitchReduction);
			WritePair(RCS, CARSMGYawReduction);
			// Energy
			WritePair(RCS, HavocPitch);
			WritePair(RCS, HavocYaw);
			WritePair(RCS, HavocPitchReduction);
			WritePair(RCS, HavocYawReduction);
			WritePair(RCS, DevotionPitch);
			WritePair(RCS, DevotionYaw);
			WritePair(RCS, DevotionPitchReduction);
			WritePair(RCS, DevotionYawReduction);
			WritePair(RCS, LSTARPitch);
			WritePair(RCS, LSTARYaw);
			WritePair(RCS, LSTARPitchReduction);
			WritePair(RCS, LSTARYawReduction);
			WritePair(RCS, TripleTakePitch);
			WritePair(RCS, TripleTakeYaw);
			WritePair(RCS, TripleTakePitchReduction);
			WritePair(RCS, TripleTakeYawReduction);
			WritePair(RCS, VoltPitch);
			WritePair(RCS, VoltYaw);
			WritePair(RCS, VoltPitchReduction);
			WritePair(RCS, VoltYawReduction);
			WritePair(RCS, NemesisPitch);
			WritePair(RCS, NemesisYaw);
			WritePair(RCS, NemesisPitchReduction);
			WritePair(RCS, NemesisYawReduction);
			// Shotguns
			WritePair(RCS, MozambiquePitch);
			WritePair(RCS, MozambiqueYaw);
			WritePair(RCS, MozambiquePitchReduction);
			WritePair(RCS, MozambiqueYawReduction);
			WritePair(RCS, EVA8Pitch);
			WritePair(RCS, EVA8Yaw);
			WritePair(RCS, EVA8PitchReduction);
			WritePair(RCS, EVA8YawReduction);
			WritePair(RCS, PeacekeeperPitch);
			WritePair(RCS, PeacekeeperYaw);
			WritePair(RCS, PeacekeeperPitchReduction);
			WritePair(RCS, PeacekeeperYawReduction);
			WritePair(RCS, MastiffPitch);
			WritePair(RCS, MastiffYaw);
			WritePair(RCS, MastiffPitchReduction);
			WritePair(RCS, MastiffYawReduction);
			// Snipers
			WritePair(RCS, LongbowPitch);
			WritePair(RCS, LongbowYaw);
			WritePair(RCS, LongbowPitchReduction);
			WritePair(RCS, LongbowYawReduction);
			WritePair(RCS, ChargeRiflePitch);
			WritePair(RCS, ChargeRifleYaw);
			WritePair(RCS, ChargeRiflePitchReduction);
			WritePair(RCS, ChargeRifleYawReduction);
			WritePair(RCS, SentinelPitch);
			WritePair(RCS, SentinelYaw);
			WritePair(RCS, SentinelPitchReduction);
			WritePair(RCS, SentinelYawReduction);
			// Legendary
			WritePair(RCS, WingmanPitch);
			WritePair(RCS, WingmanYaw);
			WritePair(RCS, WingmanPitchReduction);
			WritePair(RCS, WingmanYawReduction);
			WritePair(RCS, ProwlerPitch);
			WritePair(RCS, ProwlerYaw);
			WritePair(RCS, ProwlerPitchReduction);
			WritePair(RCS, ProwlerYawReduction);
			WritePair(RCS, KraberPitch);
			WritePair(RCS, KraberYaw);
			WritePair(RCS, KraberPitchReduction);
			WritePair(RCS, KraberYawReduction);

			WriteSectionEnd();

			WriteSection(Triggerbot);
			WritePair(Triggerbot, Enabled);
			WritePair(Triggerbot, BindMethod);
			WritePair(Triggerbot, TriggerBind);
			WritePair(Triggerbot, OnADS);
			WritePair(Triggerbot, HipfireShotguns);
			WritePair(Triggerbot, Delay);
			WritePair(Triggerbot, Range);

			// Weapon Toggles
			// Light
			WritePair(Triggerbot, P2020);
			WritePair(Triggerbot, RE45);
			WritePair(Triggerbot, Alternator);
			WritePair(Triggerbot, R99);
			WritePair(Triggerbot, R301);
			WritePair(Triggerbot, Spitfire);
			WritePair(Triggerbot, G7);
			// Heavy
			WritePair(Triggerbot, Flatline);
			WritePair(Triggerbot, Hemlock);
			WritePair(Triggerbot, Repeater);
			WritePair(Triggerbot, Rampage);
			WritePair(Triggerbot, CARSMG);
			// Energy
			WritePair(Triggerbot, Havoc);
			WritePair(Triggerbot, Devotion);
			WritePair(Triggerbot, LSTAR);
			WritePair(Triggerbot, TripleTake);
			WritePair(Triggerbot, Volt);
			WritePair(Triggerbot, Nemesis);
			// Shotgun
			WritePair(Triggerbot, Mozambique);
			WritePair(Triggerbot, EVA8);
			WritePair(Triggerbot, Peacekeeper);
			WritePair(Triggerbot, Mastiff);
			// Snipers
			WritePair(Triggerbot, Longbow);
			WritePair(Triggerbot, ChargeRifle);
			WritePair(Triggerbot, Sentinel);
			// Legendary
			WritePair(Triggerbot, Wingman);
			WritePair(Triggerbot, Prowler);
			WritePair(Triggerbot, Bocek);
			WritePair(Triggerbot, Kraber);
			WritePair(Triggerbot, Knife);

			// Advanced Triggerbot
			WritePair(Triggerbot, AdvancedTriggerbot);
			WritePair(Triggerbot, P2020Range);
			WritePair(Triggerbot, RE45Range);
			WritePair(Triggerbot, AlternatorRange);
			WritePair(Triggerbot, R99Range);
			WritePair(Triggerbot, R301Range);
			WritePair(Triggerbot, SpitfireRange);
			WritePair(Triggerbot, G7Range);
			WritePair(Triggerbot, FlatlineRange);
			WritePair(Triggerbot, HemlockRange);
			WritePair(Triggerbot, ProwlerRange);
			WritePair(Triggerbot, RepeaterRange);
			WritePair(Triggerbot, RampageRange);
			WritePair(Triggerbot, CARSMGRange);
			WritePair(Triggerbot, HavocRange);
			WritePair(Triggerbot, DevotionRange);
			WritePair(Triggerbot, LSTARRange);
			WritePair(Triggerbot, TripleTakeRange);
			WritePair(Triggerbot, VoltRange);
			WritePair(Triggerbot, NemesisRange);
			WritePair(Triggerbot, MozambiqueRange);
			WritePair(Triggerbot, PeacekeeperRange);
			WritePair(Triggerbot, MastiffRange);
			WritePair(Triggerbot, LongbowRange);
			WritePair(Triggerbot, ChargeRifleRange);
			WritePair(Triggerbot, SentinelRange);
			WritePair(Triggerbot, WingmanRange);
			WritePair(Triggerbot, EVA8Range);
			WritePair(Triggerbot, BocekRange);
			WritePair(Triggerbot, KraberRange);
			WritePair(Triggerbot, ThrowingKnifeRange);

			WriteSectionEnd();

			WriteSection(Glow);

			// Glow
			WritePair(Glow, NewGlow);
			WritePair(Glow, GlowMaxDistance);
			WritePair(Glow, GlowColorMode);
			WritePair(Glow, GlowColorShieldMode);
			WritePair(Glow, GlowRadius);
			WritePair(Glow, InsideFunction);	   // Leave
			WritePair(Glow, OutlineFunction); // Leave
			WritePair(Glow, BodyStyle);
			WritePair(Glow, OutlineStyle);
			WritePair(Glow, ViewModelGlow);
			WritePair(Glow, ViewModelGlowSetting);
			WritePair(Glow, ViewModelGlowCombo);

			WriteSectionEnd();

			WriteSection(ItemGlow);

			WritePair(ItemGlow, ItemGlow);
			WritePair(ItemGlow, SelectedItemSelection ); // 0 = Simple, 1 = Custom
			WritePair(ItemGlow, Common);
			WritePair(ItemGlow, Rare);
			WritePair(ItemGlow, Epic);
			WritePair(ItemGlow, Gold);
			WritePair(ItemGlow, Legendary);
			WritePair(ItemGlow, Weapons);
			WritePair(ItemGlow, Ammo);
			WritePair(ItemGlow, ItemGlowThickness);
			WritePair(ItemGlow, SelectedInsideStyle);
			WritePair(ItemGlow, SelectedOutlineStyle);

			WriteSectionEnd();

			WriteSection(Sense);

			WritePair(Sense, Enabled);

			// Aimbot
			WritePair(Sense, DrawFOVCircle);
			WritePair(Sense, DrawFilledFOVCircle);
			WritePair(Sense, FOVThickness);
			WritePair(Sense, GameFOV);
			// Other
			WritePair(Sense, DrawCrosshair);
			WritePair(Sense, CrosshairSize);
			WritePair(Sense, CrosshairThickness);
			WritePair(Sense, ShowSpectators);
			WritePair(Sense, DrawSpectatorWarning);
			WritePair(Sense, DrawVisibleWarning);
			WritePair(Sense, WarningTextOutline);
			WritePair(Sense, WarningTextX);
			WritePair(Sense, WarningTextY);
			// Settings
			WritePair(Sense, TextOutline);
			WritePair(Sense, VisibilityCheck);
			WritePair(Sense, ESPMaxDistance);

			WriteSectionEnd();

			WriteSection(SenseEnemy);

			WritePair(SenseEnemy, DrawEnemy);
			WritePair(SenseEnemy, DrawBoxes);
			WritePair(SenseEnemy, BoxType);
			WritePair(SenseEnemy, BoxStyle);
			WritePair(SenseEnemy, BoxThickness);
			WritePair(SenseEnemy, DrawSkeleton);
			WritePair(SenseEnemy, SkeletonThickness);
			WritePair(SenseEnemy, DrawHeadCircle);
			WritePair(SenseEnemy, HeadCircleThickness);
			WritePair(SenseEnemy, DrawBars);
			WritePair(SenseEnemy, HealthBar);
			WritePair(SenseEnemy, ShieldBar);
			WritePair(SenseEnemy, BarMode);
			WritePair(SenseEnemy, BarStyle);
			WritePair(SenseEnemy, BarThickness);
			WritePair(SenseEnemy, BarThickness2);
			WritePair(SenseEnemy, BarHeight);
			WritePair(SenseEnemy, BarWidth);
			WritePair(SenseEnemy, BarColorMode);
			WritePair(SenseEnemy, WeaponColorType);
			WritePair(SenseEnemy, DrawDistance);
			WritePair(SenseEnemy, TracerPosition);
			WritePair(SenseEnemy, TracerBone);
			WritePair(SenseEnemy, DrawTracers);
			WritePair(SenseEnemy, TracerThickness);
			WritePair(SenseEnemy, DrawNames);
			WritePair(SenseEnemy, DrawSeer);
			WritePair(SenseEnemy, DrawStatus);
			WritePair(SenseEnemy, DrawWeapon);
			WritePair(SenseEnemy, DrawLegend);
			WritePair(SenseEnemy, ShowMaxStatusValues);

			WriteSectionEnd();

			WriteSection(SenseTeammate);

			WritePair(SenseTeammate, DrawTeam);
			WritePair(SenseTeammate, DrawBoxes);
			WritePair(SenseTeammate, BoxType);
			WritePair(SenseTeammate, BoxStyle);
			WritePair(SenseTeammate, BoxThickness);
			WritePair(SenseTeammate, DrawSkeleton);
			WritePair(SenseTeammate, SkeletonThickness);
			WritePair(SenseTeammate, DrawHeadCircle);
			WritePair(SenseTeammate, HeadCircleThickness);
			WritePair(SenseTeammate, DrawBars);
			WritePair(SenseTeammate, HealthBar);
			WritePair(SenseTeammate, ShieldBar);
			WritePair(SenseTeammate, BarMode);
			WritePair(SenseTeammate, BarStyle);
			WritePair(SenseTeammate, BarThickness);
			WritePair(SenseTeammate, BarThickness2);
			WritePair(SenseTeammate, BarHeight);
			WritePair(SenseTeammate, BarWidth);
			WritePair(SenseTeammate, BarColorMode);
			WritePair(SenseTeammate, BarBackground);
			WritePair(SenseTeammate, WeaponColorType);
			WritePair(SenseTeammate, DrawDistance);
			WritePair(SenseTeammate, TracerPosition);
			WritePair(SenseTeammate, TracerBone);
			WritePair(SenseTeammate, DrawTracers);
			WritePair(SenseTeammate, TracerThickness);
			WritePair(SenseTeammate, DrawNames);
			WritePair(SenseTeammate, DrawSeer);
			WritePair(SenseTeammate, DrawStatus);
			WritePair(SenseTeammate, DrawWeapon);
			WritePair(SenseTeammate, DrawLegend);
			WritePair(SenseTeammate, ShowMaxStatusValues);

			WriteSectionEnd();

			WriteSection(SensePositions);

			WritePair(SensePositions, NamePosition);
			WritePair(SensePositions, DistancePosition);
			WritePair(SensePositions, LegendPosition);
			WritePair(SensePositions, WeaponPosition);
			WritePair(SensePositions, StatusPosition);

			WriteSectionEnd();

			WriteSection(Radar);

			WritePair(Radar, MiniMap);
			WritePair(Radar, MiniMapRange);
			WritePair(Radar, MiniMapScaleX);
			WritePair(Radar, MiniMapScaleY);
			WritePair(Radar, MiniMapDotSize);
			WritePair(Radar, MiniMapBlackBGSize);
			WritePair(Radar, MiniMapGuides);
			WritePair(Radar, BigMap);
			WritePair(Radar, BigMapBind);
			WritePair(Radar, CircleColorR);
			WritePair(Radar, CircleColorG);
			WritePair(Radar, CircleColorB);
			WritePair(Radar, CircleColorA);

			WriteSectionEnd();

			WriteSection(Misc);

			WritePair(Misc, SuperGlide);

			WritePair(Misc, QuickTurn);
			WritePair(Misc, QuickTurnAngle);
			WritePair(Misc, QuickTurnBind);

			WritePair(Misc, BHop);
			WritePair(Misc, BHopDelay);
			WritePair(Misc, BHopBind);

			WritePair(Misc, RapidFire);
			WritePair(Misc, RapidFireDelay);
			WritePair(Misc, RapidFireBind);

			// Rapid Fire Weapon Toggles
			// Light
			WritePair(Misc, RapidP2020);
			WritePair(Misc, RapidR301);
			WritePair(Misc, RapidG7);
			// Heavy
			WritePair(Misc, RapidFlatline);
			WritePair(Misc, RapidHemlock);
			WritePair(Misc, RapidProwler);
			// Energy
			WritePair(Misc, RapidNemesis);
			// Shotgun
			WritePair(Misc, RapidMozambique);
			WritePair(Misc, RapidEVA8);
			// Legendary
			WritePair(Misc, RapidWingman);

			WritePair(Misc, TeamGamemode);

			WritePair(Misc, SkinChanger);
			// Weapon IDs
			// Light
			WritePair(Misc, SkinP2020);
			WritePair(Misc, SkinRE45);
			WritePair(Misc, SkinALTERNATOR);
			WritePair(Misc, SkinR99);
			WritePair(Misc, SkinR301);
			WritePair(Misc, SkinSPITFIRE);
			WritePair(Misc, SkinG7);

			// Heavy
			WritePair(Misc, SkinFLATLINE);
			WritePair(Misc, SkinHEMLOCK);
			WritePair(Misc, SkinREPEATER);
			WritePair(Misc, SkinRAMPAGE);
			WritePair(Misc, SkinCAR);

			// Energy
			WritePair(Misc, SkinHAVOC);
			WritePair(Misc, SkinDEVOTION);
			WritePair(Misc, SkinLSTAR);
			WritePair(Misc, SkinTRIPLETAKE);
			WritePair(Misc, SkinVOLT);
			WritePair(Misc, SkinNEMESIS);

			// Shotgun
			WritePair(Misc, SkinMOZAMBIQUE);
			WritePair(Misc, SkinEVA8);
			WritePair(Misc, SkinPEACEKEEPER);
			WritePair(Misc, SkinMASTIFF);

			// Snipers
			WritePair(Misc, SkinLONGBOW);
			WritePair(Misc, SkinCHARGE_RIFLE);
			WritePair(Misc, SkinSENTINEL);

			// Legendary
			WritePair(Misc, SkinWINGMAN);
			WritePair(Misc, SkinPROWLER);
			WritePair(Misc, SkinBOCEK);
			WritePair(Misc, SkinKRABER);

			WriteSectionEnd();

			WriteSection(Colors);

			WritePair(Colors, WeaponColorMode);
			WritePair(Colors, FOVColorR);
			WritePair(Colors, FOVColorG);
			WritePair(Colors, FOVColorB);
			WritePair(Colors, FOVColorA);
			WritePair(Colors, FilledFOVColorR);
			WritePair(Colors, FilledFOVColorG);
			WritePair(Colors, FilledFOVColorB);
			WritePair(Colors, FilledFOVColorA);
			WritePair(Colors, CrosshairColorR);
			WritePair(Colors, CrosshairColorG);
			WritePair(Colors, CrosshairColorB);
			WritePair(Colors, CrosshairColorA);
			WritePair(Colors, SpectatorWarningColorR);
			WritePair(Colors, SpectatorWarningColorG);
			WritePair(Colors, SpectatorWarningColorB);
			WritePair(Colors, SpectatorWarningColorA);
			WritePair(Colors, VisibleWarningColorR);
			WritePair(Colors, VisibleWarningColorG);
			WritePair(Colors, VisibleWarningColorB);
			WritePair(Colors, VisibleWarningColorA);

			WriteSectionEnd();

			WriteSection(EnemyColors);

			WritePair(EnemyColors, InvisibleBoxColorR);
			WritePair(EnemyColors, InvisibleBoxColorG);
			WritePair(EnemyColors, InvisibleBoxColorB);
			WritePair(EnemyColors, InvisibleBoxColorA);
			WritePair(EnemyColors, VisibleBoxColorR);
			WritePair(EnemyColors, VisibleBoxColorG);
			WritePair(EnemyColors, VisibleBoxColorB);
			WritePair(EnemyColors, VisibleBoxColorA);
			WritePair(EnemyColors, InvisibleFilledBoxColorR);
			WritePair(EnemyColors, InvisibleFilledBoxColorG);
			WritePair(EnemyColors, InvisibleFilledBoxColorB);
			WritePair(EnemyColors, InvisibleFilledBoxColorA);
			WritePair(EnemyColors, VisibleFilledBoxColorR);
			WritePair(EnemyColors, VisibleFilledBoxColorG);
			WritePair(EnemyColors, VisibleFilledBoxColorB);
			WritePair(EnemyColors, VisibleFilledBoxColorA);
			WritePair(EnemyColors, InvisibleTracerColorR);
			WritePair(EnemyColors, InvisibleTracerColorG);
			WritePair(EnemyColors, InvisibleTracerColorB);
			WritePair(EnemyColors, InvisibleTracerColorA);
			WritePair(EnemyColors, VisibleTracerColorR);
			WritePair(EnemyColors, VisibleTracerColorG);
			WritePair(EnemyColors, VisibleTracerColorB);
			WritePair(EnemyColors, VisibleTracerColorA);
			WritePair(EnemyColors, InvisibleSkeletonColorR);
			WritePair(EnemyColors, InvisibleSkeletonColorG);
			WritePair(EnemyColors, InvisibleSkeletonColorB);
			WritePair(EnemyColors, InvisibleSkeletonColorA);
			WritePair(EnemyColors, VisibleSkeletonColorR);
			WritePair(EnemyColors, VisibleSkeletonColorG);
			WritePair(EnemyColors, VisibleSkeletonColorB);
			WritePair(EnemyColors, VisibleSkeletonColorA);
			WritePair(EnemyColors, InvisibleHeadCircleColorR);
			WritePair(EnemyColors, InvisibleHeadCircleColorG);
			WritePair(EnemyColors, InvisibleHeadCircleColorB);
			WritePair(EnemyColors, InvisibleHeadCircleColorA);
			WritePair(EnemyColors, VisibleHeadCircleColorR);
			WritePair(EnemyColors, VisibleHeadCircleColorG);
			WritePair(EnemyColors, VisibleHeadCircleColorB);
			WritePair(EnemyColors, VisibleHeadCircleColorA);
			WritePair(EnemyColors, InvisibleNameColorR);
			WritePair(EnemyColors, InvisibleNameColorG);
			WritePair(EnemyColors, InvisibleNameColorB);
			WritePair(EnemyColors, InvisibleNameColorA);
			WritePair(EnemyColors, VisibleNameColorR);
			WritePair(EnemyColors, VisibleNameColorG);
			WritePair(EnemyColors, VisibleNameColorB);
			WritePair(EnemyColors, VisibleNameColorA);
			WritePair(EnemyColors, InvisibleDistanceColorR);
			WritePair(EnemyColors, InvisibleDistanceColorG);
			WritePair(EnemyColors, InvisibleDistanceColorB);
			WritePair(EnemyColors, InvisibleDistanceColorA);
			WritePair(EnemyColors, VisibleDistanceColorR);
			WritePair(EnemyColors, VisibleDistanceColorG);
			WritePair(EnemyColors, VisibleDistanceColorB);
			WritePair(EnemyColors, VisibleDistanceColorA);
			WritePair(EnemyColors, InvisibleLegendColorR);
			WritePair(EnemyColors, InvisibleLegendColorG);
			WritePair(EnemyColors, InvisibleLegendColorB);
			WritePair(EnemyColors, InvisibleLegendColorA);
			WritePair(EnemyColors, VisibleLegendColorR);
			WritePair(EnemyColors, VisibleLegendColorG);
			WritePair(EnemyColors, VisibleLegendColorB);
			WritePair(EnemyColors, VisibleLegendColorA);
			// Bar
			WritePair(EnemyColors, BarColorMode);
			// WeaponESP Colors
			WritePair(EnemyColors, InvisibleWeaponColorR);
			WritePair(EnemyColors, InvisibleWeaponColorG);
			WritePair(EnemyColors, InvisibleWeaponColorB);
			WritePair(EnemyColors, InvisibleWeaponColorA);
			WritePair(EnemyColors, VisibleWeaponColorR);
			WritePair(EnemyColors, VisibleWeaponColorG);
			WritePair(EnemyColors, VisibleWeaponColorB);
			WritePair(EnemyColors, VisibleWeaponColorA);
			// Multiple
			WritePair(EnemyColors, LightWeaponColorR);
			WritePair(EnemyColors, LightWeaponColorG);
			WritePair(EnemyColors, LightWeaponColorB);
			WritePair(EnemyColors, LightWeaponColorA);
			WritePair(EnemyColors, HeavyWeaponColorR);
			WritePair(EnemyColors, HeavyWeaponColorG);
			WritePair(EnemyColors, HeavyWeaponColorB);
			WritePair(EnemyColors, HeavyWeaponColorA);
			WritePair(EnemyColors, EnergyWeaponColorR);
			WritePair(EnemyColors, EnergyWeaponColorG);
			WritePair(EnemyColors, EnergyWeaponColorB);
			WritePair(EnemyColors, EnergyWeaponColorA);
			WritePair(EnemyColors, ShotgunWeaponColorR);
			WritePair(EnemyColors, ShotgunWeaponColorG);
			WritePair(EnemyColors, ShotgunWeaponColorB);
			WritePair(EnemyColors, ShotgunWeaponColorA);
			WritePair(EnemyColors, SniperWeaponColorR);
			WritePair(EnemyColors, SniperWeaponColorG);
			WritePair(EnemyColors, SniperWeaponColorB);
			WritePair(EnemyColors, SniperWeaponColorA);
			WritePair(EnemyColors, LegendaryWeaponColorR);
			WritePair(EnemyColors, LegendaryWeaponColorG);
			WritePair(EnemyColors, LegendaryWeaponColorB);
			WritePair(EnemyColors, LegendaryWeaponColorA);
			WritePair(EnemyColors, MeleeWeaponColorR);
			WritePair(EnemyColors, MeleeWeaponColorG);
			WritePair(EnemyColors, MeleeWeaponColorB);
			WritePair(EnemyColors, MeleeWeaponColorA);
			WritePair(EnemyColors, ThrowableWeaponColorR);
			WritePair(EnemyColors, ThrowableWeaponColorG);
			WritePair(EnemyColors, ThrowableWeaponColorB);
			WritePair(EnemyColors, ThrowableWeaponColorA);
			// Glow
			WritePair(EnemyColors, InvisibleGlowColorR);
			WritePair(EnemyColors, InvisibleGlowColorG);
			WritePair(EnemyColors, InvisibleGlowColorB);
			WritePair(EnemyColors, VisibleGlowColorR);
			WritePair(EnemyColors, VisibleGlowColorG);
			WritePair(EnemyColors, VisibleGlowColorB);

			WritePair(EnemyColors, RedShieldColorR);
			WritePair(EnemyColors, RedShieldColorG);
			WritePair(EnemyColors, RedShieldColorG);
			WritePair(EnemyColors, PurpleShieldColorR);
			WritePair(EnemyColors, PurpleShieldColorG);
			WritePair(EnemyColors, PurpleShieldColorB);
			WritePair(EnemyColors, BlueShieldColorR);
			WritePair(EnemyColors, BlueShieldColorG);
			WritePair(EnemyColors, BlueShieldColorB);
			WritePair(EnemyColors, GreyShieldColorR);
			WritePair(EnemyColors, GreyShieldColorG);
			WritePair(EnemyColors, GreyShieldColorB);

			WriteSectionEnd();

			WriteSection(TeammateColors);

			WritePair(TeammateColors, InvisibleBoxColorR);
			WritePair(TeammateColors, InvisibleBoxColorG);
			WritePair(TeammateColors, InvisibleBoxColorB);
			WritePair(TeammateColors, InvisibleBoxColorA);
			WritePair(TeammateColors, VisibleBoxColorR);
			WritePair(TeammateColors, VisibleBoxColorG);
			WritePair(TeammateColors, VisibleBoxColorB);
			WritePair(TeammateColors, VisibleBoxColorA);
			WritePair(TeammateColors, InvisibleFilledBoxColorR);
			WritePair(TeammateColors, InvisibleFilledBoxColorG);
			WritePair(TeammateColors, InvisibleFilledBoxColorB);
			WritePair(TeammateColors, InvisibleFilledBoxColorA);
			WritePair(TeammateColors, VisibleFilledBoxColorR);
			WritePair(TeammateColors, VisibleFilledBoxColorG);
			WritePair(TeammateColors, VisibleFilledBoxColorB);
			WritePair(TeammateColors, VisibleFilledBoxColorA);
			WritePair(TeammateColors, InvisibleTracerColorR);
			WritePair(TeammateColors, InvisibleTracerColorG);
			WritePair(TeammateColors, InvisibleTracerColorB);
			WritePair(TeammateColors, InvisibleTracerColorA);
			WritePair(TeammateColors, VisibleTracerColorR);
			WritePair(TeammateColors, VisibleTracerColorG);
			WritePair(TeammateColors, VisibleTracerColorB);
			WritePair(TeammateColors, VisibleTracerColorA);
			WritePair(TeammateColors, InvisibleSkeletonColorR);
			WritePair(TeammateColors, InvisibleSkeletonColorG);
			WritePair(TeammateColors, InvisibleSkeletonColorB);
			WritePair(TeammateColors, InvisibleSkeletonColorA);
			WritePair(TeammateColors, VisibleSkeletonColorR);
			WritePair(TeammateColors, VisibleSkeletonColorG);
			WritePair(TeammateColors, VisibleSkeletonColorB);
			WritePair(TeammateColors, VisibleSkeletonColorA);
			WritePair(TeammateColors, InvisibleHeadCircleColorR);
			WritePair(TeammateColors, InvisibleHeadCircleColorG);
			WritePair(TeammateColors, InvisibleHeadCircleColorB);
			WritePair(TeammateColors, InvisibleHeadCircleColorA);
			WritePair(TeammateColors, VisibleHeadCircleColorR);
			WritePair(TeammateColors, VisibleHeadCircleColorG);
			WritePair(TeammateColors, VisibleHeadCircleColorB);
			WritePair(TeammateColors, VisibleHeadCircleColorA);
			WritePair(TeammateColors, InvisibleNameColorR);
			WritePair(TeammateColors, InvisibleNameColorG);
			WritePair(TeammateColors, InvisibleNameColorB);
			WritePair(TeammateColors, InvisibleNameColorA);
			WritePair(TeammateColors, VisibleNameColorR);
			WritePair(TeammateColors, VisibleNameColorG);
			WritePair(TeammateColors, VisibleNameColorB);
			WritePair(TeammateColors, VisibleNameColorA);

			WritePair(TeammateColors, InvisibleDistanceColorR);
			WritePair(TeammateColors, InvisibleDistanceColorG);
			WritePair(TeammateColors, InvisibleDistanceColorB);
			WritePair(TeammateColors, InvisibleDistanceColorA);
			WritePair(TeammateColors, VisibleDistanceColorR);
			WritePair(TeammateColors, VisibleDistanceColorG);
			WritePair(TeammateColors, VisibleDistanceColorB);
			WritePair(TeammateColors, VisibleDistanceColorA);

			WritePair(TeammateColors, InvisibleLegendColorR);
			WritePair(TeammateColors, InvisibleLegendColorG);
			WritePair(TeammateColors, InvisibleLegendColorB);
			WritePair(TeammateColors, InvisibleLegendColorA);
			WritePair(TeammateColors, VisibleLegendColorR);
			WritePair(TeammateColors, VisibleLegendColorG);
			WritePair(TeammateColors, VisibleLegendColorB);
			WritePair(TeammateColors, VisibleLegendColorA);
			// Bar
			WritePair(TeammateColors, BarColorMode);
			// WeaponESP Colors
			WritePair(TeammateColors, InvisibleWeaponColorR);
			WritePair(TeammateColors, InvisibleWeaponColorG);
			WritePair(TeammateColors, InvisibleWeaponColorB);
			WritePair(TeammateColors, InvisibleWeaponColorA);
			WritePair(TeammateColors, VisibleWeaponColorR);
			WritePair(TeammateColors, VisibleWeaponColorG);
			WritePair(TeammateColors, VisibleWeaponColorB);
			WritePair(TeammateColors, VisibleWeaponColorA);
			// Multiple
			WritePair(TeammateColors, LightWeaponColorR);
			WritePair(TeammateColors, LightWeaponColorG);
			WritePair(TeammateColors, LightWeaponColorB);
			WritePair(TeammateColors, LightWeaponColorA);
			WritePair(TeammateColors, HeavyWeaponColorR);
			WritePair(TeammateColors, HeavyWeaponColorG);
			WritePair(TeammateColors, HeavyWeaponColorB);
			WritePair(TeammateColors, HeavyWeaponColorA);
			WritePair(TeammateColors, EnergyWeaponColorR);
			WritePair(TeammateColors, EnergyWeaponColorG);
			WritePair(TeammateColors, EnergyWeaponColorB);
			WritePair(TeammateColors, EnergyWeaponColorA);
			WritePair(TeammateColors, ShotgunWeaponColorR);
			WritePair(TeammateColors, ShotgunWeaponColorG);
			WritePair(TeammateColors, ShotgunWeaponColorB);
			WritePair(TeammateColors, ShotgunWeaponColorA);
			WritePair(TeammateColors, SniperWeaponColorR);
			WritePair(TeammateColors, SniperWeaponColorG);
			WritePair(TeammateColors, SniperWeaponColorB);
			WritePair(TeammateColors, SniperWeaponColorA);
			WritePair(TeammateColors, LegendaryWeaponColorR);
			WritePair(TeammateColors, LegendaryWeaponColorG);
			WritePair(TeammateColors, LegendaryWeaponColorB);
			WritePair(TeammateColors, LegendaryWeaponColorA);
			WritePair(TeammateColors, MeleeWeaponColorR);
			WritePair(TeammateColors, MeleeWeaponColorG);
			WritePair(TeammateColors, MeleeWeaponColorB);
			WritePair(TeammateColors, MeleeWeaponColorA);
			WritePair(TeammateColors, ThrowableWeaponColorR);
			WritePair(TeammateColors, ThrowableWeaponColorG);
			WritePair(TeammateColors, ThrowableWeaponColorB);
			WritePair(TeammateColors, ThrowableWeaponColorA);

			WriteSectionEnd();

			WriteSection(Watermark);

			WritePair(Watermark, Watermark);
			WritePair(Watermark, WatermarkPosition);
			WritePair(Watermark, Name);
			WritePair(Watermark, ProcessingSpeed);
			WritePair(Watermark, Spectators);

			WriteSectionEnd();

			WriteSection(Settings);

			WritePair(Settings, GamemodeCheck);
			WritePair(Settings, ESPEnabled);
			WritePair(Settings, OverlayEnabled);
			WritePair(Settings, FPSCap);
			WritePair(Settings, CappedFPS);

			WriteSectionEnd();

			conf.close();
		}
	}

	void SaveConfig()
	{
		if (!Legit->Save()) std::cout << "something went wrong trying to save Legitbot settings" << std::endl;
		if (!Rage->Save()) std::cout << "something went wrong trying to save Ragebot settings" << std::endl;
		if (!Trigger->Save()) std::cout << "something went wrong trying to save Triggerbot settings" << std::endl;
		if (!Flick->Save()) std::cout << "something went wrong trying to save Flickbot settings" << std::endl;
		if (!GlowESP->Save()) std::cout << "something went wrong trying to save Glow settings" << std::endl;
		if (!ESP->Save()) std::cout << "something went wrong trying to save Sense/ESP settings" << std::endl;
		if (!MiscTab->Save()) std::cout << "something went wrong trying to save Misc settings" << std::endl;
		if (!MapRadar->Save()) std::cout << "something went wrong trying to save Radar settings" << std::endl;
		if (!SaveOtherSettings()) std::cout << "something went wrong trying to save Other settings" << std::endl;
		UpdateConfig();
	}

	bool SaveOtherSettings()
	{
		try
		{
			Config::Settings::GamemodeCheck = Features::Settings::GamemodeCheck;
			Config::Settings::ESPEnabled = Features::Settings::ESPEnabled;
			Config::Settings::OverlayEnabled = Features::Settings::OverlayEnabled;
			Config::Settings::FPSCap = Features::Settings::FPSCap;
			Config::Settings::CappedFPS = Features::Settings::CappedFPS;

			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	void LoadConfig()
	{
		std::string ConfigName1 = "Configs/" + std::string(configName) + ".ini";
		bool success = ReadConfig();
		if (success)
			std::cout << "successfully read config" << std::endl;
		else
			std::cout << "can't read config for some reason so new config file has been created" << std::endl;

		Features::Aimbot::AimbotEnabled = Config::Aimbot::AimbotEnabled;
		Features::Aimbot::BindMethod = Config::Aimbot::BindMethod;
		Features::Aimbot::AimbotMode = Config::Aimbot::AimbotMode;
		Features::Aimbot::InputMethod = Config::Aimbot::InputMethod;
		Features::Aimbot::ClosestHitbox = Config::Aimbot::ClosestHitbox;
		Features::Aimbot::OnFire = Config::Aimbot::OnFire;
		Features::Aimbot::OnADS = Config::Aimbot::OnADS;
		Features::Aimbot::VisCheck = Config::Aimbot::VisCheck;
		Features::Aimbot::TeamCheck = Config::Aimbot::TeamCheck;
		Features::Aimbot::TargetSwitching = Config::Aimbot::TargetSwitching;
		Features::Aimbot::Priority = Config::Aimbot::Priority;
		Features::Aimbot::PredictMovement = Config::Aimbot::PredictMovement;
		Features::Aimbot::PredictBulletDrop = Config::Aimbot::PredictBulletDrop;
		Features::Aimbot::FinalDistance = Config::Aimbot::FinalDistance;
		Features::Aimbot::Smooth = Config::Aimbot::Smooth;
		Features::Aimbot::Speed = Config::Aimbot::Speed;
		Features::Aimbot::HipfireSmooth = Config::Aimbot::HipfireSmooth;
        Features::Aimbot::ADSSmooth = Config::Aimbot::ADSSmooth;
        Features::Aimbot::SmoothDistance = Config::Aimbot::SmoothDistance;
        Features::Aimbot::Delay = Config::Aimbot::Delay;
        Features::Aimbot::FOV = Config::Aimbot::FOV;
        Features::Aimbot::ZoomScale = Config::Aimbot::ZoomScale;
        Features::Aimbot::MinDistance = Config::Aimbot::MinDistance;
        Features::Aimbot::HipfireDistance = Config::Aimbot::HipfireDistance;
        Features::Aimbot::ZoomDistance = Config::Aimbot::ZoomDistance;
        //AimMode 2
        Features::Aimbot::HipfireSmooth1 = Config::Aimbot::HipfireSmooth1;
        Features::Aimbot::ADSSmooth1 = Config::Aimbot::ADSSmooth1;
        Features::Aimbot::ExtraSmoothing = Config::Aimbot::ExtraSmoothing;
        Features::Aimbot::Deadzone = Config::Aimbot::Deadzone;
        Features::Aimbot::FOV1 = Config::Aimbot::FOV1;
        Features::Aimbot::MinDistance2 = Config::Aimbot::MinDistance2;
        Features::Aimbot::MaxDistance2 = Config::Aimbot::MaxDistance2;
        //Weapon Toggles
        //Light
        Features::Aimbot::P2020 = Config::Aimbot::P2020;
        Features::Aimbot::RE45 = Config::Aimbot::RE45;
        Features::Aimbot::Alternator = Config::Aimbot::Alternator;
        Features::Aimbot::R99 = Config::Aimbot::R99;
        Features::Aimbot::R301 = Config::Aimbot::R301;
        Features::Aimbot::Spitfire = Config::Aimbot::Spitfire;
        Features::Aimbot::G7 = Config::Aimbot::G7;
        //Heavy
        Features::Aimbot::Flatline = Config::Aimbot::Flatline;
        Features::Aimbot::Hemlock = Config::Aimbot::Hemlock;
        Features::Aimbot::Repeater = Config::Aimbot::Repeater;
        Features::Aimbot::Rampage = Config::Aimbot::Rampage;
        Features::Aimbot::CARSMG = Config::Aimbot::CARSMG;
        //Energy
        Features::Aimbot::Havoc = Config::Aimbot::Havoc;
        Features::Aimbot::Devotion = Config::Aimbot::Devotion;
        Features::Aimbot::LSTAR = Config::Aimbot::LSTAR;
        Features::Aimbot::TripleTake = Config::Aimbot::TripleTake;
        Features::Aimbot::Volt = Config::Aimbot::Volt;
        Features::Aimbot::Nemesis = Config::Aimbot::Nemesis;
        //Shotgun
        Features::Aimbot::Mozambique = Config::Aimbot::Mozambique;
        Features::Aimbot::EVA8 = Config::Aimbot::EVA8;
        Features::Aimbot::Peacekeeper = Config::Aimbot::Peacekeeper;
        Features::Aimbot::Mastiff = Config::Aimbot::Mastiff;
        //Snipers
        Features::Aimbot::Longbow = Config::Aimbot::Longbow;
        Features::Aimbot::ChargeRifle = Config::Aimbot::ChargeRifle;
        Features::Aimbot::Sentinel = Config::Aimbot::Sentinel;
        //Legendary
        Features::Aimbot::Wingman = Config::Aimbot::Wingman;
        Features::Aimbot::Prowler = Config::Aimbot::Prowler;
        Features::Aimbot::Bocek = Config::Aimbot::Bocek;
        Features::Aimbot::Kraber = Config::Aimbot::Kraber;
        Features::Aimbot::Knife = Config::Aimbot::Knife;
        //---------------Advanced---------------//
        Features::Aimbot::AdvancedAim = Config::Aimbot::AdvancedAim;
        Features::Aimbot::AdvancedFire = Config::Aimbot::AdvancedFire;
        Features::Aimbot::AdvancedADS = Config::Aimbot::AdvancedADS;
        //Aimbot Mode 0 - xap-client
        Features::Aimbot::AdvancedClosestHitbox = Config::Aimbot::AdvancedClosestHitbox;
        Features::Aimbot::AdvancedHitbox = Config::Aimbot::AdvancedHitbox;
        Features::Aimbot::AdvancedSpeed = Config::Aimbot::AdvancedSpeed;
        Features::Aimbot::AdvancedSmooth = Config::Aimbot::AdvancedSmooth;
        Features::Aimbot::AdvancedHipfireSmooth = Config::Aimbot::AdvancedHipfireSmooth;
        Features::Aimbot::AdvancedADSSmooth = Config::Aimbot::AdvancedADSSmooth;
        //Aimbot Mode 1 - Grinder
        Features::Aimbot::AdvancedHipfireSmooth1 = Config::Aimbot::AdvancedHipfireSmooth1;
        Features::Aimbot::AdvancedADSSmooth1 = Config::Aimbot::AdvancedADSSmooth1;
        Features::Aimbot::AdvancedExtraSmooth1 = Config::Aimbot::AdvancedExtraSmooth1;
        Features::Aimbot::AdvancedFOV1 = Config::Aimbot::AdvancedFOV1;
        Features::Aimbot::AdvancedDeadzone = Config::Aimbot::AdvancedDeadzone;
        Features::Aimbot::AdvancedMinDistance1 = Config::Aimbot::AdvancedMinDistance1;
        Features::Aimbot::AdvancedMaxDistance1 = Config::Aimbot::AdvancedMaxDistance1;
		//Advanced Aimbot Settings
		Features::Aimbot::P2020Fire = Config::Aimbot::P2020Fire;
		Features::Aimbot::P2020ADS = Config::Aimbot::P2020ADS;
		Features::Aimbot::P2020ClosestHitbox = Config::Aimbot::P2020ClosestHitbox;
		Features::Aimbot::P2020Hitbox = Config::Aimbot::P2020Hitbox;
		Features::Aimbot::P2020Speed = Config::Aimbot::P2020Speed;
		Features::Aimbot::P2020HipfireSmooth = Config::Aimbot::P2020HipfireSmooth;
		Features::Aimbot::P2020ADSSmooth = Config::Aimbot::P2020ADSSmooth;
		Features::Aimbot::P2020FOV = Config::Aimbot::P2020FOV;
		Features::Aimbot::P2020ZoomScale = Config::Aimbot::P2020ZoomScale;
		Features::Aimbot::P2020HipfireSmooth1 = Config::Aimbot::P2020HipfireSmooth1;
		Features::Aimbot::P2020ADSSmooth1 = Config::Aimbot::P2020ADSSmooth1;
		Features::Aimbot::P2020ExtraSmooth1 = Config::Aimbot::P2020ExtraSmooth1;
		Features::Aimbot::P2020Deadzone = Config::Aimbot::P2020Deadzone;
		Features::Aimbot::P2020FOV1 = Config::Aimbot::P2020FOV1;
		Features::Aimbot::P2020MinDistance1 = Config::Aimbot::P2020MinDistance1;
		Features::Aimbot::P2020MaxDistance1 = Config::Aimbot::P2020MaxDistance1;
		Features::Aimbot::RE45Fire = Config::Aimbot::RE45Fire;
		Features::Aimbot::RE45ADS = Config::Aimbot::RE45ADS;
		Features::Aimbot::RE45ClosestHitbox = Config::Aimbot::RE45ClosestHitbox;
		Features::Aimbot::RE45Hitbox = Config::Aimbot::RE45Hitbox;
		Features::Aimbot::RE45Speed = Config::Aimbot::RE45Speed;
		Features::Aimbot::RE45HipfireSmooth = Config::Aimbot::RE45HipfireSmooth;
		Features::Aimbot::RE45ADSSmooth = Config::Aimbot::RE45ADSSmooth;
		Features::Aimbot::RE45FOV = Config::Aimbot::RE45FOV;
		Features::Aimbot::RE45ZoomScale = Config::Aimbot::RE45ZoomScale;
		Features::Aimbot::RE45HipfireSmooth1 = Config::Aimbot::RE45HipfireSmooth1;
		Features::Aimbot::RE45ADSSmooth1 = Config::Aimbot::RE45ADSSmooth1;
		Features::Aimbot::RE45ExtraSmooth1 = Config::Aimbot::RE45ExtraSmooth1;
		Features::Aimbot::RE45Deadzone = Config::Aimbot::RE45Deadzone;
		Features::Aimbot::RE45FOV1 = Config::Aimbot::RE45FOV1;
		Features::Aimbot::RE45MinDistance1 = Config::Aimbot::RE45MinDistance1;
		Features::Aimbot::RE45MaxDistance1 = Config::Aimbot::RE45MaxDistance1;
		Features::Aimbot::AlternatorFire = Config::Aimbot::AlternatorFire;
		Features::Aimbot::AlternatorADS = Config::Aimbot::AlternatorADS;
		Features::Aimbot::AlternatorClosestHitbox = Config::Aimbot::AlternatorClosestHitbox;
		Features::Aimbot::AlternatorHitbox = Config::Aimbot::AlternatorHitbox;
		Features::Aimbot::AlternatorSpeed = Config::Aimbot::AlternatorSpeed;
		Features::Aimbot::AlternatorHipfireSmooth = Config::Aimbot::AlternatorHipfireSmooth;
		Features::Aimbot::AlternatorADSSmooth = Config::Aimbot::AlternatorADSSmooth;
		Features::Aimbot::AlternatorFOV = Config::Aimbot::AlternatorFOV;
		Features::Aimbot::AlternatorZoomScale = Config::Aimbot::AlternatorZoomScale;
		Features::Aimbot::AlternatorHipfireSmooth1 = Config::Aimbot::AlternatorHipfireSmooth1;
		Features::Aimbot::AlternatorADSSmooth1 = Config::Aimbot::AlternatorADSSmooth1;
		Features::Aimbot::AlternatorExtraSmooth1 = Config::Aimbot::AlternatorExtraSmooth1;
		Features::Aimbot::AlternatorDeadzone = Config::Aimbot::AlternatorDeadzone;
		Features::Aimbot::AlternatorFOV1 = Config::Aimbot::AlternatorFOV1;
		Features::Aimbot::AlternatorMinDistance1 = Config::Aimbot::AlternatorMinDistance1;
		Features::Aimbot::AlternatorMaxDistance1 = Config::Aimbot::AlternatorMaxDistance1;
		Features::Aimbot::R99Fire = Config::Aimbot::R99Fire;
		Features::Aimbot::R99ADS = Config::Aimbot::R99ADS;
		Features::Aimbot::R99ClosestHitbox = Config::Aimbot::R99ClosestHitbox;
		Features::Aimbot::R99Hitbox = Config::Aimbot::R99Hitbox;
		Features::Aimbot::R99Speed = Config::Aimbot::R99Speed;
		Features::Aimbot::R99HipfireSmooth = Config::Aimbot::R99HipfireSmooth;
		Features::Aimbot::R99ADSSmooth = Config::Aimbot::R99ADSSmooth;
		Features::Aimbot::R99FOV = Config::Aimbot::R99FOV;
		Features::Aimbot::R99ZoomScale = Config::Aimbot::R99ZoomScale;
		Features::Aimbot::R99HipfireSmooth1 = Config::Aimbot::R99HipfireSmooth1;
		Features::Aimbot::R99ADSSmooth1 = Config::Aimbot::R99ADSSmooth1;
		Features::Aimbot::R99ExtraSmooth1 = Config::Aimbot::R99ExtraSmooth1;
		Features::Aimbot::R99Deadzone = Config::Aimbot::R99Deadzone;
		Features::Aimbot::R99FOV1 = Config::Aimbot::R99FOV1;
		Features::Aimbot::R99MinDistance1 = Config::Aimbot::R99MinDistance1;
		Features::Aimbot::R99MaxDistance1 = Config::Aimbot::R99MaxDistance1;
		Features::Aimbot::R301Fire = Config::Aimbot::R301Fire;
		Features::Aimbot::R301ADS = Config::Aimbot::R301ADS;
		Features::Aimbot::R301ClosestHitbox = Config::Aimbot::R301ClosestHitbox;
		Features::Aimbot::R301Hitbox = Config::Aimbot::R301Hitbox;
		Features::Aimbot::R301Speed = Config::Aimbot::R301Speed;
		Features::Aimbot::R301HipfireSmooth = Config::Aimbot::R301HipfireSmooth;
		Features::Aimbot::R301ADSSmooth = Config::Aimbot::R301ADSSmooth;
		Features::Aimbot::R301FOV = Config::Aimbot::R301FOV;
		Features::Aimbot::R301ZoomScale = Config::Aimbot::R301ZoomScale;
		Features::Aimbot::R301HipfireSmooth1 = Config::Aimbot::R301HipfireSmooth1;
		Features::Aimbot::R301ADSSmooth1 = Config::Aimbot::R301ADSSmooth1;
		Features::Aimbot::R301ExtraSmooth1 = Config::Aimbot::R301ExtraSmooth1;
		Features::Aimbot::R301Deadzone = Config::Aimbot::R301Deadzone;
		Features::Aimbot::R301FOV1 = Config::Aimbot::R301FOV1;
		Features::Aimbot::R301MinDistance1 = Config::Aimbot::R301MinDistance1;
		Features::Aimbot::R301MaxDistance1 = Config::Aimbot::R301MaxDistance1;
		Features::Aimbot::SpitfireFire = Config::Aimbot::SpitfireFire;
		Features::Aimbot::SpitfireADS = Config::Aimbot::SpitfireADS;
		Features::Aimbot::SpitfireClosestHitbox = Config::Aimbot::SpitfireClosestHitbox;
		Features::Aimbot::SpitfireHitbox = Config::Aimbot::SpitfireHitbox;
		Features::Aimbot::SpitfireSpeed = Config::Aimbot::SpitfireSpeed;
		Features::Aimbot::SpitfireHipfireSmooth = Config::Aimbot::SpitfireHipfireSmooth;
		Features::Aimbot::SpitfireADSSmooth = Config::Aimbot::SpitfireADSSmooth;
		Features::Aimbot::SpitfireFOV = Config::Aimbot::SpitfireFOV;
		Features::Aimbot::SpitfireZoomScale = Config::Aimbot::SpitfireZoomScale;
		Features::Aimbot::SpitfireHipfireSmooth1 = Config::Aimbot::SpitfireHipfireSmooth1;
		Features::Aimbot::SpitfireADSSmooth1 = Config::Aimbot::SpitfireADSSmooth1;
		Features::Aimbot::SpitfireExtraSmooth1 = Config::Aimbot::SpitfireExtraSmooth1;
		Features::Aimbot::SpitfireDeadzone = Config::Aimbot::SpitfireDeadzone;
		Features::Aimbot::SpitfireFOV1 = Config::Aimbot::SpitfireFOV1;
		Features::Aimbot::SpitfireMinDistance1 = Config::Aimbot::SpitfireMinDistance1;
		Features::Aimbot::SpitfireMaxDistance1 = Config::Aimbot::SpitfireMaxDistance1;
		Features::Aimbot::G7Fire = Config::Aimbot::G7Fire;
		Features::Aimbot::G7ADS = Config::Aimbot::G7ADS;
		Features::Aimbot::G7ClosestHitbox = Config::Aimbot::G7ClosestHitbox;
		Features::Aimbot::G7Hitbox = Config::Aimbot::G7Hitbox;
		Features::Aimbot::G7Speed = Config::Aimbot::G7Speed;
		Features::Aimbot::G7HipfireSmooth = Config::Aimbot::G7HipfireSmooth;
		Features::Aimbot::G7ADSSmooth = Config::Aimbot::G7ADSSmooth;
		Features::Aimbot::G7FOV = Config::Aimbot::G7FOV;
		Features::Aimbot::G7ZoomScale = Config::Aimbot::G7ZoomScale;
		Features::Aimbot::G7HipfireSmooth1 = Config::Aimbot::G7HipfireSmooth1;
		Features::Aimbot::G7ADSSmooth1 = Config::Aimbot::G7ADSSmooth1;
		Features::Aimbot::G7ExtraSmooth1 = Config::Aimbot::G7ExtraSmooth1;
		Features::Aimbot::G7Deadzone = Config::Aimbot::G7Deadzone;
		Features::Aimbot::G7FOV1 = Config::Aimbot::G7FOV1;
		Features::Aimbot::G7MinDistance1 = Config::Aimbot::G7MinDistance1;
		Features::Aimbot::G7MaxDistance1 = Config::Aimbot::G7MaxDistance1;
		Features::Aimbot::FlatlineFire = Config::Aimbot::FlatlineFire;
		Features::Aimbot::FlatlineADS = Config::Aimbot::FlatlineADS;
		Features::Aimbot::FlatlineClosestHitbox = Config::Aimbot::FlatlineClosestHitbox;
		Features::Aimbot::FlatlineHitbox = Config::Aimbot::FlatlineHitbox;
		Features::Aimbot::FlatlineSpeed = Config::Aimbot::FlatlineSpeed;
		Features::Aimbot::FlatlineHipfireSmooth = Config::Aimbot::FlatlineHipfireSmooth;
		Features::Aimbot::FlatlineADSSmooth = Config::Aimbot::FlatlineADSSmooth;
		Features::Aimbot::FlatlineFOV = Config::Aimbot::FlatlineFOV;
		Features::Aimbot::FlatlineZoomScale = Config::Aimbot::FlatlineZoomScale;
		Features::Aimbot::FlatlineHipfireSmooth1 = Config::Aimbot::FlatlineHipfireSmooth1;
		Features::Aimbot::FlatlineADSSmooth1 = Config::Aimbot::FlatlineADSSmooth1;
		Features::Aimbot::FlatlineExtraSmooth1 = Config::Aimbot::FlatlineExtraSmooth1;
		Features::Aimbot::FlatlineDeadzone = Config::Aimbot::FlatlineDeadzone;
		Features::Aimbot::FlatlineFOV1 = Config::Aimbot::FlatlineFOV1;
		Features::Aimbot::FlatlineMinDistance1 = Config::Aimbot::FlatlineMinDistance1;
		Features::Aimbot::FlatlineMaxDistance1 = Config::Aimbot::FlatlineMaxDistance1;
		Features::Aimbot::HemlockFire = Config::Aimbot::HemlockFire;
		Features::Aimbot::HemlockADS = Config::Aimbot::HemlockADS;
		Features::Aimbot::HemlockClosestHitbox = Config::Aimbot::HemlockClosestHitbox;
		Features::Aimbot::HemlockHitbox = Config::Aimbot::HemlockHitbox;
		Features::Aimbot::HemlockSpeed = Config::Aimbot::HemlockSpeed;
		Features::Aimbot::HemlockHipfireSmooth = Config::Aimbot::HemlockHipfireSmooth;
		Features::Aimbot::HemlockADSSmooth = Config::Aimbot::HemlockADSSmooth;
		Features::Aimbot::HemlockFOV = Config::Aimbot::HemlockFOV;
		Features::Aimbot::HemlockZoomScale = Config::Aimbot::HemlockZoomScale;
		Features::Aimbot::HemlockHipfireSmooth1 = Config::Aimbot::HemlockHipfireSmooth1;
		Features::Aimbot::HemlockADSSmooth1 = Config::Aimbot::HemlockADSSmooth1;
		Features::Aimbot::HemlockExtraSmooth1 = Config::Aimbot::HemlockExtraSmooth1;
		Features::Aimbot::HemlockDeadzone = Config::Aimbot::HemlockDeadzone;
		Features::Aimbot::HemlockFOV1 = Config::Aimbot::HemlockFOV1;
		Features::Aimbot::HemlockMinDistance1 = Config::Aimbot::HemlockMinDistance1;
		Features::Aimbot::HemlockMaxDistance1 = Config::Aimbot::HemlockMaxDistance1;
		Features::Aimbot::ProwlerFire = Config::Aimbot::ProwlerFire;
		Features::Aimbot::ProwlerADS = Config::Aimbot::ProwlerADS;
		Features::Aimbot::ProwlerClosestHitbox = Config::Aimbot::ProwlerClosestHitbox;
		Features::Aimbot::ProwlerHitbox = Config::Aimbot::ProwlerHitbox;
		Features::Aimbot::ProwlerSpeed = Config::Aimbot::ProwlerSpeed;
		Features::Aimbot::ProwlerHipfireSmooth = Config::Aimbot::ProwlerHipfireSmooth;
		Features::Aimbot::ProwlerADSSmooth = Config::Aimbot::ProwlerADSSmooth;
		Features::Aimbot::ProwlerFOV = Config::Aimbot::ProwlerFOV;
		Features::Aimbot::ProwlerZoomScale = Config::Aimbot::ProwlerZoomScale;
		Features::Aimbot::ProwlerHipfireSmooth1 = Config::Aimbot::ProwlerHipfireSmooth1;
		Features::Aimbot::ProwlerADSSmooth1 = Config::Aimbot::ProwlerADSSmooth1;
		Features::Aimbot::ProwlerExtraSmooth1 = Config::Aimbot::ProwlerExtraSmooth1;
		Features::Aimbot::ProwlerDeadzone = Config::Aimbot::ProwlerDeadzone;
		Features::Aimbot::ProwlerFOV1 = Config::Aimbot::ProwlerFOV1;
		Features::Aimbot::ProwlerMinDistance1 = Config::Aimbot::ProwlerMinDistance1;
		Features::Aimbot::ProwlerMaxDistance1 = Config::Aimbot::ProwlerMaxDistance1;
		Features::Aimbot::RepeaterFire = Config::Aimbot::RepeaterFire;
		Features::Aimbot::RepeaterADS = Config::Aimbot::RepeaterADS;
		Features::Aimbot::RepeaterClosestHitbox = Config::Aimbot::RepeaterClosestHitbox;
		Features::Aimbot::RepeaterHitbox = Config::Aimbot::RepeaterHitbox;
		Features::Aimbot::RepeaterSpeed = Config::Aimbot::RepeaterSpeed;
		Features::Aimbot::RepeaterHipfireSmooth = Config::Aimbot::RepeaterHipfireSmooth;
		Features::Aimbot::RepeaterADSSmooth = Config::Aimbot::RepeaterADSSmooth;
		Features::Aimbot::RepeaterFOV = Config::Aimbot::RepeaterFOV;
		Features::Aimbot::RepeaterZoomScale = Config::Aimbot::RepeaterZoomScale;
		Features::Aimbot::RepeaterHipfireSmooth1 = Config::Aimbot::RepeaterHipfireSmooth1;
		Features::Aimbot::RepeaterADSSmooth1 = Config::Aimbot::RepeaterADSSmooth1;
		Features::Aimbot::RepeaterExtraSmooth1 = Config::Aimbot::RepeaterExtraSmooth1;
		Features::Aimbot::RepeaterDeadzone = Config::Aimbot::RepeaterDeadzone;
		Features::Aimbot::RepeaterFOV1 = Config::Aimbot::RepeaterFOV1;
		Features::Aimbot::RepeaterMinDistance1 = Config::Aimbot::RepeaterMinDistance1;
		Features::Aimbot::RepeaterMaxDistance1 = Config::Aimbot::RepeaterMaxDistance1;
		Features::Aimbot::RampageFire = Config::Aimbot::RampageFire;
		Features::Aimbot::RampageADS = Config::Aimbot::RampageADS;
		Features::Aimbot::RampageClosestHitbox = Config::Aimbot::RampageClosestHitbox;
		Features::Aimbot::RampageHitbox = Config::Aimbot::RampageHitbox;
		Features::Aimbot::RampageSpeed = Config::Aimbot::RampageSpeed;
		Features::Aimbot::RampageHipfireSmooth = Config::Aimbot::RampageHipfireSmooth;
		Features::Aimbot::RampageADSSmooth = Config::Aimbot::RampageADSSmooth;
		Features::Aimbot::RampageFOV = Config::Aimbot::RampageFOV;
		Features::Aimbot::RampageZoomScale = Config::Aimbot::RampageZoomScale;
		Features::Aimbot::RampageHipfireSmooth1 = Config::Aimbot::RampageHipfireSmooth1;
		Features::Aimbot::RampageADSSmooth1 = Config::Aimbot::RampageADSSmooth1;
		Features::Aimbot::RampageExtraSmooth1 = Config::Aimbot::RampageExtraSmooth1;
		Features::Aimbot::RampageDeadzone = Config::Aimbot::RampageDeadzone;
		Features::Aimbot::RampageFOV1 = Config::Aimbot::RampageFOV1;
		Features::Aimbot::RampageMinDistance1 = Config::Aimbot::RampageMinDistance1;
		Features::Aimbot::RampageMaxDistance1 = Config::Aimbot::RampageMaxDistance1;
		Features::Aimbot::CARSMGFire = Config::Aimbot::CARSMGFire;
		Features::Aimbot::CARSMGADS = Config::Aimbot::CARSMGADS;
		Features::Aimbot::CARSMGClosestHitbox = Config::Aimbot::CARSMGClosestHitbox;
		Features::Aimbot::CARSMGHitbox = Config::Aimbot::CARSMGHitbox;
		Features::Aimbot::CARSMGSpeed = Config::Aimbot::CARSMGSpeed;
		Features::Aimbot::CARSMGHipfireSmooth = Config::Aimbot::CARSMGHipfireSmooth;
		Features::Aimbot::CARSMGADSSmooth = Config::Aimbot::CARSMGADSSmooth;
		Features::Aimbot::CARSMGFOV = Config::Aimbot::CARSMGFOV;
		Features::Aimbot::CARSMGZoomScale = Config::Aimbot::CARSMGZoomScale;
		Features::Aimbot::CARSMGHipfireSmooth1 = Config::Aimbot::CARSMGHipfireSmooth1;
		Features::Aimbot::CARSMGADSSmooth1 = Config::Aimbot::CARSMGADSSmooth1;
		Features::Aimbot::CARSMGExtraSmooth1 = Config::Aimbot::CARSMGExtraSmooth1;
		Features::Aimbot::CARSMGDeadzone = Config::Aimbot::CARSMGDeadzone;
		Features::Aimbot::CARSMGFOV1 = Config::Aimbot::CARSMGFOV1;
		Features::Aimbot::CARSMGMinDistance1 = Config::Aimbot::CARSMGMinDistance1;
		Features::Aimbot::CARSMGMaxDistance1 = Config::Aimbot::CARSMGMaxDistance1;
		Features::Aimbot::HavocFire = Config::Aimbot::HavocFire;
		Features::Aimbot::HavocADS = Config::Aimbot::HavocADS;
		Features::Aimbot::HavocClosestHitbox = Config::Aimbot::HavocClosestHitbox;
		Features::Aimbot::HavocHitbox = Config::Aimbot::HavocHitbox;
		Features::Aimbot::HavocSpeed = Config::Aimbot::HavocSpeed;
		Features::Aimbot::HavocHipfireSmooth = Config::Aimbot::HavocHipfireSmooth;
		Features::Aimbot::HavocADSSmooth = Config::Aimbot::HavocADSSmooth;
		Features::Aimbot::HavocFOV = Config::Aimbot::HavocFOV;
		Features::Aimbot::HavocZoomScale = Config::Aimbot::HavocZoomScale;
		Features::Aimbot::HavocHipfireSmooth1 = Config::Aimbot::HavocHipfireSmooth1;
		Features::Aimbot::HavocADSSmooth1 = Config::Aimbot::HavocADSSmooth1;
		Features::Aimbot::HavocExtraSmooth1 = Config::Aimbot::HavocExtraSmooth1;
		Features::Aimbot::HavocDeadzone = Config::Aimbot::HavocDeadzone;
		Features::Aimbot::HavocFOV1 = Config::Aimbot::HavocFOV1;
		Features::Aimbot::HavocMinDistance1 = Config::Aimbot::HavocMinDistance1;
		Features::Aimbot::HavocMaxDistance1 = Config::Aimbot::HavocMaxDistance1;
		Features::Aimbot::DevotionFire = Config::Aimbot::DevotionFire;
		Features::Aimbot::DevotionADS = Config::Aimbot::DevotionADS;
		Features::Aimbot::DevotionClosestHitbox = Config::Aimbot::DevotionClosestHitbox;
		Features::Aimbot::DevotionHitbox = Config::Aimbot::DevotionHitbox;
		Features::Aimbot::DevotionSpeed = Config::Aimbot::DevotionSpeed;
		Features::Aimbot::DevotionHipfireSmooth = Config::Aimbot::DevotionHipfireSmooth;
		Features::Aimbot::DevotionADSSmooth = Config::Aimbot::DevotionADSSmooth;
		Features::Aimbot::DevotionFOV = Config::Aimbot::DevotionFOV;
		Features::Aimbot::DevotionZoomScale = Config::Aimbot::DevotionZoomScale;
		Features::Aimbot::DevotionHipfireSmooth1 = Config::Aimbot::DevotionHipfireSmooth1;
		Features::Aimbot::DevotionADSSmooth1 = Config::Aimbot::DevotionADSSmooth1;
		Features::Aimbot::DevotionExtraSmooth1 = Config::Aimbot::DevotionExtraSmooth1;
		Features::Aimbot::DevotionDeadzone = Config::Aimbot::DevotionDeadzone;
		Features::Aimbot::DevotionFOV1 = Config::Aimbot::DevotionFOV1;
		Features::Aimbot::DevotionMinDistance1 = Config::Aimbot::DevotionMinDistance1;
		Features::Aimbot::DevotionMaxDistance1 = Config::Aimbot::DevotionMaxDistance1;
		Features::Aimbot::LSTARFire = Config::Aimbot::LSTARFire;
		Features::Aimbot::LSTARADS = Config::Aimbot::LSTARADS;
		Features::Aimbot::LSTARClosestHitbox = Config::Aimbot::LSTARClosestHitbox;
		Features::Aimbot::LSTARHitbox = Config::Aimbot::LSTARHitbox;
		Features::Aimbot::LSTARSpeed = Config::Aimbot::LSTARSpeed;
		Features::Aimbot::LSTARHipfireSmooth = Config::Aimbot::LSTARHipfireSmooth;
		Features::Aimbot::LSTARADSSmooth = Config::Aimbot::LSTARADSSmooth;
		Features::Aimbot::LSTARFOV = Config::Aimbot::LSTARFOV;
		Features::Aimbot::LSTARZoomScale = Config::Aimbot::LSTARZoomScale;
		Features::Aimbot::LSTARHipfireSmooth1 = Config::Aimbot::LSTARHipfireSmooth1;
		Features::Aimbot::LSTARADSSmooth1 = Config::Aimbot::LSTARADSSmooth1;
		Features::Aimbot::LSTARExtraSmooth1 = Config::Aimbot::LSTARExtraSmooth1;
		Features::Aimbot::LSTARDeadzone = Config::Aimbot::LSTARDeadzone;
		Features::Aimbot::LSTARFOV1 = Config::Aimbot::LSTARFOV1;
		Features::Aimbot::LSTARMinDistance1 = Config::Aimbot::LSTARMinDistance1;
		Features::Aimbot::LSTARMaxDistance1 = Config::Aimbot::LSTARMaxDistance1;
		Features::Aimbot::TripleTakeFire = Config::Aimbot::TripleTakeFire;
		Features::Aimbot::TripleTakeADS = Config::Aimbot::TripleTakeADS;
		Features::Aimbot::TripleTakeClosestHitbox = Config::Aimbot::TripleTakeClosestHitbox;
		Features::Aimbot::TripleTakeHitbox = Config::Aimbot::TripleTakeHitbox;
		Features::Aimbot::TripleTakeSpeed = Config::Aimbot::TripleTakeSpeed;
		Features::Aimbot::TripleTakeHipfireSmooth = Config::Aimbot::TripleTakeHipfireSmooth;
		Features::Aimbot::TripleTakeADSSmooth = Config::Aimbot::TripleTakeADSSmooth;
		Features::Aimbot::TripleTakeFOV = Config::Aimbot::TripleTakeFOV;
		Features::Aimbot::TripleTakeZoomScale = Config::Aimbot::TripleTakeZoomScale;
		Features::Aimbot::TripleTakeHipfireSmooth1 = Config::Aimbot::TripleTakeHipfireSmooth1;
		Features::Aimbot::TripleTakeADSSmooth1 = Config::Aimbot::TripleTakeADSSmooth1;
		Features::Aimbot::TripleTakeExtraSmooth1 = Config::Aimbot::TripleTakeExtraSmooth1;
		Features::Aimbot::TripleTakeDeadzone = Config::Aimbot::TripleTakeDeadzone;
		Features::Aimbot::TripleTakeFOV1 = Config::Aimbot::TripleTakeFOV1;
		Features::Aimbot::TripleTakeMinDistance1 = Config::Aimbot::TripleTakeMinDistance1;
		Features::Aimbot::TripleTakeMaxDistance1 = Config::Aimbot::TripleTakeMaxDistance1;
		Features::Aimbot::VoltFire = Config::Aimbot::VoltFire;
		Features::Aimbot::VoltADS = Config::Aimbot::VoltADS;
		Features::Aimbot::VoltClosestHitbox = Config::Aimbot::VoltClosestHitbox;
		Features::Aimbot::VoltHitbox = Config::Aimbot::VoltHitbox;
		Features::Aimbot::VoltSpeed = Config::Aimbot::VoltSpeed;
		Features::Aimbot::VoltHipfireSmooth = Config::Aimbot::VoltHipfireSmooth;
		Features::Aimbot::VoltADSSmooth = Config::Aimbot::VoltADSSmooth;
		Features::Aimbot::VoltFOV = Config::Aimbot::VoltFOV;
		Features::Aimbot::VoltZoomScale = Config::Aimbot::VoltZoomScale;
		Features::Aimbot::VoltHipfireSmooth1 = Config::Aimbot::VoltHipfireSmooth1;
		Features::Aimbot::VoltADSSmooth1 = Config::Aimbot::VoltADSSmooth1;
		Features::Aimbot::VoltExtraSmooth1 = Config::Aimbot::VoltExtraSmooth1;
		Features::Aimbot::VoltDeadzone = Config::Aimbot::VoltDeadzone;
		Features::Aimbot::VoltFOV1 = Config::Aimbot::VoltFOV1;
		Features::Aimbot::VoltMinDistance1 = Config::Aimbot::VoltMinDistance1;
		Features::Aimbot::VoltMaxDistance1 = Config::Aimbot::VoltMaxDistance1;
		Features::Aimbot::NemesisFire = Config::Aimbot::NemesisFire;
		Features::Aimbot::NemesisADS = Config::Aimbot::NemesisADS;
		Features::Aimbot::NemesisClosestHitbox = Config::Aimbot::NemesisClosestHitbox;
		Features::Aimbot::NemesisHitbox = Config::Aimbot::NemesisHitbox;
		Features::Aimbot::NemesisSpeed = Config::Aimbot::NemesisSpeed;
		Features::Aimbot::NemesisHipfireSmooth = Config::Aimbot::NemesisHipfireSmooth;
		Features::Aimbot::NemesisADSSmooth = Config::Aimbot::NemesisADSSmooth;
		Features::Aimbot::NemesisFOV = Config::Aimbot::NemesisFOV;
		Features::Aimbot::NemesisZoomScale = Config::Aimbot::NemesisZoomScale;
		Features::Aimbot::NemesisHipfireSmooth1 = Config::Aimbot::NemesisHipfireSmooth1;
		Features::Aimbot::NemesisADSSmooth1 = Config::Aimbot::NemesisADSSmooth1;
		Features::Aimbot::NemesisExtraSmooth1 = Config::Aimbot::NemesisExtraSmooth1;
		Features::Aimbot::NemesisDeadzone = Config::Aimbot::NemesisDeadzone;
		Features::Aimbot::NemesisFOV1 = Config::Aimbot::NemesisFOV1;
		Features::Aimbot::NemesisMinDistance1 = Config::Aimbot::NemesisMinDistance1;
		Features::Aimbot::NemesisMaxDistance1 = Config::Aimbot::NemesisMaxDistance1;
		Features::Aimbot::MozambiqueFire = Config::Aimbot::MozambiqueFire;
		Features::Aimbot::MozambiqueADS = Config::Aimbot::MozambiqueADS;
		Features::Aimbot::MozambiqueClosestHitbox = Config::Aimbot::MozambiqueClosestHitbox;
		Features::Aimbot::MozambiqueHitbox = Config::Aimbot::MozambiqueHitbox;
		Features::Aimbot::MozambiqueSpeed = Config::Aimbot::MozambiqueSpeed;
		Features::Aimbot::MozambiqueHipfireSmooth = Config::Aimbot::MozambiqueHipfireSmooth;
		Features::Aimbot::MozambiqueADSSmooth = Config::Aimbot::MozambiqueADSSmooth;
		Features::Aimbot::MozambiqueFOV = Config::Aimbot::MozambiqueFOV;
		Features::Aimbot::MozambiqueZoomScale = Config::Aimbot::MozambiqueZoomScale;
		Features::Aimbot::MozambiqueHipfireSmooth1 = Config::Aimbot::MozambiqueHipfireSmooth1;
		Features::Aimbot::MozambiqueADSSmooth1 = Config::Aimbot::MozambiqueADSSmooth1;
		Features::Aimbot::MozambiqueExtraSmooth1 = Config::Aimbot::MozambiqueExtraSmooth1;
		Features::Aimbot::MozambiqueDeadzone = Config::Aimbot::MozambiqueDeadzone;
		Features::Aimbot::MozambiqueFOV1 = Config::Aimbot::MozambiqueFOV1;
		Features::Aimbot::MozambiqueMinDistance1 = Config::Aimbot::MozambiqueMinDistance1;
		Features::Aimbot::MozambiqueMaxDistance1 = Config::Aimbot::MozambiqueMaxDistance1;
		Features::Aimbot::PeacekeeperFire = Config::Aimbot::PeacekeeperFire;
		Features::Aimbot::PeacekeeperADS = Config::Aimbot::PeacekeeperADS;
		Features::Aimbot::PeacekeeperClosestHitbox = Config::Aimbot::PeacekeeperClosestHitbox;
		Features::Aimbot::PeacekeeperHitbox = Config::Aimbot::PeacekeeperHitbox;
		Features::Aimbot::PeacekeeperSpeed = Config::Aimbot::PeacekeeperSpeed;
		Features::Aimbot::PeacekeeperHipfireSmooth = Config::Aimbot::PeacekeeperHipfireSmooth;
		Features::Aimbot::PeacekeeperADSSmooth = Config::Aimbot::PeacekeeperADSSmooth;
		Features::Aimbot::PeacekeeperFOV = Config::Aimbot::PeacekeeperFOV;
		Features::Aimbot::PeacekeeperZoomScale = Config::Aimbot::PeacekeeperZoomScale;
		Features::Aimbot::PeacekeeperHipfireSmooth1 = Config::Aimbot::PeacekeeperHipfireSmooth1;
		Features::Aimbot::PeacekeeperADSSmooth1 = Config::Aimbot::PeacekeeperADSSmooth1;
		Features::Aimbot::PeacekeeperExtraSmooth1 = Config::Aimbot::PeacekeeperExtraSmooth1;
		Features::Aimbot::PeacekeeperDeadzone = Config::Aimbot::PeacekeeperDeadzone;
		Features::Aimbot::PeacekeeperFOV1 = Config::Aimbot::PeacekeeperFOV1;
		Features::Aimbot::PeacekeeperMinDistance1 = Config::Aimbot::PeacekeeperMinDistance1;
		Features::Aimbot::PeacekeeperMaxDistance1 = Config::Aimbot::PeacekeeperMaxDistance1;
		Features::Aimbot::MastiffFire = Config::Aimbot::MastiffFire;
		Features::Aimbot::MastiffADS = Config::Aimbot::MastiffADS;
		Features::Aimbot::MastiffClosestHitbox = Config::Aimbot::MastiffClosestHitbox;
		Features::Aimbot::MastiffHitbox = Config::Aimbot::MastiffHitbox;
		Features::Aimbot::MastiffSpeed = Config::Aimbot::MastiffSpeed;
		Features::Aimbot::MastiffHipfireSmooth = Config::Aimbot::MastiffHipfireSmooth;
		Features::Aimbot::MastiffADSSmooth = Config::Aimbot::MastiffADSSmooth;
		Features::Aimbot::MastiffFOV = Config::Aimbot::MastiffFOV;
		Features::Aimbot::MastiffZoomScale = Config::Aimbot::MastiffZoomScale;
		Features::Aimbot::MastiffHipfireSmooth1 = Config::Aimbot::MastiffHipfireSmooth1;
		Features::Aimbot::MastiffADSSmooth1 = Config::Aimbot::MastiffADSSmooth1;
		Features::Aimbot::MastiffExtraSmooth1 = Config::Aimbot::MastiffExtraSmooth1;
		Features::Aimbot::MastiffDeadzone = Config::Aimbot::MastiffDeadzone;
		Features::Aimbot::MastiffFOV1 = Config::Aimbot::MastiffFOV1;
		Features::Aimbot::MastiffMinDistance1 = Config::Aimbot::MastiffMinDistance1;
		Features::Aimbot::MastiffMaxDistance1 = Config::Aimbot::MastiffMaxDistance1;
		Features::Aimbot::LongbowFire = Config::Aimbot::LongbowFire;
		Features::Aimbot::LongbowADS = Config::Aimbot::LongbowADS;
		Features::Aimbot::LongbowClosestHitbox = Config::Aimbot::LongbowClosestHitbox;
		Features::Aimbot::LongbowHitbox = Config::Aimbot::LongbowHitbox;
		Features::Aimbot::LongbowSpeed = Config::Aimbot::LongbowSpeed;
		Features::Aimbot::LongbowHipfireSmooth = Config::Aimbot::LongbowHipfireSmooth;
		Features::Aimbot::LongbowADSSmooth = Config::Aimbot::LongbowADSSmooth;
		Features::Aimbot::LongbowFOV = Config::Aimbot::LongbowFOV;
		Features::Aimbot::LongbowZoomScale = Config::Aimbot::LongbowZoomScale;
		Features::Aimbot::LongbowHipfireSmooth1 = Config::Aimbot::LongbowHipfireSmooth1;
		Features::Aimbot::LongbowADSSmooth1 = Config::Aimbot::LongbowADSSmooth1;
		Features::Aimbot::LongbowExtraSmooth1 = Config::Aimbot::LongbowExtraSmooth1;
		Features::Aimbot::LongbowDeadzone = Config::Aimbot::LongbowDeadzone;
		Features::Aimbot::LongbowFOV1 = Config::Aimbot::LongbowFOV1;
		Features::Aimbot::LongbowMinDistance1 = Config::Aimbot::LongbowMinDistance1;
		Features::Aimbot::LongbowMaxDistance1 = Config::Aimbot::LongbowMaxDistance1;
		Features::Aimbot::ChargeRifleFire = Config::Aimbot::ChargeRifleFire;
		Features::Aimbot::ChargeRifleADS = Config::Aimbot::ChargeRifleADS;
		Features::Aimbot::ChargeRifleClosestHitbox = Config::Aimbot::ChargeRifleClosestHitbox;
		Features::Aimbot::ChargeRifleHitbox = Config::Aimbot::ChargeRifleHitbox;
		Features::Aimbot::ChargeRifleSpeed = Config::Aimbot::ChargeRifleSpeed;
		Features::Aimbot::ChargeRifleHipfireSmooth = Config::Aimbot::ChargeRifleHipfireSmooth;
		Features::Aimbot::ChargeRifleADSSmooth = Config::Aimbot::ChargeRifleADSSmooth;
		Features::Aimbot::ChargeRifleFOV = Config::Aimbot::ChargeRifleFOV;
		Features::Aimbot::ChargeRifleZoomScale = Config::Aimbot::ChargeRifleZoomScale;
		Features::Aimbot::ChargeRifleHipfireSmooth1 = Config::Aimbot::ChargeRifleHipfireSmooth1;
		Features::Aimbot::ChargeRifleADSSmooth1 = Config::Aimbot::ChargeRifleADSSmooth1;
		Features::Aimbot::ChargeRifleExtraSmooth1 = Config::Aimbot::ChargeRifleExtraSmooth1;
		Features::Aimbot::ChargeRifleDeadzone = Config::Aimbot::ChargeRifleDeadzone;
		Features::Aimbot::ChargeRifleFOV1 = Config::Aimbot::ChargeRifleFOV1;
		Features::Aimbot::ChargeRifleMinDistance1 = Config::Aimbot::ChargeRifleMinDistance1;
		Features::Aimbot::ChargeRifleMaxDistance1 = Config::Aimbot::ChargeRifleMaxDistance1;
		Features::Aimbot::SentinelFire = Config::Aimbot::SentinelFire;
		Features::Aimbot::SentinelADS = Config::Aimbot::SentinelADS;
		Features::Aimbot::SentinelClosestHitbox = Config::Aimbot::SentinelClosestHitbox;
		Features::Aimbot::SentinelHitbox = Config::Aimbot::SentinelHitbox;
		Features::Aimbot::SentinelSpeed = Config::Aimbot::SentinelSpeed;
		Features::Aimbot::SentinelHipfireSmooth = Config::Aimbot::SentinelHipfireSmooth;
		Features::Aimbot::SentinelADSSmooth = Config::Aimbot::SentinelADSSmooth;
		Features::Aimbot::SentinelFOV = Config::Aimbot::SentinelFOV;
		Features::Aimbot::SentinelZoomScale = Config::Aimbot::SentinelZoomScale;
		Features::Aimbot::SentinelHipfireSmooth1 = Config::Aimbot::SentinelHipfireSmooth1;
		Features::Aimbot::SentinelADSSmooth1 = Config::Aimbot::SentinelADSSmooth1;
		Features::Aimbot::SentinelExtraSmooth1 = Config::Aimbot::SentinelExtraSmooth1;
		Features::Aimbot::SentinelDeadzone = Config::Aimbot::SentinelDeadzone;
		Features::Aimbot::SentinelFOV1 = Config::Aimbot::SentinelFOV1;
		Features::Aimbot::SentinelMinDistance1 = Config::Aimbot::SentinelMinDistance1;
		Features::Aimbot::SentinelMaxDistance1 = Config::Aimbot::SentinelMaxDistance1;
		Features::Aimbot::WingmanFire = Config::Aimbot::WingmanFire;
		Features::Aimbot::WingmanADS = Config::Aimbot::WingmanADS;
		Features::Aimbot::WingmanClosestHitbox = Config::Aimbot::WingmanClosestHitbox;
		Features::Aimbot::WingmanHitbox = Config::Aimbot::WingmanHitbox;
		Features::Aimbot::WingmanSpeed = Config::Aimbot::WingmanSpeed;
		Features::Aimbot::WingmanHipfireSmooth = Config::Aimbot::WingmanHipfireSmooth;
		Features::Aimbot::WingmanADSSmooth = Config::Aimbot::WingmanADSSmooth;
		Features::Aimbot::WingmanFOV = Config::Aimbot::WingmanFOV;
		Features::Aimbot::WingmanZoomScale = Config::Aimbot::WingmanZoomScale;
		Features::Aimbot::WingmanHipfireSmooth1 = Config::Aimbot::WingmanHipfireSmooth1;
		Features::Aimbot::WingmanADSSmooth1 = Config::Aimbot::WingmanADSSmooth1;
		Features::Aimbot::WingmanExtraSmooth1 = Config::Aimbot::WingmanExtraSmooth1;
		Features::Aimbot::WingmanDeadzone = Config::Aimbot::WingmanDeadzone;
		Features::Aimbot::WingmanFOV1 = Config::Aimbot::WingmanFOV1;
		Features::Aimbot::WingmanMinDistance1 = Config::Aimbot::WingmanMinDistance1;
		Features::Aimbot::WingmanMaxDistance1 = Config::Aimbot::WingmanMaxDistance1;
		Features::Aimbot::EVA8Fire = Config::Aimbot::EVA8Fire;
		Features::Aimbot::EVA8ADS = Config::Aimbot::EVA8ADS;
		Features::Aimbot::EVA8ClosestHitbox = Config::Aimbot::EVA8ClosestHitbox;
		Features::Aimbot::EVA8Hitbox = Config::Aimbot::EVA8Hitbox;
		Features::Aimbot::EVA8Speed = Config::Aimbot::EVA8Speed;
		Features::Aimbot::EVA8HipfireSmooth = Config::Aimbot::EVA8HipfireSmooth;
		Features::Aimbot::EVA8ADSSmooth = Config::Aimbot::EVA8ADSSmooth;
		Features::Aimbot::EVA8FOV = Config::Aimbot::EVA8FOV;
		Features::Aimbot::EVA8ZoomScale = Config::Aimbot::EVA8ZoomScale;
		Features::Aimbot::EVA8HipfireSmooth1 = Config::Aimbot::EVA8HipfireSmooth1;
		Features::Aimbot::EVA8ADSSmooth1 = Config::Aimbot::EVA8ADSSmooth1;
		Features::Aimbot::EVA8ExtraSmooth1 = Config::Aimbot::EVA8ExtraSmooth1;
		Features::Aimbot::EVA8Deadzone = Config::Aimbot::EVA8Deadzone;
		Features::Aimbot::EVA8FOV1 = Config::Aimbot::EVA8FOV1;
		Features::Aimbot::EVA8MinDistance1 = Config::Aimbot::EVA8MinDistance1;
		Features::Aimbot::EVA8MaxDistance1 = Config::Aimbot::EVA8MaxDistance1;
		Features::Aimbot::BocekFire = Config::Aimbot::BocekFire;
		Features::Aimbot::BocekADS = Config::Aimbot::BocekADS;
		Features::Aimbot::BocekClosestHitbox = Config::Aimbot::BocekClosestHitbox;
		Features::Aimbot::BocekHitbox = Config::Aimbot::BocekHitbox;
		Features::Aimbot::BocekSpeed = Config::Aimbot::BocekSpeed;
		Features::Aimbot::BocekHipfireSmooth = Config::Aimbot::BocekHipfireSmooth;
		Features::Aimbot::BocekADSSmooth = Config::Aimbot::BocekADSSmooth;
		Features::Aimbot::BocekFOV = Config::Aimbot::BocekFOV;
		Features::Aimbot::BocekZoomScale = Config::Aimbot::BocekZoomScale;
		Features::Aimbot::BocekHipfireSmooth1 = Config::Aimbot::BocekHipfireSmooth1;
		Features::Aimbot::BocekADSSmooth1 = Config::Aimbot::BocekADSSmooth1;
		Features::Aimbot::BocekExtraSmooth1 = Config::Aimbot::BocekExtraSmooth1;
		Features::Aimbot::BocekDeadzone = Config::Aimbot::BocekDeadzone;
		Features::Aimbot::BocekFOV1 = Config::Aimbot::BocekFOV1;
		Features::Aimbot::BocekMinDistance1 = Config::Aimbot::BocekMinDistance1;
		Features::Aimbot::BocekMaxDistance1 = Config::Aimbot::BocekMaxDistance1;
		Features::Aimbot::KraberFire = Config::Aimbot::KraberFire;
		Features::Aimbot::KraberADS = Config::Aimbot::KraberADS;
		Features::Aimbot::KraberClosestHitbox = Config::Aimbot::KraberClosestHitbox;
		Features::Aimbot::KraberHitbox = Config::Aimbot::KraberHitbox;
		Features::Aimbot::KraberSpeed = Config::Aimbot::KraberSpeed;
		Features::Aimbot::KraberHipfireSmooth = Config::Aimbot::KraberHipfireSmooth;
		Features::Aimbot::KraberADSSmooth = Config::Aimbot::KraberADSSmooth;
		Features::Aimbot::KraberFOV = Config::Aimbot::KraberFOV;
		Features::Aimbot::KraberZoomScale = Config::Aimbot::KraberZoomScale;
		Features::Aimbot::KraberHipfireSmooth1 = Config::Aimbot::KraberHipfireSmooth1;
		Features::Aimbot::KraberADSSmooth1 = Config::Aimbot::KraberADSSmooth1;
		Features::Aimbot::KraberExtraSmooth1 = Config::Aimbot::KraberExtraSmooth1;
		Features::Aimbot::KraberDeadzone = Config::Aimbot::KraberDeadzone;
		Features::Aimbot::KraberFOV1 = Config::Aimbot::KraberFOV1;
		Features::Aimbot::KraberMinDistance1 = Config::Aimbot::KraberMinDistance1;
		Features::Aimbot::KraberMaxDistance1 = Config::Aimbot::KraberMaxDistance1;
		Features::Aimbot::ThrowingKnifeFire = Config::Aimbot::ThrowingKnifeFire;
		Features::Aimbot::ThrowingKnifeADS = Config::Aimbot::ThrowingKnifeADS;
		Features::Aimbot::ThrowingKnifeClosestHitbox = Config::Aimbot::ThrowingKnifeClosestHitbox;
		Features::Aimbot::ThrowingKnifeHitbox = Config::Aimbot::ThrowingKnifeHitbox;
		Features::Aimbot::ThrowingKnifeSpeed = Config::Aimbot::ThrowingKnifeSpeed;
		Features::Aimbot::ThrowingKnifeHipfireSmooth = Config::Aimbot::ThrowingKnifeHipfireSmooth;
		Features::Aimbot::ThrowingKnifeADSSmooth = Config::Aimbot::ThrowingKnifeADSSmooth;
		Features::Aimbot::ThrowingKnifeFOV = Config::Aimbot::ThrowingKnifeFOV;
		Features::Aimbot::ThrowingKnifeZoomScale = Config::Aimbot::ThrowingKnifeZoomScale;
		Features::Aimbot::ThrowingKnifeHipfireSmooth1 = Config::Aimbot::ThrowingKnifeHipfireSmooth1;
		Features::Aimbot::ThrowingKnifeADSSmooth1 = Config::Aimbot::ThrowingKnifeADSSmooth1;
		Features::Aimbot::ThrowingKnifeExtraSmooth1 = Config::Aimbot::ThrowingKnifeExtraSmooth1;
		Features::Aimbot::ThrowingKnifeDeadzone = Config::Aimbot::ThrowingKnifeDeadzone;
		Features::Aimbot::ThrowingKnifeFOV1 = Config::Aimbot::ThrowingKnifeFOV1;
		Features::Aimbot::ThrowingKnifeMinDistance1 = Config::Aimbot::ThrowingKnifeMinDistance1;
		Features::Aimbot::ThrowingKnifeMaxDistance1 = Config::Aimbot::ThrowingKnifeMaxDistance1;

		Features::AimbotBinds::AimBind = static_cast<InputKeyType>(Config::AimbotBinds::AimBind);
		Features::AimbotBinds::ExtraBind = static_cast<InputKeyType>(Config::AimbotBinds::ExtraBind);
		//Advanced
		Features::AimbotBinds::P2020AimBind = static_cast<InputKeyType>(Config::AimbotBinds::P2020AimBind);
		Features::AimbotBinds::P2020ExtraBind = static_cast<InputKeyType>(Config::AimbotBinds::P2020ExtraBind);
		Features::AimbotBinds::RE45AimBind = static_cast<InputKeyType>(Config::AimbotBinds::RE45AimBind);
		Features::AimbotBinds::RE45ExtraBind = static_cast<InputKeyType>(Config::AimbotBinds::RE45ExtraBind);
		Features::AimbotBinds::AlternatorAimBind = static_cast<InputKeyType>(Config::AimbotBinds::AlternatorAimBind);
		Features::AimbotBinds::AlternatorExtraBind = static_cast<InputKeyType>(Config::AimbotBinds::AlternatorExtraBind);
		Features::AimbotBinds::R99AimBind = static_cast<InputKeyType>(Config::AimbotBinds::R99AimBind);
		Features::AimbotBinds::R99ExtraBind = static_cast<InputKeyType>(Config::AimbotBinds::R99ExtraBind);
		Features::AimbotBinds::R301AimBind = static_cast<InputKeyType>(Config::AimbotBinds::R301AimBind);
		Features::AimbotBinds::R301ExtraBind = static_cast<InputKeyType>(Config::AimbotBinds::R301ExtraBind);
		Features::AimbotBinds::SpitfireAimBind = static_cast<InputKeyType>(Config::AimbotBinds::SpitfireAimBind);
		Features::AimbotBinds::SpitfireExtraBind = static_cast<InputKeyType>(Config::AimbotBinds::SpitfireExtraBind);
		Features::AimbotBinds::G7AimBind = static_cast<InputKeyType>(Config::AimbotBinds::G7AimBind);
		Features::AimbotBinds::G7ExtraBind = static_cast<InputKeyType>(Config::AimbotBinds::G7ExtraBind);
		Features::AimbotBinds::FlatlineAimBind = static_cast<InputKeyType>(Config::AimbotBinds::FlatlineAimBind);
		Features::AimbotBinds::FlatlineExtraBind = static_cast<InputKeyType>(Config::AimbotBinds::FlatlineExtraBind);
		Features::AimbotBinds::HemlockAimBind = static_cast<InputKeyType>(Config::AimbotBinds::HemlockAimBind);
		Features::AimbotBinds::HemlockExtraBind = static_cast<InputKeyType>(Config::AimbotBinds::HemlockExtraBind);
		Features::AimbotBinds::ProwlerAimBind = static_cast<InputKeyType>(Config::AimbotBinds::ProwlerAimBind);
		Features::AimbotBinds::ProwlerExtraBind = static_cast<InputKeyType>(Config::AimbotBinds::ProwlerExtraBind);
		Features::AimbotBinds::RepeaterAimBind = static_cast<InputKeyType>(Config::AimbotBinds::RepeaterAimBind);
		Features::AimbotBinds::RepeaterExtraBind = static_cast<InputKeyType>(Config::AimbotBinds::RepeaterExtraBind);
		Features::AimbotBinds::RampageAimBind = static_cast<InputKeyType>(Config::AimbotBinds::RampageAimBind);
		Features::AimbotBinds::RampageExtraBind = static_cast<InputKeyType>(Config::AimbotBinds::RampageExtraBind);
		Features::AimbotBinds::CARSMGAimBind = static_cast<InputKeyType>(Config::AimbotBinds::CARSMGAimBind);
		Features::AimbotBinds::CARSMGExtraBind = static_cast<InputKeyType>(Config::AimbotBinds::CARSMGExtraBind);
		Features::AimbotBinds::HavocAimBind = static_cast<InputKeyType>(Config::AimbotBinds::HavocAimBind);
		Features::AimbotBinds::HavocExtraBind = static_cast<InputKeyType>(Config::AimbotBinds::HavocExtraBind);
		Features::AimbotBinds::DevotionAimBind = static_cast<InputKeyType>(Config::AimbotBinds::DevotionAimBind);
		Features::AimbotBinds::DevotionExtraBind = static_cast<InputKeyType>(Config::AimbotBinds::DevotionExtraBind);
		Features::AimbotBinds::LSTARAimBind = static_cast<InputKeyType>(Config::AimbotBinds::LSTARAimBind);
		Features::AimbotBinds::LSTARExtraBind = static_cast<InputKeyType>(Config::AimbotBinds::LSTARExtraBind);
		Features::AimbotBinds::TripleTakeAimBind = static_cast<InputKeyType>(Config::AimbotBinds::TripleTakeAimBind);
		Features::AimbotBinds::TripleTakeExtraBind = static_cast<InputKeyType>(Config::AimbotBinds::TripleTakeExtraBind);
		Features::AimbotBinds::VoltAimBind = static_cast<InputKeyType>(Config::AimbotBinds::VoltAimBind);
		Features::AimbotBinds::VoltExtraBind = static_cast<InputKeyType>(Config::AimbotBinds::VoltExtraBind);
		Features::AimbotBinds::NemesisAimBind = static_cast<InputKeyType>(Config::AimbotBinds::NemesisAimBind);
		Features::AimbotBinds::NemesisExtraBind = static_cast<InputKeyType>(Config::AimbotBinds::NemesisExtraBind);
		Features::AimbotBinds::MozambiqueAimBind = static_cast<InputKeyType>(Config::AimbotBinds::MozambiqueAimBind);
		Features::AimbotBinds::MozambiqueExtraBind = static_cast<InputKeyType>(Config::AimbotBinds::MozambiqueExtraBind);
		Features::AimbotBinds::PeacekeeperAimBind = static_cast<InputKeyType>(Config::AimbotBinds::PeacekeeperAimBind);
		Features::AimbotBinds::PeacekeeperExtraBind = static_cast<InputKeyType>(Config::AimbotBinds::PeacekeeperExtraBind);
		Features::AimbotBinds::MastiffAimBind = static_cast<InputKeyType>(Config::AimbotBinds::MastiffAimBind);
		Features::AimbotBinds::MastiffExtraBind = static_cast<InputKeyType>(Config::AimbotBinds::MastiffExtraBind);
		Features::AimbotBinds::LongbowAimBind = static_cast<InputKeyType>(Config::AimbotBinds::LongbowAimBind);
		Features::AimbotBinds::LongbowExtraBind = static_cast<InputKeyType>(Config::AimbotBinds::LongbowExtraBind);
		Features::AimbotBinds::ChargeRifleAimBind = static_cast<InputKeyType>(Config::AimbotBinds::ChargeRifleAimBind);
		Features::AimbotBinds::ChargeRifleExtraBind = static_cast<InputKeyType>(Config::AimbotBinds::ChargeRifleExtraBind);
		Features::AimbotBinds::SentinelAimBind = static_cast<InputKeyType>(Config::AimbotBinds::SentinelAimBind);
		Features::AimbotBinds::SentinelExtraBind = static_cast<InputKeyType>(Config::AimbotBinds::SentinelExtraBind);
		Features::AimbotBinds::WingmanAimBind = static_cast<InputKeyType>(Config::AimbotBinds::WingmanAimBind);
		Features::AimbotBinds::WingmanExtraBind = static_cast<InputKeyType>(Config::AimbotBinds::WingmanExtraBind);
		Features::AimbotBinds::EVA8AimBind = static_cast<InputKeyType>(Config::AimbotBinds::EVA8AimBind);
		Features::AimbotBinds::EVA8ExtraBind = static_cast<InputKeyType>(Config::AimbotBinds::EVA8ExtraBind);
		Features::AimbotBinds::BocekAimBind = static_cast<InputKeyType>(Config::AimbotBinds::BocekAimBind);
		Features::AimbotBinds::BocekExtraBind = static_cast<InputKeyType>(Config::AimbotBinds::BocekExtraBind);
		Features::AimbotBinds::KraberAimBind = static_cast<InputKeyType>(Config::AimbotBinds::KraberAimBind);
		Features::AimbotBinds::KraberExtraBind = static_cast<InputKeyType>(Config::AimbotBinds::KraberExtraBind);
		Features::AimbotBinds::ThrowingKnifeAimBind = static_cast<InputKeyType>(Config::AimbotBinds::ThrowingKnifeAimBind);
		Features::AimbotBinds::ThrowingKnifeExtraBind = static_cast<InputKeyType>(Config::AimbotBinds::ThrowingKnifeExtraBind);

		Features::AimbotHitboxes::Hitbox = static_cast<HitboxType>(Config::AimbotHitboxes::Hitbox);
		//Advanced
		Features::AimbotHitboxes::P2020Hitbox = static_cast<HitboxType>(Config::AimbotHitboxes::P2020Hitbox);
		Features::AimbotHitboxes::RE45Hitbox = static_cast<HitboxType>(Config::AimbotHitboxes::RE45Hitbox);
		Features::AimbotHitboxes::AlternatorHitbox = static_cast<HitboxType>(Config::AimbotHitboxes::AlternatorHitbox);
		Features::AimbotHitboxes::R99Hitbox = static_cast<HitboxType>(Config::AimbotHitboxes::R99Hitbox);
		Features::AimbotHitboxes::R301Hitbox = static_cast<HitboxType>(Config::AimbotHitboxes::R301Hitbox);
		Features::AimbotHitboxes::SpitfireHitbox = static_cast<HitboxType>(Config::AimbotHitboxes::SpitfireHitbox);
		Features::AimbotHitboxes::G7Hitbox = static_cast<HitboxType>(Config::AimbotHitboxes::G7Hitbox);
		Features::AimbotHitboxes::FlatlineHitbox = static_cast<HitboxType>(Config::AimbotHitboxes::FlatlineHitbox);
		Features::AimbotHitboxes::HemlockHitbox = static_cast<HitboxType>(Config::AimbotHitboxes::HemlockHitbox);
		Features::AimbotHitboxes::ProwlerHitbox = static_cast<HitboxType>(Config::AimbotHitboxes::ProwlerHitbox);
		Features::AimbotHitboxes::RepeaterHitbox = static_cast<HitboxType>(Config::AimbotHitboxes::RepeaterHitbox);
		Features::AimbotHitboxes::RampageHitbox = static_cast<HitboxType>(Config::AimbotHitboxes::RampageHitbox);
		Features::AimbotHitboxes::CARSMGHitbox = static_cast<HitboxType>(Config::AimbotHitboxes::CARSMGHitbox);
		Features::AimbotHitboxes::HavocHitbox = static_cast<HitboxType>(Config::AimbotHitboxes::HavocHitbox);
		Features::AimbotHitboxes::DevotionHitbox = static_cast<HitboxType>(Config::AimbotHitboxes::DevotionHitbox);
		Features::AimbotHitboxes::LSTARHitbox = static_cast<HitboxType>(Config::AimbotHitboxes::LSTARHitbox);
		Features::AimbotHitboxes::TripleTakeHitbox = static_cast<HitboxType>(Config::AimbotHitboxes::TripleTakeHitbox);
		Features::AimbotHitboxes::VoltHitbox = static_cast<HitboxType>(Config::AimbotHitboxes::VoltHitbox);
		Features::AimbotHitboxes::NemesisHitbox = static_cast<HitboxType>(Config::AimbotHitboxes::NemesisHitbox);
		Features::AimbotHitboxes::MozambiqueHitbox = static_cast<HitboxType>(Config::AimbotHitboxes::MozambiqueHitbox);
		Features::AimbotHitboxes::PeacekeeperHitbox = static_cast<HitboxType>(Config::AimbotHitboxes::PeacekeeperHitbox);
		Features::AimbotHitboxes::MastiffHitbox = static_cast<HitboxType>(Config::AimbotHitboxes::MastiffHitbox);
		Features::AimbotHitboxes::LongbowHitbox = static_cast<HitboxType>(Config::AimbotHitboxes::LongbowHitbox);
		Features::AimbotHitboxes::ChargeRifleHitbox = static_cast<HitboxType>(Config::AimbotHitboxes::ChargeRifleHitbox);
		Features::AimbotHitboxes::SentinelHitbox = static_cast<HitboxType>(Config::AimbotHitboxes::SentinelHitbox);
		Features::AimbotHitboxes::WingmanHitbox = static_cast<HitboxType>(Config::AimbotHitboxes::WingmanHitbox);
		Features::AimbotHitboxes::EVA8Hitbox = static_cast<HitboxType>(Config::AimbotHitboxes::EVA8Hitbox);
		Features::AimbotHitboxes::BocekHitbox = static_cast<HitboxType>(Config::AimbotHitboxes::BocekHitbox);
		Features::AimbotHitboxes::KraberHitbox = static_cast<HitboxType>(Config::AimbotHitboxes::KraberHitbox);
		Features::AimbotHitboxes::ThrowingKnifeHitbox = static_cast<HitboxType>(Config::AimbotHitboxes::ThrowingKnifeHitbox);

		Features::Ragebot::RagebotEnabled = Config::Ragebot::RagebotEnabled;
		Features::Ragebot::RageAimbot = Config::Ragebot::RageAimbot;
		Features::Ragebot::AimMethod = Config::Ragebot::AimMethod;
		Features::Ragebot::ClosestHitbox = Config::Ragebot::ClosestHitbox;
		Features::Ragebot::Hitbox = static_cast<HitboxType>(Config::Ragebot::Hitbox);
		Features::Ragebot::BindMethod = Config::Ragebot::BindMethod;
		Features::Ragebot::OnFire = Config::Ragebot::OnFire;
		Features::Ragebot::OnADS = Config::Ragebot::OnADS;
		Features::Ragebot::AimBind = static_cast<InputKeyType>(Config::Ragebot::AimBind);
		Features::Ragebot::ExtraBind = static_cast<InputKeyType>(Config::Ragebot::ExtraBind);
		Features::Ragebot::VisCheck = Config::Ragebot::VisCheck;
		Features::Ragebot::TeamCheck = Config::Ragebot::TeamCheck;
		Features::Ragebot::Priority = Config::Ragebot::Priority;
		Features::Ragebot::AutoShoot = Config::Ragebot::AutoShoot;
		Features::Ragebot::PredictMovement = Config::Ragebot::PredictMovement;
		Features::Ragebot::PredictBulletDrop = Config::Ragebot::PredictBulletDrop;
		Features::Ragebot::PreditcionAmount = Config::Ragebot::PreditcionAmount;
		Features::Ragebot::FinalDistance = Config::Ragebot::FinalDistance;
		Features::Ragebot::Speed = Config::Ragebot::Speed;
		Features::Ragebot::Smooth = Config::Ragebot::Smooth;
		Features::Ragebot::Speed = Config::Ragebot::Speed;
		Features::Ragebot::HipfireSmooth = Config::Ragebot::HipfireSmooth;
        Features::Ragebot::ADSSmooth = Config::Ragebot::ADSSmooth;
        Features::Ragebot::SmoothDistance = Config::Ragebot::SmoothDistance;
        Features::Ragebot::Delay = Config::Ragebot::Delay;
        Features::Ragebot::FOV = Config::Ragebot::FOV;
        Features::Ragebot::ZoomScale = Config::Ragebot::ZoomScale;
        Features::Ragebot::MinDistance = Config::Ragebot::MinDistance;
        Features::Ragebot::HipfireDistance = Config::Ragebot::HipfireDistance;
        Features::Ragebot::ZoomDistance = Config::Ragebot::ZoomDistance;
		Features::Ragebot::RecoilRate = Config::Ragebot::RecoilRate;
        //Weapon Toggles
        //Light
        Features::Ragebot::P2020 = Config::Ragebot::P2020;
        Features::Ragebot::RE45 = Config::Ragebot::RE45;
        Features::Ragebot::Alternator = Config::Ragebot::Alternator;
        Features::Ragebot::R99 = Config::Ragebot::R99;
        Features::Ragebot::R301 = Config::Ragebot::R301;
        Features::Ragebot::Spitfire = Config::Ragebot::Spitfire;
        Features::Ragebot::G7 = Config::Ragebot::G7;
        //Heavy
        Features::Ragebot::Flatline = Config::Ragebot::Flatline;
        Features::Ragebot::Hemlock = Config::Ragebot::Hemlock;
        Features::Ragebot::Repeater = Config::Ragebot::Repeater;
        Features::Ragebot::Rampage = Config::Ragebot::Rampage;
        Features::Ragebot::CARSMG = Config::Ragebot::CARSMG;
        //Energy
        Features::Ragebot::Havoc = Config::Ragebot::Havoc;
        Features::Ragebot::Devotion = Config::Ragebot::Devotion;
        Features::Ragebot::LSTAR = Config::Ragebot::LSTAR;
        Features::Ragebot::TripleTake = Config::Ragebot::TripleTake;
        Features::Ragebot::Volt = Config::Ragebot::Volt;
        Features::Ragebot::Nemesis = Config::Ragebot::Nemesis;
        //Shotgun
        Features::Ragebot::Mozambique = Config::Ragebot::Mozambique;
        Features::Ragebot::EVA8 = Config::Ragebot::EVA8;
        Features::Ragebot::Peacekeeper = Config::Ragebot::Peacekeeper;
        Features::Ragebot::Mastiff = Config::Ragebot::Mastiff;
        //Snipers
        Features::Ragebot::Longbow = Config::Ragebot::Longbow;
        Features::Ragebot::ChargeRifle = Config::Ragebot::ChargeRifle;
        Features::Ragebot::Sentinel = Config::Ragebot::Sentinel;
        //Legendary
        Features::Ragebot::Wingman = Config::Ragebot::Wingman;
        Features::Ragebot::Prowler = Config::Ragebot::Prowler;
        Features::Ragebot::Bocek = Config::Ragebot::Bocek;
        Features::Ragebot::Kraber = Config::Ragebot::Kraber;
        Features::Ragebot::Knife = Config::Ragebot::Knife;

		Features::Flickbot::Flickbot = Config::Flickbot::Flickbot;
		Features::Flickbot::FlickbotMethod = Config::Flickbot::FlickbotMethod;
		Features::Flickbot::FlickBind = static_cast<InputKeyType>(Config::Flickbot::FlickBind);
		Features::Flickbot::ClosestHitbox = Config::Flickbot::ClosestHitbox;
		Features::Flickbot::Hitbox = static_cast<HitboxType>(Config::Flickbot::Hitbox);
		Features::Flickbot::Priority = Config::Flickbot::Priority;
		Features::Flickbot::AutoShoot = Config::Flickbot::AutoShoot;
		Features::Flickbot::AutoShootDelay = Config::Flickbot::AutoShootDelay;
		Features::Flickbot::FlickBack = Config::Flickbot::FlickBack;
		Features::Flickbot::FlickBackDelay = Config::Flickbot::FlickBackDelay;
		Features::Flickbot::PredictMovement = Config::Flickbot::PredictMovement;
		Features::Flickbot::PredictBulletDrop = Config::Flickbot::PredictBulletDrop;
		Features::Flickbot::TeamCheck = Config::Flickbot::TeamCheck;
		Features::Flickbot::PreditcionAmount = Config::Flickbot::PreditcionAmount;
		Features::Flickbot::FinalDistance = Config::Flickbot::FinalDistance;
		Features::Flickbot::HipfireSmooth = Config::Flickbot::HipfireSmooth;
        Features::Flickbot::ADSSmooth = Config::Flickbot::ADSSmooth;
        Features::Flickbot::SmoothDistance = Config::Flickbot::SmoothDistance;
        Features::Flickbot::Delay = Config::Flickbot::Delay;
        Features::Flickbot::FOV = Config::Flickbot::FOV;
        Features::Flickbot::ZoomScale = Config::Flickbot::ZoomScale;
        Features::Flickbot::MinDistance = Config::Flickbot::MinDistance;
        Features::Flickbot::HipfireDistance = Config::Flickbot::HipfireDistance;
        Features::Flickbot::ZoomDistance = Config::Flickbot::ZoomDistance;
		//Weapon Toggles
        //Light
        Features::Flickbot::P2020 = Config::Flickbot::P2020;
        Features::Flickbot::RE45 = Config::Flickbot::RE45;
        Features::Flickbot::Alternator = Config::Flickbot::Alternator;
        Features::Flickbot::R99 = Config::Flickbot::R99;
        Features::Flickbot::R301 = Config::Flickbot::R301;
        Features::Flickbot::Spitfire = Config::Flickbot::Spitfire;
        Features::Flickbot::G7 = Config::Flickbot::G7;
        //Heavy
        Features::Flickbot::Flatline = Config::Flickbot::Flatline;
        Features::Flickbot::Hemlock = Config::Flickbot::Hemlock;
        Features::Flickbot::Repeater = Config::Flickbot::Repeater;
        Features::Flickbot::Rampage = Config::Flickbot::Rampage;
        Features::Flickbot::CARSMG = Config::Flickbot::CARSMG;
        //Energy
        Features::Flickbot::Havoc = Config::Flickbot::Havoc;
        Features::Flickbot::Devotion = Config::Flickbot::Devotion;
        Features::Flickbot::LSTAR = Config::Flickbot::LSTAR;
        Features::Flickbot::TripleTake = Config::Flickbot::TripleTake;
        Features::Flickbot::Volt = Config::Flickbot::Volt;
        Features::Flickbot::Nemesis = Config::Flickbot::Nemesis;
        //Shotgun
        Features::Flickbot::Mozambique = Config::Flickbot::Mozambique;
        Features::Flickbot::EVA8 = Config::Flickbot::EVA8;
        Features::Flickbot::Peacekeeper = Config::Flickbot::Peacekeeper;
        Features::Flickbot::Mastiff = Config::Flickbot::Mastiff;
        //Snipers
        Features::Flickbot::Longbow = Config::Flickbot::Longbow;
        Features::Flickbot::ChargeRifle = Config::Flickbot::ChargeRifle;
        Features::Flickbot::Sentinel = Config::Flickbot::Sentinel;
        //Legendary
        Features::Flickbot::Wingman = Config::Flickbot::Wingman;
        Features::Flickbot::Prowler = Config::Flickbot::Prowler;
        Features::Flickbot::Bocek = Config::Flickbot::Bocek;
        Features::Flickbot::Kraber = Config::Flickbot::Kraber;
        Features::Flickbot::Knife = Config::Flickbot::Knife;

		Features::RCS::RCSEnabled = Config::RCS::RCSEnabled;
		Features::RCS::RCSMode = Config::RCS::RCSMode;
		Features::RCS::OnADS = Config::RCS::OnADS;
		Features::RCS::PitchPower = Config::RCS::PitchPower;
		Features::RCS::YawPower = Config::RCS::YawPower;
		Features::RCS::PitchReduction = Config::RCS::PitchReduction;
		Features::RCS::YawReduction = Config::RCS::YawReduction;
		//Weapon Toggles
        //Light
        Features::RCS::P2020 = Config::RCS::P2020;
        Features::RCS::RE45 = Config::RCS::RE45;
        Features::RCS::Alternator = Config::RCS::Alternator;
        Features::RCS::R99 = Config::RCS::R99;
        Features::RCS::R301 = Config::RCS::R301;
        Features::RCS::Spitfire = Config::RCS::Spitfire;
        Features::RCS::G7 = Config::RCS::G7;
        //Heavy
        Features::RCS::Flatline = Config::RCS::Flatline;
        Features::RCS::Hemlock = Config::RCS::Hemlock;
        Features::RCS::Repeater = Config::RCS::Repeater;
        Features::RCS::Rampage = Config::RCS::Rampage;
        Features::RCS::CARSMG = Config::RCS::CARSMG;
        //Energy
        Features::RCS::Havoc = Config::RCS::Havoc;
        Features::RCS::Devotion = Config::RCS::Devotion;
        Features::RCS::LSTAR = Config::RCS::LSTAR;
        Features::RCS::TripleTake = Config::RCS::TripleTake;
        Features::RCS::Volt = Config::RCS::Volt;
        Features::RCS::Nemesis = Config::RCS::Nemesis;
        //Shotgun
        Features::RCS::Mozambique = Config::RCS::Mozambique;
        Features::RCS::EVA8 = Config::RCS::EVA8;
        Features::RCS::Peacekeeper = Config::RCS::Peacekeeper;
        Features::RCS::Mastiff = Config::RCS::Mastiff;
        //Snipers
        Features::RCS::Longbow = Config::RCS::Longbow;
        Features::RCS::ChargeRifle = Config::RCS::ChargeRifle;
        Features::RCS::Sentinel = Config::RCS::Sentinel;
        //Legendary
        Features::RCS::Wingman = Config::RCS::Wingman;
        Features::RCS::Prowler = Config::RCS::Prowler;
        Features::RCS::Kraber = Config::RCS::Kraber;
		//Advanced
		Features::RCS::AdvancedRCS = Config::RCS::AdvancedRCS;
		Features::RCS::AdvancedPitchPower = Config::RCS::AdvancedPitchPower;
		Features::RCS::AdvancedYawPower = Config::RCS::AdvancedYawPower;
		Features::RCS::AdvancedPitchReduction = Config::RCS::AdvancedPitchReduction;
		Features::RCS::AdvancedYawReduction = Config::RCS::AdvancedYawReduction;
		Features::RCS::P2020Pitch = Config::RCS::P2020Pitch;
		Features::RCS::P2020Yaw = Config::RCS::P2020Yaw;
		Features::RCS::P2020PitchReduction = Config::RCS::P2020PitchReduction;
		Features::RCS::P2020YawReduction = Config::RCS::P2020YawReduction;
		Features::RCS::RE45Pitch = Config::RCS::RE45Pitch;
		Features::RCS::RE45Yaw = Config::RCS::RE45Yaw;
		Features::RCS::RE45PitchReduction = Config::RCS::RE45PitchReduction;
		Features::RCS::RE45YawReduction = Config::RCS::RE45YawReduction;
		Features::RCS::AlternatorPitch = Config::RCS::AlternatorPitch;
		Features::RCS::AlternatorYaw = Config::RCS::AlternatorYaw;
		Features::RCS::AlternatorPitchReduction = Config::RCS::AlternatorPitchReduction;
		Features::RCS::AlternatorYawReduction = Config::RCS::AlternatorYawReduction;
		Features::RCS::R99Pitch = Config::RCS::R99Pitch;
		Features::RCS::R99Yaw = Config::RCS::R99Yaw;
		Features::RCS::R99PitchReduction = Config::RCS::R99PitchReduction;
		Features::RCS::R99YawReduction = Config::RCS::R99YawReduction;
		Features::RCS::R301Pitch = Config::RCS::R301Pitch;
		Features::RCS::R301Yaw = Config::RCS::R301Yaw;
		Features::RCS::R301PitchReduction = Config::RCS::R301PitchReduction;
		Features::RCS::R301YawReduction = Config::RCS::R301YawReduction;
		Features::RCS::SpitfirePitch = Config::RCS::SpitfirePitch;
		Features::RCS::SpitfireYaw = Config::RCS::SpitfireYaw;
		Features::RCS::SpitfirePitchReduction = Config::RCS::SpitfirePitchReduction;
		Features::RCS::SpitfireYawReduction = Config::RCS::SpitfireYawReduction;
		Features::RCS::G7Pitch = Config::RCS::G7Pitch;
		Features::RCS::G7Yaw = Config::RCS::G7Yaw;
		Features::RCS::G7PitchReduction = Config::RCS::G7PitchReduction;
		Features::RCS::G7YawReduction = Config::RCS::G7YawReduction;
		Features::RCS::FlatlinePitch = Config::RCS::FlatlinePitch;
		Features::RCS::FlatlineYaw = Config::RCS::FlatlineYaw;
		Features::RCS::FlatlinePitchReduction = Config::RCS::FlatlinePitchReduction;
		Features::RCS::FlatlineYawReduction = Config::RCS::FlatlineYawReduction;
		Features::RCS::HemlockPitch = Config::RCS::HemlockPitch;
		Features::RCS::HemlockYaw = Config::RCS::HemlockYaw;
		Features::RCS::HemlockPitchReduction = Config::RCS::HemlockPitchReduction;
		Features::RCS::HemlockYawReduction = Config::RCS::HemlockYawReduction;
		Features::RCS::ProwlerPitch = Config::RCS::ProwlerPitch;
		Features::RCS::ProwlerYaw = Config::RCS::ProwlerYaw;
		Features::RCS::ProwlerPitchReduction = Config::RCS::ProwlerPitchReduction;
		Features::RCS::ProwlerYawReduction = Config::RCS::ProwlerYawReduction;
		Features::RCS::RepeaterPitch = Config::RCS::RepeaterPitch;
		Features::RCS::RepeaterYaw = Config::RCS::RepeaterYaw;
		Features::RCS::RepeaterPitchReduction = Config::RCS::RepeaterPitchReduction;
		Features::RCS::RepeaterYawReduction = Config::RCS::RepeaterYawReduction;
		Features::RCS::RampagePitch = Config::RCS::RampagePitch;
		Features::RCS::RampageYaw = Config::RCS::RampageYaw;
		Features::RCS::RampagePitchReduction = Config::RCS::RampagePitchReduction;
		Features::RCS::RampageYawReduction = Config::RCS::RampageYawReduction;
		Features::RCS::CARSMGPitch = Config::RCS::CARSMGPitch;
		Features::RCS::CARSMGYaw = Config::RCS::CARSMGYaw;
		Features::RCS::CARSMGPitchReduction = Config::RCS::CARSMGPitchReduction;
		Features::RCS::CARSMGYawReduction = Config::RCS::CARSMGYawReduction;
		Features::RCS::HavocPitch = Config::RCS::HavocPitch;
		Features::RCS::HavocYaw = Config::RCS::HavocYaw;
		Features::RCS::HavocPitchReduction = Config::RCS::HavocPitchReduction;
		Features::RCS::HavocYawReduction = Config::RCS::HavocYawReduction;
		Features::RCS::DevotionPitch = Config::RCS::DevotionPitch;
		Features::RCS::DevotionYaw = Config::RCS::DevotionYaw;
		Features::RCS::DevotionPitchReduction = Config::RCS::DevotionPitchReduction;
		Features::RCS::DevotionYawReduction = Config::RCS::DevotionYawReduction;
		Features::RCS::LSTARPitch = Config::RCS::LSTARPitch;
		Features::RCS::LSTARYaw = Config::RCS::LSTARYaw;
		Features::RCS::LSTARPitchReduction = Config::RCS::LSTARPitchReduction;
		Features::RCS::LSTARYawReduction = Config::RCS::LSTARYawReduction;
		Features::RCS::TripleTakePitch = Config::RCS::TripleTakePitch;
		Features::RCS::TripleTakeYaw = Config::RCS::TripleTakeYaw;
		Features::RCS::TripleTakePitchReduction = Config::RCS::TripleTakePitchReduction;
		Features::RCS::TripleTakeYawReduction = Config::RCS::TripleTakeYawReduction;
		Features::RCS::VoltPitch = Config::RCS::VoltPitch;
		Features::RCS::VoltYaw = Config::RCS::VoltYaw;
		Features::RCS::VoltPitchReduction = Config::RCS::VoltPitchReduction;
		Features::RCS::VoltYawReduction = Config::RCS::VoltYawReduction;
		Features::RCS::NemesisPitch = Config::RCS::NemesisPitch;
		Features::RCS::NemesisYaw = Config::RCS::NemesisYaw;
		Features::RCS::NemesisPitchReduction = Config::RCS::NemesisPitchReduction;
		Features::RCS::NemesisYawReduction = Config::RCS::NemesisYawReduction;
		Features::RCS::MozambiquePitch = Config::RCS::MozambiquePitch;
		Features::RCS::MozambiqueYaw = Config::RCS::MozambiqueYaw;
		Features::RCS::MozambiquePitchReduction = Config::RCS::MozambiquePitchReduction;
		Features::RCS::MozambiqueYawReduction = Config::RCS::MozambiqueYawReduction;
		Features::RCS::PeacekeeperPitch = Config::RCS::PeacekeeperPitch;
		Features::RCS::PeacekeeperYaw = Config::RCS::PeacekeeperYaw;
		Features::RCS::PeacekeeperPitchReduction = Config::RCS::PeacekeeperPitchReduction;
		Features::RCS::PeacekeeperYawReduction = Config::RCS::PeacekeeperYawReduction;
		Features::RCS::MastiffPitch = Config::RCS::MastiffPitch;
		Features::RCS::MastiffYaw = Config::RCS::MastiffYaw;
		Features::RCS::MastiffPitchReduction = Config::RCS::MastiffPitchReduction;
		Features::RCS::MastiffYawReduction = Config::RCS::MastiffYawReduction;
		Features::RCS::LongbowPitch = Config::RCS::LongbowPitch;
		Features::RCS::LongbowYaw = Config::RCS::LongbowYaw;
		Features::RCS::LongbowPitchReduction = Config::RCS::LongbowPitchReduction;
		Features::RCS::LongbowYawReduction = Config::RCS::LongbowYawReduction;
		Features::RCS::ChargeRiflePitch = Config::RCS::ChargeRiflePitch;
		Features::RCS::ChargeRifleYaw = Config::RCS::ChargeRifleYaw;
		Features::RCS::ChargeRiflePitchReduction = Config::RCS::ChargeRiflePitchReduction;
		Features::RCS::ChargeRifleYawReduction = Config::RCS::ChargeRifleYawReduction;
		Features::RCS::SentinelPitch = Config::RCS::SentinelPitch;
		Features::RCS::SentinelYaw = Config::RCS::SentinelYaw;
		Features::RCS::SentinelPitchReduction = Config::RCS::SentinelPitchReduction;
		Features::RCS::SentinelYawReduction = Config::RCS::SentinelYawReduction;
		Features::RCS::WingmanPitch = Config::RCS::WingmanPitch;
		Features::RCS::WingmanYaw = Config::RCS::WingmanYaw;
		Features::RCS::WingmanPitchReduction = Config::RCS::WingmanPitchReduction;
		Features::RCS::WingmanYawReduction = Config::RCS::WingmanYawReduction;
		Features::RCS::EVA8Pitch = Config::RCS::EVA8Pitch;
		Features::RCS::EVA8Yaw = Config::RCS::EVA8Yaw;
		Features::RCS::EVA8PitchReduction = Config::RCS::EVA8PitchReduction;
		Features::RCS::EVA8YawReduction = Config::RCS::EVA8YawReduction;
		Features::RCS::KraberPitch = Config::RCS::KraberPitch;
		Features::RCS::KraberYaw = Config::RCS::KraberYaw;
		Features::RCS::KraberPitchReduction = Config::RCS::KraberPitchReduction;
		Features::RCS::KraberYawReduction = Config::RCS::KraberYawReduction;

		Features::Triggerbot::Enabled = Config::Triggerbot::Enabled;
		Features::Triggerbot::BindMethod = Config::Triggerbot::BindMethod;
		Features::Triggerbot::TriggerBind = static_cast<InputKeyType>(Config::Triggerbot::TriggerBind);
		Features::Triggerbot::OnADS = Config::Triggerbot::OnADS;
		Features::Triggerbot::HipfireShotguns = Config::Triggerbot::HipfireShotguns;
		Features::Triggerbot::Range = Config::Triggerbot::Range;
		//Weapon Toggles
        //Light
        Features::Triggerbot::P2020 = Config::Triggerbot::P2020;
        Features::Triggerbot::RE45 = Config::Triggerbot::RE45;
        Features::Triggerbot::Alternator = Config::Triggerbot::Alternator;
        Features::Triggerbot::R99 = Config::Triggerbot::R99;
        Features::Triggerbot::R301 = Config::Triggerbot::R301;
        Features::Triggerbot::Spitfire = Config::Triggerbot::Spitfire;
        Features::Triggerbot::G7 = Config::Triggerbot::G7;
        //Heavy
        Features::Triggerbot::Flatline = Config::Triggerbot::Flatline;
        Features::Triggerbot::Hemlock = Config::Triggerbot::Hemlock;
        Features::Triggerbot::Repeater = Config::Triggerbot::Repeater;
        Features::Triggerbot::Rampage = Config::Triggerbot::Rampage;
        Features::Triggerbot::CARSMG = Config::Triggerbot::CARSMG;
        //Energy
        Features::Triggerbot::Havoc = Config::Triggerbot::Havoc;
        Features::Triggerbot::Devotion = Config::Triggerbot::Devotion;
        Features::Triggerbot::LSTAR = Config::Triggerbot::LSTAR;
        Features::Triggerbot::TripleTake = Config::Triggerbot::TripleTake;
        Features::Triggerbot::Volt = Config::Triggerbot::Volt;
        Features::Triggerbot::Nemesis = Config::Triggerbot::Nemesis;
        //Shotgun
        Features::Triggerbot::Mozambique = Config::Triggerbot::Mozambique;
        Features::Triggerbot::EVA8 = Config::Triggerbot::EVA8;
        Features::Triggerbot::Peacekeeper = Config::Triggerbot::Peacekeeper;
        Features::Triggerbot::Mastiff = Config::Triggerbot::Mastiff;
        //Snipers
        Features::Triggerbot::Longbow = Config::Triggerbot::Longbow;
        Features::Triggerbot::ChargeRifle = Config::Triggerbot::ChargeRifle;
        Features::Triggerbot::Sentinel = Config::Triggerbot::Sentinel;
        //Legendary
        Features::Triggerbot::Wingman = Config::Triggerbot::Wingman;
        Features::Triggerbot::Prowler = Config::Triggerbot::Prowler;
        Features::Triggerbot::Bocek = Config::Triggerbot::Bocek;
        Features::Triggerbot::Kraber = Config::Triggerbot::Kraber;
        Features::Triggerbot::Knife = Config::Triggerbot::Knife;
		//Advanced
		Features::Triggerbot::AdvancedTriggerbot = Config::Triggerbot::AdvancedTriggerbot;
		Features::Triggerbot::P2020Range = Config::Triggerbot::P2020Range;
		Features::Triggerbot::RE45Range = Config::Triggerbot::RE45Range;
		Features::Triggerbot::AlternatorRange = Config::Triggerbot::AlternatorRange;
		Features::Triggerbot::R99Range = Config::Triggerbot::R99Range;
		Features::Triggerbot::R301Range = Config::Triggerbot::R301Range;
		Features::Triggerbot::SpitfireRange = Config::Triggerbot::SpitfireRange;
		Features::Triggerbot::G7Range = Config::Triggerbot::G7Range;
		Features::Triggerbot::FlatlineRange = Config::Triggerbot::FlatlineRange;
		Features::Triggerbot::HemlockRange = Config::Triggerbot::HemlockRange;
		Features::Triggerbot::ProwlerRange = Config::Triggerbot::ProwlerRange;
		Features::Triggerbot::RepeaterRange = Config::Triggerbot::RepeaterRange;
		Features::Triggerbot::RampageRange = Config::Triggerbot::RampageRange;
		Features::Triggerbot::CARSMGRange = Config::Triggerbot::CARSMGRange;
		Features::Triggerbot::HavocRange = Config::Triggerbot::HavocRange;
		Features::Triggerbot::DevotionRange = Config::Triggerbot::DevotionRange;
		Features::Triggerbot::LSTARRange = Config::Triggerbot::LSTARRange;
		Features::Triggerbot::TripleTakeRange = Config::Triggerbot::TripleTakeRange;
		Features::Triggerbot::VoltRange = Config::Triggerbot::VoltRange;
		Features::Triggerbot::NemesisRange = Config::Triggerbot::NemesisRange;
		Features::Triggerbot::LongbowRange = Config::Triggerbot::LongbowRange;
		Features::Triggerbot::ChargeRifleRange = Config::Triggerbot::ChargeRifleRange;
		Features::Triggerbot::SentinelRange = Config::Triggerbot::SentinelRange;
		Features::Triggerbot::MozambiqueRange = Config::Triggerbot::MozambiqueRange;
		Features::Triggerbot::PeacekeeperRange = Config::Triggerbot::PeacekeeperRange;
		Features::Triggerbot::MastiffRange = Config::Triggerbot::MastiffRange;
		Features::Triggerbot::LongbowRange = Config::Triggerbot::LongbowRange;
		Features::Triggerbot::ChargeRifleRange = Config::Triggerbot::ChargeRifleRange;
		Features::Triggerbot::SentinelRange = Config::Triggerbot::SentinelRange;
		Features::Triggerbot::WingmanRange = Config::Triggerbot::WingmanRange;
		Features::Triggerbot::EVA8Range = Config::Triggerbot::EVA8Range;
		Features::Triggerbot::KraberRange = Config::Triggerbot::KraberRange;
		Features::Triggerbot::BocekRange = Config::Triggerbot::BocekRange;
		Features::Triggerbot::ThrowingKnifeRange = Config::Triggerbot::ThrowingKnifeRange;

		Features::Glow::NewGlow = Config::Glow::NewGlow;
		Features::Glow::GlowMaxDistance = Config::Glow::GlowMaxDistance;
		Features::Glow::GlowColorMode = Config::Glow::GlowColorMode;
		Features::Glow::GlowColorShieldMode = Config::Glow::GlowColorShieldMode;
		Features::Glow::GlowRadius = Config::Glow::GlowRadius;
		Features::Glow::BodyStyle = Config::Glow::BodyStyle;
		Features::Glow::OutlineStyle = Config::Glow::OutlineStyle;
		Features::Glow::ViewModelGlow = Config::Glow::ViewModelGlow;
		Features::Glow::ViewModelGlowSetting = Config::Glow::ViewModelGlowSetting;
		Features::Glow::ViewModelGlowCombo = Config::Glow::ViewModelGlowCombo;

		Features::Glow::Item::ItemGlow = Config::ItemGlow::ItemGlow;
		Features::Glow::Item::SelectedItemSelection = Config::ItemGlow::SelectedItemSelection;
		Features::Glow::Item::Common = Config::ItemGlow::Common;
		Features::Glow::Item::Rare = Config::ItemGlow::Rare;
		Features::Glow::Item::Epic = Config::ItemGlow::Epic;
		Features::Glow::Item::Gold = Config::ItemGlow::Gold;
		Features::Glow::Item::Legendary = Config::ItemGlow::Legendary;
		Features::Glow::Item::Weapons = Config::ItemGlow::Weapons;
		Features::Glow::Item::Ammo = Config::ItemGlow::Ammo;
		Features::Glow::Item::ItemGlowThickness = Config::ItemGlow::ItemGlowThickness;
		Features::Glow::Item::SelectedInsideStyle = Config::ItemGlow::SelectedInsideStyle;
		Features::Glow::Item::SelectedOutlineStyle = Config::ItemGlow::SelectedOutlineStyle;

		Features::Sense::Enabled = Config::Sense::Enabled;
		Features::Sense::DrawFOVCircle = Config::Sense::DrawFOVCircle;
		Features::Sense::DrawFilledFOVCircle = Config::Sense::DrawFilledFOVCircle;
		Features::Sense::FOVThickness = Config::Sense::FOVThickness;
		Features::Sense::GameFOV = Config::Sense::GameFOV;
		Features::Sense::DrawCrosshair = Config::Sense::DrawCrosshair;
		Features::Sense::CrosshairSize = Config::Sense::CrosshairSize;
		Features::Sense::CrosshairThickness = Config::Sense::CrosshairThickness;
		Features::Sense::ShowSpectators = Config::Sense::ShowSpectators;
		Features::Sense::DrawSpectatorWarning = Config::Sense::DrawSpectatorWarning;
		Features::Sense::DrawVisibleWarning = Config::Sense::DrawVisibleWarning;
		Features::Sense::WarningTextOutline = Config::Sense::WarningTextOutline;
		Features::Sense::WarningTextX = Config::Sense::WarningTextX;
		Features::Sense::WarningTextY = Config::Sense::WarningTextY;
		Features::Sense::TextOutline = Config::Sense::TextOutline;
		Features::Sense::VisibilityCheck = Config::Sense::VisibilityCheck;
		Features::Sense::ESPMaxDistance = Config::Sense::ESPMaxDistance;

		Features::Sense::Enemy::DrawEnemy = Config::SenseEnemy::DrawEnemy;
		Features::Sense::Enemy::DrawBoxes = Config::SenseEnemy::DrawBoxes;
		Features::Sense::Enemy::BoxType = Config::SenseEnemy::BoxType;
		Features::Sense::Enemy::BoxStyle = Config::SenseEnemy::BoxStyle;
		Features::Sense::Enemy::BoxThickness = Config::SenseEnemy::BoxThickness;
		Features::Sense::Enemy::DrawSkeleton = Config::SenseEnemy::DrawSkeleton;
		Features::Sense::Enemy::SkeletonThickness = Config::SenseEnemy::SkeletonThickness;
		Features::Sense::Enemy::DrawHeadCircle = Config::SenseEnemy::DrawHeadCircle;
		Features::Sense::Enemy::HeadCircleThickness = Config::SenseEnemy::HeadCircleThickness;
		Features::Sense::Enemy::DrawBars = Config::SenseEnemy::DrawBars;
		Features::Sense::Enemy::HealthBar = Config::SenseEnemy::HealthBar;
		Features::Sense::Enemy::ShieldBar = Config::SenseEnemy::ShieldBar;
		Features::Sense::Enemy::BarMode = Config::SenseEnemy::BarMode;
		Features::Sense::Enemy::BarStyle = Config::SenseEnemy::BarStyle;
		Features::Sense::Enemy::BarThickness = Config::SenseEnemy::BarThickness;
		Features::Sense::Enemy::BarThickness2 = Config::SenseEnemy::BarThickness2;
		Features::Sense::Enemy::BarHeight = Config::SenseEnemy::BarHeight;
		Features::Sense::Enemy::BarWidth = Config::SenseEnemy::BarWidth;
		Features::Sense::Enemy::BarColorMode = Config::SenseEnemy::BarColorMode;
		Features::Sense::Enemy::WeaponColorType = Config::SenseEnemy::WeaponColorType;
		Features::Sense::Enemy::DrawDistance = Config::SenseEnemy::DrawDistance;
		Features::Sense::Enemy::TracerPosition = Config::SenseEnemy::TracerPosition;
		Features::Sense::Enemy::TracerBone = Config::SenseEnemy::TracerBone;
		Features::Sense::Enemy::DrawTracers = Config::SenseEnemy::DrawTracers;
		Features::Sense::Enemy::TracerThickness = Config::SenseEnemy::TracerThickness;
		Features::Sense::Enemy::DrawNames = Config::SenseEnemy::DrawNames;
		Features::Sense::Enemy::DrawSeer = Config::SenseEnemy::DrawSeer;
		Features::Sense::Enemy::DrawStatus = Config::SenseEnemy::DrawStatus;
		Features::Sense::Enemy::ShowMaxStatusValues = Config::SenseEnemy::ShowMaxStatusValues;
		Features::Sense::Enemy::DrawWeapon = Config::SenseEnemy::DrawWeapon;
		Features::Sense::Enemy::DrawLegend = Config::SenseEnemy::DrawLegend;

		Features::Sense::Teammate::DrawTeam = Config::SenseTeammate::DrawTeam;
		Features::Sense::Teammate::DrawBoxes = Config::SenseTeammate::DrawBoxes;
		Features::Sense::Teammate::BoxType = Config::SenseTeammate::BoxType;
		Features::Sense::Teammate::BoxStyle = Config::SenseTeammate::BoxStyle;
		Features::Sense::Teammate::BoxThickness = Config::SenseTeammate::BoxThickness;
		Features::Sense::Teammate::DrawSkeleton = Config::SenseTeammate::DrawSkeleton;
		Features::Sense::Teammate::SkeletonThickness = Config::SenseTeammate::SkeletonThickness;
		Features::Sense::Teammate::DrawHeadCircle = Config::SenseTeammate::DrawHeadCircle;
		Features::Sense::Teammate::HeadCircleThickness = Config::SenseTeammate::HeadCircleThickness;
		Features::Sense::Teammate::DrawBars = Config::SenseTeammate::DrawBars;
		Features::Sense::Teammate::HealthBar = Config::SenseTeammate::HealthBar;
		Features::Sense::Teammate::ShieldBar = Config::SenseTeammate::ShieldBar;
		Features::Sense::Teammate::BarMode = Config::SenseTeammate::BarMode;
		Features::Sense::Teammate::BarStyle = Config::SenseTeammate::BarStyle;
		Features::Sense::Teammate::BarThickness = Config::SenseTeammate::BarThickness;
		Features::Sense::Teammate::BarThickness2 = Config::SenseTeammate::BarThickness2;
		Features::Sense::Teammate::BarHeight = Config::SenseTeammate::BarHeight;
		Features::Sense::Teammate::BarWidth = Config::SenseTeammate::BarWidth;
		Features::Sense::Teammate::BarColorMode = Config::SenseTeammate::BarColorMode;
		Features::Sense::Teammate::WeaponColorType = Config::SenseTeammate::WeaponColorType;
		Features::Sense::Teammate::DrawDistance = Config::SenseTeammate::DrawDistance;
		Features::Sense::Teammate::TracerPosition = Config::SenseTeammate::TracerPosition;
		Features::Sense::Teammate::TracerBone = Config::SenseTeammate::TracerBone;
		Features::Sense::Teammate::DrawTracers = Config::SenseTeammate::DrawTracers;
		Features::Sense::Teammate::TracerThickness = Config::SenseTeammate::TracerThickness;
		Features::Sense::Teammate::DrawNames = Config::SenseTeammate::DrawNames;
		Features::Sense::Teammate::DrawSeer = Config::SenseTeammate::DrawSeer;
		Features::Sense::Teammate::DrawStatus = Config::SenseTeammate::DrawStatus;
		Features::Sense::Teammate::ShowMaxStatusValues = Config::SenseTeammate::ShowMaxStatusValues;
		Features::Sense::Teammate::DrawWeapon = Config::SenseTeammate::DrawWeapon;
		Features::Sense::Teammate::DrawLegend = Config::SenseTeammate::DrawLegend;

		Features::Sense::Positions::NamePosition = Config::SensePositions::NamePosition;
		Features::Sense::Positions::DistancePosition = Config::SensePositions::DistancePosition;
		Features::Sense::Positions::LegendPosition = Config::SensePositions::LegendPosition;
		Features::Sense::Positions::WeaponPosition = Config::SensePositions::WeaponPosition;
		Features::Sense::Positions::StatusPosition = Config::SensePositions::StatusPosition;

		Features::Radar::MiniMap = Config::Radar::MiniMap;
		Features::Radar::MiniMapRange = Config::Radar::MiniMapRange;
		Features::Radar::MiniMapScaleX = Config::Radar::MiniMapScaleX;
		Features::Radar::MiniMapScaleY = Config::Radar::MiniMapScaleY;
		Features::Radar::MiniMapDotSize = Config::Radar::MiniMapDotSize;
		Features::Radar::MiniMapBlackBGSize = Config::Radar::MiniMapBlackBGSize;
		Features::Radar::MiniMapGuides = Config::Radar::MiniMapGuides;
		Features::Radar::BigMap = Config::Radar::BigMap;
		Features::Radar::BigMapBind = static_cast<InputKeyType>(Config::Radar::BigMapBind);
		Features::Radar::CircleColor[0] = Config::Radar::CircleColorR;
		Features::Radar::CircleColor[1] = Config::Radar::CircleColorG;
		Features::Radar::CircleColor[2] = Config::Radar::CircleColorB;
		Features::Radar::CircleColor[3] = Config::Radar::CircleColorA;

		Features::Misc::SuperGlide = Config::Misc::SuperGlide;
		Features::Misc::QuickTurn = Config::Misc::QuickTurn;
		Features::Misc::QuickTurnAngle = Config::Misc::QuickTurnAngle;
		Features::Misc::QuickTurnBind = static_cast<InputKeyType>(Config::Misc::QuickTurnBind);
		Features::Misc::BHop = Config::Misc::BHop;
		Features::Misc::BHopDelay = Config::Misc::BHopDelay;
		Features::Misc::BHopBind = static_cast<InputKeyType>(Config::Misc::BHopBind);
		Features::Misc::RapidFire = Config::Misc::RapidFire;
		Features::Misc::RapidFireDelay = Config::Misc::RapidFireDelay;
		Features::Misc::RapidFireBind = static_cast<InputKeyType>(Config::Misc::RapidFireBind);

		Features::Misc::RapidP2020 = Config::Misc::RapidP2020;
		Features::Misc::RapidR301 = Config::Misc::RapidR301;
		Features::Misc::RapidG7 = Config::Misc::RapidG7;
		Features::Misc::RapidFlatline = Config::Misc::RapidFlatline;
		Features::Misc::RapidHemlock = Config::Misc::RapidHemlock;
		Features::Misc::RapidProwler = Config::Misc::RapidProwler;
		Features::Misc::RapidNemesis = Config::Misc::RapidNemesis;
		Features::Misc::RapidMozambique = Config::Misc::RapidMozambique;
		Features::Misc::RapidEVA8 = Config::Misc::RapidEVA8;
		Features::Misc::RapidWingman = Config::Misc::RapidWingman;
		Features::Misc::SkinChanger = Config::Misc::SkinChanger;
		// Weapons //
		Features::Misc::SkinP2020 = Config::Misc::SkinP2020;
		Features::Misc::SkinRE45 = Config::Misc::SkinRE45;
		Features::Misc::SkinALTERNATOR = Config::Misc::SkinALTERNATOR;
		Features::Misc::SkinR99 = Config::Misc::SkinR99;
		Features::Misc::SkinR301 = Config::Misc::SkinR301;
		Features::Misc::SkinSPITFIRE = Config::Misc::SkinSPITFIRE;
		Features::Misc::SkinG7 = Config::Misc::SkinG7;
		Features::Misc::SkinFLATLINE = Config::Misc::SkinFLATLINE;
		Features::Misc::SkinHEMLOCK = Config::Misc::SkinHEMLOCK;
		Features::Misc::SkinREPEATER = Config::Misc::SkinREPEATER;
		Features::Misc::SkinRAMPAGE = Config::Misc::SkinRAMPAGE;
		Features::Misc::SkinCAR = Config::Misc::SkinCAR;
		Features::Misc::SkinHAVOC = Config::Misc::SkinHAVOC;
		Features::Misc::SkinDEVOTION = Config::Misc::SkinDEVOTION;
		Features::Misc::SkinLSTAR = Config::Misc::SkinLSTAR;
		Features::Misc::SkinTRIPLETAKE = Config::Misc::SkinTRIPLETAKE;
		Features::Misc::SkinVOLT = Config::Misc::SkinVOLT;
		Features::Misc::SkinNEMESIS = Config::Misc::SkinNEMESIS;
		Features::Misc::SkinMOZAMBIQUE = Config::Misc::SkinMOZAMBIQUE;
		Features::Misc::SkinEVA8 = Config::Misc::SkinEVA8;
		Features::Misc::SkinPEACEKEEPER = Config::Misc::SkinPEACEKEEPER;
		Features::Misc::SkinMASTIFF = Config::Misc::SkinMASTIFF;
		Features::Misc::SkinLONGBOW = Config::Misc::SkinLONGBOW;
		Features::Misc::SkinCHARGE_RIFLE = Config::Misc::SkinCHARGE_RIFLE;
		Features::Misc::SkinSENTINEL = Config::Misc::SkinSENTINEL;
		Features::Misc::SkinWINGMAN = Config::Misc::SkinWINGMAN;
		Features::Misc::SkinPROWLER = Config::Misc::SkinPROWLER;
		Features::Misc::SkinBOCEK = Config::Misc::SkinBOCEK;
		Features::Misc::SkinKRABER = Config::Misc::SkinKRABER;

		Features::Colors::WeaponColorMode = Config::Colors::WeaponColorMode;
		Features::Colors::FOVColor[0] = Config::Colors::FOVColorR;
		Features::Colors::FOVColor[1] = Config::Colors::FOVColorG;
		Features::Colors::FOVColor[2] = Config::Colors::FOVColorB;
		Features::Colors::FOVColor[3] = Config::Colors::FOVColorA;
		Features::Colors::FilledFOVColor[0] = Config::Colors::FilledFOVColorR;
		Features::Colors::FilledFOVColor[1] = Config::Colors::FilledFOVColorG;
		Features::Colors::FilledFOVColor[2] = Config::Colors::FilledFOVColorB;
		Features::Colors::FilledFOVColor[3] = Config::Colors::FilledFOVColorA;
		Features::Colors::CrosshairColor[0] = Config::Colors::CrosshairColorR;
		Features::Colors::CrosshairColor[1] = Config::Colors::CrosshairColorG;
		Features::Colors::CrosshairColor[2] = Config::Colors::CrosshairColorB;
		Features::Colors::CrosshairColor[3] = Config::Colors::CrosshairColorA;
		Features::Colors::SpectatorWarningColor[0] = Config::Colors::SpectatorWarningColorR;
		Features::Colors::SpectatorWarningColor[1] = Config::Colors::SpectatorWarningColorG;
		Features::Colors::SpectatorWarningColor[2] = Config::Colors::SpectatorWarningColorB;
		Features::Colors::SpectatorWarningColor[3] = Config::Colors::SpectatorWarningColorA;
		Features::Colors::VisibleWarningColor[0] = Config::Colors::VisibleWarningColorR;
		Features::Colors::VisibleWarningColor[1] = Config::Colors::VisibleWarningColorG;
		Features::Colors::VisibleWarningColor[2] = Config::Colors::VisibleWarningColorB;
		Features::Colors::VisibleWarningColor[3] = Config::Colors::VisibleWarningColorA;

		Features::Colors::Enemy::InvisibleBoxColor[0] = Config::EnemyColors::InvisibleBoxColorR;
		Features::Colors::Enemy::InvisibleBoxColor[1] = Config::EnemyColors::InvisibleBoxColorG;
		Features::Colors::Enemy::InvisibleBoxColor[2] = Config::EnemyColors::InvisibleBoxColorB;
		Features::Colors::Enemy::InvisibleBoxColor[3] = Config::EnemyColors::InvisibleBoxColorA;
		Features::Colors::Enemy::VisibleBoxColor[0] = Config::EnemyColors::VisibleBoxColorR;
		Features::Colors::Enemy::VisibleBoxColor[1] = Config::EnemyColors::VisibleBoxColorG;
		Features::Colors::Enemy::VisibleBoxColor[2] = Config::EnemyColors::VisibleBoxColorB;
		Features::Colors::Enemy::VisibleBoxColor[3] = Config::EnemyColors::VisibleBoxColorA;
		Features::Colors::Enemy::InvisibleFilledBoxColor[0] = Config::EnemyColors::InvisibleFilledBoxColorR;
		Features::Colors::Enemy::InvisibleFilledBoxColor[1] = Config::EnemyColors::InvisibleFilledBoxColorG;
		Features::Colors::Enemy::InvisibleFilledBoxColor[2] = Config::EnemyColors::InvisibleFilledBoxColorB;
		Features::Colors::Enemy::InvisibleFilledBoxColor[3] = Config::EnemyColors::InvisibleFilledBoxColorA;
		Features::Colors::Enemy::VisibleFilledBoxColor[0] = Config::EnemyColors::VisibleFilledBoxColorR;
		Features::Colors::Enemy::VisibleFilledBoxColor[1] = Config::EnemyColors::VisibleFilledBoxColorG;
		Features::Colors::Enemy::VisibleFilledBoxColor[2] = Config::EnemyColors::VisibleFilledBoxColorB;
		Features::Colors::Enemy::VisibleFilledBoxColor[3] = Config::EnemyColors::VisibleFilledBoxColorA;
		Features::Colors::Enemy::InvisibleTracerColor[0] = Config::EnemyColors::InvisibleTracerColorR;
		Features::Colors::Enemy::InvisibleTracerColor[1] = Config::EnemyColors::InvisibleTracerColorG;
		Features::Colors::Enemy::InvisibleTracerColor[2] = Config::EnemyColors::InvisibleTracerColorB;
		Features::Colors::Enemy::InvisibleTracerColor[3] = Config::EnemyColors::InvisibleTracerColorA;
		Features::Colors::Enemy::VisibleTracerColor[0] = Config::EnemyColors::VisibleTracerColorR;
		Features::Colors::Enemy::VisibleTracerColor[1] = Config::EnemyColors::VisibleTracerColorG;
		Features::Colors::Enemy::VisibleTracerColor[2] = Config::EnemyColors::VisibleTracerColorB;
		Features::Colors::Enemy::VisibleTracerColor[3] = Config::EnemyColors::VisibleTracerColorA;
		Features::Colors::Enemy::InvisibleSkeletonColor[0] = Config::EnemyColors::InvisibleSkeletonColorR;
		Features::Colors::Enemy::InvisibleSkeletonColor[1] = Config::EnemyColors::InvisibleSkeletonColorG;
		Features::Colors::Enemy::InvisibleSkeletonColor[2] = Config::EnemyColors::InvisibleSkeletonColorB;
		Features::Colors::Enemy::InvisibleSkeletonColor[3] = Config::EnemyColors::InvisibleSkeletonColorA;
		Features::Colors::Enemy::VisibleSkeletonColor[0] = Config::EnemyColors::VisibleSkeletonColorR;
		Features::Colors::Enemy::VisibleSkeletonColor[1] = Config::EnemyColors::VisibleSkeletonColorG;
		Features::Colors::Enemy::VisibleSkeletonColor[2] = Config::EnemyColors::VisibleSkeletonColorB;
		Features::Colors::Enemy::VisibleSkeletonColor[3] = Config::EnemyColors::VisibleSkeletonColorA;
		Features::Colors::Enemy::InvisibleHeadCircleColor[0] = Config::EnemyColors::InvisibleHeadCircleColorR;
		Features::Colors::Enemy::InvisibleHeadCircleColor[1] = Config::EnemyColors::InvisibleHeadCircleColorG;
		Features::Colors::Enemy::InvisibleHeadCircleColor[2] = Config::EnemyColors::InvisibleHeadCircleColorB;
		Features::Colors::Enemy::InvisibleHeadCircleColor[3] = Config::EnemyColors::InvisibleHeadCircleColorA;
		Features::Colors::Enemy::VisibleHeadCircleColor[0] = Config::EnemyColors::VisibleHeadCircleColorR;
		Features::Colors::Enemy::VisibleHeadCircleColor[1] = Config::EnemyColors::VisibleHeadCircleColorG;
		Features::Colors::Enemy::VisibleHeadCircleColor[2] = Config::EnemyColors::VisibleHeadCircleColorB;
		Features::Colors::Enemy::VisibleHeadCircleColor[3] = Config::EnemyColors::VisibleHeadCircleColorA;
		Features::Colors::Enemy::InvisibleNameColor[0] = Config::EnemyColors::InvisibleNameColorR;
		Features::Colors::Enemy::InvisibleNameColor[1] = Config::EnemyColors::InvisibleNameColorG;
		Features::Colors::Enemy::InvisibleNameColor[2] = Config::EnemyColors::InvisibleNameColorB;
		Features::Colors::Enemy::InvisibleNameColor[3] = Config::EnemyColors::InvisibleNameColorA;
		Features::Colors::Enemy::VisibleNameColor[0] = Config::EnemyColors::VisibleNameColorR;
		Features::Colors::Enemy::VisibleNameColor[1] = Config::EnemyColors::VisibleNameColorG;
		Features::Colors::Enemy::VisibleNameColor[2] = Config::EnemyColors::VisibleNameColorB;
		Features::Colors::Enemy::VisibleNameColor[3] = Config::EnemyColors::VisibleNameColorA;
		Features::Colors::Enemy::InvisibleDistanceColor[0] = Config::EnemyColors::InvisibleDistanceColorR;
		Features::Colors::Enemy::InvisibleDistanceColor[1] = Config::EnemyColors::InvisibleDistanceColorG;
		Features::Colors::Enemy::InvisibleDistanceColor[2] = Config::EnemyColors::InvisibleDistanceColorB;
		Features::Colors::Enemy::InvisibleDistanceColor[3] = Config::EnemyColors::InvisibleDistanceColorA;
		Features::Colors::Enemy::VisibleDistanceColor[0] = Config::EnemyColors::VisibleDistanceColorR;
		Features::Colors::Enemy::VisibleDistanceColor[1] = Config::EnemyColors::VisibleDistanceColorG;
		Features::Colors::Enemy::VisibleDistanceColor[2] = Config::EnemyColors::VisibleDistanceColorB;
		Features::Colors::Enemy::VisibleDistanceColor[3] = Config::EnemyColors::VisibleDistanceColorA;
		Features::Colors::Enemy::InvisibleLegendColor[0] = Config::EnemyColors::InvisibleLegendColorR;
		Features::Colors::Enemy::InvisibleLegendColor[1] = Config::EnemyColors::InvisibleLegendColorG;
		Features::Colors::Enemy::InvisibleLegendColor[2] = Config::EnemyColors::InvisibleLegendColorB;
		Features::Colors::Enemy::InvisibleLegendColor[3] = Config::EnemyColors::InvisibleLegendColorA;
		Features::Colors::Enemy::VisibleLegendColor[0] = Config::EnemyColors::VisibleLegendColorR;
		Features::Colors::Enemy::VisibleLegendColor[1] = Config::EnemyColors::VisibleLegendColorG;
		Features::Colors::Enemy::VisibleLegendColor[2] = Config::EnemyColors::VisibleLegendColorB;
		Features::Colors::Enemy::VisibleLegendColor[3] = Config::EnemyColors::VisibleLegendColorA;
		Features::Colors::Enemy::InvisibleWeaponColor[0] = Config::EnemyColors::InvisibleWeaponColorR;
		Features::Colors::Enemy::InvisibleWeaponColor[1] = Config::EnemyColors::InvisibleWeaponColorG;
		Features::Colors::Enemy::InvisibleWeaponColor[2] = Config::EnemyColors::InvisibleWeaponColorB;
		Features::Colors::Enemy::InvisibleWeaponColor[3] = Config::EnemyColors::InvisibleWeaponColorA;
		Features::Colors::Enemy::VisibleWeaponColor[0] = Config::EnemyColors::VisibleWeaponColorR;
		Features::Colors::Enemy::VisibleWeaponColor[1] = Config::EnemyColors::VisibleWeaponColorG;
		Features::Colors::Enemy::VisibleWeaponColor[2] = Config::EnemyColors::VisibleWeaponColorB;
		Features::Colors::Enemy::VisibleWeaponColor[3] = Config::EnemyColors::VisibleWeaponColorA;
		Features::Colors::Enemy::BarColorMode = Config::EnemyColors::BarColorMode;
		Features::Colors::Enemy::LightWeaponColor[0] = Config::EnemyColors::LightWeaponColorR;
		Features::Colors::Enemy::LightWeaponColor[1] = Config::EnemyColors::LightWeaponColorG;
		Features::Colors::Enemy::LightWeaponColor[2] = Config::EnemyColors::LightWeaponColorB;
		Features::Colors::Enemy::LightWeaponColor[3] = Config::EnemyColors::LightWeaponColorA;
		Features::Colors::Enemy::HeavyWeaponColor[0] = Config::EnemyColors::HeavyWeaponColorR;
		Features::Colors::Enemy::HeavyWeaponColor[1] = Config::EnemyColors::HeavyWeaponColorG;
		Features::Colors::Enemy::HeavyWeaponColor[2] = Config::EnemyColors::HeavyWeaponColorB;
		Features::Colors::Enemy::HeavyWeaponColor[3] = Config::EnemyColors::HeavyWeaponColorA;
		Features::Colors::Enemy::EnergyWeaponColor[0] = Config::EnemyColors::EnergyWeaponColorR;
		Features::Colors::Enemy::EnergyWeaponColor[1] = Config::EnemyColors::EnergyWeaponColorG;
		Features::Colors::Enemy::EnergyWeaponColor[2] = Config::EnemyColors::EnergyWeaponColorB;
		Features::Colors::Enemy::EnergyWeaponColor[3] = Config::EnemyColors::EnergyWeaponColorA;
		Features::Colors::Enemy::ShotgunWeaponColor[0] = Config::EnemyColors::ShotgunWeaponColorR;
		Features::Colors::Enemy::ShotgunWeaponColor[1] = Config::EnemyColors::ShotgunWeaponColorG;
		Features::Colors::Enemy::ShotgunWeaponColor[2] = Config::EnemyColors::ShotgunWeaponColorB;
		Features::Colors::Enemy::ShotgunWeaponColor[3] = Config::EnemyColors::ShotgunWeaponColorA;
		Features::Colors::Enemy::SniperWeaponColor[0] = Config::EnemyColors::SniperWeaponColorR;
		Features::Colors::Enemy::SniperWeaponColor[1] = Config::EnemyColors::SniperWeaponColorG;
		Features::Colors::Enemy::SniperWeaponColor[2] = Config::EnemyColors::SniperWeaponColorB;
		Features::Colors::Enemy::SniperWeaponColor[3] = Config::EnemyColors::SniperWeaponColorA;
		Features::Colors::Enemy::LegendaryWeaponColor[0] = Config::EnemyColors::LegendaryWeaponColorR;
		Features::Colors::Enemy::LegendaryWeaponColor[1] = Config::EnemyColors::LegendaryWeaponColorG;
		Features::Colors::Enemy::LegendaryWeaponColor[2] = Config::EnemyColors::LegendaryWeaponColorB;
		Features::Colors::Enemy::LegendaryWeaponColor[3] = Config::EnemyColors::LegendaryWeaponColorA;
		Features::Colors::Enemy::MeleeWeaponColor[0] = Config::EnemyColors::MeleeWeaponColorR;
		Features::Colors::Enemy::MeleeWeaponColor[1] = Config::EnemyColors::MeleeWeaponColorG;
		Features::Colors::Enemy::MeleeWeaponColor[2] = Config::EnemyColors::MeleeWeaponColorB;
		Features::Colors::Enemy::MeleeWeaponColor[3] = Config::EnemyColors::MeleeWeaponColorA;
		Features::Colors::Enemy::ThrowableWeaponColor[0] = Config::EnemyColors::ThrowableWeaponColorR;
		Features::Colors::Enemy::ThrowableWeaponColor[1] = Config::EnemyColors::ThrowableWeaponColorG;
		Features::Colors::Enemy::ThrowableWeaponColor[2] = Config::EnemyColors::ThrowableWeaponColorB;
		Features::Colors::Enemy::ThrowableWeaponColor[3] = Config::EnemyColors::ThrowableWeaponColorA;
		Features::Colors::Enemy::InvisibleGlowColor[0] = Config::EnemyColors::InvisibleGlowColorR;
		Features::Colors::Enemy::InvisibleGlowColor[1] = Config::EnemyColors::InvisibleGlowColorG;
		Features::Colors::Enemy::InvisibleGlowColor[2] = Config::EnemyColors::InvisibleGlowColorB;
		Features::Colors::Enemy::VisibleGlowColor[0] = Config::EnemyColors::VisibleGlowColorR;
		Features::Colors::Enemy::VisibleGlowColor[1] = Config::EnemyColors::VisibleGlowColorG;
		Features::Colors::Enemy::VisibleGlowColor[2] = Config::EnemyColors::VisibleGlowColorB;
		Features::Colors::Enemy::RedShieldColor[0] = Config::EnemyColors::RedShieldColorR;
		Features::Colors::Enemy::RedShieldColor[1] = Config::EnemyColors::RedShieldColorG;
		Features::Colors::Enemy::RedShieldColor[2] = Config::EnemyColors::RedShieldColorR;
		Features::Colors::Enemy::PurpleShieldColor[0] = Config::EnemyColors::PurpleShieldColorR;
		Features::Colors::Enemy::PurpleShieldColor[1] = Config::EnemyColors::PurpleShieldColorG;
		Features::Colors::Enemy::PurpleShieldColor[2] = Config::EnemyColors::PurpleShieldColorB;
		Features::Colors::Enemy::BlueShieldColor[0] = Config::EnemyColors::BlueShieldColorR;
		Features::Colors::Enemy::BlueShieldColor[1] = Config::EnemyColors::BlueShieldColorG;
		Features::Colors::Enemy::BlueShieldColor[2] = Config::EnemyColors::BlueShieldColorB;
		Features::Colors::Enemy::GreyShieldColor[0] = Config::EnemyColors::GreyShieldColorR;
		Features::Colors::Enemy::GreyShieldColor[1] = Config::EnemyColors::GreyShieldColorG;
		Features::Colors::Enemy::GreyShieldColor[2] = Config::EnemyColors::GreyShieldColorB;

		Features::Colors::Teammate::InvisibleBoxColor[0] = Config::TeammateColors::InvisibleBoxColorR;
		Features::Colors::Teammate::InvisibleBoxColor[1] = Config::TeammateColors::InvisibleBoxColorG;
		Features::Colors::Teammate::InvisibleBoxColor[2] = Config::TeammateColors::InvisibleBoxColorB;
		Features::Colors::Teammate::InvisibleBoxColor[3] = Config::TeammateColors::InvisibleBoxColorA;
		Features::Colors::Teammate::VisibleBoxColor[0] = Config::TeammateColors::VisibleBoxColorR;
		Features::Colors::Teammate::VisibleBoxColor[1] = Config::TeammateColors::VisibleBoxColorG;
		Features::Colors::Teammate::VisibleBoxColor[2] = Config::TeammateColors::VisibleBoxColorB;
		Features::Colors::Teammate::VisibleBoxColor[3] = Config::TeammateColors::VisibleBoxColorA;
		Features::Colors::Teammate::InvisibleFilledBoxColor[0] = Config::TeammateColors::InvisibleFilledBoxColorR;
		Features::Colors::Teammate::InvisibleFilledBoxColor[1] = Config::TeammateColors::InvisibleFilledBoxColorG;
		Features::Colors::Teammate::InvisibleFilledBoxColor[2] = Config::TeammateColors::InvisibleFilledBoxColorB;
		Features::Colors::Teammate::InvisibleFilledBoxColor[3] = Config::TeammateColors::InvisibleFilledBoxColorA;
		Features::Colors::Teammate::VisibleFilledBoxColor[0] = Config::TeammateColors::VisibleFilledBoxColorR;
		Features::Colors::Teammate::VisibleFilledBoxColor[1] = Config::TeammateColors::VisibleFilledBoxColorG;
		Features::Colors::Teammate::VisibleFilledBoxColor[2] = Config::TeammateColors::VisibleFilledBoxColorB;
		Features::Colors::Teammate::VisibleFilledBoxColor[3] = Config::TeammateColors::VisibleFilledBoxColorA;
		Features::Colors::Teammate::InvisibleTracerColor[0] = Config::TeammateColors::InvisibleTracerColorR;
		Features::Colors::Teammate::InvisibleTracerColor[1] = Config::TeammateColors::InvisibleTracerColorG;
		Features::Colors::Teammate::InvisibleTracerColor[2] = Config::TeammateColors::InvisibleTracerColorB;
		Features::Colors::Teammate::InvisibleTracerColor[3] = Config::TeammateColors::InvisibleTracerColorA;
		Features::Colors::Teammate::VisibleTracerColor[0] = Config::TeammateColors::VisibleTracerColorR;
		Features::Colors::Teammate::VisibleTracerColor[1] = Config::TeammateColors::VisibleTracerColorG;
		Features::Colors::Teammate::VisibleTracerColor[2] = Config::TeammateColors::VisibleTracerColorB;
		Features::Colors::Teammate::VisibleTracerColor[3] = Config::TeammateColors::VisibleTracerColorA;
		Features::Colors::Teammate::InvisibleSkeletonColor[0] = Config::TeammateColors::InvisibleSkeletonColorR;
		Features::Colors::Teammate::InvisibleSkeletonColor[1] = Config::TeammateColors::InvisibleSkeletonColorG;
		Features::Colors::Teammate::InvisibleSkeletonColor[2] = Config::TeammateColors::InvisibleSkeletonColorB;
		Features::Colors::Teammate::InvisibleSkeletonColor[3] = Config::TeammateColors::InvisibleSkeletonColorA;
		Features::Colors::Teammate::VisibleSkeletonColor[0] = Config::TeammateColors::VisibleSkeletonColorR;
		Features::Colors::Teammate::VisibleSkeletonColor[1] = Config::TeammateColors::VisibleSkeletonColorG;
		Features::Colors::Teammate::VisibleSkeletonColor[2] = Config::TeammateColors::VisibleSkeletonColorB;
		Features::Colors::Teammate::VisibleSkeletonColor[3] = Config::TeammateColors::VisibleSkeletonColorA;
		Features::Colors::Teammate::InvisibleHeadCircleColor[0] = Config::TeammateColors::InvisibleHeadCircleColorR;
		Features::Colors::Teammate::InvisibleHeadCircleColor[1] = Config::TeammateColors::InvisibleHeadCircleColorG;
		Features::Colors::Teammate::InvisibleHeadCircleColor[2] = Config::TeammateColors::InvisibleHeadCircleColorB;
		Features::Colors::Teammate::InvisibleHeadCircleColor[3] = Config::TeammateColors::InvisibleHeadCircleColorA;
		Features::Colors::Teammate::VisibleHeadCircleColor[0] = Config::TeammateColors::VisibleHeadCircleColorR;
		Features::Colors::Teammate::VisibleHeadCircleColor[1] = Config::TeammateColors::VisibleHeadCircleColorG;
		Features::Colors::Teammate::VisibleHeadCircleColor[2] = Config::TeammateColors::VisibleHeadCircleColorB;
		Features::Colors::Teammate::VisibleHeadCircleColor[3] = Config::TeammateColors::VisibleHeadCircleColorA;
		Features::Colors::Teammate::InvisibleNameColor[0] = Config::TeammateColors::InvisibleNameColorR;
		Features::Colors::Teammate::InvisibleNameColor[1] = Config::TeammateColors::InvisibleNameColorG;
		Features::Colors::Teammate::InvisibleNameColor[2] = Config::TeammateColors::InvisibleNameColorB;
		Features::Colors::Teammate::InvisibleNameColor[3] = Config::TeammateColors::InvisibleNameColorA;
		Features::Colors::Teammate::VisibleNameColor[0] = Config::TeammateColors::VisibleNameColorR;
		Features::Colors::Teammate::VisibleNameColor[1] = Config::TeammateColors::VisibleNameColorG;
		Features::Colors::Teammate::VisibleNameColor[2] = Config::TeammateColors::VisibleNameColorB;
		Features::Colors::Teammate::VisibleNameColor[3] = Config::TeammateColors::VisibleNameColorA;
		Features::Colors::Teammate::InvisibleDistanceColor[0] = Config::TeammateColors::InvisibleDistanceColorR;
		Features::Colors::Teammate::InvisibleDistanceColor[1] = Config::TeammateColors::InvisibleDistanceColorG;
		Features::Colors::Teammate::InvisibleDistanceColor[2] = Config::TeammateColors::InvisibleDistanceColorB;
		Features::Colors::Teammate::InvisibleDistanceColor[3] = Config::TeammateColors::InvisibleDistanceColorA;
		Features::Colors::Teammate::VisibleDistanceColor[0] = Config::TeammateColors::VisibleDistanceColorR;
		Features::Colors::Teammate::VisibleDistanceColor[1] = Config::TeammateColors::VisibleDistanceColorG;
		Features::Colors::Teammate::VisibleDistanceColor[2] = Config::TeammateColors::VisibleDistanceColorB;
		Features::Colors::Teammate::VisibleDistanceColor[3] = Config::TeammateColors::VisibleDistanceColorA;
		Features::Colors::Teammate::InvisibleLegendColor[0] = Config::TeammateColors::InvisibleLegendColorR;
		Features::Colors::Teammate::InvisibleLegendColor[1] = Config::TeammateColors::InvisibleLegendColorG;
		Features::Colors::Teammate::InvisibleLegendColor[2] = Config::TeammateColors::InvisibleLegendColorB;
		Features::Colors::Teammate::InvisibleLegendColor[3] = Config::TeammateColors::InvisibleLegendColorA;
		Features::Colors::Teammate::VisibleLegendColor[0] = Config::TeammateColors::VisibleLegendColorR;
		Features::Colors::Teammate::VisibleLegendColor[1] = Config::TeammateColors::VisibleLegendColorG;
		Features::Colors::Teammate::VisibleLegendColor[2] = Config::TeammateColors::VisibleLegendColorB;
		Features::Colors::Teammate::VisibleLegendColor[3] = Config::TeammateColors::VisibleLegendColorA;
		Features::Colors::Teammate::InvisibleWeaponColor[0] = Config::TeammateColors::InvisibleWeaponColorR;
		Features::Colors::Teammate::InvisibleWeaponColor[1] = Config::TeammateColors::InvisibleWeaponColorG;
		Features::Colors::Teammate::InvisibleWeaponColor[2] = Config::TeammateColors::InvisibleWeaponColorB;
		Features::Colors::Teammate::InvisibleWeaponColor[3] = Config::TeammateColors::InvisibleWeaponColorA;
		Features::Colors::Teammate::VisibleWeaponColor[0] = Config::TeammateColors::VisibleWeaponColorR;
		Features::Colors::Teammate::VisibleWeaponColor[1] = Config::TeammateColors::VisibleWeaponColorG;
		Features::Colors::Teammate::VisibleWeaponColor[2] = Config::TeammateColors::VisibleWeaponColorB;
		Features::Colors::Teammate::VisibleWeaponColor[3] = Config::TeammateColors::VisibleWeaponColorA;
		Features::Colors::Teammate::BarColorMode = Config::TeammateColors::BarColorMode;
		Features::Colors::Teammate::LightWeaponColor[0] = Config::TeammateColors::LightWeaponColorR;
		Features::Colors::Teammate::LightWeaponColor[1] = Config::TeammateColors::LightWeaponColorG;
		Features::Colors::Teammate::LightWeaponColor[2] = Config::TeammateColors::LightWeaponColorB;
		Features::Colors::Teammate::LightWeaponColor[3] = Config::TeammateColors::LightWeaponColorA;
		Features::Colors::Teammate::HeavyWeaponColor[0] = Config::TeammateColors::HeavyWeaponColorR;
		Features::Colors::Teammate::HeavyWeaponColor[1] = Config::TeammateColors::HeavyWeaponColorG;
		Features::Colors::Teammate::HeavyWeaponColor[2] = Config::TeammateColors::HeavyWeaponColorB;
		Features::Colors::Teammate::HeavyWeaponColor[3] = Config::TeammateColors::HeavyWeaponColorA;
		Features::Colors::Teammate::EnergyWeaponColor[0] = Config::TeammateColors::EnergyWeaponColorR;
		Features::Colors::Teammate::EnergyWeaponColor[1] = Config::TeammateColors::EnergyWeaponColorG;
		Features::Colors::Teammate::EnergyWeaponColor[2] = Config::TeammateColors::EnergyWeaponColorB;
		Features::Colors::Teammate::EnergyWeaponColor[3] = Config::TeammateColors::EnergyWeaponColorA;
		Features::Colors::Teammate::ShotgunWeaponColor[0] = Config::TeammateColors::ShotgunWeaponColorR;
		Features::Colors::Teammate::ShotgunWeaponColor[1] = Config::TeammateColors::ShotgunWeaponColorG;
		Features::Colors::Teammate::ShotgunWeaponColor[2] = Config::TeammateColors::ShotgunWeaponColorB;
		Features::Colors::Teammate::ShotgunWeaponColor[3] = Config::TeammateColors::ShotgunWeaponColorA;
		Features::Colors::Teammate::SniperWeaponColor[0] = Config::TeammateColors::SniperWeaponColorR;
		Features::Colors::Teammate::SniperWeaponColor[1] = Config::TeammateColors::SniperWeaponColorG;
		Features::Colors::Teammate::SniperWeaponColor[2] = Config::TeammateColors::SniperWeaponColorB;
		Features::Colors::Teammate::SniperWeaponColor[3] = Config::TeammateColors::SniperWeaponColorA;
		Features::Colors::Teammate::LegendaryWeaponColor[0] = Config::TeammateColors::LegendaryWeaponColorR;
		Features::Colors::Teammate::LegendaryWeaponColor[1] = Config::TeammateColors::LegendaryWeaponColorG;
		Features::Colors::Teammate::LegendaryWeaponColor[2] = Config::TeammateColors::LegendaryWeaponColorB;
		Features::Colors::Teammate::LegendaryWeaponColor[3] = Config::TeammateColors::LegendaryWeaponColorA;
		Features::Colors::Teammate::MeleeWeaponColor[0] = Config::TeammateColors::MeleeWeaponColorR;
		Features::Colors::Teammate::MeleeWeaponColor[1] = Config::TeammateColors::MeleeWeaponColorG;
		Features::Colors::Teammate::MeleeWeaponColor[2] = Config::TeammateColors::MeleeWeaponColorB;
		Features::Colors::Teammate::MeleeWeaponColor[3] = Config::TeammateColors::MeleeWeaponColorA;
		Features::Colors::Teammate::ThrowableWeaponColor[0] = Config::TeammateColors::ThrowableWeaponColorR;
		Features::Colors::Teammate::ThrowableWeaponColor[1] = Config::TeammateColors::ThrowableWeaponColorG;
		Features::Colors::Teammate::ThrowableWeaponColor[2] = Config::TeammateColors::ThrowableWeaponColorB;
		Features::Colors::Teammate::ThrowableWeaponColor[3] = Config::TeammateColors::ThrowableWeaponColorA;

		Features::Watermark::Watermark = Config::Watermark::Watermark;
		Features::Watermark::WatermarkPosition = Config::Watermark::WatermarkPosition;
		Features::Watermark::Name = Config::Watermark::Name;
		Features::Watermark::ProcessingSpeed = Config::Watermark::ProcessingSpeed;
		Features::Watermark::Spectators = Config::Watermark::Spectators;

		Features::Settings::GamemodeCheck = Config::Settings::GamemodeCheck;
		Features::Settings::ESPEnabled = Config::Settings::ESPEnabled;
		Features::Settings::OverlayEnabled = Config::Settings::OverlayEnabled;
		Features::Settings::FPSCap = Config::Settings::FPSCap;
		Features::Settings::CappedFPS = Config::Settings::CappedFPS;
	}

	bool ReadConfig()
	{
		std::string ConfigName = "Configs/" + std::string(configName) + ".ini";
		INIReader reader(ConfigName);
		if (reader.ParseError() < 0) {
			UpdateConfig();
			return false;
		}
		
		ReadBool(Aimbot, AimbotEnabled);
		ReadInt(Aimbot, BindMethod);
		ReadInt(Aimbot, AimbotMode);	 // Cubic Beizer (xap-client) or Grinder (Possibly linear?)
		ReadInt(Aimbot, InputMethod); // MoveMouse or Controller (Write To ViewAngles)
		ReadBool(Aimbot, ClosestHitbox);
		ReadBool(Aimbot, OnFire);
		ReadBool(Aimbot, OnADS);
		ReadBool(Aimbot, VisCheck);
		ReadBool(Aimbot, TeamCheck);
		ReadBool(Aimbot, TargetSwitching);
		ReadInt(Aimbot, Priority);
		ReadBool(Aimbot, PredictMovement);
		ReadBool(Aimbot, PredictBulletDrop);
		ReadFloat(Aimbot, FinalDistance);
		ReadFloat(Aimbot, Smooth);
		ReadFloat(Aimbot, Speed);
		ReadFloat(Aimbot, HipfireSmooth);
		ReadFloat(Aimbot, ADSSmooth);
		ReadFloat(Aimbot, SmoothDistance);
		ReadInt(Aimbot, Delay);
		ReadFloat(Aimbot, FOV);
		ReadFloat(Aimbot, ZoomScale);
		ReadFloat(Aimbot, MinDistance);
		ReadFloat(Aimbot, HipfireDistance);
		ReadFloat(Aimbot, ZoomDistance);
		// AimMode 2
		ReadFloat(Aimbot, HipfireSmooth1);
		ReadFloat(Aimbot, ADSSmooth1);
		ReadFloat(Aimbot, ExtraSmoothing);
		ReadFloat(Aimbot, Deadzone);
		ReadFloat(Aimbot, FOV1);
		ReadFloat(Aimbot, MinDistance2);
		ReadFloat(Aimbot, MaxDistance2);
		// Weapon Toggles
		// Light
		ReadBool(Aimbot, P2020);
		ReadBool(Aimbot, RE45);
		ReadBool(Aimbot, Alternator);
		ReadBool(Aimbot, R99);
		ReadBool(Aimbot, R301);
		ReadBool(Aimbot, Spitfire);
		ReadBool(Aimbot, G7);
		// Heavy
		ReadBool(Aimbot, Flatline);
		ReadBool(Aimbot, Hemlock);
		ReadBool(Aimbot, Repeater);
		ReadBool(Aimbot, Rampage);
		ReadBool(Aimbot, CARSMG);
		// Energy
		ReadBool(Aimbot, Havoc);
		ReadBool(Aimbot, Devotion);
		ReadBool(Aimbot, LSTAR);
		ReadBool(Aimbot, TripleTake);
		ReadBool(Aimbot, Volt);
		ReadBool(Aimbot, Nemesis);
		// Shotgun
		ReadBool(Aimbot, Mozambique);
		ReadBool(Aimbot, EVA8);
		ReadBool(Aimbot, Peacekeeper);
		ReadBool(Aimbot, Mastiff);
		// Snipers
		ReadBool(Aimbot, Longbow);
		ReadBool(Aimbot, ChargeRifle);
		ReadBool(Aimbot, Sentinel);
		// Legendary
		ReadBool(Aimbot, Wingman);
		ReadBool(Aimbot, Prowler);
		ReadBool(Aimbot, Bocek);
		ReadBool(Aimbot, Kraber);
		ReadBool(Aimbot, Knife);
		//---------------Advanced---------------//
		ReadBool(Aimbot, AdvancedAim);
		ReadBool(Aimbot, AdvancedFire);
		ReadBool(Aimbot, AdvancedADS);
		// Aimbot Mode 0 - xap-client
		ReadBool(Aimbot, AdvancedClosestHitbox);
		ReadFloat(Aimbot, AdvancedHitbox);
		ReadFloat(Aimbot, AdvancedSpeed);
		ReadFloat(Aimbot, AdvancedSmooth);
		ReadFloat(Aimbot, AdvancedHipfireSmooth);
		ReadFloat(Aimbot, AdvancedADSSmooth);
		// Aimbot Mode 1 - Grinder
		ReadFloat(Aimbot, AdvancedHipfireSmooth1);
		ReadFloat(Aimbot, AdvancedADSSmooth1);
		ReadFloat(Aimbot, AdvancedExtraSmooth1);
		ReadFloat(Aimbot, AdvancedFOV1);
		ReadFloat(Aimbot, AdvancedDeadzone);
		ReadFloat(Aimbot, AdvancedMinDistance1);
		ReadFloat(Aimbot, AdvancedMaxDistance1);
		// Advanced OnFire & OnADS - Aimbot Mode 0 & 1 - xap-client & grinder
		ReadBool(Aimbot, P2020Fire);
		ReadBool(Aimbot, P2020ADS);
		ReadBool(Aimbot, RE45Fire);
		ReadBool(Aimbot, RE45ADS);
		ReadBool(Aimbot, AlternatorFire);
		ReadBool(Aimbot, AlternatorADS);
		ReadBool(Aimbot, R99Fire);
		ReadBool(Aimbot, R99ADS);
		ReadBool(Aimbot, R301Fire);
		ReadBool(Aimbot, R301ADS);
		ReadBool(Aimbot, SpitfireFire);
		ReadBool(Aimbot, SpitfireADS);
		ReadBool(Aimbot, G7Fire);
		ReadBool(Aimbot, G7ADS);
		ReadBool(Aimbot, FlatlineFire);
		ReadBool(Aimbot, FlatlineADS);
		ReadBool(Aimbot, HemlockFire);
		ReadBool(Aimbot, HemlockADS);
		ReadBool(Aimbot, RepeaterFire);
		ReadBool(Aimbot, RepeaterADS);
		ReadBool(Aimbot, RampageFire);
		ReadBool(Aimbot, RampageADS);
		ReadBool(Aimbot, CARSMGFire);
		ReadBool(Aimbot, CARSMGADS);
		ReadBool(Aimbot, HavocFire);
		ReadBool(Aimbot, HavocADS);
		ReadBool(Aimbot, DevotionFire);
		ReadBool(Aimbot, DevotionADS);
		ReadBool(Aimbot, LSTARFire);
		ReadBool(Aimbot, LSTARADS);
		ReadBool(Aimbot, TripleTakeFire);
		ReadBool(Aimbot, TripleTakeADS);
		ReadBool(Aimbot, VoltFire);
		ReadBool(Aimbot, VoltADS);
		ReadBool(Aimbot, NemesisFire);
		ReadBool(Aimbot, NemesisADS);
		ReadBool(Aimbot, MozambiqueFire);
		ReadBool(Aimbot, MozambiqueADS);
		ReadBool(Aimbot, EVA8Fire);
		ReadBool(Aimbot, EVA8ADS);
		ReadBool(Aimbot, PeacekeeperFire);
		ReadBool(Aimbot, PeacekeeperADS);
		ReadBool(Aimbot, MastiffFire);
		ReadBool(Aimbot, MastiffADS);
		ReadBool(Aimbot, LongbowFire);
		ReadBool(Aimbot, LongbowADS);
		ReadBool(Aimbot, ChargeRifleFire);
		ReadBool(Aimbot, ChargeRifleADS);
		ReadBool(Aimbot, SentinelFire);
		ReadBool(Aimbot, SentinelADS);
		ReadBool(Aimbot, WingmanFire);
		ReadBool(Aimbot, WingmanADS);
		ReadBool(Aimbot, ProwlerFire);
		ReadBool(Aimbot, ProwlerADS);
		ReadBool(Aimbot, KraberFire);
		ReadBool(Aimbot, KraberADS);
		ReadBool(Aimbot, BocekFire);
		ReadBool(Aimbot, BocekADS);
		ReadBool(Aimbot, ThrowingKnifeFire);
		ReadBool(Aimbot, ThrowingKnifeADS);
		// Advanced Speed, Smooth + Hitbox - Aimbot Mode 0 - xap-client
		ReadBool(Aimbot, P2020ClosestHitbox);
		ReadFloat(Aimbot, P2020Hitbox);
		ReadFloat(Aimbot, P2020Speed);
		ReadFloat(Aimbot, P2020HipfireSmooth);
		ReadFloat(Aimbot, P2020ADSSmooth);
		ReadFloat(Aimbot, P2020FOV);
		ReadFloat(Aimbot, P2020ZoomScale);
		ReadBool(Aimbot, RE45ClosestHitbox);
		ReadFloat(Aimbot, RE45Hitbox);
		ReadFloat(Aimbot, RE45Speed);
		ReadFloat(Aimbot, RE45HipfireSmooth);
		ReadFloat(Aimbot, RE45ADSSmooth);
		ReadFloat(Aimbot, RE45FOV);
		ReadFloat(Aimbot, RE45ZoomScale);
		ReadBool(Aimbot, AlternatorClosestHitbox);
		ReadFloat(Aimbot, AlternatorHitbox);
		ReadFloat(Aimbot, AlternatorSpeed);
		ReadFloat(Aimbot, AlternatorHipfireSmooth);
		ReadFloat(Aimbot, AlternatorADSSmooth);
		ReadFloat(Aimbot, AlternatorFOV);
		ReadFloat(Aimbot, AlternatorZoomScale);
		ReadBool(Aimbot, R99ClosestHitbox);
		ReadFloat(Aimbot, R99Hitbox);
		ReadFloat(Aimbot, R99Speed);
		ReadFloat(Aimbot, R99HipfireSmooth);
		ReadFloat(Aimbot, R99ADSSmooth);
		ReadFloat(Aimbot, R99FOV);
		ReadFloat(Aimbot, R99ZoomScale);
		ReadBool(Aimbot, R301ClosestHitbox);
		ReadFloat(Aimbot, R301Hitbox);
		ReadFloat(Aimbot, R301Speed);
		ReadFloat(Aimbot, R301HipfireSmooth);
		ReadFloat(Aimbot, R301ADSSmooth);
		ReadFloat(Aimbot, R301FOV);
		ReadFloat(Aimbot, R301ZoomScale);
		ReadBool(Aimbot, SpitfireClosestHitbox);
		ReadFloat(Aimbot, SpitfireHitbox);
		ReadFloat(Aimbot, SpitfireSpeed);
		ReadFloat(Aimbot, SpitfireHipfireSmooth);
		ReadFloat(Aimbot, SpitfireADSSmooth);
		ReadFloat(Aimbot, SpitfireFOV);
		ReadFloat(Aimbot, SpitfireZoomScale);
		ReadBool(Aimbot, G7ClosestHitbox);
		ReadFloat(Aimbot, G7Hitbox);
		ReadFloat(Aimbot, G7Speed);
		ReadFloat(Aimbot, G7HipfireSmooth);
		ReadFloat(Aimbot, G7ADSSmooth);
		ReadFloat(Aimbot, G7FOV);
		ReadFloat(Aimbot, G7ZoomScale);
		// Heavy
		ReadBool(Aimbot, FlatlineClosestHitbox);
		ReadFloat(Aimbot, FlatlineHitbox);
		ReadFloat(Aimbot, FlatlineSpeed);
		ReadFloat(Aimbot, FlatlineHipfireSmooth);
		ReadFloat(Aimbot, FlatlineADSSmooth);
		ReadFloat(Aimbot, FlatlineFOV);
		ReadFloat(Aimbot, FlatlineZoomScale);
		ReadBool(Aimbot, HemlockClosestHitbox);
		ReadFloat(Aimbot, HemlockHitbox);
		ReadFloat(Aimbot, HemlockSpeed);
		ReadFloat(Aimbot, HemlockHipfireSmooth);
		ReadFloat(Aimbot, HemlockADSSmooth);
		ReadFloat(Aimbot, HemlockFOV);
		ReadFloat(Aimbot, HemlockZoomScale);
		ReadBool(Aimbot, RepeaterClosestHitbox);
		ReadFloat(Aimbot, RepeaterHitbox);
		ReadFloat(Aimbot, RepeaterSpeed);
		ReadFloat(Aimbot, RepeaterHipfireSmooth);
		ReadFloat(Aimbot, RepeaterADSSmooth);
		ReadFloat(Aimbot, RepeaterFOV);
		ReadFloat(Aimbot, RepeaterZoomScale);
		ReadBool(Aimbot, RampageClosestHitbox);
		ReadFloat(Aimbot, RampageHitbox);
		ReadFloat(Aimbot, RampageSpeed);
		ReadFloat(Aimbot, RampageHipfireSmooth);
		ReadFloat(Aimbot, RampageADSSmooth);
		ReadFloat(Aimbot, RampageFOV);
		ReadFloat(Aimbot, RampageZoomScale);
		ReadBool(Aimbot, CARSMGClosestHitbox);
		ReadFloat(Aimbot, CARSMGHitbox);
		ReadFloat(Aimbot, CARSMGSpeed);
		ReadFloat(Aimbot, CARSMGHipfireSmooth);
		ReadFloat(Aimbot, CARSMGADSSmooth);
		ReadFloat(Aimbot, CARSMGFOV);
		ReadFloat(Aimbot, CARSMGZoomScale);
		// Energy
		ReadBool(Aimbot, HavocClosestHitbox);
		ReadFloat(Aimbot, HavocHitbox);
		ReadFloat(Aimbot, HavocSpeed);
		ReadFloat(Aimbot, HavocHipfireSmooth);
		ReadFloat(Aimbot, HavocADSSmooth);
		ReadFloat(Aimbot, HavocFOV);
		ReadFloat(Aimbot, HavocZoomScale);
		ReadBool(Aimbot, DevotionClosestHitbox);
		ReadFloat(Aimbot, DevotionHitbox);
		ReadFloat(Aimbot, DevotionSpeed);
		ReadFloat(Aimbot, DevotionHipfireSmooth);
		ReadFloat(Aimbot, DevotionADSSmooth);
		ReadFloat(Aimbot, DevotionFOV);
		ReadFloat(Aimbot, DevotionZoomScale);
		ReadBool(Aimbot, LSTARClosestHitbox);
		ReadFloat(Aimbot, LSTARHitbox);
		ReadFloat(Aimbot, LSTARSpeed);
		ReadFloat(Aimbot, LSTARHipfireSmooth);
		ReadFloat(Aimbot, LSTARADSSmooth);
		ReadFloat(Aimbot, LSTARFOV);
		ReadFloat(Aimbot, LSTARZoomScale);
		ReadBool(Aimbot, TripleTakeClosestHitbox);
		ReadFloat(Aimbot, TripleTakeHitbox);
		ReadFloat(Aimbot, TripleTakeSpeed);
		ReadFloat(Aimbot, TripleTakeHipfireSmooth);
		ReadFloat(Aimbot, TripleTakeADSSmooth);
		ReadFloat(Aimbot, TripleTakeFOV);
		ReadFloat(Aimbot, TripleTakeZoomScale);
		ReadBool(Aimbot, VoltClosestHitbox);
		ReadFloat(Aimbot, VoltHitbox);
		ReadFloat(Aimbot, VoltSpeed);
		ReadFloat(Aimbot, VoltHipfireSmooth);
		ReadFloat(Aimbot, VoltADSSmooth);
		ReadFloat(Aimbot, VoltFOV);
		ReadFloat(Aimbot, VoltZoomScale);
		ReadBool(Aimbot, NemesisClosestHitbox);
		ReadFloat(Aimbot, NemesisHitbox);
		ReadFloat(Aimbot, NemesisSpeed);
		ReadFloat(Aimbot, NemesisHipfireSmooth);
		ReadFloat(Aimbot, NemesisADSSmooth);
		ReadFloat(Aimbot, NemesisFOV);
		ReadFloat(Aimbot, NemesisZoomScale);
		// Shotguns
		ReadBool(Aimbot, MozambiqueClosestHitbox);
		ReadFloat(Aimbot, MozambiqueHitbox);
		ReadFloat(Aimbot, MozambiqueSpeed);
		ReadFloat(Aimbot, MozambiqueHipfireSmooth);
		ReadFloat(Aimbot, MozambiqueADSSmooth);
		ReadFloat(Aimbot, MozambiqueFOV);
		ReadFloat(Aimbot, MozambiqueZoomScale);
		ReadBool(Aimbot, EVA8ClosestHitbox);
		ReadFloat(Aimbot, EVA8Hitbox);
		ReadFloat(Aimbot, EVA8Speed);
		ReadFloat(Aimbot, EVA8HipfireSmooth);
		ReadFloat(Aimbot, EVA8ADSSmooth);
		ReadFloat(Aimbot, EVA8FOV);
		ReadFloat(Aimbot, EVA8ZoomScale);
		ReadBool(Aimbot, PeacekeeperClosestHitbox);
		ReadFloat(Aimbot, PeacekeeperHitbox);
		ReadFloat(Aimbot, PeacekeeperSpeed);
		ReadFloat(Aimbot, PeacekeeperHipfireSmooth);
		ReadFloat(Aimbot, PeacekeeperADSSmooth);
		ReadFloat(Aimbot, PeacekeeperFOV);
		ReadFloat(Aimbot, PeacekeeperZoomScale);
		ReadBool(Aimbot, MastiffClosestHitbox);
		ReadFloat(Aimbot, MastiffHitbox);
		ReadFloat(Aimbot, MastiffSpeed);
		ReadFloat(Aimbot, MastiffHipfireSmooth);
		ReadFloat(Aimbot, MastiffADSSmooth);
		ReadFloat(Aimbot, MastiffFOV);
		ReadFloat(Aimbot, MastiffZoomScale);
		// Snipers
		ReadBool(Aimbot, LongbowClosestHitbox);
		ReadFloat(Aimbot, LongbowHitbox);
		ReadFloat(Aimbot, LongbowSpeed);
		ReadFloat(Aimbot, LongbowHipfireSmooth);
		ReadFloat(Aimbot, LongbowADSSmooth);
		ReadFloat(Aimbot, LongbowFOV);
		ReadFloat(Aimbot, LongbowZoomScale);
		ReadBool(Aimbot, ChargeRifleClosestHitbox);
		ReadFloat(Aimbot, ChargeRifleHitbox);
		ReadFloat(Aimbot, ChargeRifleSpeed);
		ReadFloat(Aimbot, ChargeRifleHipfireSmooth);
		ReadFloat(Aimbot, ChargeRifleADSSmooth);
		ReadFloat(Aimbot, ChargeRifleFOV);
		ReadFloat(Aimbot, ChargeRifleZoomScale);
		ReadBool(Aimbot, SentinelClosestHitbox);
		ReadFloat(Aimbot, SentinelHitbox);
		ReadFloat(Aimbot, SentinelSpeed);
		ReadFloat(Aimbot, SentinelHipfireSmooth);
		ReadFloat(Aimbot, SentinelADSSmooth);
		ReadFloat(Aimbot, SentinelFOV);
		ReadFloat(Aimbot, SentinelZoomScale);
		// Legendary
		ReadBool(Aimbot, WingmanClosestHitbox);
		ReadFloat(Aimbot, WingmanHitbox);
		ReadFloat(Aimbot, WingmanSpeed);
		ReadFloat(Aimbot, WingmanHipfireSmooth);
		ReadFloat(Aimbot, WingmanADSSmooth);
		ReadFloat(Aimbot, WingmanFOV);
		ReadFloat(Aimbot, WingmanZoomScale);
		ReadBool(Aimbot, ProwlerClosestHitbox);
		ReadFloat(Aimbot, ProwlerHitbox);
		ReadFloat(Aimbot, ProwlerSpeed);
		ReadFloat(Aimbot, ProwlerHipfireSmooth);
		ReadFloat(Aimbot, ProwlerADSSmooth);
		ReadFloat(Aimbot, ProwlerFOV);
		ReadFloat(Aimbot, ProwlerZoomScale);
		ReadBool(Aimbot, KraberClosestHitbox);
		ReadFloat(Aimbot, KraberHitbox);
		ReadFloat(Aimbot, KraberSpeed);
		ReadFloat(Aimbot, KraberHipfireSmooth);
		ReadFloat(Aimbot, KraberADSSmooth);
		ReadFloat(Aimbot, KraberFOV);
		ReadFloat(Aimbot, KraberZoomScale);
		ReadBool(Aimbot, BocekClosestHitbox);
		ReadFloat(Aimbot, BocekHitbox);
		ReadFloat(Aimbot, BocekSpeed);
		ReadFloat(Aimbot, BocekHipfireSmooth);
		ReadFloat(Aimbot, BocekADSSmooth);
		ReadFloat(Aimbot, BocekFOV);
		ReadFloat(Aimbot, BocekZoomScale);
		ReadBool(Aimbot, ThrowingKnifeClosestHitbox);
		ReadFloat(Aimbot, ThrowingKnifeHitbox);
		ReadFloat(Aimbot, ThrowingKnifeSpeed);
		ReadFloat(Aimbot, ThrowingKnifeHipfireSmooth);
		ReadFloat(Aimbot, ThrowingKnifeADSSmooth);
		ReadFloat(Aimbot, ThrowingKnifeFOV);
		ReadFloat(Aimbot, ThrowingKnifeZoomScale);
		// Advanced Smooth - Aimbot Mode 1 - Grinder
		ReadFloat(Aimbot, P2020HipfireSmooth1);
		ReadFloat(Aimbot, P2020ADSSmooth1);
		ReadFloat(Aimbot, P2020ExtraSmooth1);
		ReadFloat(Aimbot, P2020Deadzone);
		ReadFloat(Aimbot, P2020FOV1);
		ReadFloat(Aimbot, P2020MinDistance1);
		ReadFloat(Aimbot, P2020MaxDistance1);
		ReadFloat(Aimbot, RE45HipfireSmooth1);
		ReadFloat(Aimbot, RE45ADSSmooth1);
		ReadFloat(Aimbot, RE45ExtraSmooth1);
		ReadFloat(Aimbot, RE45Deadzone);
		ReadFloat(Aimbot, RE45FOV1);
		ReadFloat(Aimbot, RE45MinDistance1);
		ReadFloat(Aimbot, RE45MaxDistance1);
		ReadFloat(Aimbot, AlternatorHipfireSmooth1);
		ReadFloat(Aimbot, AlternatorADSSmooth1);
		ReadFloat(Aimbot, AlternatorExtraSmooth1);
		ReadFloat(Aimbot, AlternatorDeadzone);
		ReadFloat(Aimbot, AlternatorFOV1);
		ReadFloat(Aimbot, AlternatorMinDistance1);
		ReadFloat(Aimbot, AlternatorMaxDistance1);
		ReadFloat(Aimbot, R99HipfireSmooth1);
		ReadFloat(Aimbot, R99ADSSmooth1);
		ReadFloat(Aimbot, R99ExtraSmooth1);
		ReadFloat(Aimbot, R99Deadzone);
		ReadFloat(Aimbot, R99FOV1);
		ReadFloat(Aimbot, R99MinDistance1);
		ReadFloat(Aimbot, R99MaxDistance1);
		ReadFloat(Aimbot, R301HipfireSmooth1);
		ReadFloat(Aimbot, R301ADSSmooth1);
		ReadFloat(Aimbot, R301ExtraSmooth1);
		ReadFloat(Aimbot, R301Deadzone);
		ReadFloat(Aimbot, R301FOV1);
		ReadFloat(Aimbot, R301MinDistance1);
		ReadFloat(Aimbot, R301MaxDistance1);
		ReadFloat(Aimbot, SpitfireHipfireSmooth1);
		ReadFloat(Aimbot, SpitfireADSSmooth1);
		ReadFloat(Aimbot, SpitfireExtraSmooth1);
		ReadFloat(Aimbot, SpitfireDeadzone);
		ReadFloat(Aimbot, SpitfireFOV1);
		ReadFloat(Aimbot, SpitfireMinDistance1);
		ReadFloat(Aimbot, SpitfireMaxDistance1);
		ReadFloat(Aimbot, G7HipfireSmooth1);
		ReadFloat(Aimbot, G7ADSSmooth1);
		ReadFloat(Aimbot, G7ExtraSmooth1);
		ReadFloat(Aimbot, G7Deadzone);
		ReadFloat(Aimbot, G7FOV1);
		ReadFloat(Aimbot, G7MinDistance1);
		ReadFloat(Aimbot, G7MaxDistance1);

		ReadFloat(Aimbot, FlatlineHipfireSmooth1);
		ReadFloat(Aimbot, FlatlineADSSmooth1);
		ReadFloat(Aimbot, FlatlineExtraSmooth1);
		ReadFloat(Aimbot, FlatlineDeadzone);
		ReadFloat(Aimbot, FlatlineFOV1);
		ReadFloat(Aimbot, FlatlineMinDistance1);
		ReadFloat(Aimbot, FlatlineMaxDistance1);
		ReadFloat(Aimbot, HemlockHipfireSmooth1);
		ReadFloat(Aimbot, HemlockADSSmooth1);
		ReadFloat(Aimbot, HemlockExtraSmooth1);
		ReadFloat(Aimbot, HemlockDeadzone);
		ReadFloat(Aimbot, HemlockFOV1);
		ReadFloat(Aimbot, HemlockMinDistance1);
		ReadFloat(Aimbot, HemlockMaxDistance1);
		ReadFloat(Aimbot, RepeaterHipfireSmooth1);
		ReadFloat(Aimbot, RepeaterADSSmooth1);
		ReadFloat(Aimbot, RepeaterExtraSmooth1);
		ReadFloat(Aimbot, RepeaterDeadzone);
		ReadFloat(Aimbot, RepeaterFOV1);
		ReadFloat(Aimbot, RepeaterMinDistance1);
		ReadFloat(Aimbot, RepeaterMaxDistance1);
		ReadFloat(Aimbot, RampageHipfireSmooth1);
		ReadFloat(Aimbot, RampageADSSmooth1);
		ReadFloat(Aimbot, RampageExtraSmooth1);
		ReadFloat(Aimbot, RampageDeadzone);
		ReadFloat(Aimbot, RampageFOV1);
		ReadFloat(Aimbot, RampageMinDistance1);
		ReadFloat(Aimbot, RampageMaxDistance1);
		ReadFloat(Aimbot, CARSMGHipfireSmooth1);
		ReadFloat(Aimbot, CARSMGADSSmooth1);
		ReadFloat(Aimbot, CARSMGExtraSmooth1);
		ReadFloat(Aimbot, CARSMGDeadzone);
		ReadFloat(Aimbot, CARSMGFOV1);
		ReadFloat(Aimbot, CARSMGMinDistance1);
		ReadFloat(Aimbot, CARSMGMaxDistance1);

		ReadFloat(Aimbot, HavocHipfireSmooth1);
		ReadFloat(Aimbot, HavocADSSmooth1);
		ReadFloat(Aimbot, HavocExtraSmooth1);
		ReadFloat(Aimbot, HavocDeadzone);
		ReadFloat(Aimbot, HavocFOV1);
		ReadFloat(Aimbot, HavocMinDistance1);
		ReadFloat(Aimbot, HavocMaxDistance1);
		ReadFloat(Aimbot, DevotionHipfireSmooth1);
		ReadFloat(Aimbot, DevotionADSSmooth1);
		ReadFloat(Aimbot, DevotionExtraSmooth1);
		ReadFloat(Aimbot, DevotionDeadzone);
		ReadFloat(Aimbot, DevotionFOV1);
		ReadFloat(Aimbot, DevotionMinDistance1);
		ReadFloat(Aimbot, DevotionMaxDistance1);
		ReadFloat(Aimbot, LSTARHipfireSmooth1);
		ReadFloat(Aimbot, LSTARADSSmooth1);
		ReadFloat(Aimbot, LSTARExtraSmooth1);
		ReadFloat(Aimbot, LSTARDeadzone);
		ReadFloat(Aimbot, LSTARFOV1);
		ReadFloat(Aimbot, LSTARMinDistance1);
		ReadFloat(Aimbot, LSTARMaxDistance1);
		ReadFloat(Aimbot, TripleTakeHipfireSmooth1);
		ReadFloat(Aimbot, TripleTakeADSSmooth1);
		ReadFloat(Aimbot, TripleTakeExtraSmooth1);
		ReadFloat(Aimbot, TripleTakeDeadzone);
		ReadFloat(Aimbot, TripleTakeFOV1);
		ReadFloat(Aimbot, TripleTakeMinDistance1);
		ReadFloat(Aimbot, TripleTakeMaxDistance1);
		ReadFloat(Aimbot, VoltHipfireSmooth1);
		ReadFloat(Aimbot, VoltADSSmooth1);
		ReadFloat(Aimbot, VoltExtraSmooth1);
		ReadFloat(Aimbot, VoltDeadzone);
		ReadFloat(Aimbot, VoltFOV1);
		ReadFloat(Aimbot, VoltMinDistance1);
		ReadFloat(Aimbot, VoltMaxDistance1);
		ReadFloat(Aimbot, NemesisHipfireSmooth1);
		ReadFloat(Aimbot, NemesisADSSmooth1);
		ReadFloat(Aimbot, NemesisExtraSmooth1);
		ReadFloat(Aimbot, NemesisDeadzone);
		ReadFloat(Aimbot, NemesisFOV1);
		ReadFloat(Aimbot, NemesisMinDistance1);
		ReadFloat(Aimbot, NemesisMaxDistance1);

		ReadFloat(Aimbot, MozambiqueHipfireSmooth1);
		ReadFloat(Aimbot, MozambiqueADSSmooth1);
		ReadFloat(Aimbot, MozambiqueExtraSmooth1);
		ReadFloat(Aimbot, MozambiqueDeadzone);
		ReadFloat(Aimbot, MozambiqueFOV1);
		ReadFloat(Aimbot, MozambiqueMinDistance1);
		ReadFloat(Aimbot, MozambiqueMaxDistance1);
		ReadFloat(Aimbot, EVA8HipfireSmooth1);
		ReadFloat(Aimbot, EVA8ADSSmooth1);
		ReadFloat(Aimbot, EVA8ExtraSmooth1);
		ReadFloat(Aimbot, EVA8Deadzone);
		ReadFloat(Aimbot, EVA8FOV1);
		ReadFloat(Aimbot, EVA8MinDistance1);
		ReadFloat(Aimbot, EVA8MaxDistance1);
		ReadFloat(Aimbot, PeacekeeperHipfireSmooth1);
		ReadFloat(Aimbot, PeacekeeperADSSmooth1);
		ReadFloat(Aimbot, PeacekeeperExtraSmooth1);
		ReadFloat(Aimbot, PeacekeeperDeadzone);
		ReadFloat(Aimbot, PeacekeeperFOV1);
		ReadFloat(Aimbot, PeacekeeperMinDistance1);
		ReadFloat(Aimbot, PeacekeeperMaxDistance1);
		ReadFloat(Aimbot, MastiffHipfireSmooth1);
		ReadFloat(Aimbot, MastiffADSSmooth1);
		ReadFloat(Aimbot, MastiffExtraSmooth1);
		ReadFloat(Aimbot, MastiffDeadzone);
		ReadFloat(Aimbot, MastiffFOV1);
		ReadFloat(Aimbot, MastiffMinDistance1);
		ReadFloat(Aimbot, MastiffMaxDistance1);

		ReadFloat(Aimbot, LongbowHipfireSmooth1);
		ReadFloat(Aimbot, LongbowADSSmooth1);
		ReadFloat(Aimbot, LongbowExtraSmooth1);
		ReadFloat(Aimbot, LongbowDeadzone);
		ReadFloat(Aimbot, LongbowFOV1);
		ReadFloat(Aimbot, LongbowMinDistance1);
		ReadFloat(Aimbot, LongbowMaxDistance1);
		ReadFloat(Aimbot, ChargeRifleHipfireSmooth1);
		ReadFloat(Aimbot, ChargeRifleADSSmooth1);
		ReadFloat(Aimbot, ChargeRifleExtraSmooth1);
		ReadFloat(Aimbot, ChargeRifleDeadzone);
		ReadFloat(Aimbot, ChargeRifleFOV1);
		ReadFloat(Aimbot, ChargeRifleMinDistance1);
		ReadFloat(Aimbot, ChargeRifleMaxDistance1);
		ReadFloat(Aimbot, SentinelHipfireSmooth1);
		ReadFloat(Aimbot, SentinelADSSmooth1);
		ReadFloat(Aimbot, SentinelExtraSmooth1);
		ReadFloat(Aimbot, SentinelDeadzone);
		ReadFloat(Aimbot, SentinelFOV1);
		ReadFloat(Aimbot, SentinelMinDistance1);
		ReadFloat(Aimbot, SentinelMaxDistance1);

		ReadFloat(Aimbot, WingmanHipfireSmooth1);
		ReadFloat(Aimbot, WingmanADSSmooth1);
		ReadFloat(Aimbot, WingmanExtraSmooth1);
		ReadFloat(Aimbot, WingmanDeadzone);
		ReadFloat(Aimbot, WingmanFOV1);
		ReadFloat(Aimbot, WingmanMinDistance1);
		ReadFloat(Aimbot, WingmanMaxDistance1);
		ReadFloat(Aimbot, ProwlerHipfireSmooth1);
		ReadFloat(Aimbot, ProwlerADSSmooth1);
		ReadFloat(Aimbot, ProwlerExtraSmooth1);
		ReadFloat(Aimbot, ProwlerDeadzone);
		ReadFloat(Aimbot, ProwlerFOV1);
		ReadFloat(Aimbot, ProwlerMinDistance1);
		ReadFloat(Aimbot, ProwlerMaxDistance1);
		ReadFloat(Aimbot, BocekHipfireSmooth1);
		ReadFloat(Aimbot, BocekADSSmooth1);
		ReadFloat(Aimbot, BocekExtraSmooth1);
		ReadFloat(Aimbot, BocekDeadzone);
		ReadFloat(Aimbot, BocekFOV1);
		ReadFloat(Aimbot, BocekMinDistance1);
		ReadFloat(Aimbot, BocekMaxDistance1);
		ReadFloat(Aimbot, KraberHipfireSmooth1);
		ReadFloat(Aimbot, KraberADSSmooth1);
		ReadFloat(Aimbot, KraberExtraSmooth1);
		ReadFloat(Aimbot, KraberDeadzone);
		ReadFloat(Aimbot, KraberFOV1);
		ReadFloat(Aimbot, KraberMinDistance1);
		ReadFloat(Aimbot, KraberMaxDistance1);
		ReadFloat(Aimbot, ThrowingKnifeHipfireSmooth1);
		ReadFloat(Aimbot, ThrowingKnifeADSSmooth1);
		ReadFloat(Aimbot, ThrowingKnifeExtraSmooth1);
		ReadFloat(Aimbot, ThrowingKnifeDeadzone);
		ReadFloat(Aimbot, ThrowingKnifeFOV1);
		ReadFloat(Aimbot, ThrowingKnifeMinDistance1);
		ReadFloat(Aimbot, ThrowingKnifeMaxDistance1);

		ReadInt(AimbotBinds, AimBind);
		ReadInt(AimbotBinds, ExtraBind);
		// Advanced Binds
		ReadInt(AimbotBinds, P2020AimBind);
		ReadInt(AimbotBinds, P2020ExtraBind);
		ReadInt(AimbotBinds, RE45AimBind);
		ReadInt(AimbotBinds, RE45ExtraBind);
		ReadInt(AimbotBinds, AlternatorAimBind);
		ReadInt(AimbotBinds, AlternatorExtraBind);
		ReadInt(AimbotBinds, R99AimBind);
		ReadInt(AimbotBinds, R99ExtraBind);
		ReadInt(AimbotBinds, R301AimBind);
		ReadInt(AimbotBinds, R301ExtraBind);
		ReadInt(AimbotBinds, SpitfireAimBind);
		ReadInt(AimbotBinds, SpitfireExtraBind);
		ReadInt(AimbotBinds, G7AimBind);
		ReadInt(AimbotBinds, G7ExtraBind);
		ReadInt(AimbotBinds, FlatlineAimBind);
		ReadInt(AimbotBinds, FlatlineExtraBind);
		ReadInt(AimbotBinds, HemlockAimBind);
		ReadInt(AimbotBinds, HemlockExtraBind);
		ReadInt(AimbotBinds, RepeaterAimBind);
		ReadInt(AimbotBinds, RepeaterExtraBind);
		ReadInt(AimbotBinds, RampageAimBind);
		ReadInt(AimbotBinds, RampageExtraBind);
		ReadInt(AimbotBinds, CARSMGAimBind);
		ReadInt(AimbotBinds, CARSMGExtraBind);
		ReadInt(AimbotBinds, HavocAimBind);
		ReadInt(AimbotBinds, HavocExtraBind);
		ReadInt(AimbotBinds, DevotionAimBind);
		ReadInt(AimbotBinds, DevotionExtraBind);
		ReadInt(AimbotBinds, LSTARAimBind);
		ReadInt(AimbotBinds, LSTARExtraBind);
		ReadInt(AimbotBinds, TripleTakeAimBind);
		ReadInt(AimbotBinds, TripleTakeExtraBind);
		ReadInt(AimbotBinds, VoltAimBind);
		ReadInt(AimbotBinds, VoltExtraBind);
		ReadInt(AimbotBinds, NemesisAimBind);
		ReadInt(AimbotBinds, NemesisExtraBind);
		ReadInt(AimbotBinds, MozambiqueAimBind);
		ReadInt(AimbotBinds, MozambiqueExtraBind);
		ReadInt(AimbotBinds, EVA8AimBind);
		ReadInt(AimbotBinds, EVA8ExtraBind);
		ReadInt(AimbotBinds, PeacekeeperAimBind);
		ReadInt(AimbotBinds, PeacekeeperExtraBind);
		ReadInt(AimbotBinds, MastiffAimBind);
		ReadInt(AimbotBinds, MastiffExtraBind);
		ReadInt(AimbotBinds, LongbowAimBind);
		ReadInt(AimbotBinds, LongbowExtraBind);
		ReadInt(AimbotBinds, ChargeRifleAimBind);
		ReadInt(AimbotBinds, ChargeRifleExtraBind);
		ReadInt(AimbotBinds, SentinelAimBind);
		ReadInt(AimbotBinds, SentinelExtraBind);
		ReadInt(AimbotBinds, WingmanAimBind);
		ReadInt(AimbotBinds, WingmanExtraBind);
		ReadInt(AimbotBinds, ProwlerAimBind);
		ReadInt(AimbotBinds, ProwlerExtraBind);
		ReadInt(AimbotBinds, BocekAimBind);
		ReadInt(AimbotBinds, BocekExtraBind);
		ReadInt(AimbotBinds, KraberAimBind);
		ReadInt(AimbotBinds, KraberExtraBind);
		ReadInt(AimbotBinds, ThrowingKnifeAimBind);
		ReadInt(AimbotBinds, ThrowingKnifeExtraBind);

		ReadInt(AimbotHitboxes, Hitbox);
		// Advanced Hitboxes
		ReadInt(AimbotHitboxes, P2020Hitbox);
		ReadInt(AimbotHitboxes, RE45Hitbox);
		ReadInt(AimbotHitboxes, AlternatorHitbox);
		ReadInt(AimbotHitboxes, R99Hitbox);
		ReadInt(AimbotHitboxes, R301Hitbox);
		ReadInt(AimbotHitboxes, SpitfireHitbox);
		ReadInt(AimbotHitboxes, G7Hitbox);
		ReadInt(AimbotHitboxes, FlatlineHitbox);
		ReadInt(AimbotHitboxes, HemlockHitbox);
		ReadInt(AimbotHitboxes, RepeaterHitbox);
		ReadInt(AimbotHitboxes, RampageHitbox);
		ReadInt(AimbotHitboxes, CARSMGHitbox);
		ReadInt(AimbotHitboxes, HavocHitbox);
		ReadInt(AimbotHitboxes, DevotionHitbox);
		ReadInt(AimbotHitboxes, LSTARHitbox);
		ReadInt(AimbotHitboxes, TripleTakeHitbox);
		ReadInt(AimbotHitboxes, VoltHitbox);
		ReadInt(AimbotHitboxes, NemesisHitbox);
		ReadInt(AimbotHitboxes, MozambiqueHitbox);
		ReadInt(AimbotHitboxes, EVA8Hitbox);
		ReadInt(AimbotHitboxes, PeacekeeperHitbox);
		ReadInt(AimbotHitboxes, MastiffHitbox);
		ReadInt(AimbotHitboxes, LongbowHitbox);
		ReadInt(AimbotHitboxes, ChargeRifleHitbox);
		ReadInt(AimbotHitboxes, SentinelHitbox);
		ReadInt(AimbotHitboxes, WingmanHitbox);
		ReadInt(AimbotHitboxes, ProwlerHitbox);
		ReadInt(AimbotHitboxes, BocekHitbox);
		ReadInt(AimbotHitboxes, KraberHitbox);
		ReadInt(AimbotHitboxes, ThrowingKnifeHitbox);

		ReadBool(Ragebot, RagebotEnabled);
		ReadBool(Ragebot, RageAimbot);
		ReadInt(Ragebot, AimMethod);
		ReadBool(Ragebot, ClosestHitbox);
		ReadInt(Ragebot, Hitbox);
		ReadInt(Ragebot, BindMethod);
		ReadBool(Ragebot, OnFire);
		ReadBool(Ragebot, OnADS);
		ReadInt(Ragebot, AimBind);
		ReadInt(Ragebot, ExtraBind);
		ReadBool(Ragebot, VisCheck);
		ReadBool(Ragebot, TeamCheck);
		ReadInt(Ragebot, Priority);
		ReadBool(Ragebot, AutoShoot);
		ReadBool(Ragebot, PredictMovement);
		ReadBool(Ragebot, PredictBulletDrop);
		ReadFloat(Ragebot, PreditcionAmount);
		ReadFloat(Ragebot, FinalDistance);
		ReadFloat(Ragebot, Speed);
		ReadFloat(Ragebot, Smooth);
		ReadFloat(Ragebot, HipfireSmooth);
		ReadFloat(Ragebot, ADSSmooth);
		ReadFloat(Ragebot, SmoothDistance);
		ReadInt(Ragebot, Delay);
		ReadFloat(Ragebot, FOV);
		ReadFloat(Ragebot, ZoomScale);
		ReadFloat(Ragebot, MinDistance);
		ReadFloat(Ragebot, HipfireDistance);
		ReadFloat(Ragebot, ZoomDistance);
		// Weapon Toggles
		// Light
		ReadBool(Ragebot, P2020);
		ReadBool(Ragebot, RE45);
		ReadBool(Ragebot, Alternator);
		ReadBool(Ragebot, R99);
		ReadBool(Ragebot, R301);
		ReadBool(Ragebot, Spitfire);
		ReadBool(Ragebot, G7);
		// Heavy
		ReadBool(Ragebot, Flatline);
		ReadBool(Ragebot, Hemlock);
		ReadBool(Ragebot, Repeater);
		ReadBool(Ragebot, Rampage);
		ReadBool(Ragebot, CARSMG);
		// Energy
		ReadBool(Ragebot, Havoc);
		ReadBool(Ragebot, Devotion);
		ReadBool(Ragebot, LSTAR);
		ReadBool(Ragebot, TripleTake);
		ReadBool(Ragebot, Volt);
		ReadBool(Ragebot, Nemesis);
		// Shotgun
		ReadBool(Ragebot, Mozambique);
		ReadBool(Ragebot, EVA8);
		ReadBool(Ragebot, Peacekeeper);
		ReadBool(Ragebot, Mastiff);
		// Snipers
		ReadBool(Ragebot, Longbow);
		ReadBool(Ragebot, ChargeRifle);
		ReadBool(Ragebot, Sentinel);
		// Legendary
		ReadBool(Ragebot, Wingman);
		ReadBool(Ragebot, Prowler);
		ReadBool(Ragebot, Bocek);
		ReadBool(Ragebot, Kraber);
		ReadBool(Ragebot, Knife);
		ReadBool(Ragebot, RageRCS);
		ReadFloat(Ragebot, RecoilRate);

		ReadBool(Flickbot, Flickbot);
		ReadInt(Flickbot, FlickbotMethod); // 0 = Mouse, 1 = Memory
		ReadInt(Flickbot, FlickBind);
		ReadBool(Flickbot, ClosestHitbox);
		ReadInt(Flickbot, Hitbox);
		ReadInt(Flickbot, Priority);
		ReadBool(Flickbot, AutoShoot);
		ReadInt(Flickbot, AutoShootDelay);
		ReadBool(Flickbot, FlickBack);
		ReadInt(Flickbot, FlickBackDelay);
		ReadFloat(Flickbot, FlickBackSmoothing);
		ReadBool(Flickbot, PredictMovement);
		ReadBool(Flickbot, PredictBulletDrop);
		ReadBool(Flickbot, TeamCheck);
		ReadFloat(Flickbot, PreditcionAmount);
		ReadFloat(Flickbot, FinalDistance);
		ReadFloat(Flickbot, HipfireSmooth);
		ReadFloat(Flickbot, ADSSmooth);
		ReadFloat(Flickbot, SmoothDistance);
		ReadInt(Flickbot, Delay);
		ReadFloat(Flickbot, FOV);
		ReadFloat(Flickbot, ZoomScale);
		ReadFloat(Flickbot, MinDistance);
		ReadFloat(Flickbot, HipfireDistance);
		ReadFloat(Flickbot, ZoomDistance);
		ReadFloat(Flickbot, Smooth);
		ReadFloat(Flickbot, FlickBackSmooth);
		// Weapon Toggles
		// Light
		ReadBool(Flickbot, P2020);
		ReadBool(Flickbot, RE45);
		ReadBool(Flickbot, Alternator);
		ReadBool(Flickbot, R99);
		ReadBool(Flickbot, R301);
		ReadBool(Flickbot, Spitfire);
		ReadBool(Flickbot, G7);
		// Heavy
		ReadBool(Flickbot, Flatline);
		ReadBool(Flickbot, Hemlock);
		ReadBool(Flickbot, Repeater);
		ReadBool(Flickbot, Rampage);
		ReadBool(Flickbot, CARSMG);
		// Energy
		ReadBool(Flickbot, Havoc);
		ReadBool(Flickbot, Devotion);
		ReadBool(Flickbot, LSTAR);
		ReadBool(Flickbot, TripleTake);
		ReadBool(Flickbot, Volt);
		ReadBool(Flickbot, Nemesis);
		// Shotgun
		ReadBool(Flickbot, Mozambique);
		ReadBool(Flickbot, EVA8);
		ReadBool(Flickbot, Peacekeeper);
		ReadBool(Flickbot, Mastiff);
		// Snipers
		ReadBool(Flickbot, Longbow);
		ReadBool(Flickbot, ChargeRifle);
		ReadBool(Flickbot, Sentinel);
		// Legendary
		ReadBool(Flickbot, Wingman);
		ReadBool(Flickbot, Prowler);
		ReadBool(Flickbot, Bocek);
		ReadBool(Flickbot, Kraber);
		ReadBool(Flickbot, Knife);

		ReadBool(RCS, RCSEnabled);
		ReadInt(RCS, RCSMode);

		ReadBool(RCS, OnADS);

		ReadFloat(RCS, PitchPower);
		ReadFloat(RCS, YawPower);
		ReadFloat(RCS, PitchReduction);
		ReadFloat(RCS, YawReduction);

		// Weapon Toggles
		// Light
		ReadBool(RCS, P2020);
		ReadBool(RCS, RE45);
		ReadBool(RCS, Alternator);
		ReadBool(RCS, R99);
		ReadBool(RCS, R301);
		ReadBool(RCS, Spitfire);
		ReadBool(RCS, G7);

		// Heavy
		ReadBool(RCS, Flatline);
		ReadBool(RCS, Hemlock);
		ReadBool(RCS, Repeater);
		ReadBool(RCS, Rampage);
		ReadBool(RCS, CARSMG);

		// Energy
		ReadBool(RCS, Havoc);
		ReadBool(RCS, Devotion);
		ReadBool(RCS, LSTAR);
		ReadBool(RCS, TripleTake);
		ReadBool(RCS, Volt);
		ReadBool(RCS, Nemesis);

		// Shotgun
		ReadBool(RCS, Mozambique);
		ReadBool(RCS, EVA8);
		ReadBool(RCS, Peacekeeper);
		ReadBool(RCS, Mastiff);

		// Snipers
		ReadBool(RCS, Longbow);
		ReadBool(RCS, ChargeRifle);
		ReadBool(RCS, Sentinel);

		// Legendary
		ReadBool(RCS, Wingman); // Emotional damage!
		ReadBool(RCS, Prowler);
		ReadBool(RCS, Kraber);

		//---------------Advanced---------------//
		ReadBool(RCS, AdvancedRCS);
		ReadFloat(RCS, AdvancedPitchPower);
		ReadFloat(RCS, AdvancedYawPower);
		ReadFloat(RCS, AdvancedPitchReduction);
		ReadFloat(RCS, AdvancedYawReduction);

		// Weapons
		// Light
		ReadFloat(RCS, P2020Pitch);
		ReadFloat(RCS, P2020Yaw);
		ReadFloat(RCS, P2020PitchReduction);
		ReadFloat(RCS, P2020YawReduction);
		ReadFloat(RCS, RE45Pitch);
		ReadFloat(RCS, RE45Yaw);
		ReadFloat(RCS, RE45PitchReduction);
		ReadFloat(RCS, RE45YawReduction);
		ReadFloat(RCS, AlternatorPitch);
		ReadFloat(RCS, AlternatorYaw);
		ReadFloat(RCS, AlternatorPitchReduction);
		ReadFloat(RCS, AlternatorYawReduction);
		ReadFloat(RCS, R99Pitch);
		ReadFloat(RCS, R99Yaw);
		ReadFloat(RCS, R99PitchReduction);
		ReadFloat(RCS, R99YawReduction);
		ReadFloat(RCS, R301Pitch);
		ReadFloat(RCS, R301Yaw);
		ReadFloat(RCS, R301PitchReduction);
		ReadFloat(RCS, R301YawReduction);
		ReadFloat(RCS, SpitfirePitch);
		ReadFloat(RCS, SpitfireYaw);
		ReadFloat(RCS, SpitfirePitchReduction);
		ReadFloat(RCS, SpitfireYawReduction);
		ReadFloat(RCS, G7Pitch);
		ReadFloat(RCS, G7Yaw);
		ReadFloat(RCS, G7PitchReduction);
		ReadFloat(RCS, G7YawReduction);
		// Heavy
		ReadFloat(RCS, FlatlinePitch);
		ReadFloat(RCS, FlatlineYaw);
		ReadFloat(RCS, FlatlinePitchReduction);
		ReadFloat(RCS, FlatlineYawReduction);
		ReadFloat(RCS, HemlockPitch);
		ReadFloat(RCS, HemlockYaw);
		ReadFloat(RCS, HemlockPitchReduction);
		ReadFloat(RCS, HemlockYawReduction);
		ReadFloat(RCS, RepeaterPitch);
		ReadFloat(RCS, RepeaterYaw);
		ReadFloat(RCS, RepeaterPitchReduction);
		ReadFloat(RCS, RepeaterYawReduction);
		ReadFloat(RCS, RampagePitch);
		ReadFloat(RCS, RampageYaw);
		ReadFloat(RCS, RampagePitchReduction);
		ReadFloat(RCS, RampageYawReduction);
		ReadFloat(RCS, CARSMGPitch);
		ReadFloat(RCS, CARSMGYaw);
		ReadFloat(RCS, CARSMGPitchReduction);
		ReadFloat(RCS, CARSMGYawReduction);
		// Energy
		ReadFloat(RCS, HavocPitch);
		ReadFloat(RCS, HavocYaw);
		ReadFloat(RCS, HavocPitchReduction);
		ReadFloat(RCS, HavocYawReduction);
		ReadFloat(RCS, DevotionPitch);
		ReadFloat(RCS, DevotionYaw);
		ReadFloat(RCS, DevotionPitchReduction);
		ReadFloat(RCS, DevotionYawReduction);
		ReadFloat(RCS, LSTARPitch);
		ReadFloat(RCS, LSTARYaw);
		ReadFloat(RCS, LSTARPitchReduction);
		ReadFloat(RCS, LSTARYawReduction);
		ReadFloat(RCS, TripleTakePitch);
		ReadFloat(RCS, TripleTakeYaw);
		ReadFloat(RCS, TripleTakePitchReduction);
		ReadFloat(RCS, TripleTakeYawReduction);
		ReadFloat(RCS, VoltPitch);
		ReadFloat(RCS, VoltYaw);
		ReadFloat(RCS, VoltPitchReduction);
		ReadFloat(RCS, VoltYawReduction);
		ReadFloat(RCS, NemesisPitch);
		ReadFloat(RCS, NemesisYaw);
		ReadFloat(RCS, NemesisPitchReduction);
		ReadFloat(RCS, NemesisYawReduction);
		// Shotguns
		ReadFloat(RCS, MozambiquePitch);
		ReadFloat(RCS, MozambiqueYaw);
		ReadFloat(RCS, MozambiquePitchReduction);
		ReadFloat(RCS, MozambiqueYawReduction);
		ReadFloat(RCS, EVA8Pitch);
		ReadFloat(RCS, EVA8Yaw);
		ReadFloat(RCS, EVA8PitchReduction);
		ReadFloat(RCS, EVA8YawReduction);
		ReadFloat(RCS, PeacekeeperPitch);
		ReadFloat(RCS, PeacekeeperYaw);
		ReadFloat(RCS, PeacekeeperPitchReduction);
		ReadFloat(RCS, PeacekeeperYawReduction);
		ReadFloat(RCS, MastiffPitch);
		ReadFloat(RCS, MastiffYaw);
		ReadFloat(RCS, MastiffPitchReduction);
		ReadFloat(RCS, MastiffYawReduction);
		// Snipers
		ReadFloat(RCS, LongbowPitch);
		ReadFloat(RCS, LongbowYaw);
		ReadFloat(RCS, LongbowPitchReduction);
		ReadFloat(RCS, LongbowYawReduction);
		ReadFloat(RCS, ChargeRiflePitch);
		ReadFloat(RCS, ChargeRifleYaw);
		ReadFloat(RCS, ChargeRiflePitchReduction);
		ReadFloat(RCS, ChargeRifleYawReduction);
		ReadFloat(RCS, SentinelPitch);
		ReadFloat(RCS, SentinelYaw);
		ReadFloat(RCS, SentinelPitchReduction);
		ReadFloat(RCS, SentinelYawReduction);
		// Legendary
		ReadFloat(RCS, WingmanPitch);
		ReadFloat(RCS, WingmanYaw);
		ReadFloat(RCS, WingmanPitchReduction);
		ReadFloat(RCS, WingmanYawReduction);
		ReadFloat(RCS, ProwlerPitch);
		ReadFloat(RCS, ProwlerYaw);
		ReadFloat(RCS, ProwlerPitchReduction);
		ReadFloat(RCS, ProwlerYawReduction);
		ReadFloat(RCS, KraberPitch);
		ReadFloat(RCS, KraberYaw);
		ReadFloat(RCS, KraberPitchReduction);
		ReadFloat(RCS, KraberYawReduction);

		ReadBool(Triggerbot, Enabled);
		ReadInt(Triggerbot, BindMethod);
		ReadInt(Triggerbot, TriggerBind);
		ReadBool(Triggerbot, OnADS);
		ReadBool(Triggerbot, HipfireShotguns);
		ReadInt(Triggerbot, Delay);
		ReadFloat(Triggerbot, Range);
		// Weapon Toggles
		// Light
		ReadBool(Triggerbot, P2020);
		ReadBool(Triggerbot, RE45);
		ReadBool(Triggerbot, Alternator);
		ReadBool(Triggerbot, R99);
		ReadBool(Triggerbot, R301);
		ReadBool(Triggerbot, Spitfire);
		ReadBool(Triggerbot, G7);
		// Heavy
		ReadBool(Triggerbot, Flatline);
		ReadBool(Triggerbot, Hemlock);
		ReadBool(Triggerbot, Repeater);
		ReadBool(Triggerbot, Rampage);
		ReadBool(Triggerbot, CARSMG);
		// Energy
		ReadBool(Triggerbot, Havoc);
		ReadBool(Triggerbot, Devotion);
		ReadBool(Triggerbot, LSTAR);
		ReadBool(Triggerbot, TripleTake);
		ReadBool(Triggerbot, Volt);
		ReadBool(Triggerbot, Nemesis);
		// Shotgun
		ReadBool(Triggerbot, Mozambique);
		ReadBool(Triggerbot, EVA8);
		ReadBool(Triggerbot, Peacekeeper);
		ReadBool(Triggerbot, Mastiff);
		// Snipers
		ReadBool(Triggerbot, Longbow);
		ReadBool(Triggerbot, ChargeRifle);
		ReadBool(Triggerbot, Sentinel);
		// Legendary
		ReadBool(Triggerbot, Wingman);
		ReadBool(Triggerbot, Prowler);
		ReadBool(Triggerbot, Bocek);
		ReadBool(Triggerbot, Kraber);
		ReadBool(Triggerbot, Knife);
		// Advanced Triggerbot
		ReadBool(Triggerbot, AdvancedTriggerbot);
		ReadFloat(Triggerbot, P2020Range);
		ReadFloat(Triggerbot, RE45Range);
		ReadFloat(Triggerbot, AlternatorRange);
		ReadFloat(Triggerbot, R99Range);
		ReadFloat(Triggerbot, R301Range);
		ReadFloat(Triggerbot, SpitfireRange);
		ReadFloat(Triggerbot, G7Range);
		ReadFloat(Triggerbot, FlatlineRange);
		ReadFloat(Triggerbot, HemlockRange);
		ReadFloat(Triggerbot, ProwlerRange);
		ReadFloat(Triggerbot, RepeaterRange);
		ReadFloat(Triggerbot, RampageRange);
		ReadFloat(Triggerbot, CARSMGRange);
		ReadFloat(Triggerbot, HavocRange);
		ReadFloat(Triggerbot, DevotionRange);
		ReadFloat(Triggerbot, LSTARRange);
		ReadFloat(Triggerbot, TripleTakeRange);
		ReadFloat(Triggerbot, VoltRange);
		ReadFloat(Triggerbot, NemesisRange);
		ReadFloat(Triggerbot, MozambiqueRange);
		ReadFloat(Triggerbot, PeacekeeperRange);
		ReadFloat(Triggerbot, MastiffRange);
		ReadFloat(Triggerbot, LongbowRange);
		ReadFloat(Triggerbot, ChargeRifleRange);
		ReadFloat(Triggerbot, SentinelRange);
		ReadFloat(Triggerbot, WingmanRange);
		ReadFloat(Triggerbot, EVA8Range);
		ReadFloat(Triggerbot, BocekRange);
		ReadFloat(Triggerbot, KraberRange);
		ReadFloat(Triggerbot, ThrowingKnifeRange);

		ReadBool(Glow, NewGlow);
		ReadFloat(Glow, GlowMaxDistance);
		ReadInt(Glow, GlowColorMode);
		ReadInt(Glow, GlowColorShieldMode);
		ReadInt(Glow, GlowRadius);
		ReadInt(Glow, InsideFunction);
		ReadInt(Glow, OutlineFunction);
		ReadInt(Glow, BodyStyle);
		ReadInt(Glow, OutlineStyle);
		ReadBool(Glow, ViewModelGlow);
		ReadInt(Glow, ViewModelGlowSetting);
		ReadInt(Glow, ViewModelGlowCombo);

		ReadBool(ItemGlow, ItemGlow);
		ReadInt(ItemGlow, SelectedItemSelection);
		ReadBool(ItemGlow, Common);
		ReadBool(ItemGlow, Rare);
		ReadBool(ItemGlow, Epic);
		ReadBool(ItemGlow, Gold);
		ReadBool(ItemGlow, Legendary);
		ReadBool(ItemGlow, Weapons);
		ReadBool(ItemGlow, Ammo);
		ReadInt(ItemGlow, ItemGlowThickness);
		ReadInt(ItemGlow, SelectedInsideStyle);
		ReadInt(ItemGlow, SelectedOutlineStyle);

		ReadBool(Sense, Enabled);
		// Aimbot
		ReadBool(Sense, DrawFOVCircle);
		ReadBool(Sense, DrawFilledFOVCircle);
		ReadFloat(Sense, FOVThickness);
		ReadFloat(Sense, GameFOV);
		// Other
		ReadBool(Sense, DrawCrosshair);
		ReadFloat(Sense, CrosshairSize);
		ReadFloat(Sense, CrosshairThickness);
		ReadBool(Sense, ShowSpectators);
		ReadBool(Sense, DrawSpectatorWarning);
		ReadBool(Sense, DrawVisibleWarning);
		ReadBool(Sense, WarningTextOutline);
		ReadInt(Sense, WarningTextX);
		ReadInt(Sense, WarningTextY);
		// Settings
		ReadBool(Sense, TextOutline);
		ReadBool(Sense, VisibilityCheck);
		ReadFloat(Sense, ESPMaxDistance);

		ReadBool(SenseEnemy, DrawEnemy);
		ReadBool(SenseEnemy, DrawBoxes);
		ReadInt(SenseEnemy, BoxType);
		ReadInt(SenseEnemy, BoxStyle);
		ReadFloat(SenseEnemy, BoxThickness);
		ReadBool(SenseEnemy, DrawSkeleton);
		ReadFloat(SenseEnemy, SkeletonThickness);		
		ReadBool(SenseEnemy, DrawHeadCircle);
		ReadFloat(SenseEnemy, HeadCircleThickness);
		ReadBool(SenseEnemy, DrawBars);
		ReadBool(SenseEnemy, HealthBar);
		ReadBool(SenseEnemy, ShieldBar);
		ReadInt(SenseEnemy, BarMode);
		ReadInt(SenseEnemy, BarStyle);
		ReadFloat(SenseEnemy, BarThickness);
		ReadFloat(SenseEnemy, BarThickness2);
		ReadFloat(SenseEnemy, BarHeight);
		ReadFloat(SenseEnemy, BarWidth);
		ReadInt(SenseEnemy, BarColorMode);
		ReadInt(SenseEnemy, WeaponColorType);
		ReadBool(SenseEnemy, DrawDistance);
		ReadInt(SenseEnemy, TracerPosition);
		ReadInt(SenseEnemy, TracerBone);
		ReadBool(SenseEnemy, DrawTracers);
		ReadFloat(SenseEnemy, TracerThickness);
		ReadBool(SenseEnemy, DrawNames);
		ReadBool(SenseEnemy, DrawSeer);
		ReadBool(SenseEnemy, DrawStatus);
		ReadBool(SenseEnemy, DrawWeapon);
		ReadBool(SenseEnemy, DrawLegend);
		ReadBool(SenseEnemy, ShowMaxStatusValues);

		ReadBool(SenseTeammate, DrawTeam);
		ReadBool(SenseTeammate, DrawBoxes);
		ReadInt(SenseTeammate, BoxType);
		ReadInt(SenseTeammate, BoxStyle);
		ReadFloat(SenseTeammate, BoxThickness);
		ReadBool(SenseTeammate, DrawSkeleton);
		ReadFloat(SenseTeammate, SkeletonThickness);
		ReadBool(SenseTeammate, DrawHeadCircle);
		ReadFloat(SenseTeammate, HeadCircleThickness);
		ReadBool(SenseTeammate, DrawBars);
		ReadBool(SenseTeammate, HealthBar);
		ReadBool(SenseTeammate, ShieldBar);
		ReadInt(SenseTeammate, BarMode);
		ReadInt(SenseTeammate, BarStyle);
		ReadFloat(SenseTeammate, BarThickness);
		ReadFloat(SenseTeammate, BarThickness2);
		ReadFloat(SenseTeammate, BarHeight);
		ReadFloat(SenseTeammate, BarWidth);
		ReadInt(SenseTeammate, BarColorMode);
		ReadBool(SenseTeammate, BarBackground);
		ReadInt(SenseTeammate, WeaponColorType);
		ReadBool(SenseTeammate, DrawDistance);
		ReadInt(SenseTeammate, TracerPosition);
		ReadInt(SenseTeammate, TracerBone);
		ReadBool(SenseTeammate, DrawTracers);
		ReadFloat(SenseTeammate, TracerThickness);
		ReadBool(SenseTeammate, DrawNames);
		ReadBool(SenseTeammate, DrawSeer);
		ReadBool(SenseTeammate, DrawStatus);
		ReadBool(SenseTeammate, DrawWeapon);
		ReadBool(SenseTeammate, DrawLegend);
		ReadBool(SenseTeammate, ShowMaxStatusValues);

		ReadInt(SensePositions, NamePosition);
		ReadInt(SensePositions, DistancePosition);
		ReadInt(SensePositions, LegendPosition);
		ReadInt(SensePositions, WeaponPosition);
		ReadInt(SensePositions, StatusPosition);

		ReadBool(Radar, MiniMap);
		ReadFloat(Radar, MiniMapRange);
		ReadInt(Radar, MiniMapScaleX);
		ReadInt(Radar, MiniMapScaleY);
		ReadInt(Radar, MiniMapDotSize);
		ReadInt(Radar, MiniMapBlackBGSize);
		ReadBool(Radar, MiniMapGuides);
		ReadBool(Radar, BigMap);
		ReadInt(Radar, BigMapBind);
		ReadFloat(Radar, CircleColorR);
		ReadFloat(Radar, CircleColorG);
		ReadFloat(Radar, CircleColorB);
		ReadFloat(Radar, CircleColorA);

		ReadBool(Misc, SuperGlide);
		ReadBool(Misc, QuickTurn);
		ReadInt(Misc, QuickTurnAngle);
		ReadInt(Misc, QuickTurnBind);
		ReadBool(Misc, BHop);
		ReadInt(Misc, BHopDelay);
		ReadInt(Misc, BHopBind);
		ReadBool(Misc, RapidFire);
		ReadInt(Misc, RapidFireDelay);
		ReadInt(Misc, RapidFireBind);
		// Rapid Fire Weapon Toggles
		// Light
		ReadBool(Misc, RapidP2020);
		ReadBool(Misc, RapidR301);
		ReadBool(Misc, RapidG7);
		// Heavy
		ReadBool(Misc, RapidFlatline);
		ReadBool(Misc, RapidHemlock);
		ReadBool(Misc, RapidProwler);
		// Energy
		ReadBool(Misc, RapidNemesis);
		// Shotgun
		ReadBool(Misc, RapidMozambique);
		ReadBool(Misc, RapidEVA8);
		// Legendary
		ReadBool(Misc, RapidWingman);
		ReadBool(Misc, TeamGamemode);
		ReadBool(Misc, SkinChanger);
		// Skin IDs
		// Light
		ReadInt(Misc, SkinP2020);
		ReadInt(Misc, SkinRE45);
		ReadInt(Misc, SkinALTERNATOR);
		ReadInt(Misc, SkinR99);
		ReadInt(Misc, SkinR301);
		ReadInt(Misc, SkinSPITFIRE);
		ReadInt(Misc, SkinG7);
		// Heavy
		ReadInt(Misc, SkinFLATLINE);
		ReadInt(Misc, SkinHEMLOCK);
		ReadInt(Misc, SkinREPEATER);
		ReadInt(Misc, SkinRAMPAGE);
		ReadInt(Misc, SkinCAR);
		// Energy
		ReadInt(Misc, SkinHAVOC);
		ReadInt(Misc, SkinDEVOTION);
		ReadInt(Misc, SkinLSTAR);
		ReadInt(Misc, SkinTRIPLETAKE);
		ReadInt(Misc, SkinVOLT);
		ReadInt(Misc, SkinNEMESIS);
		// Shotgun
		ReadInt(Misc, SkinMOZAMBIQUE);
		ReadInt(Misc, SkinEVA8);
		ReadInt(Misc, SkinPEACEKEEPER);
		ReadInt(Misc, SkinMASTIFF);
		// Snipers
		ReadInt(Misc, SkinLONGBOW);
		ReadInt(Misc, SkinCHARGE_RIFLE);
		ReadInt(Misc, SkinSENTINEL);
		// Legendary
		ReadInt(Misc, SkinWINGMAN);
		ReadInt(Misc, SkinPROWLER);
		ReadInt(Misc, SkinBOCEK);
		ReadInt(Misc, SkinKRABER);

		ReadInt(Colors, WeaponColorMode);
		ReadFloat(Colors, FOVColorR);
		ReadFloat(Colors, FOVColorG);
		ReadFloat(Colors, FOVColorB);
		ReadFloat(Colors, FOVColorA);
		ReadFloat(Colors, FilledFOVColorR);
		ReadFloat(Colors, FilledFOVColorG);
		ReadFloat(Colors, FilledFOVColorB);
		ReadFloat(Colors, FilledFOVColorA);
		ReadFloat(Colors, CrosshairColorR);
		ReadFloat(Colors, CrosshairColorG);
		ReadFloat(Colors, CrosshairColorB);
		ReadFloat(Colors, CrosshairColorA);
		ReadFloat(Colors, SpectatorWarningColorR);
		ReadFloat(Colors, SpectatorWarningColorG);
		ReadFloat(Colors, SpectatorWarningColorB);
		ReadFloat(Colors, SpectatorWarningColorA);
		ReadFloat(Colors, VisibleWarningColorR);
		ReadFloat(Colors, VisibleWarningColorG);
		ReadFloat(Colors, VisibleWarningColorB);
		ReadFloat(Colors, VisibleWarningColorA);

		ReadFloat(EnemyColors, InvisibleBoxColorR);
		ReadFloat(EnemyColors, InvisibleBoxColorG);
		ReadFloat(EnemyColors, InvisibleBoxColorB);
		ReadFloat(EnemyColors, InvisibleBoxColorA);
		ReadFloat(EnemyColors, VisibleBoxColorR);
		ReadFloat(EnemyColors, VisibleBoxColorG);
		ReadFloat(EnemyColors, VisibleBoxColorB);
		ReadFloat(EnemyColors, VisibleBoxColorA);
		ReadFloat(EnemyColors, InvisibleFilledBoxColorR);
		ReadFloat(EnemyColors, InvisibleFilledBoxColorG);
		ReadFloat(EnemyColors, InvisibleFilledBoxColorB);
		ReadFloat(EnemyColors, InvisibleFilledBoxColorA);
		ReadFloat(EnemyColors, VisibleFilledBoxColorR);
		ReadFloat(EnemyColors, VisibleFilledBoxColorG);
		ReadFloat(EnemyColors, VisibleFilledBoxColorB);
		ReadFloat(EnemyColors, VisibleFilledBoxColorA);
		ReadFloat(EnemyColors, InvisibleTracerColorR);
		ReadFloat(EnemyColors, InvisibleTracerColorG);
		ReadFloat(EnemyColors, InvisibleTracerColorB);
		ReadFloat(EnemyColors, InvisibleTracerColorA);
		ReadFloat(EnemyColors, VisibleTracerColorR);
		ReadFloat(EnemyColors, VisibleTracerColorG);
		ReadFloat(EnemyColors, VisibleTracerColorB);
		ReadFloat(EnemyColors, VisibleTracerColorA);
		ReadFloat(EnemyColors, InvisibleSkeletonColorR);
		ReadFloat(EnemyColors, InvisibleSkeletonColorG);
		ReadFloat(EnemyColors, InvisibleSkeletonColorB);
		ReadFloat(EnemyColors, InvisibleSkeletonColorA);
		ReadFloat(EnemyColors, VisibleSkeletonColorR);
		ReadFloat(EnemyColors, VisibleSkeletonColorG);
		ReadFloat(EnemyColors, VisibleSkeletonColorB);
		ReadFloat(EnemyColors, VisibleSkeletonColorA);
		ReadFloat(EnemyColors, InvisibleHeadCircleColorR);
		ReadFloat(EnemyColors, InvisibleHeadCircleColorG);
		ReadFloat(EnemyColors, InvisibleHeadCircleColorB);
		ReadFloat(EnemyColors, InvisibleHeadCircleColorA);
		ReadFloat(EnemyColors, VisibleHeadCircleColorR);
		ReadFloat(EnemyColors, VisibleHeadCircleColorG);
		ReadFloat(EnemyColors, VisibleHeadCircleColorB);
		ReadFloat(EnemyColors, VisibleHeadCircleColorA);
		ReadFloat(EnemyColors, InvisibleNameColorR);
		ReadFloat(EnemyColors, InvisibleNameColorG);
		ReadFloat(EnemyColors, InvisibleNameColorB);
		ReadFloat(EnemyColors, InvisibleNameColorA);
		ReadFloat(EnemyColors, VisibleNameColorR);
		ReadFloat(EnemyColors, VisibleNameColorG);
		ReadFloat(EnemyColors, VisibleNameColorB);
		ReadFloat(EnemyColors, VisibleNameColorA);
		ReadFloat(EnemyColors, InvisibleDistanceColorR);
		ReadFloat(EnemyColors, InvisibleDistanceColorG);
		ReadFloat(EnemyColors, InvisibleDistanceColorB);
		ReadFloat(EnemyColors, InvisibleDistanceColorA);
		ReadFloat(EnemyColors, VisibleDistanceColorR);
		ReadFloat(EnemyColors, VisibleDistanceColorG);
		ReadFloat(EnemyColors, VisibleDistanceColorB);
		ReadFloat(EnemyColors, VisibleDistanceColorA);
		ReadFloat(EnemyColors, InvisibleLegendColorR);
		ReadFloat(EnemyColors, InvisibleLegendColorG);
		ReadFloat(EnemyColors, InvisibleLegendColorB);
		ReadFloat(EnemyColors, InvisibleLegendColorA);
		ReadFloat(EnemyColors, VisibleLegendColorR);
		ReadFloat(EnemyColors, VisibleLegendColorG);
		ReadFloat(EnemyColors, VisibleLegendColorB);
		ReadFloat(EnemyColors, VisibleLegendColorA);
		// Bar
		ReadInt(EnemyColors, BarColorMode);
		// WeaponESP Colors
		ReadFloat(EnemyColors, InvisibleWeaponColorR);
		ReadFloat(EnemyColors, InvisibleWeaponColorG);
		ReadFloat(EnemyColors, InvisibleWeaponColorB);
		ReadFloat(EnemyColors, InvisibleWeaponColorA);
		ReadFloat(EnemyColors, VisibleWeaponColorR);
		ReadFloat(EnemyColors, VisibleWeaponColorG);
		ReadFloat(EnemyColors, VisibleWeaponColorB);
		ReadFloat(EnemyColors, VisibleWeaponColorA);
		// Multiple
		ReadFloat(EnemyColors, LightWeaponColorR);
		ReadFloat(EnemyColors, LightWeaponColorG);
		ReadFloat(EnemyColors, LightWeaponColorB);
		ReadFloat(EnemyColors, LightWeaponColorA);
		ReadFloat(EnemyColors, HeavyWeaponColorR);
		ReadFloat(EnemyColors, HeavyWeaponColorG);
		ReadFloat(EnemyColors, HeavyWeaponColorB);
		ReadFloat(EnemyColors, HeavyWeaponColorA);
		ReadFloat(EnemyColors, EnergyWeaponColorR);
		ReadFloat(EnemyColors, EnergyWeaponColorG);
		ReadFloat(EnemyColors, EnergyWeaponColorB);
		ReadFloat(EnemyColors, EnergyWeaponColorA);
		ReadFloat(EnemyColors, ShotgunWeaponColorR);
		ReadFloat(EnemyColors, ShotgunWeaponColorG);
		ReadFloat(EnemyColors, ShotgunWeaponColorB);
		ReadFloat(EnemyColors, ShotgunWeaponColorA);
		ReadFloat(EnemyColors, SniperWeaponColorR);
		ReadFloat(EnemyColors, SniperWeaponColorG);
		ReadFloat(EnemyColors, SniperWeaponColorB);
		ReadFloat(EnemyColors, SniperWeaponColorA);
		ReadFloat(EnemyColors, LegendaryWeaponColorR);
		ReadFloat(EnemyColors, LegendaryWeaponColorG);
		ReadFloat(EnemyColors, LegendaryWeaponColorB);
		ReadFloat(EnemyColors, LegendaryWeaponColorA);
		ReadFloat(EnemyColors, MeleeWeaponColorR);
		ReadFloat(EnemyColors, MeleeWeaponColorG);
		ReadFloat(EnemyColors, MeleeWeaponColorB);
		ReadFloat(EnemyColors, MeleeWeaponColorA);
		ReadFloat(EnemyColors, ThrowableWeaponColorR);
		ReadFloat(EnemyColors, ThrowableWeaponColorG);
		ReadFloat(EnemyColors, ThrowableWeaponColorB);
		ReadFloat(EnemyColors, ThrowableWeaponColorA);
		// Glow
		ReadFloat(EnemyColors, InvisibleGlowColorR);
		ReadFloat(EnemyColors, InvisibleGlowColorG);
		ReadFloat(EnemyColors, InvisibleGlowColorB);
		ReadFloat(EnemyColors, VisibleGlowColorR);
		ReadFloat(EnemyColors, VisibleGlowColorG);
		ReadFloat(EnemyColors, VisibleGlowColorB);
		ReadFloat(EnemyColors, RedShieldColorR);
		ReadFloat(EnemyColors, RedShieldColorG);
		ReadFloat(EnemyColors, RedShieldColorG);
		ReadFloat(EnemyColors, PurpleShieldColorR);
		ReadFloat(EnemyColors, PurpleShieldColorG);
		ReadFloat(EnemyColors, PurpleShieldColorB);
		ReadFloat(EnemyColors, BlueShieldColorR);
		ReadFloat(EnemyColors, BlueShieldColorG);
		ReadFloat(EnemyColors, BlueShieldColorB);
		ReadFloat(EnemyColors, GreyShieldColorR);
		ReadFloat(EnemyColors, GreyShieldColorG);
		ReadFloat(EnemyColors, GreyShieldColorB);

		ReadFloat(TeammateColors, InvisibleBoxColorR);
		ReadFloat(TeammateColors, InvisibleBoxColorG);
		ReadFloat(TeammateColors, InvisibleBoxColorB);
		ReadFloat(TeammateColors, InvisibleBoxColorA);
		ReadFloat(TeammateColors, VisibleBoxColorR);
		ReadFloat(TeammateColors, VisibleBoxColorG);
		ReadFloat(TeammateColors, VisibleBoxColorB);
		ReadFloat(TeammateColors, VisibleBoxColorA);
		ReadFloat(TeammateColors, InvisibleFilledBoxColorR);
		ReadFloat(TeammateColors, InvisibleFilledBoxColorG);
		ReadFloat(TeammateColors, InvisibleFilledBoxColorB);
		ReadFloat(TeammateColors, InvisibleFilledBoxColorA);
		ReadFloat(TeammateColors, VisibleFilledBoxColorR);
		ReadFloat(TeammateColors, VisibleFilledBoxColorG);
		ReadFloat(TeammateColors, VisibleFilledBoxColorB);
		ReadFloat(TeammateColors, VisibleFilledBoxColorA);
		ReadFloat(TeammateColors, InvisibleTracerColorR);
		ReadFloat(TeammateColors, InvisibleTracerColorG);
		ReadFloat(TeammateColors, InvisibleTracerColorB);
		ReadFloat(TeammateColors, InvisibleTracerColorA);
		ReadFloat(TeammateColors, VisibleTracerColorR);
		ReadFloat(TeammateColors, VisibleTracerColorG);
		ReadFloat(TeammateColors, VisibleTracerColorB);
		ReadFloat(TeammateColors, VisibleTracerColorA);
		ReadFloat(TeammateColors, InvisibleSkeletonColorR);
		ReadFloat(TeammateColors, InvisibleSkeletonColorG);
		ReadFloat(TeammateColors, InvisibleSkeletonColorB);
		ReadFloat(TeammateColors, InvisibleSkeletonColorA);
		ReadFloat(TeammateColors, VisibleSkeletonColorR);
		ReadFloat(TeammateColors, VisibleSkeletonColorG);
		ReadFloat(TeammateColors, VisibleSkeletonColorB);
		ReadFloat(TeammateColors, VisibleSkeletonColorA);
		ReadFloat(TeammateColors, InvisibleHeadCircleColorR);
		ReadFloat(TeammateColors, InvisibleHeadCircleColorG);
		ReadFloat(TeammateColors, InvisibleHeadCircleColorB);
		ReadFloat(TeammateColors, InvisibleHeadCircleColorA);
		ReadFloat(TeammateColors, VisibleHeadCircleColorR);
		ReadFloat(TeammateColors, VisibleHeadCircleColorG);
		ReadFloat(TeammateColors, VisibleHeadCircleColorB);
		ReadFloat(TeammateColors, VisibleHeadCircleColorA);
		ReadFloat(TeammateColors, InvisibleNameColorR);
		ReadFloat(TeammateColors, InvisibleNameColorG);
		ReadFloat(TeammateColors, InvisibleNameColorB);
		ReadFloat(TeammateColors, InvisibleNameColorA);
		ReadFloat(TeammateColors, VisibleNameColorR);
		ReadFloat(TeammateColors, VisibleNameColorG);
		ReadFloat(TeammateColors, VisibleNameColorB);
		ReadFloat(TeammateColors, VisibleNameColorA);
		ReadFloat(TeammateColors, InvisibleDistanceColorR);
		ReadFloat(TeammateColors, InvisibleDistanceColorG);
		ReadFloat(TeammateColors, InvisibleDistanceColorB);
		ReadFloat(TeammateColors, InvisibleDistanceColorA);
		ReadFloat(TeammateColors, VisibleDistanceColorR);
		ReadFloat(TeammateColors, VisibleDistanceColorG);
		ReadFloat(TeammateColors, VisibleDistanceColorB);
		ReadFloat(TeammateColors, VisibleDistanceColorA);
		ReadFloat(TeammateColors, InvisibleLegendColorR);
		ReadFloat(TeammateColors, InvisibleLegendColorG);
		ReadFloat(TeammateColors, InvisibleLegendColorB);
		ReadFloat(TeammateColors, InvisibleLegendColorA);
		ReadFloat(TeammateColors, VisibleLegendColorR);
		ReadFloat(TeammateColors, VisibleLegendColorG);
		ReadFloat(TeammateColors, VisibleLegendColorB);
		ReadFloat(TeammateColors, VisibleLegendColorA);
		// Bar
		ReadInt(TeammateColors, BarColorMode);
		// WeaponESP Colors
		ReadFloat(TeammateColors, InvisibleWeaponColorR);
		ReadFloat(TeammateColors, InvisibleWeaponColorG);
		ReadFloat(TeammateColors, InvisibleWeaponColorB);
		ReadFloat(TeammateColors, InvisibleWeaponColorA);
		ReadFloat(TeammateColors, VisibleWeaponColorR);
		ReadFloat(TeammateColors, VisibleWeaponColorG);
		ReadFloat(TeammateColors, VisibleWeaponColorB);
		ReadFloat(TeammateColors, VisibleWeaponColorA);
		// Multiple
		ReadFloat(TeammateColors, LightWeaponColorR);
		ReadFloat(TeammateColors, LightWeaponColorG);
		ReadFloat(TeammateColors, LightWeaponColorB);
		ReadFloat(TeammateColors, LightWeaponColorA);
		ReadFloat(TeammateColors, HeavyWeaponColorR);
		ReadFloat(TeammateColors, HeavyWeaponColorG);
		ReadFloat(TeammateColors, HeavyWeaponColorB);
		ReadFloat(TeammateColors, HeavyWeaponColorA);
		ReadFloat(TeammateColors, EnergyWeaponColorR);
		ReadFloat(TeammateColors, EnergyWeaponColorG);
		ReadFloat(TeammateColors, EnergyWeaponColorB);
		ReadFloat(TeammateColors, EnergyWeaponColorA);
		ReadFloat(TeammateColors, ShotgunWeaponColorR);
		ReadFloat(TeammateColors, ShotgunWeaponColorG);
		ReadFloat(TeammateColors, ShotgunWeaponColorB);
		ReadFloat(TeammateColors, ShotgunWeaponColorA);
		ReadFloat(TeammateColors, SniperWeaponColorR);
		ReadFloat(TeammateColors, SniperWeaponColorG);
		ReadFloat(TeammateColors, SniperWeaponColorB);
		ReadFloat(TeammateColors, SniperWeaponColorA);
		ReadFloat(TeammateColors, LegendaryWeaponColorR);
		ReadFloat(TeammateColors, LegendaryWeaponColorG);
		ReadFloat(TeammateColors, LegendaryWeaponColorB);
		ReadFloat(TeammateColors, LegendaryWeaponColorA);
		ReadFloat(TeammateColors, MeleeWeaponColorR);
		ReadFloat(TeammateColors, MeleeWeaponColorG);
		ReadFloat(TeammateColors, MeleeWeaponColorB);
		ReadFloat(TeammateColors, MeleeWeaponColorA);
		ReadFloat(TeammateColors, ThrowableWeaponColorR);
		ReadFloat(TeammateColors, ThrowableWeaponColorG);
		ReadFloat(TeammateColors, ThrowableWeaponColorB);
		ReadFloat(TeammateColors, ThrowableWeaponColorA);

		ReadBool(Watermark, Watermark);
		ReadInt(Watermark, WatermarkPosition);
		ReadBool(Watermark, Name);
		ReadBool(Watermark, ProcessingSpeed);
		ReadBool(Watermark, Spectators);

		ReadBool(Settings, GamemodeCheck);
		ReadBool(Settings, ESPEnabled);
		ReadBool(Settings, OverlayEnabled);
		ReadBool(Settings, FPSCap);
		ReadInt(Settings, CappedFPS);

		UpdateConfig();
    	return true;
	}
};
