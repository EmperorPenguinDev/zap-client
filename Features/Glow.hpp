#pragma once
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <iostream>
#include <vector>
#include <unistd.h>

typedef unsigned long DWORD;
typedef int *DWORD_PTR;

#include "../Core/Player.hpp"
#include "../Core/LocalPlayer.hpp"
#include "../Core/Offsets.hpp"
#include "../Core/GlowMode.hpp"
#include "../Core/Camera.hpp"
#include "../Core/Level.hpp"

#include "../Features/Legitbot.hpp"

#include "../Overlay/Overlay.hpp"
#include "../Overlay/Renderer.hpp"

#include "../Utils/Memory.hpp"
#include "../Utils/Color.hpp"
#include "../Utils/Conversion.hpp"
#include "../Utils/Config.hpp"
#include "../Utils/Features.hpp"
#include "../Utils/HitboxType.hpp"

// UI //
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"

// Geometry
#define DEG2RAD(x) ((float)(x) * (float)(M_PI / 180.f))
#define ABS(x) ((x < 0) ? (-x) : (x))

/*Viewmodel Glow Settings (OLD)
0, 2, 5 = Off (?)
1 = Pulsing Red + Red Outline
3 = Pulsing Cyan + Cyan Outline
4 = Fast Pulsing White + White Outline (?)
13 = Cyan Outline
15 = Light Red Outline
16 = WTF GREEN
24 = Yellow Outline
25 = Yellow Full Pulsing
27 = Solid Green
28 = Solid Orange
29 = Solid Yellow/Gold
30 = Solid Purple
31 = Solid Light Blue
32 = Solid White
47 = Solid Blue With Light Pulsing
49 = Blinding White
53 = Pulsing Red + Pulsing Red Outline
54 = Pulsing Whie + Pulsing Whie Outline
61 = Pulsing Orange
62 = Pulsing Orange Outline
64 = Faded Orange With Pulsing Line
65 = Green Outline
66 = Faded White With Pulsing Line
67 = Faded Red With Pulsing Line
68 = Faded Turquoise With Pulsing Line
70 = Red Outline
73 = Faded Yellow/Gold With Pulsing Line
74 = Solid Black
76 = Solid Pink
82 = Solid Pink With Red Outline
65 = Invisible with orange outline pulsing
93 = Invisible Red Pulsing
120 = Invisible
130 = Invisible With Pink Outline

Viewmodel Glow Settings (NEW)
0, 2, 5 = Off (?)
13 = Cyan Outline
15 = Light Red Outline
17 = White Outline
18 = Orange Outline
24 = Yellow Outline
25 = Solid Yellow Pulsing
27 = Solid Green
28 = Solid Orange
29 = Solid Yellow/Gold
30 = Solid Purple
31 = Solid Light Blue
32 = Solid Light Grey
34 = Solid White
35 = Solid Cyan (Light Blue)
36 = Solid Hot Pink
37 = Solid Light Yellow
38 = Solid Light Orange
39 = Solid Light Green
49 = Chrome
65 = Invisible Orange Pulsing
74 = Solid Black
93 = Invisible Red Pulsing
150 = Invisible Pink Outline


*/

/*
Item Glow Settings
Inside Function (First Value, not the outline of the object)
(May not apply for all objects, not sure)
If no color is clearly stated, then it depends on the color/rarity of the item
0 = clear
1 = pink
2 = lightly shaded
3 = fast fade from top to bottom, color is red on some and green on some
10 = invisible, disables item glow pretty much
12 = lightly shaded with lines + fading from top to bottom
13 = fading from top to bottom
14 = solid color but with a weird black fade from top to bottom
75 = dark color, with a very light fade
103 = fade from a certain point on the map? from right to left
109 = solid color, quite "thick"
115 = light orange on some objects, shaded and fading effect
117 = lightly shaded fading in and out to solid

Outline Function (Second Value, Outline Color)
0, 10, 75, 77 = off
4 = idfk multicolored or some shit ugly af
5 = flashing gold
6, 125 = solid color
7 = gold, makes thickness cap at 120 ish so its thin
8 = orange glow
9 = striped orange glow
101 = fuzzy glow
102 = light outline
103 = pulsing
107 = red
108 = brighter red
110 = fading in and out
120 = soft, requires high thickness
131 = visible outline only
135 = Distance Based Outline
138 = Distance Based Outline, Visible Only
140 = Green outline through walls, also disables insidefunction glow if not set to solid
261 = flashing gold again

*/

struct Glow
{

    // Viewmodel Glow
    int ViewmodelGlowID;
    int ItemGlowInsideFunction = 0;    // Leave
    int ItemGlowOutlineFunction = 138; // Leave

    std::chrono::milliseconds LastLoopTime;

    std::vector<GlowMode> *StoredGlowMode = new std::vector<GlowMode>;

    // Variables
    Camera *GameCamera;
    Level *Map;
    LocalPlayer *Myself;
    std::vector<Player *> *Players;
    std::chrono::milliseconds LastUpdateTime;
    int TotalSpectators = 0;
    std::vector<std::string> Spectators;

    Glow(Level *Map, std::vector<Player *> *Players, Camera *GameCamera, LocalPlayer *Myself)
    {
        this->Players = Players;
        this->Map = Map;
        this->GameCamera = GameCamera;
        this->Myself = Myself;
    }

