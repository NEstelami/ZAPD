#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>

#include "Declaration.h"

// see https://github.com/zeldaret/oot/blob/7235af2249843fb68740111b70089bad827a4730/include/z64cutscene.h#L35-L165
enum class CutsceneCommands
{
	CS_CMD_CAM_EYE_SPLINE = 0x0001,
	CS_CMD_CAM_AT_SPLINE = 0x0002,
	CS_CMD_MISC = 0x0003,
	CS_CMD_LIGHT_SETTING = 0x0004,
	CS_CMD_CAM_EYE_SPLINE_REL_TO_PLAYER = 0x0005,
	CS_CMD_CAM_AT_SPLINE_REL_TO_PLAYER = 0x0006,
	CS_CMD_CAM_EYE = 0x0007,
	CS_CMD_CAM_AT = 0x0008,
	CS_CMD_RUMBLE_CONTROLLER = 0x0009,
	CS_CMD_PLAYER_CUE = 0x000A,
	CS_CMD_ACTOR_CUE_1_0 = 0x000E,
	CS_CMD_ACTOR_CUE_0_0 = 0x000F,
	CS_CMD_ACTOR_CUE_1_1 = 0x0010,
	CS_CMD_ACTOR_CUE_0_1 = 0x0011,
	CS_CMD_ACTOR_CUE_0_2 = 0x0012,
	CS_CMD_TEXT = 0x0013,
	CS_CMD_ACTOR_CUE_0_3 = 0x0017,
	CS_CMD_ACTOR_CUE_1_2 = 0x0018,
	CS_CMD_ACTOR_CUE_2_0 = 0x0019,
	CS_CMD_ACTOR_CUE_3_0 = 0x001D,
	CS_CMD_ACTOR_CUE_4_0 = 0x001E,
	CS_CMD_ACTOR_CUE_6_0 = 0x001F,
	CS_CMD_ACTOR_CUE_0_4 = 0x0022,
	CS_CMD_ACTOR_CUE_1_3 = 0x0023,
	CS_CMD_ACTOR_CUE_2_1 = 0x0024,
	CS_CMD_ACTOR_CUE_3_1 = 0x0025,
	CS_CMD_ACTOR_CUE_4_1 = 0x0026,
	CS_CMD_ACTOR_CUE_0_5 = 0x0027,
	CS_CMD_ACTOR_CUE_1_4 = 0x0028,
	CS_CMD_ACTOR_CUE_2_2 = 0x0029,
	CS_CMD_ACTOR_CUE_3_2 = 0x002A,
	CS_CMD_ACTOR_CUE_4_2 = 0x002B,
	CS_CMD_ACTOR_CUE_5_0 = 0x002C,
	CS_CMD_TRANSITION = 0x002D,
	CS_CMD_ACTOR_CUE_0_6 = 0x002E,
	CS_CMD_ACTOR_CUE_4_3 = 0x002F,
	CS_CMD_ACTOR_CUE_1_5 = 0x0030,
	CS_CMD_ACTOR_CUE_7_0 = 0x0031,
	CS_CMD_ACTOR_CUE_2_3 = 0x0032,
	CS_CMD_ACTOR_CUE_3_3 = 0x0033,
	CS_CMD_ACTOR_CUE_6_1 = 0x0034,
	CS_CMD_ACTOR_CUE_3_4 = 0x0035,
	CS_CMD_ACTOR_CUE_4_4 = 0x0036,
	CS_CMD_ACTOR_CUE_5_1 = 0x0037,
	CS_CMD_ACTOR_CUE_6_2 = 0x0039,
	CS_CMD_ACTOR_CUE_6_3 = 0x003A,
	CS_CMD_ACTOR_CUE_7_1 = 0x003C,
	CS_CMD_ACTOR_CUE_8_0 = 0x003E,
	CS_CMD_ACTOR_CUE_3_5 = 0x003F,
	CS_CMD_ACTOR_CUE_1_6 = 0x0040,
	CS_CMD_ACTOR_CUE_3_6 = 0x0041,
	CS_CMD_ACTOR_CUE_3_7 = 0x0042,
	CS_CMD_ACTOR_CUE_2_4 = 0x0043,
	CS_CMD_ACTOR_CUE_1_7 = 0x0044,
	CS_CMD_ACTOR_CUE_2_5 = 0x0045,
	CS_CMD_ACTOR_CUE_1_8 = 0x0046,
	CS_CMD_ACTOR_CUE_2_6 = 0x0048,
	CS_CMD_ACTOR_CUE_2_7 = 0x004A,
	CS_CMD_ACTOR_CUE_3_8 = 0x004B,
	CS_CMD_ACTOR_CUE_0_7 = 0x004C,
	CS_CMD_ACTOR_CUE_5_2 = 0x004D,
	CS_CMD_ACTOR_CUE_1_9 = 0x004E,
	CS_CMD_ACTOR_CUE_4_5 = 0x004F,
	CS_CMD_ACTOR_CUE_1_10 = 0x0050,
	CS_CMD_ACTOR_CUE_2_8 = 0x0051,
	CS_CMD_ACTOR_CUE_3_9 = 0x0052,
	CS_CMD_ACTOR_CUE_4_6 = 0x0053,
	CS_CMD_ACTOR_CUE_5_3 = 0x0054,
	CS_CMD_ACTOR_CUE_0_8 = 0x0055,
	CS_CMD_START_SEQ = 0x0056,
	CS_CMD_STOP_SEQ = 0x0057,
	CS_CMD_ACTOR_CUE_6_4 = 0x0058,
	CS_CMD_ACTOR_CUE_7_2 = 0x0059,
	CS_CMD_ACTOR_CUE_5_4 = 0x005A,
	CS_CMD_ACTOR_CUE_0_9 = 0x005D,
	CS_CMD_ACTOR_CUE_1_11 = 0x005E,
	CS_CMD_ACTOR_CUE_0_10 = 0x0069,
	CS_CMD_ACTOR_CUE_2_9 = 0x006A,
	CS_CMD_ACTOR_CUE_0_11 = 0x006B,
	CS_CMD_ACTOR_CUE_3_10 = 0x006C,
	CS_CMD_ACTOR_CUE_0_12 = 0x006E,
	CS_CMD_ACTOR_CUE_7_3 = 0x006F,
	CS_CMD_ACTOR_CUE_7_4 = 0x0072,
	CS_CMD_ACTOR_CUE_6_5 = 0x0073,
	CS_CMD_ACTOR_CUE_1_12 = 0x0074,
	CS_CMD_ACTOR_CUE_2_10 = 0x0075,
	CS_CMD_ACTOR_CUE_1_13 = 0x0076,
	CS_CMD_ACTOR_CUE_0_13 = 0x0077,
	CS_CMD_ACTOR_CUE_1_14 = 0x0078,
	CS_CMD_ACTOR_CUE_2_11 = 0x0079,
	CS_CMD_ACTOR_CUE_0_14 = 0x007B,
	CS_CMD_FADE_OUT_SEQ = 0x007C,
	CS_CMD_ACTOR_CUE_1_15 = 0x007D,
	CS_CMD_ACTOR_CUE_2_12 = 0x007E,
	CS_CMD_ACTOR_CUE_3_11 = 0x007F,
	CS_CMD_ACTOR_CUE_4_7 = 0x0080,
	CS_CMD_ACTOR_CUE_5_5 = 0x0081,
	CS_CMD_ACTOR_CUE_6_6 = 0x0082,
	CS_CMD_ACTOR_CUE_1_16 = 0x0083,
	CS_CMD_ACTOR_CUE_2_13 = 0x0084,
	CS_CMD_ACTOR_CUE_3_12 = 0x0085,
	CS_CMD_ACTOR_CUE_7_5 = 0x0086,
	CS_CMD_ACTOR_CUE_4_8 = 0x0087,
	CS_CMD_ACTOR_CUE_5_6 = 0x0088,
	CS_CMD_ACTOR_CUE_6_7 = 0x0089,
	CS_CMD_ACTOR_CUE_0_15 = 0x008A,
	CS_CMD_ACTOR_CUE_0_16 = 0x008B,
	CS_CMD_TIME = 0x008C,
	CS_CMD_ACTOR_CUE_1_17 = 0x008D,
	CS_CMD_ACTOR_CUE_7_6 = 0x008E,
	CS_CMD_ACTOR_CUE_9_0 = 0x008F,
	CS_CMD_ACTOR_CUE_0_17 = 0x0090,
	CS_CMD_DESTINATION = 0x03E8,
	CS_CMD_END = 0xFFFF
};