    bool Save()
    {
        try
        {
            
            Config::Glow::NewGlow = Features::Glow::NewGlow;
            Config::Glow::GlowMaxDistance = Features::Glow::GlowMaxDistance;
            Config::Glow::GlowColorMode = Features::Glow::GlowColorMode;
            Config::Glow::GlowColorShieldMode = Features::Glow::GlowColorShieldMode;
            Config::Glow::GlowRadius = Features::Glow::GlowRadius;
            Config::Glow::BodyStyle = Features::Glow::BodyStyle;
            Config::Glow::OutlineStyle = Features::Glow::OutlineStyle;
            Config::Glow::ViewModelGlow = Features::Glow::ViewModelGlow;
            Config::Glow::ViewModelGlowSetting = Features::Glow::ViewModelGlowSetting;
            Config::Glow::ViewModelGlowCombo = Features::Glow::ViewModelGlowCombo;

            Config::ItemGlow::ItemGlow = Features::Glow::Item::ItemGlow;
            Config::ItemGlow::SelectedItemSelection = Features::Glow::Item::SelectedItemSelection;
            Config::ItemGlow::Common = Features::Glow::Item::Common;
            Config::ItemGlow::Rare = Features::Glow::Item::Rare;
            Config::ItemGlow::Epic = Features::Glow::Item::Epic;
            Config::ItemGlow::Gold = Features::Glow::Item::Gold;
            Config::ItemGlow::Legendary = Features::Glow::Item::Legendary;
            Config::ItemGlow::Weapons = Features::Glow::Item::Weapons;
            Config::ItemGlow::Ammo = Features::Glow::Item::Ammo;
            Config::ItemGlow::ItemGlowThickness = Features::Glow::Item::ItemGlowThickness;
            Config::ItemGlow::SelectedInsideStyle = Features::Glow::Item::SelectedInsideStyle;
            Config::ItemGlow::SelectedOutlineStyle = Features::Glow::Item::SelectedOutlineStyle;

            Config::EnemyColors::InvisibleGlowColorR = Features::Colors::Enemy::InvisibleGlowColor[0];
            Config::EnemyColors::InvisibleGlowColorG = Features::Colors::Enemy::InvisibleGlowColor[1];
            Config::EnemyColors::InvisibleGlowColorB = Features::Colors::Enemy::InvisibleGlowColor[2];
            Config::EnemyColors::VisibleGlowColorR = Features::Colors::Enemy::VisibleGlowColor[0];
            Config::EnemyColors::VisibleGlowColorG = Features::Colors::Enemy::VisibleGlowColor[1];
            Config::EnemyColors::VisibleGlowColorB = Features::Colors::Enemy::VisibleGlowColor[2];
            Config::EnemyColors::RedShieldColorR = Features::Colors::Enemy::RedShieldColor[0];
            Config::EnemyColors::RedShieldColorG = Features::Colors::Enemy::RedShieldColor[1];
            Config::EnemyColors::RedShieldColorB = Features::Colors::Enemy::RedShieldColor[2];
            Config::EnemyColors::PurpleShieldColorR = Features::Colors::Enemy::PurpleShieldColor[0];
            Config::EnemyColors::PurpleShieldColorG = Features::Colors::Enemy::PurpleShieldColor[1];
            Config::EnemyColors::PurpleShieldColorB = Features::Colors::Enemy::PurpleShieldColor[2];
            Config::EnemyColors::BlueShieldColorR = Features::Colors::Enemy::BlueShieldColor[0];
            Config::EnemyColors::BlueShieldColorG = Features::Colors::Enemy::BlueShieldColor[1];
            Config::EnemyColors::BlueShieldColorB = Features::Colors::Enemy::BlueShieldColor[2];
            Config::EnemyColors::GreyShieldColorR = Features::Colors::Enemy::GreyShieldColor[0];
            Config::EnemyColors::GreyShieldColorG = Features::Colors::Enemy::GreyShieldColor[1];
            Config::EnemyColors::GreyShieldColorB = Features::Colors::Enemy::GreyShieldColor[2];

            return true;
        }
        catch (...)
        {
            return false;
        }
    }

    /*void Initialize()
    {
        for (int placebo = 0; placebo < 101; placebo++)
        {
            const GlowMode Ehh = {135, 132, 35, 127};
            StoredGlowMode->push_back(Ehh);
        }

        const GlowMode FirstStyle = {135, 135, 128, 64};
        const GlowMode SecondStyle = {135, 135, 160, 64};
        const GlowMode ThirdStyle = {135, 135, 255, 64};
        const GlowMode FourthStyle = {135, 135, 32, 64};

        StoredGlowMode->push_back(FirstStyle);
        StoredGlowMode->push_back(SecondStyle);
        StoredGlowMode->push_back(ThirdStyle);
        StoredGlowMode->push_back(FourthStyle);
    }*/

    // Old Glow & Item Glow
    void SetGlowState(long HighlightSettingsPointer, long HighlightSize, int HighlightID, GlowMode NewGlowMode)
    {
        const GlowMode oldGlowMode = Memory::Read<GlowMode>(HighlightSettingsPointer + (HighlightSize * HighlightID) + 0);
        if (NewGlowMode != oldGlowMode)
            Memory::Write<GlowMode>(HighlightSettingsPointer + (HighlightSize * HighlightID) + 0, NewGlowMode);
    }

    void SetColorState(long HighlightSettingsPointer, long HighlightSize, int HighlightID, Color NewColor)
    {
        const Color oldColor = Memory::Read<Color>(HighlightSettingsPointer + (HighlightSize * HighlightID) + 4);
        if (oldColor != NewColor)
            Memory::Write<Color>(HighlightSettingsPointer + (HighlightSize * HighlightID) + 4, NewColor);
    }

    void SetGlow(Player *Target, int GlowEnabled, int GlowThroughWall, int HighlightID)
    {
        if (Target->GlowEnable != GlowEnabled)
            Memory::Write<int>(Target->BasePointer + OFF_GLOW_ENABLE, GlowEnabled);
        if (Target->GlowThroughWall != GlowThroughWall)
        {
            Memory::Write<int>(Target->BasePointer + OFF_GLOW_THROUGH_WALL, GlowThroughWall);
            Memory::Write<int>(Target->BasePointer + OFF_GLOW_FIX, 1);
        }
        if (Target->HighlightID != HighlightID)
            Memory::Write<int>(Target->BasePointer + OFF_GLOW_HIGHLIGHT_ID + 1, HighlightID);
    }

    // New Glow
    void setGlowEnable(Player *Target, int glowEnable)
    {
        long ptrLong = Target->BasePointer + OFF_GLOW_ENABLE;
        Memory::Write<int>(ptrLong, glowEnable);
    }
    void setGlowThroughWall(Player *Target, int glowThroughWall)
    {
        long ptrLong = Target->BasePointer + OFF_GLOW_THROUGH_WALL;
        Memory::Write<int>(ptrLong, glowThroughWall);
    }
    int getGlowThroughWall(Player *Target)
    {
        int ptrInt = Memory::Read<int>(Target->BasePointer + OFF_GLOW_THROUGH_WALL);
        if (!Memory::IsValidPointer(ptrInt))
            return -1;
        return ptrInt;
    }
    int getGlowEnable(Player *Target)
    {
        int ptrInt = Memory::Read<int>(Target->BasePointer + OFF_GLOW_ENABLE);
        if (!Memory::IsValidPointer(ptrInt))
            return -1;
        return ptrInt;
    }