const std::unordered_map<uint32_t, std::string> csOoTEnumNameToString = {
	{(uint32_t)CutsceneCommands::CS_CMD_PLAYER_CUE, "CS_CMD_PLAYER_CUE"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_1_0, "CS_CMD_ACTOR_CUE_1_0"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_0_0, "CS_CMD_ACTOR_CUE_0_0"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_1_1, "CS_CMD_ACTOR_CUE_1_1"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_0_1, "CS_CMD_ACTOR_CUE_0_1"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_0_2, "CS_CMD_ACTOR_CUE_0_2"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_0_3, "CS_CMD_ACTOR_CUE_0_3"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_1_2, "CS_CMD_ACTOR_CUE_1_2"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_2_0, "CS_CMD_ACTOR_CUE_2_0"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_3_0, "CS_CMD_ACTOR_CUE_3_0"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_4_0, "CS_CMD_ACTOR_CUE_4_0"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_6_0, "CS_CMD_ACTOR_CUE_6_0"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_0_4, "CS_CMD_ACTOR_CUE_0_4"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_1_3, "CS_CMD_ACTOR_CUE_1_3"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_2_1, "CS_CMD_ACTOR_CUE_2_1"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_3_1, "CS_CMD_ACTOR_CUE_3_1"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_4_1, "CS_CMD_ACTOR_CUE_4_1"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_0_5, "CS_CMD_ACTOR_CUE_0_5"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_1_4, "CS_CMD_ACTOR_CUE_1_4"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_2_2, "CS_CMD_ACTOR_CUE_2_2"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_3_2, "CS_CMD_ACTOR_CUE_3_2"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_4_2, "CS_CMD_ACTOR_CUE_4_2"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_5_0, "CS_CMD_ACTOR_CUE_5_0"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_0_6, "CS_CMD_ACTOR_CUE_0_6"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_4_3, "CS_CMD_ACTOR_CUE_4_3"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_1_5, "CS_CMD_ACTOR_CUE_1_5"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_7_0, "CS_CMD_ACTOR_CUE_7_0"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_2_3, "CS_CMD_ACTOR_CUE_2_3"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_3_3, "CS_CMD_ACTOR_CUE_3_3"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_6_1, "CS_CMD_ACTOR_CUE_6_1"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_3_4, "CS_CMD_ACTOR_CUE_3_4"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_4_4, "CS_CMD_ACTOR_CUE_4_4"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_5_1, "CS_CMD_ACTOR_CUE_5_1"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_6_2, "CS_CMD_ACTOR_CUE_6_2"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_6_3, "CS_CMD_ACTOR_CUE_6_3"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_7_1, "CS_CMD_ACTOR_CUE_7_1"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_8_0, "CS_CMD_ACTOR_CUE_8_0"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_3_5, "CS_CMD_ACTOR_CUE_3_5"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_1_6, "CS_CMD_ACTOR_CUE_1_6"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_3_6, "CS_CMD_ACTOR_CUE_3_6"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_3_7, "CS_CMD_ACTOR_CUE_3_7"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_2_4, "CS_CMD_ACTOR_CUE_2_4"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_1_7, "CS_CMD_ACTOR_CUE_1_7"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_2_5, "CS_CMD_ACTOR_CUE_2_5"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_1_8, "CS_CMD_ACTOR_CUE_1_8"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_2_6, "CS_CMD_ACTOR_CUE_2_6"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_2_7, "CS_CMD_ACTOR_CUE_2_7"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_3_8, "CS_CMD_ACTOR_CUE_3_8"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_0_7, "CS_CMD_ACTOR_CUE_0_7"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_5_2, "CS_CMD_ACTOR_CUE_5_2"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_1_9, "CS_CMD_ACTOR_CUE_1_9"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_4_5, "CS_CMD_ACTOR_CUE_4_5"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_1_10, "CS_CMD_ACTOR_CUE_1_10"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_2_8, "CS_CMD_ACTOR_CUE_2_8"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_3_9, "CS_CMD_ACTOR_CUE_3_9"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_4_6, "CS_CMD_ACTOR_CUE_4_6"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_5_3, "CS_CMD_ACTOR_CUE_5_3"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_0_8, "CS_CMD_ACTOR_CUE_0_8"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_6_4, "CS_CMD_ACTOR_CUE_6_4"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_7_2, "CS_CMD_ACTOR_CUE_7_2"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_5_4, "CS_CMD_ACTOR_CUE_5_4"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_0_9, "CS_CMD_ACTOR_CUE_0_9"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_1_11, "CS_CMD_ACTOR_CUE_1_11"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_0_10, "CS_CMD_ACTOR_CUE_0_10"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_2_9, "CS_CMD_ACTOR_CUE_2_9"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_0_11, "CS_CMD_ACTOR_CUE_0_11"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_3_10, "CS_CMD_ACTOR_CUE_3_10"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_0_12, "CS_CMD_ACTOR_CUE_0_12"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_7_3, "CS_CMD_ACTOR_CUE_7_3"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_7_4, "CS_CMD_ACTOR_CUE_7_4"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_6_5, "CS_CMD_ACTOR_CUE_6_5"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_1_12, "CS_CMD_ACTOR_CUE_1_12"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_2_10, "CS_CMD_ACTOR_CUE_2_10"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_1_13, "CS_CMD_ACTOR_CUE_1_13"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_0_13, "CS_CMD_ACTOR_CUE_0_13"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_1_14, "CS_CMD_ACTOR_CUE_1_14"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_2_11, "CS_CMD_ACTOR_CUE_2_11"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_0_14, "CS_CMD_ACTOR_CUE_0_14"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_1_15, "CS_CMD_ACTOR_CUE_1_15"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_2_12, "CS_CMD_ACTOR_CUE_2_12"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_3_11, "CS_CMD_ACTOR_CUE_3_11"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_4_7, "CS_CMD_ACTOR_CUE_4_7"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_5_5, "CS_CMD_ACTOR_CUE_5_5"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_6_6, "CS_CMD_ACTOR_CUE_6_6"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_1_16, "CS_CMD_ACTOR_CUE_1_16"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_2_13, "CS_CMD_ACTOR_CUE_2_13"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_3_12, "CS_CMD_ACTOR_CUE_3_12"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_7_5, "CS_CMD_ACTOR_CUE_7_5"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_4_8, "CS_CMD_ACTOR_CUE_4_8"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_5_6, "CS_CMD_ACTOR_CUE_5_6"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_6_7, "CS_CMD_ACTOR_CUE_6_7"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_0_15, "CS_CMD_ACTOR_CUE_0_15"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_0_16, "CS_CMD_ACTOR_CUE_0_16"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_1_17, "CS_CMD_ACTOR_CUE_1_17"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_7_6, "CS_CMD_ACTOR_CUE_7_6"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_9_0, "CS_CMD_ACTOR_CUE_9_0"},
	{(uint32_t)CutsceneCommands::CS_CMD_ACTOR_CUE_0_17, "CS_CMD_ACTOR_CUE_0_17"},
};