    void setCustomGlow(Player *Target, int health, bool isVisible, bool isSameTeam)
    {
        static const int contextId = 0; // Same as glow enable
        long basePointer = Target->BasePointer;
        int settingIndex = 65;

        // Glow
        int InsideFunction = 2;    // Leave
        int OutlineFunction = 125; // Leave

        // Custom Glow Body Style
        if (Features::Glow::BodyStyle == 0)
        { // None
            InsideFunction = 0;
        }
        if (Features::Glow::BodyStyle == 1)
        { // Pink
            InsideFunction = 1;
        }
        if (Features::Glow::BodyStyle == 2)
        { // Pink Visible Only
            InsideFunction = 110;
        }
        if (Features::Glow::BodyStyle == 3)
        { // Pulsing 1
            InsideFunction = 117;
        }
        if (Features::Glow::BodyStyle == 4)
        { // Pulsing Line Invisible Only
            InsideFunction = 12;
        }
        if (Features::Glow::BodyStyle == 5)
        { // Dark Pulsing Line
            InsideFunction = 13;
        }
        if (Features::Glow::BodyStyle == 6)
        { // Sharp Pulsing Visible Only
            InsideFunction = 124;
        }
        if (Features::Glow::BodyStyle == 7)
        { // Sharp Pulsing
            InsideFunction = 126;
        }
        if (Features::Glow::BodyStyle == 8)
        { // Pulsing Red Line
            InsideFunction = 3;
        }
        if (Features::Glow::BodyStyle == 9)
        { // Fast Pulsing Invisible
            InsideFunction = -500;
        }
        if (Features::Glow::BodyStyle == 10)
        { // Pulsing Up
            InsideFunction = 132;
        }
        if (Features::Glow::BodyStyle == 11)
        { // Solid Pulsing
            InsideFunction = 14;
        }
        if (Features::Glow::BodyStyle == 12)
        { // Solid Pulsing 2
            InsideFunction = 117;
        }
        if (Features::Glow::BodyStyle == 13)
        { // Bright
            InsideFunction = 109;
        }
        if (Features::Glow::BodyStyle == 14)
        { // Bright 2
            InsideFunction = 118;
        }
        if (Features::Glow::BodyStyle == 15)
        { // Light
            InsideFunction = 101;
        }
        if (Features::Glow::BodyStyle == 16)
        { // Light Solid
            InsideFunction = 112;
        }
        if (Features::Glow::BodyStyle == 17)
        { // Red Pulsing Visible Only
            InsideFunction = 115;
        }
        if (Features::Glow::BodyStyle == 18)
        { // Wave
            InsideFunction = 103;
        }
        if (Features::Glow::BodyStyle == 19)
        { // Shaded Visible
            InsideFunction = 136;
        }
        if (Features::Glow::BodyStyle == 20)
        { // Wireframe
            InsideFunction = 134;
        }
        if (Features::Glow::BodyStyle == 21)
        { // Wireframe Visible Only
            InsideFunction = 133;
        }
        if (Features::Glow::BodyStyle == 22)
        { // Black
            InsideFunction = 75;
        }
        if (Features::Glow::BodyStyle == 23)
        { // Black Visible Only
            InsideFunction = 77;
        }

        // Custom Outline Style
        if (Features::Glow::OutlineStyle == 0)
        { // None
            OutlineFunction = 0;
        }
        if (Features::Glow::OutlineStyle == 1)
        { // Bright
            OutlineFunction = 6;
        }
        if (Features::Glow::OutlineStyle == 2)
        { // Bright Invisible Only
            OutlineFunction = 102;
        }
        if (Features::Glow::OutlineStyle == 3)
        { // Dark
            OutlineFunction = 101;
        }
        if (Features::Glow::OutlineStyle == 4)
        { // Pink
            OutlineFunction = 1;
        }
        if (Features::Glow::OutlineStyle == 5)
        { // White
            OutlineFunction = 4;
        }
        if (Features::Glow::OutlineStyle == 6)
        { // Gold Flashing
            OutlineFunction = 5;
        }
        if (Features::Glow::OutlineStyle == 7)
        { // Gold
            OutlineFunction = 7;
        }
        if (Features::Glow::OutlineStyle == 8)
        { // Brown
            OutlineFunction = 8;
        }
        if (Features::Glow::OutlineStyle == 9)
        { // Wave
            OutlineFunction = 103;
        }
        if (Features::Glow::OutlineStyle == 10)
        { // Red
            OutlineFunction = 107;
        }
        if (Features::Glow::OutlineStyle == 11)
        { // Red Bright
            OutlineFunction = 108;
        }
        if (Features::Glow::OutlineStyle == 12)
        { // Heartbeat Visible Only
            OutlineFunction = 110;
        }
        if (Features::Glow::OutlineStyle == 13)
        { // Invisible Only Green
            OutlineFunction = -372;
        }
        if (Features::Glow::OutlineStyle == 15)
        { // Orange
            OutlineFunction = -249;
        }
        if (Features::Glow::OutlineStyle == 14)
        { // Visible Only
            OutlineFunction = -151;
        }
        if (Features::Glow::OutlineStyle == 16)
        { // Red
            OutlineFunction = 364;
        }

        std::array<unsigned char, 4> highlightFunctionBits = {
            InsideFunction,             // InsideFunction							2
            OutlineFunction,            // OutlineFunction: HIGHLIGHT_OUTLINE_OBJECTIVE			125
            Features::Glow::GlowRadius, // OutlineRadius: size * 255 / 8				64
            64                          // (EntityVisible << 6) | State & 0x3F | (AfterPostProcess << 7) 	64
        };
        std::array<float, 3> glowColorRGB = {0, 0, 0};
        if (Features::Glow::GlowColorMode == 0)
        {
            if (Features::Glow::GlowColorShieldMode == 0)
            {
                if (!isVisible)
                {
                    settingIndex = 65;
                    glowColorRGB = {Features::Colors::Enemy::VisibleGlowColor[0], Features::Colors::Enemy::VisibleGlowColor[1], Features::Colors::Enemy::VisibleGlowColor[2]}; // Visible Enemies
                }
                else if (health >= 201)
                {
                    settingIndex = 66;
                    glowColorRGB = {Features::Colors::Enemy::RedShieldColor[0], Features::Colors::Enemy::RedShieldColor[1], Features::Colors::Enemy::RedShieldColor[2]}; // red shield
                }
                else if (health >= 176)
                {
                    settingIndex = 67;
                    glowColorRGB = {Features::Colors::Enemy::PurpleShieldColor[0], Features::Colors::Enemy::PurpleShieldColor[1], Features::Colors::Enemy::PurpleShieldColor[2]}; // purple shield
                }
                else if (health >= 151)
                {
                    settingIndex = 68;
                    glowColorRGB = {Features::Colors::Enemy::BlueShieldColor[0], Features::Colors::Enemy::BlueShieldColor[1], Features::Colors::Enemy::BlueShieldColor[2]}; // blue shield
                }
                else if (health >= 101)
                {
                    settingIndex = 69;
                    glowColorRGB = {Features::Colors::Enemy::GreyShieldColor[0], Features::Colors::Enemy::GreyShieldColor[1], Features::Colors::Enemy::GreyShieldColor[2]}; // gray shield // cyan
                }
                else
                {
                    settingIndex = 70;
                    glowColorRGB = {Features::Colors::Enemy::LowGlowColor[0], Features::Colors::Enemy::LowGlowColor[1], Features::Colors::Enemy::LowGlowColor[2]}; // low health enemies // greeen
                }
            }

            if (Features::Glow::GlowColorShieldMode == 1)
            {
                if (!isVisible)
                {
                    settingIndex = 65;
                    glowColorRGB = {Features::Colors::Enemy::VisibleGlowColor[0], Features::Colors::Enemy::VisibleGlowColor[1], Features::Colors::Enemy::VisibleGlowColor[2]}; // Visible Enemies
                }
                else if (health >= 125)
                {
                    settingIndex = 66;
                    glowColorRGB = {Features::Colors::Enemy::RedShieldColor[0], Features::Colors::Enemy::RedShieldColor[1], Features::Colors::Enemy::RedShieldColor[2]}; // red shield
                }
                else if (health >= 100)
                {
                    settingIndex = 67;
                    glowColorRGB = {Features::Colors::Enemy::PurpleShieldColor[0], Features::Colors::Enemy::PurpleShieldColor[1], Features::Colors::Enemy::PurpleShieldColor[2]}; // purple shield
                }
                else if (health >= 75)
                {
                    settingIndex = 68;
                    glowColorRGB = {Features::Colors::Enemy::BlueShieldColor[0], Features::Colors::Enemy::BlueShieldColor[1], Features::Colors::Enemy::BlueShieldColor[2]}; // blue shield
                }
                else if (health >= 50)
                {
                    settingIndex = 69;
                    glowColorRGB = {Features::Colors::Enemy::GreyShieldColor[0], Features::Colors::Enemy::GreyShieldColor[1], Features::Colors::Enemy::GreyShieldColor[2]}; // gray shield // cyan
                }
                else
                {
                    settingIndex = 70;
                    glowColorRGB = {Features::Colors::Enemy::LowGlowColor[0], Features::Colors::Enemy::LowGlowColor[1], Features::Colors::Enemy::LowGlowColor[2]}; // low health enemies // greeen
                }
            }
        }

        if (Features::Glow::GlowColorMode == 1)
        {
            if (!isVisible)
            {
                settingIndex = 65;
                glowColorRGB = {Features::Colors::Enemy::VisibleGlowColor[0], Features::Colors::Enemy::VisibleGlowColor[1], Features::Colors::Enemy::VisibleGlowColor[2]}; // Visible Enemies
            }
            else if (isVisible)
            {
                settingIndex = 70;
                glowColorRGB = {Features::Colors::Enemy::InvisibleGlowColor[0], Features::Colors::Enemy::InvisibleGlowColor[1], Features::Colors::Enemy::InvisibleGlowColor[2]}; // Invisible Enemies
            }
        }

        Memory::Write<unsigned char>(Target->BasePointer + OFF_GLOW_HIGHLIGHT_ID + contextId, settingIndex);
        long highlightSettingsPtr = Memory::Read<long>(OFF_REGION + OFF_GLOW_HIGHLIGHTS);
        if (!isSameTeam)
        {
            Memory::Write<typeof(highlightFunctionBits)>(
                highlightSettingsPtr + OFF_HIGHLIGHT_TYPE_SIZE * settingIndex + 0, highlightFunctionBits);
            Memory::Write<typeof(glowColorRGB)>(
                highlightSettingsPtr + OFF_HIGHLIGHT_TYPE_SIZE * settingIndex + 4, glowColorRGB);
            Memory::Write<int>(Target->BasePointer + OFF_GLOW_FIX, 0);
        }
    }