const std::string csOoTMiscTypes[] = {
	"CS_MISC_UNIMPLEMENTED_0",
	"CS_MISC_RAIN",
	"CS_MISC_LIGHTNING",
	"CS_MISC_SET_CSFLAG_0",
	"CS_MISC_UNIMPLEMENTED_4",
	"CS_MISC_UNIMPLEMENTED_5",
	"CS_MISC_LIFT_FOG",
	"CS_MISC_CLOUDY_SKY",
	"CS_MISC_FADE_KOKIRI_GRASS_ENV_ALPHA",
	"CS_MISC_SNOW",
	"CS_MISC_SET_CSFLAG_1",
	"CS_MISC_DEKU_TREE_DEATH",
	"CS_MISC_STOP_CUTSCENE",
	"CS_MISC_TRIFORCE_FLASH",
	"CS_MISC_SET_LOCKED_VIEWPOINT",
	"CS_MISC_SHOW_TITLE_CARD",
	"CS_MISC_QUAKE_START",
	"CS_MISC_QUAKE_STOP",
	"CS_MISC_STOP_STORM_AND_ADVANCE_TO_DAY",
	"CS_MISC_SET_FLAG_FAST_WINDMILL",
	"CS_MISC_SET_FLAG_WELL_DRAINED",
	"CS_MISC_SET_FLAG_LAKE_HYLIA_RESTORED",
	"CS_MISC_VISMONO_BLACK_AND_WHITE",
	"CS_MISC_VISMONO_SEPIA",
	"CS_MISC_HIDE_ROOM",
	"CS_MISC_TIME_ADVANCE_TO_NIGHT",
	"CS_MISC_SET_TIME_BASED_LIGHT_SETTING",
	"CS_MISC_RED_PULSATING_LIGHTS",
	"CS_MISC_HALT_ALL_ACTORS",
	"CS_MISC_RESUME_ALL_ACTORS",
	"CS_MISC_SET_CSFLAG_3",
	"CS_MISC_SET_CSFLAG_4",
	"CS_MISC_SANDSTORM_FILL",
	"CS_MISC_SUNSSONG_START",
	"CS_MISC_FREEZE_TIME",
	"CS_MISC_LONG_SCARECROW_SONG",
};

const std::string csOoTTextTypes[] = {
	"CS_TEXT_NORMAL",
	"CS_TEXT_CHOICE",
	"CS_TEXT_OCARINA_ACTION",
	"CS_TEXT_GORON_RUBY",
	"CS_TEXT_ZORA_SAPPHIRE",
};

const std::string csOoTTransitionType[] = {
	"CS_TRANS_GRAY_FILL_IN",
	"CS_TRANS_BLUE_FILL_IN",
	"CS_TRANS_RED_FILL_OUT",
	"CS_TRANS_GREEN_FILL_OUT",
	"CS_TRANS_GRAY_FILL_OUT",
	"CS_TRANS_BLUE_FILL_OUT",
	"CS_TRANS_RED_FILL_IN",
	"CS_TRANS_GREEN_FILL_IN",
	"CS_TRANS_TRIGGER_INSTANCE",
	"CS_TRANS_BLACK_FILL_OUT",
	"CS_TRANS_BLACK_FILL_IN",
	"CS_TRANS_BLACK_FILL_OUT_TO_HALF",
	"CS_TRANS_BLACK_FILL_IN_FROM_HALF",
};

const std::string csOoTDestinationType[] = {
	"CS_DEST_UNIMPLEMENTED_0",
	"CS_DEST_CUTSCENE_MAP_GANON_HORSE",
	"CS_DEST_CUTSCENE_MAP_THREE_GODDESSES",
	"CS_DEST_GERUDO_VALLEY_DIN_PART_1",
	"CS_DEST_DEATH_MOUNTAIN_TRAIL_NAYRU",
	"CS_DEST_KOKIRI_FOREST_FARORE",
	"CS_DEST_CUTSCENE_MAP_TRIFORCE_CREATION",
	"CS_DEST_KOKIRI_FOREST_RECEIVE_KOKIRI_EMERALD",
	"CS_DEST_TEMPLE_OF_TIME_FROM_MASTER_SWORD",
	"CS_DEST_GERUDO_VALLEY_DIN_PART_2",
	"CS_DEST_LINKS_HOUSE_INTRO",
	"CS_DEST_KOKIRI_FOREST_INTRO",
	"CS_DEST_DEATH_MOUNTAIN_TRAIL_FROM_GORON_RUBY",
	"CS_DEST_ZORAS_FOUNTAIN_FROM_ZORAS_SAPPHIRE",
	"CS_DEST_KOKIRI_FOREST_FROM_KOKIRI_EMERALD",
	"CS_DEST_TEMPLE_OF_TIME_KOKIRI_EMERALD_RESTORED",
	"CS_DEST_TEMPLE_OF_TIME_GORON_RUBY_RESTORED",
	"CS_DEST_TEMPLE_OF_TIME_ZORAS_SAPPHIRE_RESTORED",
	"CS_DEST_TEMPLE_OF_TIME_AFTER_LIGHT_MEDALLION",
	"CS_DEST_DEATH_MOUNTAIN_TRAIL",
	"CS_DEST_UNIMPLEMENTED_14",
	"CS_DEST_LAKE_HYLIA_WATER_RESTORED",
	"CS_DEST_DESERT_COLOSSUS_REQUIEM",
	"CS_DEST_CUTSCENE_MAP_GANONDORF_DEFEATED_CREDITS",
	"CS_DEST_JABU_JABU",
	"CS_DEST_CHAMBER_OF_SAGES_LIGHT_MEDALLION",
	"CS_DEST_TEMPLE_OF_TIME_KOKIRI_EMERALD_RESTORED_2",
	"CS_DEST_TEMPLE_OF_TIME_GORON_RUBY_RESTORED_2",
	"CS_DEST_TEMPLE_OF_TIME_ZORAS_SAPPHIRE_RESTORED_2",
	"CS_DEST_CHAMBER_OF_SAGES_FOREST_MEDALLION",
	"CS_DEST_CHAMBER_OF_SAGES_FIRE_MEDALLION",
	"CS_DEST_CHAMBER_OF_SAGES_WATER_MEDALLION",
	"CS_DEST_HYRULE_FIELD_FLASHBACK",
	"CS_DEST_HYRULE_FIELD_FROM_ZELDA_ESCAPE",
	"CS_DEST_CUTSCENE_MAP_GANONDORF_FROM_MASTER_SWORD",
	"CS_DEST_HYRULE_FIELD_INTRO_DREAM",
	"CS_DEST_UNIMPLEMENTED_24",
	"CS_DEST_UNIMPLEMENTED_25",
	"CS_DEST_CUTSCENE_MAP_SHEIKAH_LEGEND",
	"CS_DEST_TEMPLE_OF_TIME_ZELDA_REVEAL",
	"CS_DEST_TEMPLE_OF_TIME_GET_LIGHT_ARROWS",
	"CS_DEST_LAKE_HYLIA_FROM_LAKE_RESTORED",
	"CS_DEST_KAKARIKO_VILLAGE_DRAIN_WELL",
	"CS_DEST_WINDMILL_FROM_WELL_DRAINED",
	"CS_DEST_TEMPLE_OF_TIME_FROM_ALL_STONES_RESTORED",
	"CS_DEST_UNIMPLEMENTED_2D",
	"CS_DEST_TEMPLE_OF_TIME_AFTER_LIGHT_MEDALLION_ALT",
	"CS_DEST_KAKARIKO_VILLAGE_NOCTURNE_PART_2",
	"CS_DEST_DESERT_COLOSSUS_FROM_REQUIEM",
	"CS_DEST_TEMPLE_OF_TIME_FROM_LIGHT_ARROWS",
	"CS_DEST_KAKARIKO_VILLAGE_FROM_NOCTURNE",
	"CS_DEST_HYRULE_FIELD_FROM_ZELDAS_COURTYARD",
	"CS_DEST_TEMPLE_OF_TIME_SONG_OF_TIME",
	"CS_DEST_HYRULE_FIELD_FROM_SONG_OF_TIME",
	"CS_DEST_GERUDO_VALLEY_CREDITS",
	"CS_DEST_GERUDO_FORTRESS_CREDITS",
	"CS_DEST_KAKARIKO_VILLAGE_CREDITS",
	"CS_DEST_DEATH_MOUNTAIN_TRAIL_CREDITS_PART_1",
	"CS_DEST_GORON_CITY_CREDITS",
	"CS_DEST_LAKE_HYLIA_CREDITS",
	"CS_DEST_ZORAS_FOUNTAIN_CREDITS crashes",
	"CS_DEST_ZORAS_DOMAIN_CREDITS",
	"CS_DEST_KOKIRI_FOREST_CREDITS_PART_1",
	"CS_DEST_KOKIRI_FOREST_CREDITS_PART_2",
	"CS_DEST_HYRULE_FIELD_CREDITS",
	"CS_DEST_LON_LON_RANCH_CREDITS_PART_1_ALT",
	"CS_DEST_KAKARIKO_VILLAGE_FROM_TRAIL_OWL",
	"CS_DEST_HYRULE_FIELD_FROM_LAKE_HYLIA_OWL",
	"CS_DEST_CUTSCENE_MAP_DEKU_SPROUT_PART_2",
	"CS_DEST_KOKIRI_FOREST_DEKU_SPROUT_PART_3",
	"CS_DEST_DEATH_MOUNTAIN_TRAIL_CREDITS_PART_2",
	"CS_DEST_TEMPLE_OF_TIME_CREDITS",
	"CS_DEST_ZELDAS_COURTYARD_CREDITS",
	"CS_DEST_LON_LON_RANCH_CREDITS_PART_1",
	"CS_DEST_LON_LON_RANCH_CREDITS_PART_2",
	"CS_DEST_LON_LON_RANCH_CREDITS_PART_3",
	"CS_DEST_LON_LON_RANCH_CREDITS_PART_4",
	"CS_DEST_LON_LON_RANCH_CREDITS_PART_5",
	"CS_DEST_LON_LON_RANCH_CREDITS_PART_6",
	"CS_DEST_LON_LON_RANCH_1",
	"CS_DEST_LON_LON_RANCH_2",
	"CS_DEST_LON_LON_RANCH_3",
	"CS_DEST_LON_LON_RANCH_4",
	"CS_DEST_LON_LON_RANCH_5",
	"CS_DEST_LON_LON_RANCH_6",
	"CS_DEST_LON_LON_RANCH_7",
	"CS_DEST_LON_LON_RANCH_8",
	"CS_DEST_LON_LON_RANCH_9",
	"CS_DEST_LON_LON_RANCH_10",
	"CS_DEST_LON_LON_RANCH_11",
	"CS_DEST_LON_LON_RANCH_12",
	"CS_DEST_LON_LON_RANCH_13",
	"CS_DEST_LON_LON_RANCH_14",
	"CS_DEST_LON_LON_RANCH_15",
	"CS_DEST_LON_LON_RANCH_FROM_EPONAS_SONG",
	"CS_DEST_STONES_RESTORED_CONDITIONAL",
	"CS_DEST_DESERT_COLOSSUS_FROM_CHAMBER_OF_SAGES",
	"CS_DEST_GRAVEYARD_FROM_CHAMBER_OF_SAGES",
	"CS_DEST_DEATH_MOUNTAIN_CRATER_FROM_CHAMBER_OF_SAGES",
	"CS_DEST_SACRED_FOREST_MEADOW_WARP_PAD",
	"CS_DEST_KOKIRI_FOREST_FROM_CHAMBER_OF_SAGES",
	"CS_DEST_DESERT_COLOSSUS_FROM_NABOORU_CAPTURE",
	"CS_DEST_TEMPLE_OF_TIME_FRONT_OF_PEDESTAL",
	"CS_DEST_HYRULE_FIELD_TITLE_SCREEN",
	"CS_DEST_TITLE_SCREEN_DEMO",
	"CS_DEST_GRAVEYARD_SUNS_SONG_PART_2",
	"CS_DEST_ROYAL_FAMILYS_TOMB_SUNS_SONG_PART_3",
	"CS_DEST_GANONS_CASTLE_DISPEL_FOREST_BEAM",
	"CS_DEST_GANONS_CASTLE_DISPEL_WATER_BEAM",
	"CS_DEST_GANONS_CASTLE_DISPEL_SHADOW_BEAM",
	"CS_DEST_GANONS_CASTLE_DISPEL_FIRE_BEAM",
	"CS_DEST_GANONS_CASTLE_DISPEL_LIGHT_BEAM",
	"CS_DEST_GANONS_CASTLE_DISPEL_SPIRIT_BEAM",
	"CS_DEST_GANONS_CASTLE_DISPEL_BARRIER_CONDITONAL",
	"CS_DEST_HYRULE_FIELD_FROM_FAIRY_OCARINA",
	"CS_DEST_HYRULE_FIELD_FROM_IMPA_ESCORT",
	"CS_DEST_FROM_RAURU_FINAL_MESSAGE_CONDITIONAL",
	"CS_DEST_HYRULE_FIELD_CREDITS_SKY",
	"CS_DEST_GANON_BATTLE_TOWER_COLLAPSE",
	"CS_DEST_ZELDAS_COURTYARD_RECEIVE_LETTER",
};