    void Update()
    {
        if (!Map->IsPlayable)
            return;
        const long HighlightSettingsPointer = Memory::Read<long>(OFF_REGION + OFF_GLOW_HIGHLIGHTS);
        const long HighlightSize = OFF_HIGHLIGHT_TYPE_SIZE;

        int highlightID1;
        int highlightID2;

        // Item Glow Options
        if (!Features::Glow::Item::Ammo && !Features::Glow::Item::Weapons)
        {
            highlightID1 = 34;
            highlightID2 = 39;
        }

        else if (Features::Glow::Item::Ammo && !Features::Glow::Item::Weapons)
        {
            highlightID1 = 34;
            highlightID2 = 47;
        }

        else if (!Features::Glow::Item::Ammo && Features::Glow::Item::Weapons)
        {
            highlightID1 = 26;
            highlightID2 = 39;
        }

        else if (Features::Glow::Item::Ammo && Features::Glow::Item::Weapons)
        {
            highlightID1 = 26;
            highlightID2 = 47;
        }

        ItemGlowSettings(); // Updates Item Glow Settings

        const GlowMode newGlowMode = {
            ItemGlowInsideFunction,                  // Inside Glow
            ItemGlowOutlineFunction,                 // Outline (Border)
            Features::Glow::Item::ItemGlowThickness, // Outline Thickness
            127                                      // ItemGlowPostProcessing
        };

        const GlowMode SetGlowOff = {
            0,
            0,
            0,
            0
        };

        // Item Glow //
        if (Features::Glow::Item::ItemGlow)
        {
            if (Features::Glow::Item::SelectedItemSelection == 0)
            {
                for (int highlightId = 34; highlightId < 39; highlightId++)
                {
                    SetGlowState(HighlightSettingsPointer, HighlightSize, highlightId, newGlowMode);
                }
            }

            if (Features::Glow::Item::SelectedItemSelection == 1)
            {
                if (Features::Glow::Item::Weapons)
                {
                    SetGlowState(HighlightSettingsPointer, HighlightSize, 26, newGlowMode);
                }

                else
                {
                    const GlowMode newGlowMode = StoredGlowMode->at(26);
                    SetGlowState(HighlightSettingsPointer, HighlightSize, 26, SetGlowOff);
                }

                if (Features::Glow::Item::Common)
                {
                    SetGlowState(HighlightSettingsPointer, HighlightSize, 34, newGlowMode);
                }

                else
                {
                    const GlowMode newGlowMode = StoredGlowMode->at(34);
                    SetGlowState(HighlightSettingsPointer, HighlightSize, 34, SetGlowOff);
                }

                if (Features::Glow::Item::Rare)
                {
                    SetGlowState(HighlightSettingsPointer, HighlightSize, 35, newGlowMode);
                }

                else
                {
                    SetGlowState(HighlightSettingsPointer, HighlightSize, 35, SetGlowOff);
                }

                if (Features::Glow::Item::Epic)
                {
                    SetGlowState(HighlightSettingsPointer, HighlightSize, 36, newGlowMode);
                }

                else
                {
                    SetGlowState(HighlightSettingsPointer, HighlightSize, 36, SetGlowOff);
                }

                if (Features::Glow::Item::Gold)
                {
                    SetGlowState(HighlightSettingsPointer, HighlightSize, 37, newGlowMode);
                }

                else
                {
                    SetGlowState(HighlightSettingsPointer, HighlightSize, 37, SetGlowOff);
                }

                if (Features::Glow::Item::Legendary)
                {
                    SetGlowState(HighlightSettingsPointer, HighlightSize, 38, newGlowMode);
                }

                else
                {
                    SetGlowState(HighlightSettingsPointer, HighlightSize, 38, SetGlowOff);
                }

                SetGlowState(HighlightSettingsPointer, HighlightSize, 39, newGlowMode); // IDK what ID this is (currently)

                if (Features::Glow::Item::Ammo)
                {
                    SetGlowState(HighlightSettingsPointer, HighlightSize, 46, newGlowMode);
                }

                else
                {
                    SetGlowState(HighlightSettingsPointer, HighlightSize, 46, SetGlowOff);
                }
            }
        }

        /*
        26 = all weapons except legendary, also kinda hard to see when moving
        34 = grey items & attachments (e.g. shield cells)
        35 = blue items & attachments
        36 = purple
        37 = gold
        38 = red/legendary
        39 = ?
        47 = ammo
        */

        // Player Glow //
        // -> Visible
        // const GlowMode VisibleMode = { 2, 4, 20, 127 };
        // const Color VisibleColor = { 0, 255, 0 };
        // SetGlowState(HighlightSettingsPointer, HighlightSize, 0, VisibleMode);
        // SetColorState(HighlightSettingsPointer, HighlightSize, 0, VisibleColor);

        // -> Invisible
        // const GlowMode InvisibleMode = { 2, 4, 20, 127 };
        // const Color InvisibleColor = { 255, 0, 0 };
        // SetGlowState(HighlightSettingsPointer, HighlightSize, 1, InvisibleMode);
        // SetColorState(HighlightSettingsPointer, HighlightSize, 1, InvisibleColor);

        // -> Knocked
        // const GlowMode KnockedMode = { 2, 4, 20, 127 };
        // const Color KnockedColor = { 255, 150, 0 };
        // SetGlowState(HighlightSettingsPointer, HighlightSize, 90, KnockedMode);
        // SetColorState(HighlightSettingsPointer, HighlightSize, 90, KnockedColor);

        // -> Disabled
        // const GlowMode DisabledMode = { 0, 0, 0, 0 };
        // const Color DisabledColor = { 1, 1, 1 };
        // SetGlowState(HighlightSettingsPointer, HighlightSize, 91, DisabledMode);
        // SetColorState(HighlightSettingsPointer, HighlightSize, 91, DisabledColor);

        // -> Locked On
        // const GlowMode LockedOnMode = { 136, 6, 32, 127 };
        // const Color LockedOnColor = { 0, 0.75, 0.75 };
        // SetGlowState(HighlightSettingsPointer, HighlightSize, 92, LockedOnMode);
        // SetColorState(HighlightSettingsPointer, HighlightSize, 92, LockedOnColor);

        for (int i = 0; i < Players->size(); i++)
        {
            Player *Target = Players->at(i);
            if (!Target->IsValid())
                continue;
            if (!Target->IsHostile)
                continue;

            // Old Glow
            // if (GlowEnabled) {
            // if (Target->IsLockedOn) {
            // SetGlow(Target, 1, 2, 92);
            // continue;
            //}

            // if (Target->Distance2DToLocalPlayer < Conversion::ToGameUnits(GlowMaxDistance)) {
            // if (Target->IsKnocked) {
            // SetGlow(Target, 1, 2, 90);
            // continue;
            //}

            // int Highlight = (Target->IsVisible) ? 0 : 1;
            // SetGlow(Target, 1, 2, Highlight);
            // continue;
            //}
            //}

            // SetGlow(Target, 0, 0, 91);

            if (Features::Glow::NewGlow)
            {
                if (Features::Glow::GlowColorMode == 0)
                {
                    if (Features::Glow::GlowColorShieldMode == 0)
                    {
                        if (Target->IsVisible && !Target->IsKnocked && Target->Distance2DToLocalPlayer < Conversion::ToGameUnits(Features::Glow::GlowMaxDistance))
                        {
                            setGlowEnable(Target, 1);
                            setGlowThroughWall(Target, 1);
                            int healthShield = Target->Health + Target->Shield;
                            setCustomGlow(Target, healthShield, true, false);
                        }
                        if (!Target->IsVisible && !Target->IsKnocked && Target->Distance2DToLocalPlayer < Conversion::ToGameUnits(Features::Glow::GlowMaxDistance))
                        {
                            setGlowEnable(Target, 1);
                            setGlowThroughWall(Target, 1);
                            int healthShield = Target->Health + Target->Shield;
                            setCustomGlow(Target, healthShield, true, false);
                        }
                        // If player is out of max distance
                        else if (Target->Distance2DToLocalPlayer > Conversion::ToGameUnits(Features::Glow::GlowMaxDistance))
                        {
                            setGlowEnable(Target, 0);
                            setGlowThroughWall(Target, 0);
                            setCustomGlow(Target, 0, false, false);
                        }
                        else if (getGlowEnable(Target) == 1 && getGlowThroughWall(Target) == 1)
                        {
                            setGlowEnable(Target, 0);
                            setGlowThroughWall(Target, 0);
                        }
                    }

                    if (Features::Glow::GlowColorShieldMode == 1)
                    {
                        if (Target->IsVisible && !Target->IsKnocked && Target->Distance2DToLocalPlayer < Conversion::ToGameUnits(Features::Glow::GlowMaxDistance))
                        {
                            setGlowEnable(Target, 1);
                            setGlowThroughWall(Target, 1);
                            int healthShield = Target->MaxShield;
                            setCustomGlow(Target, healthShield, true, false);
                        }
                        if (!Target->IsVisible && !Target->IsKnocked && Target->Distance2DToLocalPlayer < Conversion::ToGameUnits(Features::Glow::GlowMaxDistance))
                        {
                            setGlowEnable(Target, 1);
                            setGlowThroughWall(Target, 1);
                            int healthShield = Target->MaxShield;
                            setCustomGlow(Target, healthShield, false, false);
                        }
                        // If player is out of max distance
                        else if (Target->Distance2DToLocalPlayer > Conversion::ToGameUnits(Features::Glow::GlowMaxDistance))
                        {
                            setGlowEnable(Target, 0);
                            setGlowThroughWall(Target, 0);
                            setCustomGlow(Target, 0, false, false);
                        }
                        else if (getGlowEnable(Target) == 1 && getGlowThroughWall(Target) == 1)
                        {
                            setGlowEnable(Target, 0);
                            setGlowThroughWall(Target, 0);
                        }
                    }
                }

                if (Features::Glow::GlowColorMode == 1)
                {
                    if (Target->IsVisible && !Target->IsKnocked && Target->Distance2DToLocalPlayer < Conversion::ToGameUnits(Features::Glow::GlowMaxDistance))
                    {
                        setGlowEnable(Target, 1);
                        setGlowThroughWall(Target, 1);
                        int healthShield = Target->MaxShield;
                        setCustomGlow(Target, healthShield, false, false);
                    }
                    if (!Target->IsVisible && !Target->IsKnocked && Target->Distance2DToLocalPlayer < Conversion::ToGameUnits(Features::Glow::GlowMaxDistance))
                    {
                        setGlowEnable(Target, 1);
                        setGlowThroughWall(Target, 1);
                        int healthShield = Target->MaxShield;
                        setCustomGlow(Target, healthShield, true, false);
                    }

                    // If player is out of max distance
                    else if (Target->Distance2DToLocalPlayer > Conversion::ToGameUnits(Features::Glow::GlowMaxDistance))
                    {
                        setGlowEnable(Target, 0);
                        setGlowThroughWall(Target, 0);
                        setCustomGlow(Target, 0, false, false);
                    }
                    else if (getGlowEnable(Target) == 1 && getGlowThroughWall(Target) == 1)
                    {
                        setGlowEnable(Target, 0);
                        setGlowThroughWall(Target, 0);
                    }
                }
            }
        }
    }