typedef struct CsCommandListDescriptor
{
	const char* cmdMacro;
	const char* args;
} CsCommandListDescriptor;

class CutsceneSubCommandEntry
{
public:
	uint16_t base;
	uint16_t startFrame;
	uint16_t endFrame;
	uint16_t pad;

	uint32_t commandID;

	CutsceneSubCommandEntry(const std::vector<uint8_t>& rawData, offset_t rawDataIndex);
	virtual ~CutsceneSubCommandEntry() = default;

	virtual std::string GetBodySourceCode() const;

	virtual size_t GetRawSize() const;
};

class CutsceneCommand
{
public:
	uint32_t commandID;
	uint32_t commandIndex;

	uint32_t numEntries;
	std::vector<CutsceneSubCommandEntry*> entries;

	CutsceneCommand(const std::vector<uint8_t>& rawData, offset_t rawDataIndex);
	virtual ~CutsceneCommand();

	virtual std::string GetCommandMacro() const;
	virtual std::string GenerateSourceCode() const;
	virtual size_t GetCommandSize() const;

	virtual void SetCommandID(uint32_t nCommandID);
};

class CutsceneSubCommandEntry_GenericCmd : public CutsceneSubCommandEntry
{
public:
	CutsceneCommands commandId;

	uint32_t word0 = 0;
	uint32_t word1 = 0;

	uint32_t unused1 = 0;
	uint32_t unused2 = 0;
	uint32_t unused3 = 0;
	uint32_t unused4 = 0;
	uint32_t unused5 = 0;
	uint32_t unused6 = 0;
	uint32_t unused7 = 0;
	uint32_t unused8 = 0;
	uint32_t unused9 = 0;
	uint32_t unused10 = 0;

	CutsceneSubCommandEntry_GenericCmd(const std::vector<uint8_t>& rawData, offset_t rawDataIndex,
									   CutsceneCommands cmdId);

	std::string GetBodySourceCode() const override;

	size_t GetRawSize() const override;
};

class CutsceneCommand_GenericCmd : public CutsceneCommand
{
public:
	CutsceneCommand_GenericCmd(const std::vector<uint8_t>& rawData, offset_t rawDataIndex,
							   CutsceneCommands cmdId);

	std::string GetCommandMacro() const override;
};

class CutsceneCameraPoint : public CutsceneSubCommandEntry
{
public:
	int8_t continueFlag;
	int8_t cameraRoll;
	int16_t nextPointFrame;
	float viewAngle;
	int16_t posX, posY, posZ;
	int16_t unused;

	CutsceneCameraPoint(const std::vector<uint8_t>& rawData, offset_t rawDataIndex);