    void ViewModelGlow()
    {
        // Viewmodel Glow
        if (Features::Glow::ViewModelGlow)
        {
            ViewmodelGlowSettings(); // Updates the ID for what glow you want (bottom of this file)
            uint64_t actWeaponID = Memory::Read<uint64_t>(Myself->BasePointer + OFF_VIEWMODELS) & 0xFFFF;
            uint64_t currentWeapon = Memory::Read<uint64_t>(OFF_REGION + OFF_ENTITY_LIST + (actWeaponID << 5));

            Memory::Write<uint8_t>(currentWeapon + OFF_GLOW_HIGHLIGHT_ID, ViewmodelGlowID);
        }

        if (!Features::Glow::ViewModelGlow)
        {
            uint64_t actWeaponID = Memory::Read<uint64_t>(Myself->BasePointer + OFF_VIEWMODELS) & 0xFFFF;
            uint64_t currentWeapon = Memory::Read<uint64_t>(OFF_REGION + OFF_ENTITY_LIST + (actWeaponID << 5));

            Memory::Write<uint8_t>(currentWeapon + OFF_GLOW_HIGHLIGHT_ID, 0);
        }
    }

    void ViewmodelGlowSettings()
    {
        if (Features::Glow::ViewModelGlowCombo == 0)
        { // Cyan Outline
            ViewmodelGlowID = 13;
        }
        if (Features::Glow::ViewModelGlowCombo == 1)
        { // Light Red Outline
            ViewmodelGlowID = 15;
        }
        if (Features::Glow::ViewModelGlowCombo == 2)
        { // White Outline
            ViewmodelGlowID = 17;
        }
        if (Features::Glow::ViewModelGlowCombo == 3)
        { // Orange Outline
            ViewmodelGlowID = 18;
        }
        if (Features::Glow::ViewModelGlowCombo == 4)
        { // Yellow Outline
            ViewmodelGlowID = 24;
        }
        if (Features::Glow::ViewModelGlowCombo == 5)
        { // Solid Green
            ViewmodelGlowID = 27;
        }
        if (Features::Glow::ViewModelGlowCombo == 6)
        { // Solid Orange
            ViewmodelGlowID = 28;
        }
        if (Features::Glow::ViewModelGlowCombo == 7)
        { // Solid Yellow
            ViewmodelGlowID = 29;
        }
        if (Features::Glow::ViewModelGlowCombo == 8)
        { // Solid Yellow Pulsing
            ViewmodelGlowID = 25;
        }
        if (Features::Glow::ViewModelGlowCombo == 9)
        { // Solid Purple
            ViewmodelGlowID = 30;
        }
        if (Features::Glow::ViewModelGlowCombo == 10)
        { // Solid Light Blue
            ViewmodelGlowID = 31;
        }
        if (Features::Glow::ViewModelGlowCombo == 11)
        { // Solid Light Grey
            ViewmodelGlowID = 32;
        }
        if (Features::Glow::ViewModelGlowCombo == 12)
        { // Solid White
            ViewmodelGlowID = 34;
        }
        if (Features::Glow::ViewModelGlowCombo == 13)
        { // Solid Cyan
            ViewmodelGlowID = 35;
        }
        if (Features::Glow::ViewModelGlowCombo == 14)
        { // Solid Hot Pink
            ViewmodelGlowID = 36;
        }
        if (Features::Glow::ViewModelGlowCombo == 15)
        { // Solid Light Yellow
            ViewmodelGlowID = 37;
        }
        if (Features::Glow::ViewModelGlowCombo == 16)
        { // Solid Light Orange
            ViewmodelGlowID = 38;
        }
        if (Features::Glow::ViewModelGlowCombo == 17)
        { // Solid Light Green
            ViewmodelGlowID = 39;
        }
        if (Features::Glow::ViewModelGlowCombo == 18)
        { // Solid Black
            ViewmodelGlowID = 74;
        }
        if (Features::Glow::ViewModelGlowCombo == 19)
        { // Chrome
            ViewmodelGlowID = 49;
        }
        if (Features::Glow::ViewModelGlowCombo == 20)
        { // Invisible Red Pulsing
            ViewmodelGlowID = 93;
        }
    }