	std::string GetBodySourceCode() const override;

	size_t GetRawSize() const override;
};

class CutsceneCommand_GenericCameraCmd : public CutsceneCommand
{
public:
	uint16_t base;
	uint16_t startFrame;
	uint16_t endFrame;
	uint16_t unused;

	CutsceneCommand_GenericCameraCmd(const std::vector<uint8_t>& rawData, offset_t rawDataIndex);

	std::string GetCommandMacro() const override;

	size_t GetCommandSize() const override;
};

class CutsceneCommand_Transition : public CutsceneCommand
{
public:
	uint16_t base;
	uint16_t startFrame;
	uint16_t endFrame;

	CutsceneCommand_Transition(const std::vector<uint8_t>& rawData, offset_t rawDataIndex);

	std::string GenerateSourceCode() const override;
	size_t GetCommandSize() const override;
};

class CutsceneSubCommandEntry_Rumble : public CutsceneSubCommandEntry
{
public:
	uint8_t sourceStrength;
	uint8_t duration;
	uint8_t decreaseRate;
	uint8_t unk_09;
	uint8_t unk_0A;

	CutsceneSubCommandEntry_Rumble(const std::vector<uint8_t>& rawData, offset_t rawDataIndex);

	std::string GetBodySourceCode() const override;

	size_t GetRawSize() const override;
};

class CutsceneCommand_Rumble : public CutsceneCommand
{
public:
	CutsceneCommand_Rumble(const std::vector<uint8_t>& rawData, offset_t rawDataIndex);

	std::string GetCommandMacro() const override;
};

class CutsceneSubCommandEntry_SetTime : public CutsceneSubCommandEntry
{
public:
	uint8_t hour;
	uint8_t minute;

	CutsceneSubCommandEntry_SetTime(const std::vector<uint8_t>& rawData, offset_t rawDataIndex);

	std::string GetBodySourceCode() const override;

	size_t GetRawSize() const override;
};

class CutsceneCommand_Time : public CutsceneCommand
{
public:
	CutsceneCommand_Time(const std::vector<uint8_t>& rawData, offset_t rawDataIndex);

	std::string GetCommandMacro() const override;
};

class CutsceneSubCommandEntry_TextBox : public CutsceneSubCommandEntry
{
public:
	uint16_t type;
	uint16_t textId1;
	uint16_t textId2;

	CutsceneSubCommandEntry_TextBox(const std::vector<uint8_t>& rawData, offset_t rawDataIndex);

	std::string GetBodySourceCode() const override;

	size_t GetRawSize() const override;
};

class CutsceneCommand_Text : public CutsceneCommand
{
public:
	CutsceneCommand_Text(const std::vector<uint8_t>& rawData, offset_t rawDataIndex);

	std::string GetCommandMacro() const override;
};

class CutsceneSubCommandEntry_ActorAction : public CutsceneSubCommandEntry
{
public:
	uint16_t rotX, rotY, rotZ;
	int32_t startPosX, startPosY, startPosZ;
	int32_t endPosX, endPosY, endPosZ;
	float normalX, normalY, normalZ;

	CutsceneSubCommandEntry_ActorAction(const std::vector<uint8_t>& rawData, offset_t rawDataIndex);
	std::string GetBodySourceCode() const override;

	size_t GetRawSize() const override;
};

class CutsceneCommand_ActorCue : public CutsceneCommand
{
public:
	CutsceneCommand_ActorCue(const std::vector<uint8_t>& rawData, offset_t rawDataIndex);

	std::string GetCommandMacro() const override;
};

class CutsceneCommand_Destination : public CutsceneCommand
{
public:
	uint16_t base;
	uint16_t startFrame;
	uint16_t endFrame;
	uint16_t unknown;

	CutsceneCommand_Destination(const std::vector<uint8_t>& rawData, offset_t rawDataIndex);

	std::string GenerateSourceCode() const override;
	size_t GetCommandSize() const override;
};