    void ItemGlowSettings()
    {
        // Inside Function (Body Style)
        if (Features::Glow::Item::SelectedInsideStyle == 0) // Clear
        {
            ItemGlowInsideFunction = 0;
        }

        if (Features::Glow::Item::SelectedInsideStyle == 1) // Light
        {
            ItemGlowInsideFunction = 137;
        }

        if (Features::Glow::Item::SelectedInsideStyle == 2) // Solid
        {
            ItemGlowInsideFunction = 109;
        }

        if (Features::Glow::Item::SelectedInsideStyle == 3) // Light To Dark Fade
        {
            ItemGlowInsideFunction = 117;
        }

        // Outside Function (Outline Style)
        if (Features::Glow::Item::SelectedOutlineStyle == 0) // None
        {
            ItemGlowOutlineFunction = 0;
        }

        if (Features::Glow::Item::SelectedOutlineStyle == 1) // Light 1
        {
            ItemGlowOutlineFunction = 138;
        }

        if (Features::Glow::Item::SelectedOutlineStyle == 2) // Light 2
        {
            ItemGlowOutlineFunction = 102;
        }

        if (Features::Glow::Item::SelectedOutlineStyle == 3) // Solid
        {
            ItemGlowOutlineFunction = 6;
        }

        if (Features::Glow::Item::SelectedOutlineStyle == 4) // Gold
        {
            ItemGlowOutlineFunction = 7;
        }

        if (Features::Glow::Item::SelectedOutlineStyle == 5) // Orange
        {
            ItemGlowOutlineFunction = 8;
        }

        if (Features::Glow::Item::SelectedOutlineStyle == 6) // Pulsing
        {
            ItemGlowOutlineFunction = 103;
        }

        if (Features::Glow::Item::SelectedOutlineStyle == 7) // Light Red
        {
            ItemGlowOutlineFunction = 107;
        }

        if (Features::Glow::Item::SelectedOutlineStyle == 8) // Red
        {
            ItemGlowOutlineFunction = 108;
        }

        if (Features::Glow::Item::SelectedOutlineStyle == 9) // Fading
        {
            ItemGlowOutlineFunction = 110;
        }

        if (Features::Glow::Item::SelectedOutlineStyle == 10) // Soft
        {
            ItemGlowOutlineFunction = 120;
        }

        if (Features::Glow::Item::SelectedOutlineStyle == 11) // Visible Only
        {
            ItemGlowOutlineFunction = 131;
        }
    }
};
