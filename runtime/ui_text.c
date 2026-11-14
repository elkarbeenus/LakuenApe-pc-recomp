// Catalog of the text the host shows the player (ui_text.h). One array per catalog language, indexed by UiTextId: a new
// string gets an id in ui_text.h and an entry in every language that has one (a missing entry falls back to English).
// The software keyboard has every language; the menu has English, Russian and German.
#include <stdatomic.h>
#include <string.h>

#include "cfg.h"
#include "ov.h"
#include "platform/platform.h"
#include "port_name.h"
#include "ui_text.h"

typedef struct { const char *lang; const char *const *text; } Row;

// The language names are the same in every row.
#define LANG_NAMES \
    [UI_MENU_LANGNAME_EN] = "English", [UI_MENU_LANGNAME_JA] = "日本語", [UI_MENU_LANGNAME_FR] = "Français", \
    [UI_MENU_LANGNAME_DE] = "Deutsch", [UI_MENU_LANGNAME_IT] = "Italiano", [UI_MENU_LANGNAME_ES] = "Español", \
    [UI_MENU_LANGNAME_NL] = "Nederlands", [UI_MENU_LANGNAME_RU] = "Русский", \
    [UI_MENU_LANGNAME_ZH_HANS] = "简体中文", [UI_MENU_LANGNAME_ZH_HANT] = "繁體中文", [UI_MENU_LANGNAME_KO] = "한국어"

// SWKBD: TITLE, PROMPT, DEFAULT_NAME (at most 9 UTF-16 units, no '@' '%' '\'), OK, CANCEL, PAD_HINT.
#define SWKBD(title, prompt, name, ok, cancel, hint) \
    [UI_SWKBD_TITLE] = title, [UI_SWKBD_PROMPT] = prompt, [UI_SWKBD_DEFAULT_NAME] = name, [UI_SWKBD_OK] = ok, \
    [UI_SWKBD_CANCEL] = cancel, [UI_SWKBD_PAD_HINT] = hint, LANG_NAMES

static const char *const text_en[UI_TEXT_COUNT] = {
    SWKBD("Text entry", "Enter a name", "Horse", "OK", "Cancel", "Controller: A = OK, B = Cancel"),

    [UI_MENU_TITLE] = "Settings",
    [UI_MENU_TAB_DISPLAY] = "Display", [UI_MENU_TAB_GRAPHICS] = "Graphics",
    [UI_MENU_TAB_GAME] = "Game", [UI_MENU_TAB_SOUND] = "Sound",
    [UI_MENU_TAB_CONTROLS] = "Controls", [UI_MENU_TAB_MENU] = "Menu", [UI_MENU_TAB_ADVANCED] = "Advanced",
    [UI_MENU_CONTINUE] = "Continue", [UI_MENU_QUIT] = "Quit game",
    [UI_MENU_ON] = "On", [UI_MENU_OFF] = "Off", [UI_MENU_AUTO] = "Automatic", [UI_MENU_IS_DEFAULT] = "Default",
    [UI_MENU_AFTER_RESTART] = "Restart required", [UI_MENU_SET_BY_ENV] = "Locked by environment variable",
    [UI_MENU_SAVE_FAILED] = "could not save settings",
    [UI_MENU_VERSION] = PORT_NAME " %s, game %s",

    [UI_MENU_FULLSCREEN] = "Fullscreen", [UI_MENU_FULLSCREEN_DESC] = "Fills the screen with the game; F11 toggles it.",
    [UI_MENU_WINDOW_MODE] = "Window mode",
    [UI_MENU_WINDOW_MODE_DESC] = "F11 switches between Windowed and the last fullscreen mode you used.",
    [UI_MENU_WINDOW_MODE_WINDOWED] = "Windowed", [UI_MENU_WINDOW_MODE_BORDERLESS] = "Borderless",
    [UI_MENU_WINDOW_MODE_FULLSCREEN] = "Fullscreen",
    [UI_MENU_WINDOW_SIZE] = "Window size", [UI_MENU_WINDOW_SIZE_DESC] = "Sets the size of the desktop window in Windowed mode.",
    [UI_MENU_WINDOW_SIZE_CUSTOM] = "Custom",
    [UI_MENU_WINDOWED_ONLY] = "Only available in windowed mode",
    [UI_MENU_PRESENT] = "Frame sync", [UI_MENU_PRESENT_DESC] = "VSync waits for the display to refresh.\n"
                             "Mailbox shows the latest finished frame at each refresh when supported.\n"
                             "Immediate sends frames as soon as they are ready and can cause tearing.",
    [UI_MENU_PRESENT_MAILBOX] = "Mailbox", [UI_MENU_PRESENT_VSYNC] = "VSync", [UI_MENU_PRESENT_IMMEDIATE] = "Immediate",
    [UI_MENU_PRESENT_IN_USE] = "in use: %s",
    [UI_MENU_GPU] = "Graphics card", [UI_MENU_GPU_DESC] = "Choose which graphics card runs the game. Use Automatic unless you need to select a specific card.",
    [UI_MENU_GPU_AUTO] = "Automatic",
    [UI_MENU_PERF_OVERLAY] = "Performance overlay",
    [UI_MENU_PERF_OVERLAY_DESC] = "Shows FPS in the top right. Full also shows a frame time graph and hardware statistics. Press F10 to cycle through the modes.",
    [UI_MENU_PERF_OFF] = "Off", [UI_MENU_PERF_FPS] = "FPS", [UI_MENU_PERF_FULL] = "Full",
    // printf format of the overlay line in FPS mode: the frame rate
    [UI_PERF_LINE_FPS] = "%.0f fps",
    [UI_MENU_RESOLUTION_GROUP] = "Resolution",
    [UI_MENU_DISPLAY_OUTPUT] = "Display output",
    [UI_MENU_RENDER_OPTIONS] = "Render Options",
    [UI_MENU_OUTPUT_RES] = "Render resolution",
    [UI_MENU_ULTRAWIDE] = "Ultrawide",
    [UI_MENU_ENABLE_ULTRAWIDE] = "Enable ultrawide",
    [UI_MENU_ULTRAWIDE_DESC] = "Widens the game view for 21:9 and 32:9 displays.",
    [UI_MENU_WIDE_RES] = "Render resolution",
    [UI_MENU_WIDE_RES_DESC] = "Sets the resolution of the game image before it is scaled to your display. "
                             "Higher resolutions can look sharper and use more GPU power and video memory.\n\n"
                             "Enable Ultrawide for 21:9 and 32:9 options.",
    [UI_MENU_HUD_POSITION] = "HUD position",
    [UI_MENU_HUD_POSITION_DESC] = "Place corner displays near the screen edges, or keep them inside a centered 16:9 "
                                 "area. Menus and aiming markers stay centered.",
    [UI_MENU_HUD_EDGES] = "Screen edges", [UI_MENU_HUD_CENTERED] = "Centered (16:9)",
    [UI_MENU_WIDE_DISABLED] = "Enable Ultrawide first",
    [UI_MENU_OUTPUT_RES_DESC] = "The size the game draws its picture and interface in. Above 1920 x 1080 the game needs "
                                "more video memory and GPU time. A wider size (21:9) widens the camera's view and moves "
                                "the HUD to the screen's corners; a taller one (16:10, Steam Deck) moves it to the top "
                                "and bottom edges.",
    [UI_MENU_RENDER_SCALE] = "3D render scale",
    [UI_MENU_RENDER_SCALE_DESC] = "Size of the 3D scene relative to the game resolution; the game's FSR scales it to "
                                  "that size. Above 100 % smooths jagged edges (supersampling) at a high GPU cost.",
    [UI_MENU_RENDER_SCALE_GAME] = "Game (83 %)",
    [UI_MENU_SCENE_RES] = "3D scene resolution",
    [UI_MENU_SCENE_RES_DESC] = "Auto lets the game scale its 3D scene by load, usually to two thirds of the render resolution "
                               "on each side, then upscale it. Full draws the 3D scene at the whole render resolution: a "
                               "sharper picture at a higher GPU cost, and the upscaler and DLSS are not used. Applies at the next start.",
    [UI_MENU_SCENE_RES_AUTO] = "Auto (game)", [UI_MENU_SCENE_RES_FULL] = "Full",
    [UI_MENU_UPSCALER] = "Upscaling method",
    [UI_MENU_UPSCALER_DESC] = "This applies when the game image is smaller than the area it fills on your screen or in your window. "
                              "AMD FSR 1 can keep edges sharper than Bilinear.",
    [UI_MENU_UPSCALER_FSR] = "AMD FSR 1", [UI_MENU_UPSCALER_BILINEAR] = "Bilinear", [UI_MENU_UPSCALER_DLSS] = "NVIDIA DLSS",
    [UI_MENU_ASPECT] = "Display scaling",
    [UI_MENU_ASPECT_DESC] = "Default keeps the game's shape and adds black bars if needed.\n"
                            "Stretch to fill uses the whole window or screen. The image stretches only if the shapes differ.",
    [UI_MENU_ASPECT_FIT] = "Default", [UI_MENU_ASPECT_STRETCH] = "Stretch to fill",
    [UI_MENU_FOV] = "Field of view",
    [UI_MENU_FOV_DESC] = "Adjusts the camera's view as a percentage of the original angle. 100% uses the game's default view.",
    [UI_MENU_SHARPNESS] = "FSR sharpness",
    [UI_MENU_SHARPNESS_DESC] = "Sharpening after FSR scaling; 0 turns it off.",
    [UI_MENU_NO_UPSCALING] = "Requires a smaller game image",
    [UI_MENU_SHARPNESS_NEEDS_FSR] = "Requires AMD FSR 1",
    [UI_MENU_SMAA] = "Anti-aliasing (SMAA)",
    [UI_MENU_SMAA_DESC] = "Smooths jagged edges in the game image and HUD. Higher settings use more GPU time.",
    [UI_MENU_SMAA_LOW] = "Low", [UI_MENU_SMAA_MEDIUM] = "Medium", [UI_MENU_SMAA_HIGH] = "High",
    [UI_MENU_FRAME_GEN] = "Frame generation",
    [UI_MENU_FRAME_GEN_DESC] = "Makes a frame between each two of the game's from their motion, so 60 fps are shown as 120. "
                               "Smoother, not faster, with a little more delay. Needs Mailbox or Immediate and a display "
                               "of at least twice the frame rate; otherwise no frames are made. Experimental.",
    [UI_MENU_FRAME_GEN_2X] = "2x",
    [UI_MENU_COLOR] = "Color",
    [UI_MENU_COLOR_PRESET] = "Color profile",
    [UI_MENU_COLOR_PRESET_DESC] = "Game default uses the original colors.\nVivid (OLED) boosts contrast and color.\n"
                                  "Clarity makes the image a little brighter and more vivid.\n\n"
                                  "Adjusting any slider switches the profile to Custom.",
    [UI_MENU_COLOR_DEFAULT] = "Game default", [UI_MENU_COLOR_VIVID] = "Vivid (OLED)", [UI_MENU_COLOR_CLARITY] = "Clarity",
    [UI_MENU_COLOR_CUSTOM] = "Custom",
    [UI_MENU_COLOR_BRIGHTNESS] = "Brightness", [UI_MENU_COLOR_BRIGHTNESS_DESC] = "Makes the image brighter or darker. The default is 0%.",
    [UI_MENU_COLOR_CONTRAST] = "Contrast",
    [UI_MENU_COLOR_CONTRAST_DESC] = "Higher values make dark areas darker and bright areas brighter. The default is 100%.",
    [UI_MENU_COLOR_SATURATION] = "Saturation",
    [UI_MENU_COLOR_SATURATION_DESC] = "Higher values make colors more vivid. The default is 100%.",
    [UI_MENU_COLOR_GAMMA] = "Gamma",
    [UI_MENU_COLOR_GAMMA_DESC] = "Below 1.00 brightens shadows and midtones. Above 1.00 darkens them.",
    [UI_MENU_HDR_GROUP] = "HDR (experimental)",
    [UI_MENU_HDR] = "HDR output",
    [UI_MENU_HDR_DESC] = "Brightens highlights in the game's SDR image on an HDR display.\n\n"
                         "Enable HDR in your system display settings first.",
    [UI_MENU_HDR_NATIVE] = "Game's own HDR",
    [UI_MENU_HDR_NATIVE_DESC] = "Uses the game’s own HDR on update 3.2.0. Highlights follow Peak brightness up to 1400 nits. "
                                "This mode disables FSR 1, SMAA, and color adjustments.",
    [UI_MENU_HDR_PEAK] = "Peak brightness",
    [UI_MENU_HDR_PEAK_DESC] = "Set the brightest highlights to match your display's peak brightness in nits.",
    [UI_MENU_HDR_GAME] = "Game brightness",
    [UI_MENU_HDR_GAME_DESC] = "Sets the brightness of ordinary whites in the game and HUD. Raise it if the picture looks too dim.",
    [UI_MENU_HDR_UI] = "Settings menu brightness",
    [UI_MENU_HDR_UI_DESC] = "Sets the brightness of whites in this settings menu. The game HUD follows Game brightness.",

    [UI_MENU_FRAME_RATE_GROUP] = "Frame rate",
    [UI_MENU_FPS_LIMIT] = "Frame rate limit",
    [UI_MENU_DKB_FPS_LIMIT_DESC] = "Caps the game’s frame rate. Monitor refresh uses the current refresh rate of your display.",
    [UI_MENU_FPS_LIMIT_DESC] = "The most frames per second the game draws. Above 60 is experimental and needs Game speed "
                               "follows the clock. Unlimited is experimental.",
    [UI_MENU_FPS_30] = "30 FPS", [UI_MENU_FPS_40] = "40 FPS", [UI_MENU_FPS_45] = "45 FPS",
    [UI_MENU_FPS_60] = "60 FPS", [UI_MENU_FPS_90] = "90 FPS", [UI_MENU_FPS_120] = "120 FPS", [UI_MENU_FPS_144] = "144 FPS",
    // the monitor's refresh rate; printf format with the rate in Hz when it is known
    [UI_MENU_FPS_DISPLAY] = "Monitor refresh", [UI_MENU_FPS_DISPLAY_HZ] = "Monitor refresh (%.0f Hz)",
    [UI_MENU_FPS_UNLIMITED] = "Unlimited",
    [UI_MENU_FPS_NEEDS_REALTIME] = "Enable \"Allow higher frame rates\" and restart",
    [UI_MENU_REALTIME] = "Game speed follows the clock",
    [UI_MENU_REALTIME_DESC] = "Steps the game by real time, so a slow frame does not slow the game down.",
    [UI_MENU_DKB_REALTIME] = "Allow higher frame rates",
    [UI_MENU_DKB_REALTIME_DESC] = "Unlocks the Frame rate limit setting, including rates above 60 FPS. "
                                "Game timing adapts as FPS changes. This mode is experimental.",
    [UI_MENU_CLOTH_REALTIME] = "Cloth on the clock",
    [UI_MENU_CLOTH_REALTIME_DESC] = "Steps cloth and hair by real time as well.",
    [UI_MENU_HIGHFPS_FIX] = "High frame rate fixes",
    [UI_MENU_HIGHFPS_FIX_DESC] = "Corrects physics that misbehave above 30 frames per second, and objects that stay see-through or invisible above 60.",
    [UI_MENU_GAME_LANG] = "Game language", [UI_MENU_GAME_LANG_DESC] = "Language of the game's text and voices.",
    [UI_MENU_GAME_LANG_AUTO] = "Automatic",
    [UI_MENU_OPERATION_MODE] = "Console mode",
    [UI_MENU_OPERATION_MODE_DESC] = "Choose which Switch mode the game uses. Docked is the TV profile. Handheld uses the portable profile.",
    [UI_MENU_MODE_DOCKED] = "Docked", [UI_MENU_MODE_HANDHELD] = "Handheld",
    [UI_MENU_ASSUME_FOCUS] = "Keep running in background",
    [UI_MENU_ASSUME_FOCUS_DESC] = "Keeps the game running while you use another window. Turn it off to pause when you switch away.\n\n"
                                  "Pausing during a load can prevent it from completing.",

    [UI_MENU_VOLUME] = "Volume", [UI_MENU_VOLUME_DESC] = "Adjusts the overall game volume. Set it to 0% to mute the game.",
    [UI_MENU_AUDIO_CHANNELS] = "Audio output",
    [UI_MENU_AUDIO_CHANNELS_DESC] = "Choose Stereo or 5.1 surround. Automatic uses 5.1 when your default audio device supports at least six channels.",
    [UI_MENU_CHANNELS_AUTO] = "Automatic", [UI_MENU_CHANNELS_STEREO] = "Stereo", [UI_MENU_CHANNELS_51] = "5.1 surround",

    [UI_MENU_CONTROLLER] = "Controller", [UI_MENU_MOUSE_KEYBOARD] = "Mouse & Keyboard",
    [UI_MENU_MOTION] = "Motion controls", [UI_MENU_MOTION_DESC] = "Aiming with the controller's gyroscope.",
    [UI_MENU_RUMBLE_STRENGTH] = "Vibration strength",
    [UI_MENU_RUMBLE_STRENGTH_DESC] = "How strong the controller vibrates.",
    [UI_MENU_RUMBLE_CURVE] = "Vibration curve",
    [UI_MENU_RUMBLE_CURVE_DESC] = "Lower values make subtle vibration easier to feel.",
    [UI_MENU_RUMBLE_TRIGGERS] = "Trigger vibration (Xbox)",
    [UI_MENU_RUMBLE_TRIGGERS_DESC] = "Enables vibration in the triggers on supported Xbox controllers.",
    [UI_MENU_PAD_LAYOUT] = "Button layout",
    [UI_MENU_PAD_LAYOUT_DESC] = "Automatic picks Nintendo for Nintendo-style controllers and Xbox otherwise.\nNintendo uses the right button for A.\n"
                                "Xbox uses the bottom button for A.\nXbox prompts uses Nintendo positions with Xbox labels.\n\n"
                                "The game still shows the original prompt for any button you remap.",
    [UI_MENU_PAD_LAYOUT_AUTO] = "Automatic", [UI_MENU_PAD_LAYOUT_NINTENDO] = "Nintendo (A right)",
    [UI_MENU_PAD_LAYOUT_XBOX] = "Xbox (A bottom)", [UI_MENU_PAD_LAYOUT_XBOX_PROMPTS] = "Xbox prompts (A right)",
    [UI_MENU_PAD_LAYOUT_IN_USE] = "in use: %s",
    [UI_MENU_PAD_LAYOUT_PROMPTS_MISSING] = "prompt files missing: run the setup again",
    [UI_MENU_PAD_LAYOUT_PROMPTS_RESTART] = "prompts change after restart",
    [UI_MENU_STICK_DEADZONE] = "Stick dead zone",
    [UI_MENU_STICK_DEADZONE_DESC] = "Sets how far either stick must move before it registers. Raise it if the character or camera drifts.",
    [UI_MENU_MOUSE_CAMERA] = "Direct mouse camera",
    [UI_MENU_MOUSE_CAMERA_DESC] = "Mouse movement turns the follow camera directly. Other camera modes interpret mouse movement as stick input.",
    [UI_MENU_MOUSE_SENS] = "Mouse sensitivity",
    [UI_MENU_MOUSE_SENS_DESC] = "Sets how far the camera turns when you move the mouse.",

    [UI_BINDS_HELP_KEYS] = "Click a binding or press Enter, then press a key or mouse button. Delete or Backspace clears it. "
                           "A key already in use moves to the new action.",
    [UI_BINDS_HELP_PAD] = "Press A on a binding, then press a controller button. X clears it. Buttons already in use "
                          "swap. The left stick moves and the right stick controls the camera.",
    [UI_BINDS_COL_KEY1] = "Primary", [UI_BINDS_COL_KEY2] = "Secondary",
    [UI_BINDS_GROUP_BUTTONS] = "Buttons", [UI_BINDS_GROUP_MOVE] = "Movement", [UI_BINDS_GROUP_CAMERA] = "Camera",
    [UI_BINDS_PRESS_KEY] = "Press a key or mouse button for %s",
    [UI_BINDS_PRESS_PAD] = "Press a controller button for %s",
    [UI_BINDS_CANCEL_HINT] = "Esc cancels, or wait %d s",
    [UI_BINDS_TAKEN] = "%s was taken from %s", [UI_BINDS_SWAPPED] = "%s swapped with %s",
    [UI_BINDS_RESET] = "Reset all bindings",
    [UI_BINDS_RESET_DESC] = "Back to the default keys and controller buttons. The button layout above applies again: remapped controller buttons stay where you put them until this reset.",
    [UI_BIND_A] = "A", [UI_BIND_B] = "B", [UI_BIND_X] = "X", [UI_BIND_Y] = "Y",
    [UI_BIND_LCLICK] = "Left stick click", [UI_BIND_RCLICK] = "Right stick click", [UI_BIND_L] = "L",
    [UI_BIND_R] = "R", [UI_BIND_ZL] = "ZL", [UI_BIND_ZR] = "ZR", [UI_BIND_PLUS] = "Plus", [UI_BIND_MINUS] = "Minus",
    [UI_BIND_LEFT] = "D-pad left", [UI_BIND_UP] = "D-pad up", [UI_BIND_RIGHT] = "D-pad right",
    [UI_BIND_DOWN] = "D-pad down",
    [UI_BIND_LSTICK_LEFT] = "Move left", [UI_BIND_LSTICK_UP] = "Move forward", [UI_BIND_LSTICK_RIGHT] = "Move right",
    [UI_BIND_LSTICK_DOWN] = "Move back",
    [UI_BIND_RSTICK_LEFT] = "Camera left", [UI_BIND_RSTICK_UP] = "Camera up", [UI_BIND_RSTICK_RIGHT] = "Camera right",
    [UI_BIND_RSTICK_DOWN] = "Camera down",
    [UI_BIND_DESC_A] = "Interact, pick up, talk.", [UI_BIND_DESC_B] = "Run, cancel, go back.",
    [UI_BIND_DESC_X] = "Jump.", [UI_BIND_DESC_Y] = "Attack.", [UI_BIND_DESC_LCLICK] = "Crouch.",
    [UI_BIND_DESC_RCLICK] = "First-person view.", [UI_BIND_DESC_L] = "Abilities.",
    [UI_BIND_DESC_R] = "Throw a weapon.", [UI_BIND_DESC_ZL] = "Shield, lock on.", [UI_BIND_DESC_ZR] = "Bow.",
    [UI_BIND_DESC_PLUS] = "Menu, inventory.", [UI_BIND_DESC_MINUS] = "Map.",
    [UI_BIND_DESC_DPAD] = "Quick-select.",
    [UI_BIND_DESC_MOVE] = "Moves Link like the left stick. The controller's left stick always works too.",
    [UI_BIND_DESC_CAMERA] = "Turns the camera like the right stick. The mouse and the controller's right stick always "
                            "work too.",
    [UI_DKB_BIND_A] = "Jump. Confirms selections in menus.",
    [UI_DKB_BIND_B] = "Punch downward. Goes back in menus.",
    [UI_DKB_BIND_X] = "Punch upward.",
    [UI_DKB_BIND_Y] = "Punch forward or swing the chunk you're holding.",
    [UI_DKB_BIND_LCLICK] = "Use the left stick press action assigned in Bananza's controls.",
    [UI_DKB_BIND_RCLICK] = "Use the right stick press action assigned in Bananza's controls.",
    [UI_DKB_BIND_L] = "Hold to whistle and reveal the route to your objective.",
    [UI_DKB_BIND_R] = "Use Hand Slap to collect nearby items and reveal hidden objects.",
    [UI_DKB_BIND_ZL] = "Roll forward or Turf Surf while holding a chunk.",
    [UI_DKB_BIND_ZR] = "Grab a chunk of terrain or throw the chunk you're holding.",
    [UI_DKB_BIND_PLUS] = "Open the pause menu.",
    [UI_DKB_BIND_MINUS] = "Open the map.",
    [UI_DKB_BIND_LEFT] = "Cycle to the previous unlocked Bananza form.",
    [UI_DKB_BIND_UP] = "Open the Skills menu.",
    [UI_DKB_BIND_RIGHT] = "Cycle to the next unlocked Bananza form.",
    [UI_DKB_BIND_DOWN] = "Open Photo Mode when available.",
    [UI_DKB_BIND_MOVE] = "Move Donkey Kong in this direction.",
    [UI_DKB_BIND_CAMERA] = "Turn the camera in this direction.",
    [UI_KEY_SPACE] = "Space", [UI_KEY_TAB] = "Tab", [UI_KEY_LSHIFT] = "Left Shift", [UI_KEY_LCTRL] = "Left Ctrl",
    [UI_KEY_UP] = "Up arrow", [UI_KEY_DOWN] = "Down arrow", [UI_KEY_LEFT] = "Left arrow",
    [UI_KEY_RIGHT] = "Right arrow",
    [UI_KEY_MOUSE_L] = "Left mouse", [UI_KEY_MOUSE_R] = "Right mouse", [UI_KEY_MOUSE_M] = "Middle mouse",
    [UI_KEY_MOUSE_X1] = "Mouse 4", [UI_KEY_MOUSE_X2] = "Mouse 5",
    [UI_KEY_ENTER] = "Enter", [UI_KEY_BACKSPACE] = "Backspace", [UI_KEY_RSHIFT] = "Right Shift",
    [UI_KEY_RCTRL] = "Right Ctrl", [UI_KEY_LALT] = "Alt", [UI_KEY_CAPSLOCK] = "Caps Lock", [UI_KEY_INSERT] = "Insert",
    [UI_KEY_DELETE] = "Delete", [UI_KEY_HOME] = "Home", [UI_KEY_END] = "End", [UI_KEY_PAGEUP] = "Page Up",
    [UI_KEY_PAGEDOWN] = "Page Down", [UI_KEY_NUM_FMT] = "Num %s",
    [UI_PAD_SOUTH] = "Bottom button", [UI_PAD_EAST] = "Right button", [UI_PAD_WEST] = "Left button",
    [UI_PAD_NORTH] = "Top button", [UI_PAD_LSTICK] = "Left stick", [UI_PAD_RSTICK] = "Right stick",
    [UI_PAD_L] = "Left bumper", [UI_PAD_R] = "Right bumper", [UI_PAD_START] = "Start", [UI_PAD_BACK] = "Back",
    [UI_PAD_DPAD_LEFT] = "D-pad left", [UI_PAD_DPAD_UP] = "D-pad up", [UI_PAD_DPAD_RIGHT] = "D-pad right",
    [UI_PAD_DPAD_DOWN] = "D-pad down", [UI_PAD_ZL] = "Left trigger", [UI_PAD_ZR] = "Right trigger",
    [UI_PAD_LPADDLE1] = "Left back button", [UI_PAD_RPADDLE1] = "Right back button",
    [UI_PAD_LPADDLE2] = "Left back button 2", [UI_PAD_RPADDLE2] = "Right back button 2",
    [UI_PAD_CROSS] = "Cross", [UI_PAD_CIRCLE] = "Circle", [UI_PAD_SQUARE] = "Square", [UI_PAD_TRIANGLE] = "Triangle",

    [UI_MENU_MENU_LANG] = "Settings language", [UI_MENU_MENU_LANG_DESC] = "Sets the language of the settings menu and performance overlay. System default uses "
                             "German or Russian when your system language is one of them, and English otherwise.",
    [UI_MENU_MENU_LANG_AUTO] = "System default",
    [UI_MENU_THEME] = "Menu theme", [UI_MENU_THEME_DK] = "Donkey Kong", [UI_MENU_THEME_CLASSIC] = "Classic",
    [UI_MENU_THEME_DESC] = "Choose the Donkey Kong theme or the classic green and gold theme.",

    [UI_MENU_ADV_WARNING] = "These settings are for testing and troubleshooting. Change them only when you know why.",
    [UI_MENU_GROUP_SCHED] = "Scheduling", [UI_MENU_GROUP_SHADERS] = "Shaders and pipelines",
    [UI_MENU_GROUP_TEXMEM] = "Textures and memory", [UI_MENU_GROUP_FIXES] = "Render fixes",
    [UI_MENU_GROUP_VULKAN] = "Vulkan", [UI_MENU_GROUP_DIAG] = "Diagnostics",
    [UI_MENU_ASYNC_PREEMPT] = "Virtual core preemption",
    [UI_MENU_ASYNC_PREEMPT_DESC] = "Lets a game thread interrupt another one on the same virtual core.",
    [UI_MENU_PREEMPT_OFF] = "Off", [UI_MENU_PREEMPT_ON] = "On", [UI_MENU_PREEMPT_INHERIT] = "Inheritance only",
    [UI_MENU_REPLAY_ASYNC] = "GPU frame prep on its own thread",
    [UI_MENU_REPLAY_ASYNC_DESC] = "Prepares frames for the GPU on a separate thread instead of the game thread. Adds up to one frame of latency.",
    [UI_MENU_REPLAY_THREADS] = "Threads recording big render passes",
    [UI_MENU_REPLAY_THREADS_DESC] = "Records the draws of big render passes on this many threads at once. Automatic uses 4 threads on 8 or more logical processors (not on AMD); Off records them on one thread. Faster in busy areas on CPUs with many cores.",
    [UI_MENU_TIMER_1MS] = "1 ms Windows timer",
    [UI_MENU_TIMER_1MS_DESC] = "Raises the system timer resolution to 1 ms for steadier frame pacing.",
    [UI_MENU_PIPE_WAIT] = "Waiting draw handling",
    [UI_MENU_PIPE_WAIT_DESC] = "Default keeps the current scheduling with a reserved worker for waiting draws; Old (draw) uses the previous scheduling.",
    [UI_MENU_PIPE_WAIT_DEFAULT] = "Default", [UI_MENU_PIPE_WAIT_DRAW] = "Old (draw)",
    [UI_MENU_PIPE_SKIP] = "Draws skipped while a pipeline builds",
    [UI_MENU_PIPE_SKIP_DESC] = "Which draws are dropped instead of waiting for their pipeline.",
    [UI_MENU_PIPE_SKIP_STRICT] = "Strict", [UI_MENU_PIPE_SKIP_ANY] = "Any",
    [UI_MENU_PIPE_SKIP_FULLSCREEN] = "Fullscreen passes",
    [UI_MENU_LIB_PREBUILD] = "Stage library prebuild",
    [UI_MENU_LIB_PREBUILD_DESC] = "Builds shader stage libraries ahead of time in the background.",
    [UI_MENU_LIB_PREBUILD_THREADS] = "Stage library prebuild threads",
    [UI_MENU_LIB_PREBUILD_THREADS_DESC] = "Threads used for the prebuild; Automatic picks a number from your CPU.",
    [UI_MENU_PIPE_PREBUILD] = "Pipeline prebuild",
    [UI_MENU_PIPE_PREBUILD_DESC] = "Builds known pipelines ahead of time.",
    [UI_MENU_PREWARM_XLATE] = "Translator prewarm",
    [UI_MENU_PREWARM_XLATE_DESC] = "Threads that translate known shaders at start; Automatic takes half the CPU threads, at most 16.",
    [UI_MENU_GPL] = "Graphics pipeline library",
    [UI_MENU_GPL_DESC] = "Uses the Vulkan pipeline library extension; turn off if your driver misbehaves with it.",
    [UI_MENU_ASTC] = "ASTC texture decoding", [UI_MENU_ASTC_DESC] = "Where compressed ASTC textures are decoded.",
    [UI_MENU_ASTC_AUTO] = "Automatic", [UI_MENU_ASTC_GPU] = "GPU shader", [UI_MENU_ASTC_NATIVE] = "Native",
    [UI_MENU_ASTC_CPU] = "CPU",
    [UI_MENU_MEMORY] = "GPU memory mode", [UI_MENU_MEMORY_DESC] = "How game memory is shared with the GPU.",
    [UI_MENU_MEMORY_AUTO] = "Automatic", [UI_MENU_MEMORY_IMPORT] = "Import", [UI_MENU_MEMORY_MIRROR] = "Mirror",
    [UI_MENU_MEMORY_VERIFY] = "Verify",
    [UI_MENU_TEX_ALIAS] = "Texture aliasing", [UI_MENU_TEX_ALIAS_DESC] = "A new texture created over a live texture's memory starts with that content instead of empty.",
    [UI_MENU_TEX_VRAM_PRESSURE] = "Texture memory limit", [UI_MENU_TEX_VRAM_PRESSURE_DESC] = "Percent of video memory above which textures the game released are dropped (they come back from storage when used again). 0 keeps them.",
    [UI_MENU_ASTC_BC] = "Compress ASTC to BC3", [UI_MENU_ASTC_BC_DESC] = "With the GPU decoder: re-encodes decoded ASTC textures to BC3, a quarter of the video memory. For GPUs with 4 GB or less; textures look slightly blockier.",
    [UI_MENU_LOD_GUARD] = "LOD guard", [UI_MENU_LOD_GUARD_DESC] = "Prevents objects flashing in the wrong place or a flat far field for a frame, by waiting for the game to finish rewriting level-of-detail data before the GPU reads it.",
    [UI_MENU_FIX_C0] = "Constant buffer fix",
    [UI_MENU_FIX_C0_DESC] = "Fills the driver-reserved constant bank c0 (storage sizes, dispatch size, point size, base instance) instead of reading zeros.",
    [UI_MENU_FIX_ZERO] = "Zero new GPU memory",
    [UI_MENU_FIX_ZERO_DESC] = "New GPU memory pools start zero-filled, as Switch memory does.",
    [UI_MENU_LIST_RESTART] = "Primitive restart on lists",
    [UI_MENU_LIST_RESTART_DESC] = "Applies the game's primitive restart to triangle-list draws, as the Switch does. Without it the restart indices are drawn as vertices: long stretched triangles, for example on the map. Needs a GPU with primitiveTopologyListRestart.",
    [UI_MENU_BIND_SKIP] = "Skip repeated binds",
    [UI_MENU_BIND_SKIP_DESC] = "Optimisation: skips bindings that did not change.",
    [UI_MENU_SHADER_MEMO] = "Shader memo", [UI_MENU_SHADER_MEMO_DESC] = "Optimisation: remembers shader lookups.",
    [UI_MENU_PIPE_MEMO] = "Pipeline memo", [UI_MENU_PIPE_MEMO_DESC] = "Optimisation: reuses the previous draw's pipeline when its state repeats.",
    [UI_MENU_TEX_MEMO] = "Texture binding memo", [UI_MENU_TEX_MEMO_DESC] = "Optimisation: remembers texture descriptors between draws.",
    [UI_MENU_DESC_TEMPLATE] = "Descriptor templates",
    [UI_MENU_DESC_TEMPLATE_DESC] = "Optimisation: updates descriptors in bulk.",
    [UI_MENU_PUSH_SKIP] = "Skip repeated push constants",
    [UI_MENU_PUSH_SKIP_DESC] = "Optimisation: skips push constants that did not change.",
    [UI_MENU_SPLIT_DRAWS] = "Early GPU submission",
    [UI_MENU_SPLIT_DRAWS_DESC] = "Submits GPU work in parts every N draws between passes so the GPU starts earlier; 0 turns it off.",
    [UI_MENU_VK_VALIDATION] = "Validation layer",
    [UI_MENU_VK_VALIDATION_DESC] = "Checks every Vulkan call and slows the game down a lot.",
    [UI_MENU_VK_VALIDATION_MISSING] = "The validation layer is not installed.",
    [UI_MENU_VK_DYN_VERTEX] = "Dynamic vertex state",
    [UI_MENU_VK_DYN_VERTEX_DESC] = "Forces dynamic vertex input on or off; the default depends on the GPU.",
    [UI_MENU_VK_DYN_A2C] = "Dynamic alpha to coverage",
    [UI_MENU_VK_DYN_A2C_DESC] = "Forces dynamic alpha to coverage on or off. Default: off on AMD's driver for RDNA 2 and "
                                "older, which draws characters black with it, on elsewhere.",
    [UI_MENU_VK_SUBGROUP_PIN] = "Pin subgroup size", [UI_MENU_VK_SUBGROUP_PIN_DESC] = "Fixes the shader subgroup size.",
    [UI_MENU_TRACE] = "Performance log", [UI_MENU_TRACE_DESC] = "Writes frame timings to a file.",
    [UI_MENU_FLIGHT] = "Flight recorder",
    [UI_MENU_FLIGHT_DESC] = "Keeps a short history and saves it when a frame freezes.",
    [UI_MENU_FLIGHT_SPIKE_MS] = "Freeze threshold (ms)",
    [UI_MENU_FLIGHT_SPIKE_MS_DESC] = "Frame time that counts as a freeze.",
    [UI_MENU_INPUT_RECORD] = "Input recording",
    [UI_MENU_INPUT_RECORD_DESC] = "Writes your inputs to a file for bug reports.",
    [UI_MENU_OPEN_LOG] = "Open log folder", [UI_MENU_OPEN_LOG_DESC] = "Shows the folder with the log files.",
    [UI_MENU_COPY_INFO] = "Copy system info",
    [UI_MENU_COPY_INFO_DESC] = "Copies GPU, driver, CPU, RAM and changed settings to the clipboard.",
    [UI_MENU_COPY_INFO_DONE] = "Copied", [UI_MENU_ACTION_FAILED] = "did not work",
    [UI_MENU_RESET] = "Reset all settings to defaults", [UI_MENU_RESET_DESC] = "Applies to every tab.",
    [UI_MENU_RESET_CONFIRM] = "Reset every setting to its default?", [UI_MENU_RESET_YES] = "Reset",
    [UI_MENU_RESET_NO] = "Cancel",

    [UI_MENU_OV_FPS] = "%.0f FPS", [UI_MENU_OV_FRAME_MS] = "%.1f ms", [UI_MENU_OV_GAME_PCT] = "game %.0f%%",
    [UI_MENU_OV_MAX_MS] = "max %.1f ms",
    [UI_MENU_OV_GPU] = "GPU", [UI_MENU_OV_CPU] = "CPU", [UI_MENU_OV_RAM] = "RAM", [UI_MENU_OV_VRAM] = "VRAM",
    [UI_MENU_OV_TEMP] = "Temperature", [UI_MENU_OV_POWER] = "Power", [UI_MENU_OV_CLOCK] = "Clock",
    [UI_MENU_OV_NONE] = "-",
    [UI_MENU_OV_REPLAY] = "GPU frame prep", [UI_MENU_OV_GPU_TIME] = "GPU busy per frame",
    [UI_MENU_OV_PIPE_WAIT] = "Pipeline waits",
    [UI_MENU_OV_SHADER_WAIT] = "Shader waits", [UI_MENU_OV_TEX_UPLOADS] = "Texture uploads",
    [UI_MENU_OV_FMT_PCT] = "%.0f%%", [UI_MENU_OV_FMT_TEMP] = "%.0f °C", [UI_MENU_OV_FMT_POWER] = "%.0f W",
    [UI_MENU_OV_FMT_MHZ] = "%.0f MHz", [UI_MENU_OV_FMT_GB] = "%.1f GB", [UI_MENU_OV_FMT_GB_OF] = "/ %.1f GB",
    [UI_MENU_OV_GAME_GB] = "game %.1f GB", [UI_MENU_OV_FMT_MS] = "%.2f ms", [UI_MENU_OV_PER_FRAME] = "%.1f per frame",
    [UI_MENU_OV_WINDOW] = "Window", [UI_MENU_OV_OUTPUT] = "Game image", [UI_MENU_OV_SCENE] = "3D scene",
    [UI_MENU_OV_SCALING] = "Scaling", [UI_MENU_OV_LIMIT] = "Frame limit", [UI_MENU_OV_PRESENT] = "Presentation",
    [UI_MENU_OV_FMT_SIZE] = "%ux%u", [UI_MENU_OV_FMT_SCENE] = "%ux%u, %.0f%%",
    [UI_MENU_OV_FMT_UPSCALE] = "%ux%u -> %ux%u",
    [UI_MENU_OV_NO_UPSCALER] = "no upscaler",
    [UI_MENU_OV_FMT_FSR] = "AMD FSR 1, sharpness %d%%", [UI_MENU_OV_SCALE_NONE] = "none (1:1)",
    [UI_MENU_OV_SCALE_BILINEAR] = "bilinear",
};

static const char *const text_ru[UI_TEXT_COUNT] = {
    SWKBD("Ввод текста", "Введите имя", "Лошадь", "ОК", "Отмена", "Контроллер: A — ОК, B — отмена"),

    [UI_MENU_TITLE] = "Настройки",
    [UI_MENU_TAB_DISPLAY] = "Экран", [UI_MENU_TAB_GRAPHICS] = "Графика",
    [UI_MENU_TAB_GAME] = "Игра", [UI_MENU_TAB_SOUND] = "Звук",
    [UI_MENU_TAB_CONTROLS] = "Управление", [UI_MENU_TAB_MENU] = "Меню", [UI_MENU_TAB_ADVANCED] = "Дополнительно",
    [UI_MENU_CONTINUE] = "Закрыть настройки", [UI_MENU_QUIT] = "Выйти из игры",
    [UI_MENU_ON] = "Вкл.", [UI_MENU_OFF] = "Выкл.", [UI_MENU_AUTO] = "Автоматически",
    [UI_MENU_IS_DEFAULT] = "По умолчанию",
    [UI_MENU_AFTER_RESTART] = "Требуется перезапуск", [UI_MENU_SET_BY_ENV] = "Задано переменной окружения",
    [UI_MENU_SAVE_FAILED] = "не удалось сохранить настройки",
    [UI_MENU_VERSION] = PORT_NAME " %s, игра %s",

    [UI_MENU_FULLSCREEN] = "Полный экран", [UI_MENU_FULLSCREEN_DESC] = "Игра на весь экран; F11 переключает режим.",
    [UI_MENU_WINDOW_MODE] = "Режим окна",
    [UI_MENU_WINDOW_MODE_DESC] = "F11 переключает между оконным и последним выбранным полноэкранным режимом.",
    [UI_MENU_WINDOW_MODE_WINDOWED] = "В окне", [UI_MENU_WINDOW_MODE_BORDERLESS] = "Без рамки",
    [UI_MENU_WINDOW_MODE_FULLSCREEN] = "Полный экран",
    [UI_MENU_WINDOW_SIZE] = "Размер окна", [UI_MENU_WINDOW_SIZE_DESC] = "Размер окна игры вне полноэкранного режима.",
    [UI_MENU_WINDOW_SIZE_CUSTOM] = "Другой",
    [UI_MENU_WINDOWED_ONLY] = "Доступно только в оконном режиме",
    [UI_MENU_PRESENT] = "Синхронизация кадров", [UI_MENU_PRESENT_DESC] = "Вертикальная синхронизация ждёт обновления экрана.\n"
                             "Mailbox показывает последний готовый кадр при обновлении, если режим поддерживается.\n"
                             "Без ожидания кадры выводятся сразу, но возможны разрывы изображения.",
    [UI_MENU_PRESENT_MAILBOX] = "Mailbox", [UI_MENU_PRESENT_VSYNC] = "Вертикальная синхронизация",
    [UI_MENU_PRESENT_IMMEDIATE] = "Без ожидания",
    [UI_MENU_PRESENT_IN_USE] = "сейчас: %s",
    [UI_MENU_GPU] = "Видеокарта", [UI_MENU_GPU_DESC] = "Устройство Vulkan, которое рисует игру.",
    [UI_MENU_GPU_AUTO] = "Автоматически",
    [UI_MENU_PERF_OVERLAY] = "Индикатор производительности",
    [UI_MENU_PERF_OVERLAY_DESC] = "Частота кадров в правом верхнем углу. «Полный» добавляет время кадров с графиком, "
                                  "загрузку ЦП, GPU и памяти, температуры и ожидания потока отрисовки. F10 переключает "
                                  "режимы.",
    [UI_MENU_PERF_OFF] = "Выкл.", [UI_MENU_PERF_FPS] = "FPS", [UI_MENU_PERF_FULL] = "Полный",
    [UI_PERF_LINE_FPS] = "%.0f к/с",
    [UI_MENU_RESOLUTION_GROUP] = "Разрешение",
    [UI_MENU_DISPLAY_OUTPUT] = "Вывод изображения",
    [UI_MENU_RENDER_OPTIONS] = "Настройки рендеринга",
    [UI_MENU_OUTPUT_RES] = "Разрешение рендеринга",
    [UI_MENU_ULTRAWIDE] = "Ультраширокий экран",
    [UI_MENU_ENABLE_ULTRAWIDE] = "Включить ультраширокий режим",
    [UI_MENU_ULTRAWIDE_DESC] = "Расширяет обзор в игре на экранах 21:9 и 32:9.",
    [UI_MENU_WIDE_RES] = "Разрешение отрисовки",
    [UI_MENU_WIDE_RES_DESC] = "Разрешение игры до масштабирования под окно или экран. Более высокое улучшает "
                             "детализацию, но требует больше ресурсов GPU и видеопамяти. "
                             "Ультраширокий режим добавляет варианты 21:9 и 32:9.",
    [UI_MENU_HUD_POSITION] = "Положение интерфейса",
    [UI_MENU_HUD_POSITION_DESC] = "Расположить угловые индикаторы у краёв экрана или в центральной области 16:9. "
                                 "Меню и прицел остаются в центре.",
    [UI_MENU_HUD_EDGES] = "У краёв экрана", [UI_MENU_HUD_CENTERED] = "По центру (16:9)",
    [UI_MENU_WIDE_DISABLED] = "Сначала включите ультраширокий экран",
    [UI_MENU_OUTPUT_RES_DESC] = "Размер, в котором игра рисует картинку и интерфейс. Выше 1920 × 1080 игре нужно больше "
                                "видеопамяти и времени GPU. Более широкий размер (21:9) расширяет обзор камеры и "
                                "переносит интерфейс в углы экрана, более высокий (16:10, Steam Deck) переносит его к "
                                "верхнему и нижнему краям.",
    [UI_MENU_RENDER_SCALE] = "Масштаб 3D-отрисовки",
    [UI_MENU_RENDER_SCALE_DESC] = "Размер 3D-сцены относительно разрешения игры; FSR игры доводит её до этого размера. "
                                  "Выше 100 % сглаживает лесенки на краях (суперсэмплинг) ценой большой нагрузки на GPU.",
    [UI_MENU_RENDER_SCALE_GAME] = "Как в игре (83 %)",
    [UI_MENU_SCENE_RES] = "Разрешение 3D-сцены",
    [UI_MENU_SCENE_RES_DESC] = "Авто: игра сама меняет размер 3D-сцены по нагрузке, обычно до двух третей разрешения рендеринга "
                               "по каждой стороне, и затем растягивает её. Полное: 3D-сцена рисуется в полном разрешении "
                               "рендеринга, картинка чётче, но нагрузка на GPU выше; апскейлер и DLSS не работают. Действует после перезапуска.",
    [UI_MENU_SCENE_RES_AUTO] = "Авто (игра)", [UI_MENU_SCENE_RES_FULL] = "Полное",
    [UI_MENU_UPSCALER] = "Масштабирование под окно",
    [UI_MENU_UPSCALER_DESC] = "Этот параметр действует, когда изображение игры меньше области вывода на экране или в окне. "
                              "AMD FSR 1 может сохранять края более чёткими, чем билинейное масштабирование.",
    [UI_MENU_UPSCALER_FSR] = "AMD FSR 1", [UI_MENU_UPSCALER_BILINEAR] = "Билинейное", [UI_MENU_UPSCALER_DLSS] = "NVIDIA DLSS",
    [UI_MENU_ASPECT] = "Масштабирование вывода",
    [UI_MENU_ASPECT_DESC] = "По умолчанию пропорции игры сохраняются, при необходимости появляются чёрные полосы.\n"
                            "В режиме «Растянуть» изображение заполняет всё окно или экран. Оно растягивается, только "
                            "если пропорции изображения и окна или экрана различаются.",
    [UI_MENU_ASPECT_FIT] = "По умолчанию", [UI_MENU_ASPECT_STRETCH] = "Растянуть",
    [UI_MENU_FOV] = "Поле зрения",
    [UI_MENU_FOV_DESC] = "Меняет угол обзора камеры в процентах от исходного. При 100 % используется исходный угол.",
    [UI_MENU_SHARPNESS] = "Резкость FSR",
    [UI_MENU_SHARPNESS_DESC] = "Повышение резкости после масштабирования FSR; 0 выключает его.",
    [UI_MENU_NO_UPSCALING] = "Нужно меньшее разрешение игры",
    [UI_MENU_SHARPNESS_NEEDS_FSR] = "Требуется AMD FSR 1",
    [UI_MENU_SMAA] = "Сглаживание (SMAA)",
    [UI_MENU_SMAA_DESC] = "Сглаживает лесенки на готовой картинке после масштабирования под окно и до отрисовки этого "
                          "меню. Интерфейс игры тоже сглаживается. Чем выше качество, тем больше нагрузка на GPU.",
    [UI_MENU_SMAA_LOW] = "Низкое", [UI_MENU_SMAA_MEDIUM] = "Среднее", [UI_MENU_SMAA_HIGH] = "Высокое",
    [UI_MENU_FRAME_GEN] = "Генерация кадров",
    [UI_MENU_FRAME_GEN_DESC] = "Создаёт кадр между каждыми двумя кадрами игры по их движению: 60 кадров/с показываются как 120. "
                               "Плавнее, но не быстрее, с немного большей задержкой. Нужны Mailbox или «Немедленно» и экран "
                               "с частотой не ниже удвоенной частоты кадров, иначе кадры не создаются. Экспериментально.",
    [UI_MENU_FRAME_GEN_2X] = "2x",
    [UI_MENU_COLOR] = "Цвет",
    [UI_MENU_COLOR_PRESET] = "Цветовой профиль",
    [UI_MENU_COLOR_PRESET_DESC] = "Задаёт четыре значения ниже. «Яркий»: больше контраста и насыщенности, тени немного "
                                  "светлее (для OLED-экранов). «Чёткость»: немного больше контраста, насыщенности и "
                                  "яркости. Изменение любого значения делает профиль «Своим». Интерфейс игры меняется "
                                  "вместе с картинкой, это меню — нет.",
    [UI_MENU_COLOR_DEFAULT] = "Как в игре", [UI_MENU_COLOR_VIVID] = "Яркий (OLED)", [UI_MENU_COLOR_CLARITY] = "Чёткость",
    [UI_MENU_COLOR_CUSTOM] = "Свой",
    [UI_MENU_COLOR_BRIGHTNESS] = "Яркость", [UI_MENU_COLOR_BRIGHTNESS_DESC] = "Прибавляется к каждому цвету; 0 — без изменений.",
    [UI_MENU_COLOR_CONTRAST] = "Контраст",
    [UI_MENU_COLOR_CONTRAST_DESC] = "Отдаляет цвета от среднего серого (выше 100 %) или приближает к нему; 100 % — без "
                                    "изменений.",
    [UI_MENU_COLOR_SATURATION] = "Насыщенность",
    [UI_MENU_COLOR_SATURATION_DESC] = "Сила цвета относительно серого; 100 % — без изменений.",
    [UI_MENU_COLOR_GAMMA] = "Гамма",
    [UI_MENU_COLOR_GAMMA_DESC] = "Ниже 1 осветляет тёмные и средние тона, выше 1 затемняет их; 1 — без изменений.",
    [UI_MENU_HDR_GROUP] = "HDR (экспериментально)",
    [UI_MENU_HDR] = "Вывод HDR",
    [UI_MENU_HDR_DESC] = "Делает светлые участки SDR-изображения ярче на HDR-дисплее.\n\n"
                         "Сначала включите HDR в настройках экрана системы.",
    [UI_MENU_HDR_NATIVE] = "Собственный HDR игры",
    [UI_MENU_HDR_NATIVE_DESC] = "Включает собственный HDR игры в версии 3.2.0. Яркость бликов ограничена пиковой яркостью и 1400 нит. "
                                "В этом режиме отключены FSR 1, SMAA и цветокоррекция.",
    [UI_MENU_HDR_PEAK] = "Пиковая яркость",
    [UI_MENU_HDR_PEAK_DESC] = "Пик вашего дисплея в нитах; самые яркие блики останавливаются на нём.",
    [UI_MENU_HDR_GAME] = "Яркость игры",
    [UI_MENU_HDR_GAME_DESC] = "Ниты белого цвета картинки (paper white). 203 — эталон HDR; в светлой комнате "
                              "поднимите.",
    [UI_MENU_HDR_UI] = "Яркость меню",
    [UI_MENU_HDR_UI_DESC] = "Ниты белого цвета этого меню. Интерфейс игры — часть картинки и следует яркости игры.",

    [UI_MENU_FRAME_RATE_GROUP] = "Частота кадров",
    [UI_MENU_FPS_LIMIT] = "Ограничение частоты кадров",
    [UI_MENU_DKB_FPS_LIMIT_DESC] = "Ограничивает частоту кадров игры. Вариант «Частота экрана» использует текущую частоту экрана.",
    [UI_MENU_FPS_LIMIT_DESC] = "Наибольшее число кадров в секунду. Выше 60 — экспериментальный режим, нужна «Скорость "
                               "игры по часам». «Без ограничения» — экспериментально.",
    [UI_MENU_FPS_30] = "30 FPS", [UI_MENU_FPS_40] = "40 FPS", [UI_MENU_FPS_45] = "45 FPS",
    [UI_MENU_FPS_60] = "60 FPS", [UI_MENU_FPS_90] = "90 FPS", [UI_MENU_FPS_120] = "120 FPS", [UI_MENU_FPS_144] = "144 FPS",
    [UI_MENU_FPS_DISPLAY] = "Частота экрана", [UI_MENU_FPS_DISPLAY_HZ] = "Частота экрана (%.0f Гц)",
    [UI_MENU_FPS_UNLIMITED] = "Без ограничения",
    [UI_MENU_FPS_NEEDS_REALTIME] = "Разрешите высокий FPS и перезапустите игру",
    [UI_MENU_REALTIME] = "Скорость игры по часам",
    [UI_MENU_REALTIME_DESC] = "Игра идёт по реальному времени, медленный кадр её не замедляет.",
    [UI_MENU_DKB_REALTIME] = "Разрешить высокий FPS",
    [UI_MENU_DKB_REALTIME_DESC] = "Позволяет выбрать лимит кадров, в том числе выше 60 FPS. "
                                "Игровое время подстраивается под частоту кадров. Режим экспериментальный.",
    [UI_MENU_CLOTH_REALTIME] = "Ткань по часам",
    [UI_MENU_CLOTH_REALTIME_DESC] = "Ткань и волосы тоже идут по реальному времени.",
    [UI_MENU_HIGHFPS_FIX] = "Исправления для высокого FPS",
    [UI_MENU_HIGHFPS_FIX_DESC] = "Исправляет физику, которая ломается при частоте выше 30 кадров в секунду, и объекты, которые выше 60 остаются полупрозрачными или невидимыми.",
    [UI_MENU_GAME_LANG] = "Язык игры", [UI_MENU_GAME_LANG_DESC] = "Язык текста и озвучки игры.",
    [UI_MENU_GAME_LANG_AUTO] = "Автоматически",
    [UI_MENU_OPERATION_MODE] = "Режим консоли",
    [UI_MENU_OPERATION_MODE_DESC] = "Игра считает, что консоль в док-станции или в портативном режиме.",
    [UI_MENU_MODE_DOCKED] = "В док-станции", [UI_MENU_MODE_HANDHELD] = "Портативный",
    [UI_MENU_ASSUME_FOCUS] = "Работать без фокуса",
    [UI_MENU_ASSUME_FOCUS_DESC] = "Включено (по умолчанию): игра идёт, пока вы в другом окне, а клавиатура не "
                                  "действует, пока окно не вернётся на передний план. Выключено: игра встаёт на паузу, "
                                  "когда окно теряет фокус, как на Switch при нажатии Home; прерванная так загрузка "
                                  "может остановиться насовсем.",

    [UI_MENU_VOLUME] = "Громкость", [UI_MENU_VOLUME_DESC] = "Общая громкость игры.",
    [UI_MENU_AUDIO_CHANNELS] = "Вывод звука",
    [UI_MENU_AUDIO_CHANNELS_DESC] = "Стерео или объёмный звук 5.1. Автоматически выбирается 5.1, если у устройства "
                                    "вывода по умолчанию шесть каналов или больше.",
    [UI_MENU_CHANNELS_AUTO] = "Автоматически", [UI_MENU_CHANNELS_STEREO] = "Стерео", [UI_MENU_CHANNELS_51] = "Объёмный 5.1",

    [UI_MENU_CONTROLLER] = "Контроллер", [UI_MENU_MOUSE_KEYBOARD] = "Мышь и клавиатура",
    [UI_MENU_MOTION] = "Управление движением", [UI_MENU_MOTION_DESC] = "Прицеливание гироскопом контроллера.",
    [UI_MENU_RUMBLE_STRENGTH] = "Сила вибрации",
    [UI_MENU_RUMBLE_STRENGTH_DESC] = "Насколько сильно вибрирует контроллер.",
    [UI_MENU_RUMBLE_CURVE] = "Кривая вибрации",
    [UI_MENU_RUMBLE_CURVE_DESC] = "Чем ниже значение, тем сильнее слабая вибрация.",
    [UI_MENU_RUMBLE_TRIGGERS] = "Вибрация триггеров (Xbox)",
    [UI_MENU_RUMBLE_TRIGGERS_DESC] = "Вибрация и в триггерах контроллера Xbox.",
    [UI_MENU_PAD_LAYOUT] = "Раскладка кнопок",
    [UI_MENU_PAD_LAYOUT_DESC] = "Какая кнопка работает как A. Nintendo: правая, как на Switch. Xbox: нижняя, как "
                                "подписано на контроллере Xbox (крест на контроллере PlayStation). Автоматически: "
                                "Nintendo для контроллеров с подписями Nintendo, Xbox для остальных. Подсказки Xbox: "
                                "A справа, как в Nintendo, а игра показывает надписи Xbox (B вместо A, A вместо B, Y "
                                "вместо X, X вместо Y); подсказки меняются после перезапуска. Буквы записаны в файлах "
                                 "игры, поэтому у кнопки, переназначенной на вкладке управления, на экране остаётся "
                                 "прежняя буква.",
    [UI_MENU_PAD_LAYOUT_AUTO] = "Автоматически", [UI_MENU_PAD_LAYOUT_NINTENDO] = "Nintendo (A справа)",
    [UI_MENU_PAD_LAYOUT_XBOX] = "Xbox (A снизу)", [UI_MENU_PAD_LAYOUT_XBOX_PROMPTS] = "Подсказки Xbox (A справа)",
    [UI_MENU_PAD_LAYOUT_IN_USE] = "сейчас: %s",
    [UI_MENU_PAD_LAYOUT_PROMPTS_MISSING] = "нет файлов подсказок: запустите установку снова",
    [UI_MENU_PAD_LAYOUT_PROMPTS_RESTART] = "подсказки сменятся после перезапуска",
    [UI_MENU_STICK_DEADZONE] = "Мёртвая зона стиков",
    [UI_MENU_STICK_DEADZONE_DESC] = "Насколько нужно отклонить стик геймпада, чтобы игра это заметила. Увеличьте, если персонаж или камера двигаются сами.",
    [UI_MENU_MOUSE_CAMERA] = "Прямое управление камерой мышью",
    [UI_MENU_MOUSE_CAMERA_DESC] = "Движение мыши напрямую поворачивает камеру слежения. "
                                 "В других режимах камеры мышь имитирует правый стик.",
    [UI_MENU_MOUSE_SENS] = "Чувствительность мыши",
    [UI_MENU_MOUSE_SENS_DESC] = "Как быстро мышь поворачивает камеру.",

    [UI_BINDS_HELP_KEYS] = "Щёлкните назначение или нажмите Enter, затем нажмите клавишу или кнопку мыши. Delete или Backspace очищает ячейку. "
                           "Занятая клавиша переходит к новому действию.",
    [UI_BINDS_HELP_PAD] = "Нажмите A на назначении, затем нажмите кнопку контроллера. X очищает ячейку. Занятые кнопки "
                          "меняются местами. Левый стик отвечает за движение, правый — за камеру.",
    [UI_BINDS_COL_KEY1] = "Основное", [UI_BINDS_COL_KEY2] = "Дополнительное",
    [UI_BINDS_GROUP_BUTTONS] = "Кнопки", [UI_BINDS_GROUP_MOVE] = "Движение", [UI_BINDS_GROUP_CAMERA] = "Камера",
    [UI_BINDS_PRESS_KEY] = "Нажмите клавишу или кнопку мыши для «%s»",
    [UI_BINDS_PRESS_PAD] = "Нажмите кнопку контроллера для «%s»",
    [UI_BINDS_CANCEL_HINT] = "Esc отменяет, или подождите %d с",
    [UI_BINDS_TAKEN] = "%s: убрано с «%s»", [UI_BINDS_SWAPPED] = "«%s» и «%s» поменялись местами",
    [UI_BINDS_RESET] = "Сбросить все привязки",
    [UI_BINDS_RESET_DESC] = "Вернуть клавиши и кнопки контроллера по умолчанию. Снова действует раскладка кнопок выше: "
                            "переназначенные кнопки контроллера остаются на своих местах до "
                            "этого сброса.",
    [UI_BIND_A] = "A", [UI_BIND_B] = "B", [UI_BIND_X] = "X", [UI_BIND_Y] = "Y",
    [UI_BIND_LCLICK] = "Нажатие левого стика", [UI_BIND_RCLICK] = "Нажатие правого стика", [UI_BIND_L] = "L",
    [UI_BIND_R] = "R", [UI_BIND_ZL] = "ZL", [UI_BIND_ZR] = "ZR", [UI_BIND_PLUS] = "Плюс", [UI_BIND_MINUS] = "Минус",
    [UI_BIND_LEFT] = "Крестовина влево", [UI_BIND_UP] = "Крестовина вверх", [UI_BIND_RIGHT] = "Крестовина вправо",
    [UI_BIND_DOWN] = "Крестовина вниз",
    [UI_BIND_LSTICK_LEFT] = "Влево", [UI_BIND_LSTICK_UP] = "Вперёд", [UI_BIND_LSTICK_RIGHT] = "Вправо",
    [UI_BIND_LSTICK_DOWN] = "Назад",
    [UI_BIND_RSTICK_LEFT] = "Камера влево", [UI_BIND_RSTICK_UP] = "Камера вверх",
    [UI_BIND_RSTICK_RIGHT] = "Камера вправо", [UI_BIND_RSTICK_DOWN] = "Камера вниз",
    [UI_BIND_DESC_A] = "Взаимодействие, подобрать, поговорить.", [UI_BIND_DESC_B] = "Бег, отмена, назад.",
    [UI_BIND_DESC_X] = "Прыжок.", [UI_BIND_DESC_Y] = "Атака.", [UI_BIND_DESC_LCLICK] = "Присесть.",
    [UI_BIND_DESC_RCLICK] = "Вид от первого лица.", [UI_BIND_DESC_L] = "Способности.",
    [UI_BIND_DESC_R] = "Бросок оружия.", [UI_BIND_DESC_ZL] = "Щит, захват цели.", [UI_BIND_DESC_ZR] = "Лук.",
    [UI_BIND_DESC_PLUS] = "Меню, инвентарь.", [UI_BIND_DESC_MINUS] = "Карта.",
    [UI_BIND_DESC_DPAD] = "Быстрый выбор.",
    [UI_BIND_DESC_MOVE] = "Двигает Линка, как левый стик. Левый стик контроллера тоже всегда работает.",
    [UI_BIND_DESC_CAMERA] = "Поворачивает камеру, как правый стик. Мышь и правый стик контроллера тоже всегда "
                            "работают.",
    [UI_DKB_BIND_A] = "Прыжок. Подтверждение в меню.",
    [UI_DKB_BIND_B] = "Удар вниз. Возврат в меню.",
    [UI_DKB_BIND_X] = "Удар вверх.",
    [UI_DKB_BIND_Y] = "Удар вперёд или взмах куском породы в руках.",
    [UI_DKB_BIND_LCLICK] = "Действие, назначенное нажатию левого стика в настройках Bananza.",
    [UI_DKB_BIND_RCLICK] = "Действие, назначенное нажатию правого стика в настройках Bananza.",
    [UI_DKB_BIND_L] = "Удерживайте, чтобы свистом указать путь к цели.",
    [UI_DKB_BIND_R] = "Хлопок по земле собирает предметы поблизости и помогает найти скрытые объекты.",
    [UI_DKB_BIND_ZL] = "Кувырок вперёд или скольжение на куске породы в руках.",
    [UI_DKB_BIND_ZR] = "Взять кусок породы или бросить тот, что уже в руках.",
    [UI_DKB_BIND_PLUS] = "Открыть меню паузы.",
    [UI_DKB_BIND_MINUS] = "Открыть карту.",
    [UI_DKB_BIND_LEFT] = "Выбрать предыдущую открытую форму Bananza.",
    [UI_DKB_BIND_UP] = "Открыть меню навыков.",
    [UI_DKB_BIND_RIGHT] = "Выбрать следующую открытую форму Bananza.",
    [UI_DKB_BIND_DOWN] = "Открыть фоторежим, если он доступен.",
    [UI_DKB_BIND_MOVE] = "Двигать Донки Конга в этом направлении.",
    [UI_DKB_BIND_CAMERA] = "Повернуть камеру в этом направлении.",
    [UI_KEY_SPACE] = "Пробел", [UI_KEY_TAB] = "Tab", [UI_KEY_LSHIFT] = "Левый Shift", [UI_KEY_LCTRL] = "Левый Ctrl",
    [UI_KEY_UP] = "Стрелка вверх", [UI_KEY_DOWN] = "Стрелка вниз", [UI_KEY_LEFT] = "Стрелка влево",
    [UI_KEY_RIGHT] = "Стрелка вправо",
    [UI_KEY_MOUSE_L] = "ЛКМ", [UI_KEY_MOUSE_R] = "ПКМ", [UI_KEY_MOUSE_M] = "СКМ", [UI_KEY_MOUSE_X1] = "Мышь 4",
    [UI_KEY_MOUSE_X2] = "Мышь 5",
    [UI_KEY_ENTER] = "Enter", [UI_KEY_BACKSPACE] = "Backspace", [UI_KEY_RSHIFT] = "Правый Shift",
    [UI_KEY_RCTRL] = "Правый Ctrl", [UI_KEY_LALT] = "Alt", [UI_KEY_CAPSLOCK] = "Caps Lock", [UI_KEY_INSERT] = "Insert",
    [UI_KEY_DELETE] = "Delete", [UI_KEY_HOME] = "Home", [UI_KEY_END] = "End", [UI_KEY_PAGEUP] = "Page Up",
    [UI_KEY_PAGEDOWN] = "Page Down", [UI_KEY_NUM_FMT] = "Num %s",
    [UI_PAD_SOUTH] = "Нижняя кнопка", [UI_PAD_EAST] = "Правая кнопка", [UI_PAD_WEST] = "Левая кнопка",
    [UI_PAD_NORTH] = "Верхняя кнопка", [UI_PAD_LSTICK] = "Левый стик", [UI_PAD_RSTICK] = "Правый стик",
    [UI_PAD_L] = "Левый бампер", [UI_PAD_R] = "Правый бампер", [UI_PAD_START] = "Start", [UI_PAD_BACK] = "Back",
    [UI_PAD_DPAD_LEFT] = "Крестовина влево", [UI_PAD_DPAD_UP] = "Крестовина вверх",
    [UI_PAD_DPAD_RIGHT] = "Крестовина вправо", [UI_PAD_DPAD_DOWN] = "Крестовина вниз", [UI_PAD_ZL] = "Левый курок",
    [UI_PAD_ZR] = "Правый курок",
    [UI_PAD_LPADDLE1] = "Левая задняя кнопка", [UI_PAD_RPADDLE1] = "Правая задняя кнопка",
    [UI_PAD_LPADDLE2] = "Левая задняя кнопка 2", [UI_PAD_RPADDLE2] = "Правая задняя кнопка 2",
    [UI_PAD_CROSS] = "Крест", [UI_PAD_CIRCLE] = "Круг", [UI_PAD_SQUARE] = "Квадрат", [UI_PAD_TRIANGLE] = "Треугольник",

    [UI_MENU_MENU_LANG] = "Язык меню", [UI_MENU_MENU_LANG_DESC] = "Язык этого меню и индикатора.",
    [UI_MENU_MENU_LANG_AUTO] = "Как в системе",
    [UI_MENU_THEME] = "Тема меню", [UI_MENU_THEME_DK] = "Donkey Kong", [UI_MENU_THEME_CLASSIC] = "Классическая",
    [UI_MENU_THEME_DESC] = "Вид этого меню: Donkey Kong (кора и банановый жёлтый) или классический тёмно-зелёный с золотом.",

    [UI_MENU_ADV_WARNING] = "Эти настройки нужны для тестов и поиска неполадок. Меняйте их, только если знаете зачем.",
    [UI_MENU_GROUP_SCHED] = "Планирование", [UI_MENU_GROUP_SHADERS] = "Шейдеры и конвейеры",
    [UI_MENU_GROUP_TEXMEM] = "Текстуры и память", [UI_MENU_GROUP_FIXES] = "Исправления графики",
    [UI_MENU_GROUP_VULKAN] = "Vulkan", [UI_MENU_GROUP_DIAG] = "Диагностика",
    [UI_MENU_ASYNC_PREEMPT] = "Вытеснение на виртуальных ядрах",
    [UI_MENU_ASYNC_PREEMPT_DESC] = "Позволяет потоку игры прервать другой поток на том же виртуальном ядре.",
    [UI_MENU_PREEMPT_OFF] = "Выкл.", [UI_MENU_PREEMPT_ON] = "Вкл.", [UI_MENU_PREEMPT_INHERIT] = "Только наследование",
    [UI_MENU_REPLAY_ASYNC] = "Подготовка кадра для GPU в отдельном потоке",
    [UI_MENU_REPLAY_ASYNC_DESC] = "Кадр для GPU готовится в отдельном потоке, а не в потоке игры. Добавляет до одного кадра задержки.",
    [UI_MENU_REPLAY_THREADS] = "Потоки записи больших проходов",
    [UI_MENU_REPLAY_THREADS_DESC] = "Отрисовки больших проходов записываются сразу в столько потоков. «Автоматически» — 4 потока при 8 и более логических процессорах (кроме AMD); «Выкл.» — в одном потоке. Быстрее в насыщенных местах на процессорах с большим числом ядер.",
    [UI_MENU_TIMER_1MS] = "Таймер Windows 1 мс",
    [UI_MENU_TIMER_1MS_DESC] = "Повышает точность системного таймера до 1 мс для ровного темпа кадров.",
    [UI_MENU_PIPE_WAIT] = "Обработка ожидающих отрисовок",
    [UI_MENU_PIPE_WAIT_DESC] = "«По умолчанию» — текущее планирование с отдельным потоком для ожидающих отрисовок; «Старый способ» — прежнее планирование.",
    [UI_MENU_PIPE_WAIT_DEFAULT] = "По умолчанию", [UI_MENU_PIPE_WAIT_DRAW] = "Старый способ",
    [UI_MENU_PIPE_SKIP] = "Пропуск отрисовок при сборке конвейера",
    [UI_MENU_PIPE_SKIP_DESC] = "Какие отрисовки отбрасываются, а не ждут свой конвейер.",
    [UI_MENU_PIPE_SKIP_STRICT] = "Строго", [UI_MENU_PIPE_SKIP_ANY] = "Любые",
    [UI_MENU_PIPE_SKIP_FULLSCREEN] = "Полноэкранные проходы",
    [UI_MENU_LIB_PREBUILD] = "Предсборка библиотек стадий",
    [UI_MENU_LIB_PREBUILD_DESC] = "Заранее собирает библиотеки стадий шейдеров в фоне.",
    [UI_MENU_LIB_PREBUILD_THREADS] = "Потоки предсборки библиотек",
    [UI_MENU_LIB_PREBUILD_THREADS_DESC] = "Сколько потоков занято предсборкой; «Автоматически» — по числу ядер процессора.",
    [UI_MENU_PIPE_PREBUILD] = "Предсборка конвейеров",
    [UI_MENU_PIPE_PREBUILD_DESC] = "Заранее собирает известные конвейеры.",
    [UI_MENU_PREWARM_XLATE] = "Прогрев транслятора",
    [UI_MENU_PREWARM_XLATE_DESC] = "Потоки, которые при запуске переводят известные шейдеры; «Автоматически» — половина потоков процессора, не больше 16.",
    [UI_MENU_GPL] = "Библиотека графических конвейеров",
    [UI_MENU_GPL_DESC] = "Использует расширение Vulkan для библиотек конвейеров; выключите, если драйвер с ним сбоит.",
    [UI_MENU_ASTC] = "Распаковка текстур ASTC", [UI_MENU_ASTC_DESC] = "Где распаковываются сжатые текстуры ASTC.",
    [UI_MENU_ASTC_AUTO] = "Автоматически", [UI_MENU_ASTC_GPU] = "Шейдер GPU", [UI_MENU_ASTC_NATIVE] = "Аппаратно",
    [UI_MENU_ASTC_CPU] = "Процессор",
    [UI_MENU_MEMORY] = "Режим памяти GPU", [UI_MENU_MEMORY_DESC] = "Как память игры передаётся видеокарте.",
    [UI_MENU_MEMORY_AUTO] = "Автоматически", [UI_MENU_MEMORY_IMPORT] = "Импорт", [UI_MENU_MEMORY_MIRROR] = "Зеркало",
    [UI_MENU_MEMORY_VERIFY] = "Проверка",
    [UI_MENU_TEX_ALIAS] = "Наследование содержимого текстур", [UI_MENU_TEX_ALIAS_DESC] = "Новая текстура поверх памяти живой текстуры получает её содержимое, а не пустую.",
    [UI_MENU_TEX_VRAM_PRESSURE] = "Предел памяти текстур", [UI_MENU_TEX_VRAM_PRESSURE_DESC] = "Процент видеопамяти, выше которого освобождённые игрой текстуры удаляются (при повторном использовании загружаются снова). 0 — не удалять.",
    [UI_MENU_ASTC_BC] = "Сжатие ASTC в BC3", [UI_MENU_ASTC_BC_DESC] = "С декодером на GPU: декодированные текстуры ASTC пересжимаются в BC3 и занимают вчетверо меньше видеопамяти. Для видеокарт с 4 ГБ и меньше; текстуры чуть более блочные.",
    [UI_MENU_LOD_GUARD] = "Защита LOD", [UI_MENU_LOD_GUARD_DESC] = "Не даёт объектам мелькать не на своём месте, а дальнему плану — на кадр становиться плоским: ждёт, пока игра допишет данные уровней детализации, и только потом их читает GPU.",
    [UI_MENU_FIX_C0] = "Исправление константного буфера",
    [UI_MENU_FIX_C0_DESC] = "Заполняет зарезервированный драйвером банк констант c0 (размеры хранилищ, размер запуска, размер точки, базовый экземпляр) вместо чтения нулей.",
    [UI_MENU_FIX_ZERO] = "Обнулять новую память GPU",
    [UI_MENU_FIX_ZERO_DESC] = "Новые пулы памяти GPU заполняются нулями, как память Switch.",
    [UI_MENU_LIST_RESTART] = "Перезапуск примитивов в списках",
    [UI_MENU_LIST_RESTART_DESC] = "Применяет перезапуск примитивов игры и к спискам треугольников, как на Switch. Без него индексы перезапуска рисуются как вершины: длинные вытянутые треугольники, например на карте. Нужна видеокарта с primitiveTopologyListRestart.",
    [UI_MENU_BIND_SKIP] = "Пропуск повторных привязок",
    [UI_MENU_BIND_SKIP_DESC] = "Оптимизация: пропускает неизменившиеся привязки.",
    [UI_MENU_SHADER_MEMO] = "Память шейдеров", [UI_MENU_SHADER_MEMO_DESC] = "Оптимизация: запоминает поиск шейдеров.",
    [UI_MENU_PIPE_MEMO] = "Память конвейера", [UI_MENU_PIPE_MEMO_DESC] = "Оптимизация: повторяет конвейер прошлой отрисовки, если её состояние не изменилось.",
    [UI_MENU_TEX_MEMO] = "Память привязок текстур", [UI_MENU_TEX_MEMO_DESC] = "Оптимизация: запоминает дескрипторы текстур между отрисовками.",
    [UI_MENU_DESC_TEMPLATE] = "Шаблоны дескрипторов",
    [UI_MENU_DESC_TEMPLATE_DESC] = "Оптимизация: обновляет дескрипторы пачками.",
    [UI_MENU_PUSH_SKIP] = "Пропуск повторных push-констант",
    [UI_MENU_PUSH_SKIP_DESC] = "Оптимизация: пропускает неизменившиеся push-константы.",
    [UI_MENU_SPLIT_DRAWS] = "Ранняя отправка работы GPU",
    [UI_MENU_SPLIT_DRAWS_DESC] = "Отправляет работу GPU частями через каждые N отрисовок между проходами, чтобы GPU начинал раньше; 0 — выключено.",
    [UI_MENU_VK_VALIDATION] = "Слой проверки",
    [UI_MENU_VK_VALIDATION_DESC] = "Проверяет каждый вызов Vulkan и сильно замедляет игру.",
    [UI_MENU_VK_VALIDATION_MISSING] = "Слой проверки не установлен.",
    [UI_MENU_VK_DYN_VERTEX] = "Динамическое состояние вершин",
    [UI_MENU_VK_DYN_VERTEX_DESC] = "Принудительно включает или выключает динамический ввод вершин; по умолчанию зависит от GPU.",
    [UI_MENU_VK_DYN_A2C] = "Динамический alpha to coverage",
    [UI_MENU_VK_DYN_A2C_DESC] = "Принудительно включает или выключает динамический alpha to coverage. По умолчанию "
                                "выключен на драйвере AMD для RDNA 2 и старше, где с ним персонажи чёрные, и включён "
                                "на остальных.",
    [UI_MENU_VK_SUBGROUP_PIN] = "Фиксировать размер подгруппы",
    [UI_MENU_VK_SUBGROUP_PIN_DESC] = "Фиксирует размер подгруппы шейдера.",
    [UI_MENU_TRACE] = "Журнал производительности", [UI_MENU_TRACE_DESC] = "Пишет время кадров в файл.",
    [UI_MENU_FLIGHT] = "Запись последних кадров (для отчёта о фризе)",
    [UI_MENU_FLIGHT_DESC] = "Хранит короткую историю и сохраняет её, когда кадр подвисает.",
    [UI_MENU_FLIGHT_SPIKE_MS] = "Порог подвисания (мс)",
    [UI_MENU_FLIGHT_SPIKE_MS_DESC] = "Время кадра, которое считается подвисанием.",
    [UI_MENU_INPUT_RECORD] = "Запись ввода",
    [UI_MENU_INPUT_RECORD_DESC] = "Пишет ваш ввод в файл для отчёта об ошибке.",
    [UI_MENU_OPEN_LOG] = "Открыть папку журналов", [UI_MENU_OPEN_LOG_DESC] = "Показывает папку с файлами журнала.",
    [UI_MENU_COPY_INFO] = "Копировать сведения о системе",
    [UI_MENU_COPY_INFO_DESC] = "Копирует в буфер обмена видеокарту, драйвер, процессор, память и изменённые настройки.",
    [UI_MENU_COPY_INFO_DONE] = "Скопировано", [UI_MENU_ACTION_FAILED] = "не получилось",
    [UI_MENU_RESET] = "Сбросить все настройки", [UI_MENU_RESET_DESC] = "Действует на все вкладки.",
    [UI_MENU_RESET_CONFIRM] = "Сбросить все настройки к значениям по умолчанию?", [UI_MENU_RESET_YES] = "Сбросить",
    [UI_MENU_RESET_NO] = "Отмена",

    [UI_MENU_OV_FPS] = "%.0f FPS", [UI_MENU_OV_FRAME_MS] = "%.1f мс", [UI_MENU_OV_GAME_PCT] = "игра %.0f%%",
    [UI_MENU_OV_MAX_MS] = "макс. %.1f мс",
    [UI_MENU_OV_GPU] = "GPU", [UI_MENU_OV_CPU] = "ЦП", [UI_MENU_OV_RAM] = "ОЗУ", [UI_MENU_OV_VRAM] = "Видеопамять",
    [UI_MENU_OV_TEMP] = "Температура", [UI_MENU_OV_POWER] = "Мощность", [UI_MENU_OV_CLOCK] = "Частота",
    [UI_MENU_OV_NONE] = "-",
    [UI_MENU_OV_REPLAY] = "Подготовка кадра для GPU", [UI_MENU_OV_GPU_TIME] = "Занятость GPU за кадр",
    [UI_MENU_OV_PIPE_WAIT] = "Ожидания конвейеров",
    [UI_MENU_OV_SHADER_WAIT] = "Ожидания шейдеров", [UI_MENU_OV_TEX_UPLOADS] = "Загрузки текстур",
    [UI_MENU_OV_FMT_PCT] = "%.0f%%", [UI_MENU_OV_FMT_TEMP] = "%.0f °C", [UI_MENU_OV_FMT_POWER] = "%.0f Вт",
    [UI_MENU_OV_FMT_MHZ] = "%.0f МГц", [UI_MENU_OV_FMT_GB] = "%.1f ГБ", [UI_MENU_OV_FMT_GB_OF] = "/ %.1f ГБ",
    [UI_MENU_OV_GAME_GB] = "игра %.1f ГБ", [UI_MENU_OV_FMT_MS] = "%.2f мс", [UI_MENU_OV_PER_FRAME] = "%.1f за кадр",
    [UI_MENU_OV_WINDOW] = "Окно", [UI_MENU_OV_OUTPUT] = "Изображение игры", [UI_MENU_OV_SCENE] = "3D-сцена",
    [UI_MENU_OV_SCALING] = "Масштабирование", [UI_MENU_OV_LIMIT] = "Лимит кадров", [UI_MENU_OV_PRESENT] = "Вывод кадров",
    [UI_MENU_OV_FMT_SIZE] = "%ux%u", [UI_MENU_OV_FMT_SCENE] = "%ux%u, %.0f%%",
    [UI_MENU_OV_FMT_UPSCALE] = "%ux%u -> %ux%u",
    [UI_MENU_OV_NO_UPSCALER] = "без апскейлера",
    [UI_MENU_OV_FMT_FSR] = "AMD FSR 1, резкость %d%%", [UI_MENU_OV_SCALE_NONE] = "нет (1:1)",
    [UI_MENU_OV_SCALE_BILINEAR] = "билинейное",
};

static const char *const text_de[UI_TEXT_COUNT] = {
    SWKBD("Texteingabe", "Gib einen Namen ein", "Pferd", "OK", "Abbrechen", "Controller: A = OK, B = Abbrechen"),

    [UI_MENU_TITLE] = "Einstellungen",
    [UI_MENU_TAB_DISPLAY] = "Anzeige", [UI_MENU_TAB_GRAPHICS] = "Grafik",
    [UI_MENU_TAB_GAME] = "Spiel", [UI_MENU_TAB_SOUND] = "Ton",
    [UI_MENU_TAB_CONTROLS] = "Steuerung", [UI_MENU_TAB_MENU] = "Menü", [UI_MENU_TAB_ADVANCED] = "Erweitert",
    [UI_MENU_CONTINUE] = "Weiter", [UI_MENU_QUIT] = "Spiel beenden",
    [UI_MENU_ON] = "An", [UI_MENU_OFF] = "Aus", [UI_MENU_AUTO] = "Automatisch", [UI_MENU_IS_DEFAULT] = "Standard",
    [UI_MENU_AFTER_RESTART] = "Neustart nötig", [UI_MENU_SET_BY_ENV] = "Durch Umgebungsvariable festgelegt",
    [UI_MENU_SAVE_FAILED] = "Einstellungen konnten nicht gespeichert werden",
    [UI_MENU_VERSION] = PORT_NAME " %s, Spiel %s",

    [UI_MENU_FULLSCREEN] = "Vollbild", [UI_MENU_FULLSCREEN_DESC] = "Das Spiel füllt den Bildschirm; F11 schaltet um.",
    [UI_MENU_WINDOW_MODE] = "Fenstermodus",
    [UI_MENU_WINDOW_MODE_DESC] = "F11 wechselt zwischen Fenster und dem zuletzt genutzten Vollbildmodus.",
    [UI_MENU_WINDOW_MODE_WINDOWED] = "Fenster", [UI_MENU_WINDOW_MODE_BORDERLESS] = "Rahmenlos",
    [UI_MENU_WINDOW_MODE_FULLSCREEN] = "Vollbild",
    [UI_MENU_WINDOW_SIZE] = "Fenstergröße", [UI_MENU_WINDOW_SIZE_DESC] = "Legt die Größe des Desktop-Fensters im Fenstermodus fest.",
    [UI_MENU_WINDOW_SIZE_CUSTOM] = "Eigene",
    [UI_MENU_WINDOWED_ONLY] = "Nur im Fenstermodus verfügbar",
    [UI_MENU_PRESENT] = "Bildsynchronisation", [UI_MENU_PRESENT_DESC] = "VSync wartet auf die Bildwiederholung des Bildschirms.\n"
                             "Mailbox zeigt bei jeder Bildwiederholung das neueste fertige Bild, wenn unterstützt.\n"
                             "Sofort gibt Bilder aus, sobald sie fertig sind, und kann Tearing verursachen.",
    [UI_MENU_PRESENT_MAILBOX] = "Mailbox", [UI_MENU_PRESENT_VSYNC] = "VSync", [UI_MENU_PRESENT_IMMEDIATE] = "Sofort",
    [UI_MENU_PRESENT_IN_USE] = "aktiv: %s",
    [UI_MENU_GPU] = "Grafikkarte", [UI_MENU_GPU_DESC] = "Wähle, welche Grafikkarte das Spiel ausführt. Nutze Automatisch, außer du brauchst eine bestimmte Karte.",
    [UI_MENU_GPU_AUTO] = "Automatisch",
    [UI_MENU_PERF_OVERLAY] = "Leistungsanzeige",
    [UI_MENU_PERF_OVERLAY_DESC] = "Zeigt die FPS oben rechts. Voll zeigt zusätzlich einen Frametime-Graphen und Hardware-Werte. F10 wechselt durch die Modi.",
    [UI_MENU_PERF_OFF] = "Aus", [UI_MENU_PERF_FPS] = "FPS", [UI_MENU_PERF_FULL] = "Voll",
    [UI_PERF_LINE_FPS] = "%.0f FPS",
    [UI_MENU_RESOLUTION_GROUP] = "Auflösung",
    [UI_MENU_DISPLAY_OUTPUT] = "Bildausgabe",
    [UI_MENU_RENDER_OPTIONS] = "Render-Optionen",
    [UI_MENU_OUTPUT_RES] = "Render-Auflösung",
    [UI_MENU_ULTRAWIDE] = "Ultrawide",
    [UI_MENU_ENABLE_ULTRAWIDE] = "Ultrawide aktivieren",
    [UI_MENU_ULTRAWIDE_DESC] = "Verbreitert die Spielansicht für 21:9- und 32:9-Bildschirme.",
    [UI_MENU_WIDE_RES] = "Render-Auflösung",
    [UI_MENU_WIDE_RES_DESC] = "Legt die Auflösung des Spielbilds fest, bevor es auf deinen Bildschirm skaliert wird. "
                             "Höhere Auflösungen können schärfer aussehen und brauchen mehr GPU-Leistung und Grafikspeicher.\n\n"
                             "Aktiviere Ultrawide für 21:9- und 32:9-Optionen.",
    [UI_MENU_HUD_POSITION] = "HUD-Position",
    [UI_MENU_HUD_POSITION_DESC] = "Eckanzeigen nah an den Bildschirmrändern platzieren oder in einem zentrierten 16:9-"
                                 "Bereich halten. Menüs und Zielmarkierungen bleiben in der Mitte.",
    [UI_MENU_HUD_EDGES] = "Bildschirmränder", [UI_MENU_HUD_CENTERED] = "Zentriert (16:9)",
    [UI_MENU_WIDE_DISABLED] = "Erst Ultrawide aktivieren",
    [UI_MENU_OUTPUT_RES_DESC] = "Die Größe, in der das Spiel Bild und Oberfläche zeichnet. Über 1920 x 1080 braucht das "
                                "Spiel mehr Grafikspeicher und GPU-Zeit. Ein breiteres Format (21:9) verbreitert die Sicht "
                                "der Kamera und rückt das HUD in die Bildschirmecken; ein höheres (16:10, Steam Deck) an den "
                                "oberen und unteren Rand.",
    [UI_MENU_RENDER_SCALE] = "3D-Renderskalierung",
    [UI_MENU_RENDER_SCALE_DESC] = "Größe der 3D-Szene im Verhältnis zur Spielauflösung; das FSR des Spiels skaliert sie "
                                  "auf diese Größe. Über 100 % glättet Kanten (Supersampling) bei hohen GPU-Kosten.",
    [UI_MENU_RENDER_SCALE_GAME] = "Spiel (83 %)",
    [UI_MENU_SCENE_RES] = "Auflösung der 3D-Szene",
    [UI_MENU_SCENE_RES_DESC] = "Auto lässt das Spiel seine 3D-Szene je nach Last skalieren, meist auf zwei Drittel der "
                               "Render-Auflösung je Seite, und dann hochskalieren. Voll zeichnet die 3D-Szene in der ganzen "
                               "Render-Auflösung: ein schärferes Bild bei höheren GPU-Kosten, ohne Upscaler und DLSS. Gilt ab dem nächsten Start.",
    [UI_MENU_SCENE_RES_AUTO] = "Auto (Spiel)", [UI_MENU_SCENE_RES_FULL] = "Voll",
    [UI_MENU_UPSCALER] = "Skalierungsmethode",
    [UI_MENU_UPSCALER_DESC] = "Greift, wenn das Spielbild kleiner ist als die Fläche, die es auf deinem Bildschirm oder im Fenster füllt. "
                              "AMD FSR 1 kann Kanten schärfer halten als Bilinear.",
    [UI_MENU_UPSCALER_FSR] = "AMD FSR 1", [UI_MENU_UPSCALER_BILINEAR] = "Bilinear", [UI_MENU_UPSCALER_DLSS] = "NVIDIA DLSS",
    [UI_MENU_ASPECT] = "Bildskalierung",
    [UI_MENU_ASPECT_DESC] = "Standard behält das Format des Spiels und fügt bei Bedarf schwarze Balken hinzu.\n"
                            "Strecken nutzt das ganze Fenster oder den ganzen Bildschirm. Das Bild wird nur gestreckt, wenn die Formate abweichen.",
    [UI_MENU_ASPECT_FIT] = "Standard", [UI_MENU_ASPECT_STRETCH] = "Strecken",
    [UI_MENU_FOV] = "Sichtfeld",
    [UI_MENU_FOV_DESC] = "Passt die Kamerasicht in Prozent des ursprünglichen Winkels an. 100 % ist die Standardsicht des Spiels.",
    [UI_MENU_SHARPNESS] = "FSR-Schärfe",
    [UI_MENU_SHARPNESS_DESC] = "Nachschärfen nach der FSR-Skalierung; 0 schaltet es aus.",
    [UI_MENU_NO_UPSCALING] = "Braucht ein kleineres Spielbild",
    [UI_MENU_SHARPNESS_NEEDS_FSR] = "Braucht AMD FSR 1",
    [UI_MENU_SMAA] = "Kantenglättung (SMAA)",
    [UI_MENU_SMAA_DESC] = "Glättet gezackte Kanten im Spielbild und HUD. Höhere Stufen brauchen mehr GPU-Zeit.",
    [UI_MENU_SMAA_LOW] = "Niedrig", [UI_MENU_SMAA_MEDIUM] = "Mittel", [UI_MENU_SMAA_HIGH] = "Hoch",
    [UI_MENU_FRAME_GEN] = "Frame Generation",
    [UI_MENU_FRAME_GEN_DESC] = "Berechnet zwischen je zwei Bildern des Spiels eins aus der Bewegung, so werden 60 FPS als 120 "
                               "angezeigt. Flüssiger, nicht schneller, mit etwas mehr Verzögerung. Braucht Mailbox oder Sofort und "
                               "einen Bildschirm mit mindestens der doppelten Bildrate, sonst entstehen keine Zwischenbilder. Experimentell.",
    [UI_MENU_FRAME_GEN_2X] = "2x",
    [UI_MENU_COLOR] = "Farbe",
    [UI_MENU_COLOR_PRESET] = "Farbprofil",
    [UI_MENU_COLOR_PRESET_DESC] = "Spielstandard nutzt die Originalfarben.\nLebendig (OLED) verstärkt Kontrast und Farben.\n"
                                  "Klarheit macht das Bild etwas heller und lebendiger.\n\n"
                                  "Jeder verschobene Regler stellt das Profil auf Eigenes.",
    [UI_MENU_COLOR_DEFAULT] = "Spielstandard", [UI_MENU_COLOR_VIVID] = "Lebendig (OLED)", [UI_MENU_COLOR_CLARITY] = "Klarheit",
    [UI_MENU_COLOR_CUSTOM] = "Eigenes",
    [UI_MENU_COLOR_BRIGHTNESS] = "Helligkeit", [UI_MENU_COLOR_BRIGHTNESS_DESC] = "Macht das Bild heller oder dunkler. Standard ist 0 %.",
    [UI_MENU_COLOR_CONTRAST] = "Kontrast",
    [UI_MENU_COLOR_CONTRAST_DESC] = "Höhere Werte machen dunkle Bereiche dunkler und helle heller. Standard ist 100 %.",
    [UI_MENU_COLOR_SATURATION] = "Sättigung",
    [UI_MENU_COLOR_SATURATION_DESC] = "Höhere Werte machen Farben kräftiger. Standard ist 100 %.",
    [UI_MENU_COLOR_GAMMA] = "Gamma",
    [UI_MENU_COLOR_GAMMA_DESC] = "Unter 1,00 hellt Schatten und Mitteltöne auf, über 1,00 dunkelt sie ab.",
    [UI_MENU_HDR_GROUP] = "HDR (experimentell)",
    [UI_MENU_HDR] = "HDR-Ausgabe",
    [UI_MENU_HDR_DESC] = "Hebt Lichter im SDR-Bild des Spiels auf einem HDR-Bildschirm an.\n\n"
                         "Aktiviere zuerst HDR in den Anzeigeeinstellungen deines Systems.",
    [UI_MENU_HDR_NATIVE] = "HDR des Spiels",
    [UI_MENU_HDR_NATIVE_DESC] = "Nutzt das eigene HDR des Spiels ab Update 3.2.0. Lichter folgen der Spitzenhelligkeit bis 1400 Nits. "
                                "Dieser Modus schaltet FSR 1, SMAA und die Farbanpassung ab.",
    [UI_MENU_HDR_PEAK] = "Spitzenhelligkeit",
    [UI_MENU_HDR_PEAK_DESC] = "Stelle die hellsten Lichter auf die Spitzenhelligkeit deines Bildschirms in Nits ein.",
    [UI_MENU_HDR_GAME] = "Spielhelligkeit",
    [UI_MENU_HDR_GAME_DESC] = "Legt die Helligkeit von normalem Weiß im Spiel und HUD fest. Erhöhe sie, wenn das Bild zu dunkel wirkt.",
    [UI_MENU_HDR_UI] = "Helligkeit des Einstellungsmenüs",
    [UI_MENU_HDR_UI_DESC] = "Legt die Helligkeit von Weiß in diesem Menü fest. Das HUD des Spiels folgt der Spielhelligkeit.",

    [UI_MENU_FRAME_RATE_GROUP] = "Bildrate",
    [UI_MENU_FPS_LIMIT] = "Bildratenbegrenzung",
    [UI_MENU_DKB_FPS_LIMIT_DESC] = "Begrenzt die Bildrate des Spiels. Bildwiederholrate nutzt die aktuelle Bildwiederholrate deines Bildschirms.",
    [UI_MENU_FPS_LIMIT_DESC] = "Die höchste Bildrate, die das Spiel zeichnet. Über 60 ist experimentell und braucht "
                               "„Spieltempo folgt der Uhr“. Unbegrenzt ist experimentell.",
    [UI_MENU_FPS_30] = "30 FPS", [UI_MENU_FPS_40] = "40 FPS", [UI_MENU_FPS_45] = "45 FPS",
    [UI_MENU_FPS_60] = "60 FPS", [UI_MENU_FPS_90] = "90 FPS", [UI_MENU_FPS_120] = "120 FPS", [UI_MENU_FPS_144] = "144 FPS",
    [UI_MENU_FPS_DISPLAY] = "Bildwiederholrate", [UI_MENU_FPS_DISPLAY_HZ] = "Bildwiederholrate (%.0f Hz)",
    [UI_MENU_FPS_UNLIMITED] = "Unbegrenzt",
    [UI_MENU_FPS_NEEDS_REALTIME] = "„Höhere Bildraten erlauben“ aktivieren und neu starten",
    [UI_MENU_REALTIME] = "Spieltempo folgt der Uhr",
    [UI_MENU_REALTIME_DESC] = "Lässt das Spiel nach echter Zeit laufen, damit ein langsames Bild das Spiel nicht bremst.",
    [UI_MENU_DKB_REALTIME] = "Höhere Bildraten erlauben",
    [UI_MENU_DKB_REALTIME_DESC] = "Schaltet die Bildratenbegrenzung frei, auch über 60 FPS. "
                                "Das Timing des Spiels passt sich den FPS an. Dieser Modus ist experimentell.",
    [UI_MENU_CLOTH_REALTIME] = "Stoff nach der Uhr",
    [UI_MENU_CLOTH_REALTIME_DESC] = "Lässt auch Stoff und Haare nach echter Zeit laufen.",
    [UI_MENU_HIGHFPS_FIX] = "Korrekturen für hohe Bildraten",
    [UI_MENU_HIGHFPS_FIX_DESC] = "Korrigiert Physik, die über 30 Bildern pro Sekunde falsch läuft, und Objekte, die über 60 durchsichtig oder unsichtbar bleiben.",
    [UI_MENU_GAME_LANG] = "Spielsprache", [UI_MENU_GAME_LANG_DESC] = "Sprache der Texte und Stimmen des Spiels.",
    [UI_MENU_GAME_LANG_AUTO] = "Automatisch",
    [UI_MENU_OPERATION_MODE] = "Konsolenmodus",
    [UI_MENU_OPERATION_MODE_DESC] = "Wähle, welchen Switch-Modus das Spiel nutzt. Docked ist das TV-Profil, Handheld das mobile Profil.",
    [UI_MENU_MODE_DOCKED] = "Docked", [UI_MENU_MODE_HANDHELD] = "Handheld",
    [UI_MENU_ASSUME_FOCUS] = "Im Hintergrund weiterlaufen",
    [UI_MENU_ASSUME_FOCUS_DESC] = "Das Spiel läuft weiter, während du ein anderes Fenster benutzt. Aus pausiert es, wenn du wechselst.\n\n"
                                  "Pausieren während eines Ladevorgangs kann verhindern, dass er fertig wird.",

    [UI_MENU_VOLUME] = "Lautstärke", [UI_MENU_VOLUME_DESC] = "Regelt die Gesamtlautstärke des Spiels. 0 % schaltet es stumm.",
    [UI_MENU_AUDIO_CHANNELS] = "Audioausgabe",
    [UI_MENU_AUDIO_CHANNELS_DESC] = "Wähle Stereo oder 5.1-Surround. Automatisch nutzt 5.1, wenn dein Standard-Audiogerät mindestens sechs Kanäle hat.",
    [UI_MENU_CHANNELS_AUTO] = "Automatisch", [UI_MENU_CHANNELS_STEREO] = "Stereo", [UI_MENU_CHANNELS_51] = "5.1-Surround",

    [UI_MENU_CONTROLLER] = "Controller", [UI_MENU_MOUSE_KEYBOARD] = "Maus & Tastatur",
    [UI_MENU_MOTION] = "Bewegungssteuerung", [UI_MENU_MOTION_DESC] = "Zielen mit dem Gyroskop des Controllers.",
    [UI_MENU_RUMBLE_STRENGTH] = "Vibrationsstärke",
    [UI_MENU_RUMBLE_STRENGTH_DESC] = "Wie stark der Controller vibriert.",
    [UI_MENU_RUMBLE_CURVE] = "Vibrationskurve",
    [UI_MENU_RUMBLE_CURVE_DESC] = "Niedrigere Werte machen leichte Vibration besser spürbar.",
    [UI_MENU_RUMBLE_TRIGGERS] = "Trigger-Vibration (Xbox)",
    [UI_MENU_RUMBLE_TRIGGERS_DESC] = "Aktiviert die Vibration in den Triggern unterstützter Xbox-Controller.",
    [UI_MENU_PAD_LAYOUT] = "Tastenbelegung",
    [UI_MENU_PAD_LAYOUT_DESC] = "Automatisch wählt Nintendo für Controller im Nintendo-Stil und sonst Xbox.\nNintendo nutzt die rechte Taste für A.\n"
                                "Xbox nutzt die untere Taste für A.\nXbox-Symbole nutzt die Nintendo-Positionen mit Xbox-Beschriftung.\n\n"
                                "Für umbelegte Tasten zeigt das Spiel weiter das ursprüngliche Symbol.",
    [UI_MENU_PAD_LAYOUT_AUTO] = "Automatisch", [UI_MENU_PAD_LAYOUT_NINTENDO] = "Nintendo (A rechts)",
    [UI_MENU_PAD_LAYOUT_XBOX] = "Xbox (A unten)", [UI_MENU_PAD_LAYOUT_XBOX_PROMPTS] = "Xbox-Symbole (A rechts)",
    [UI_MENU_PAD_LAYOUT_IN_USE] = "aktiv: %s",
    [UI_MENU_PAD_LAYOUT_PROMPTS_MISSING] = "Symboldateien fehlen: Setup erneut ausführen",
    [UI_MENU_PAD_LAYOUT_PROMPTS_RESTART] = "Symbole ändern sich nach einem Neustart",
    [UI_MENU_STICK_DEADZONE] = "Stick-Totzone",
    [UI_MENU_STICK_DEADZONE_DESC] = "Wie weit ein Stick bewegt werden muss, bevor er zählt. Erhöhe sie, wenn Figur oder Kamera driften.",
    [UI_MENU_MOUSE_CAMERA] = "Direkte Mauskamera",
    [UI_MENU_MOUSE_CAMERA_DESC] = "Mausbewegungen drehen die Folgekamera direkt. Andere Kameramodi deuten Mausbewegungen als Stick-Eingabe.",
    [UI_MENU_MOUSE_SENS] = "Mausempfindlichkeit",
    [UI_MENU_MOUSE_SENS_DESC] = "Wie weit sich die Kamera dreht, wenn du die Maus bewegst.",

    [UI_BINDS_HELP_KEYS] = "Klicke auf eine Belegung oder drücke Enter, dann eine Taste oder Maustaste. Entf oder Rücktaste löscht sie. "
                           "Eine bereits belegte Taste wandert zur neuen Aktion.",
    [UI_BINDS_HELP_PAD] = "Drücke A auf einer Belegung, dann eine Controllertaste. X löscht sie. Bereits belegte Tasten "
                          "werden getauscht. Der linke Stick bewegt, der rechte steuert die Kamera.",
    [UI_BINDS_COL_KEY1] = "Primär", [UI_BINDS_COL_KEY2] = "Sekundär",
    [UI_BINDS_GROUP_BUTTONS] = "Tasten", [UI_BINDS_GROUP_MOVE] = "Bewegung", [UI_BINDS_GROUP_CAMERA] = "Kamera",
    [UI_BINDS_PRESS_KEY] = "Drücke eine Taste oder Maustaste für %s",
    [UI_BINDS_PRESS_PAD] = "Drücke eine Controllertaste für %s",
    [UI_BINDS_CANCEL_HINT] = "Esc bricht ab, oder warte %d s",
    [UI_BINDS_TAKEN] = "%s wurde von %s übernommen", [UI_BINDS_SWAPPED] = "%s mit %s getauscht",
    [UI_BINDS_RESET] = "Alle Belegungen zurücksetzen",
    [UI_BINDS_RESET_DESC] = "Zurück zu den Standardtasten und Controllertasten. Die Tastenbelegung oben gilt dann wieder: umbelegte Controllertasten bleiben bis zu diesem Zurücksetzen, wo du sie hingelegt hast.",
    [UI_BIND_A] = "A", [UI_BIND_B] = "B", [UI_BIND_X] = "X", [UI_BIND_Y] = "Y",
    [UI_BIND_LCLICK] = "Linker Stick drücken", [UI_BIND_RCLICK] = "Rechter Stick drücken", [UI_BIND_L] = "L",
    [UI_BIND_R] = "R", [UI_BIND_ZL] = "ZL", [UI_BIND_ZR] = "ZR", [UI_BIND_PLUS] = "Plus", [UI_BIND_MINUS] = "Minus",
    [UI_BIND_LEFT] = "Steuerkreuz links", [UI_BIND_UP] = "Steuerkreuz oben", [UI_BIND_RIGHT] = "Steuerkreuz rechts",
    [UI_BIND_DOWN] = "Steuerkreuz unten",
    [UI_BIND_LSTICK_LEFT] = "Nach links", [UI_BIND_LSTICK_UP] = "Nach vorn", [UI_BIND_LSTICK_RIGHT] = "Nach rechts",
    [UI_BIND_LSTICK_DOWN] = "Zurück",
    [UI_BIND_RSTICK_LEFT] = "Kamera links", [UI_BIND_RSTICK_UP] = "Kamera hoch", [UI_BIND_RSTICK_RIGHT] = "Kamera rechts",
    [UI_BIND_RSTICK_DOWN] = "Kamera runter",
    [UI_BIND_DESC_A] = "Interagieren, aufheben, reden.", [UI_BIND_DESC_B] = "Rennen, abbrechen, zurück.",
    [UI_BIND_DESC_X] = "Springen.", [UI_BIND_DESC_Y] = "Angreifen.", [UI_BIND_DESC_LCLICK] = "Ducken.",
    [UI_BIND_DESC_RCLICK] = "Ego-Perspektive.", [UI_BIND_DESC_L] = "Fähigkeiten.",
    [UI_BIND_DESC_R] = "Waffe werfen.", [UI_BIND_DESC_ZL] = "Schild, anvisieren.", [UI_BIND_DESC_ZR] = "Bogen.",
    [UI_BIND_DESC_PLUS] = "Menü, Inventar.", [UI_BIND_DESC_MINUS] = "Karte.",
    [UI_BIND_DESC_DPAD] = "Schnellauswahl.",
    [UI_BIND_DESC_MOVE] = "Bewegt Link wie der linke Stick. Der linke Stick des Controllers funktioniert immer auch.",
    [UI_BIND_DESC_CAMERA] = "Dreht die Kamera wie der rechte Stick. Die Maus und der rechte Stick des Controllers "
                            "funktionieren immer auch.",
    [UI_DKB_BIND_A] = "Springen. Bestätigt in Menüs.",
    [UI_DKB_BIND_B] = "Nach unten schlagen. Zurück in Menüs.",
    [UI_DKB_BIND_X] = "Nach oben schlagen.",
    [UI_DKB_BIND_Y] = "Nach vorn schlagen oder den gehaltenen Brocken schwingen.",
    [UI_DKB_BIND_LCLICK] = "Die Aktion, die in der Steuerung des Spiels auf den linken Stick gelegt ist.",
    [UI_DKB_BIND_RCLICK] = "Die Aktion, die in der Steuerung des Spiels auf den rechten Stick gelegt ist.",
    [UI_DKB_BIND_L] = "Halten zum Pfeifen: zeigt den Weg zum Ziel.",
    [UI_DKB_BIND_R] = "Handklatscher: sammelt Gegenstände in der Nähe ein und deckt Verstecktes auf.",
    [UI_DKB_BIND_ZL] = "Vorwärtsrolle oder mit einem Brocken surfen.",
    [UI_DKB_BIND_ZR] = "Einen Geländebrocken greifen oder den gehaltenen werfen.",
    [UI_DKB_BIND_PLUS] = "Pausenmenü öffnen.",
    [UI_DKB_BIND_MINUS] = "Karte öffnen.",
    [UI_DKB_BIND_LEFT] = "Zur vorigen freigeschalteten Verwandlung wechseln.",
    [UI_DKB_BIND_UP] = "Fähigkeitenmenü öffnen.",
    [UI_DKB_BIND_RIGHT] = "Zur nächsten freigeschalteten Verwandlung wechseln.",
    [UI_DKB_BIND_DOWN] = "Fotomodus öffnen, wenn verfügbar.",
    [UI_DKB_BIND_MOVE] = "Die Spielfigur in diese Richtung bewegen.",
    [UI_DKB_BIND_CAMERA] = "Die Kamera in diese Richtung drehen.",
    [UI_KEY_SPACE] = "Leertaste", [UI_KEY_TAB] = "Tab", [UI_KEY_LSHIFT] = "Umschalt links", [UI_KEY_LCTRL] = "Strg links",
    [UI_KEY_UP] = "Pfeil hoch", [UI_KEY_DOWN] = "Pfeil runter", [UI_KEY_LEFT] = "Pfeil links",
    [UI_KEY_RIGHT] = "Pfeil rechts",
    [UI_KEY_MOUSE_L] = "Linke Maustaste", [UI_KEY_MOUSE_R] = "Rechte Maustaste", [UI_KEY_MOUSE_M] = "Mittlere Maustaste",
    [UI_KEY_MOUSE_X1] = "Maustaste 4", [UI_KEY_MOUSE_X2] = "Maustaste 5",
    [UI_KEY_ENTER] = "Enter", [UI_KEY_BACKSPACE] = "Rücktaste", [UI_KEY_RSHIFT] = "Umschalt rechts",
    [UI_KEY_RCTRL] = "Strg rechts", [UI_KEY_LALT] = "Alt", [UI_KEY_CAPSLOCK] = "Feststelltaste", [UI_KEY_INSERT] = "Einfg",
    [UI_KEY_DELETE] = "Entf", [UI_KEY_HOME] = "Pos1", [UI_KEY_END] = "Ende", [UI_KEY_PAGEUP] = "Bild hoch",
    [UI_KEY_PAGEDOWN] = "Bild runter", [UI_KEY_NUM_FMT] = "Num %s",
    [UI_PAD_SOUTH] = "Untere Taste", [UI_PAD_EAST] = "Rechte Taste", [UI_PAD_WEST] = "Linke Taste",
    [UI_PAD_NORTH] = "Obere Taste", [UI_PAD_LSTICK] = "Linker Stick", [UI_PAD_RSTICK] = "Rechter Stick",
    [UI_PAD_L] = "Linke Schultertaste", [UI_PAD_R] = "Rechte Schultertaste", [UI_PAD_START] = "Start", [UI_PAD_BACK] = "Zurück",
    [UI_PAD_DPAD_LEFT] = "Steuerkreuz links", [UI_PAD_DPAD_UP] = "Steuerkreuz oben", [UI_PAD_DPAD_RIGHT] = "Steuerkreuz rechts",
    [UI_PAD_DPAD_DOWN] = "Steuerkreuz unten", [UI_PAD_ZL] = "Linker Trigger", [UI_PAD_ZR] = "Rechter Trigger",
    [UI_PAD_LPADDLE1] = "Linke Rücktaste", [UI_PAD_RPADDLE1] = "Rechte Rücktaste",
    [UI_PAD_LPADDLE2] = "Linke Rücktaste 2", [UI_PAD_RPADDLE2] = "Rechte Rücktaste 2",
    [UI_PAD_CROSS] = "Kreuz", [UI_PAD_CIRCLE] = "Kreis", [UI_PAD_SQUARE] = "Quadrat", [UI_PAD_TRIANGLE] = "Dreieck",

    [UI_MENU_MENU_LANG] = "Menüsprache", [UI_MENU_MENU_LANG_DESC] = "Legt die Sprache des Einstellungsmenüs und der Leistungsanzeige fest. "
                             "Systemsprache nutzt Deutsch oder Russisch, wenn dein System so eingestellt ist, sonst Englisch.",
    [UI_MENU_MENU_LANG_AUTO] = "Systemsprache",
    [UI_MENU_THEME] = "Menüdesign", [UI_MENU_THEME_DK] = "Donkey Kong", [UI_MENU_THEME_CLASSIC] = "Klassisch",
    [UI_MENU_THEME_DESC] = "Wähle das Donkey-Kong-Design oder das klassische Grün-Gold-Design.",

    [UI_MENU_ADV_WARNING] = "Diese Einstellungen sind zum Testen und zur Fehlersuche. Ändere sie nur, wenn du weißt, warum.",
    [UI_MENU_GROUP_SCHED] = "Ablaufsteuerung", [UI_MENU_GROUP_SHADERS] = "Shader und Pipelines",
    [UI_MENU_GROUP_TEXMEM] = "Texturen und Speicher", [UI_MENU_GROUP_FIXES] = "Render-Korrekturen",
    [UI_MENU_GROUP_VULKAN] = "Vulkan", [UI_MENU_GROUP_DIAG] = "Diagnose",
    [UI_MENU_ASYNC_PREEMPT] = "Vorrang auf virtuellen Kernen",
    [UI_MENU_ASYNC_PREEMPT_DESC] = "Ein Spiel-Thread darf einen anderen auf demselben virtuellen Kern unterbrechen.",
    [UI_MENU_PREEMPT_OFF] = "Aus", [UI_MENU_PREEMPT_ON] = "An", [UI_MENU_PREEMPT_INHERIT] = "Nur Prioritätsvererbung",
    [UI_MENU_REPLAY_ASYNC] = "GPU-Bildvorbereitung auf eigenem Thread",
    [UI_MENU_REPLAY_ASYNC_DESC] = "Bereitet Bilder für die GPU auf einem eigenen Thread statt dem Spiel-Thread vor. Bis zu ein Bild mehr Verzögerung.",
    [UI_MENU_REPLAY_THREADS] = "Threads für große Render-Durchgänge",
    [UI_MENU_REPLAY_THREADS_DESC] = "Zeichnet die Draws großer Render-Durchgänge auf so vielen Threads gleichzeitig auf. Automatisch nutzt 4 Threads ab 8 logischen Prozessoren (nicht auf AMD); Aus nutzt einen Thread. Schneller in vollen Gegenden auf CPUs mit vielen Kernen.",
    [UI_MENU_TIMER_1MS] = "1-ms-Windows-Timer",
    [UI_MENU_TIMER_1MS_DESC] = "Erhöht die Auflösung des System-Timers auf 1 ms für gleichmäßigere Bildzeiten.",
    [UI_MENU_PIPE_WAIT] = "Umgang mit wartenden Draws",
    [UI_MENU_PIPE_WAIT_DESC] = "Standard behält die aktuelle Ablaufsteuerung mit einem reservierten Worker für wartende Draws; Alt (Draw) nutzt die frühere.",
    [UI_MENU_PIPE_WAIT_DEFAULT] = "Standard", [UI_MENU_PIPE_WAIT_DRAW] = "Alt (Draw)",
    [UI_MENU_PIPE_SKIP] = "Übersprungene Draws beim Pipeline-Bau",
    [UI_MENU_PIPE_SKIP_DESC] = "Welche Draws verworfen werden, statt auf ihre Pipeline zu warten.",
    [UI_MENU_PIPE_SKIP_STRICT] = "Streng", [UI_MENU_PIPE_SKIP_ANY] = "Alle",
    [UI_MENU_PIPE_SKIP_FULLSCREEN] = "Vollbild-Durchgänge",
    [UI_MENU_LIB_PREBUILD] = "Stufenbibliotheken vorab bauen",
    [UI_MENU_LIB_PREBUILD_DESC] = "Baut Shader-Stufenbibliotheken vorab im Hintergrund.",
    [UI_MENU_LIB_PREBUILD_THREADS] = "Threads für den Vorab-Bau",
    [UI_MENU_LIB_PREBUILD_THREADS_DESC] = "Threads für den Vorab-Bau; Automatisch wählt eine Zahl passend zur CPU.",
    [UI_MENU_PIPE_PREBUILD] = "Pipelines vorab bauen",
    [UI_MENU_PIPE_PREBUILD_DESC] = "Baut bekannte Pipelines vorab.",
    [UI_MENU_PREWARM_XLATE] = "Übersetzer vorwärmen",
    [UI_MENU_PREWARM_XLATE_DESC] = "Threads, die beim Start bekannte Shader übersetzen; Automatisch nimmt die Hälfte der CPU-Threads, höchstens 16.",
    [UI_MENU_GPL] = "Graphics Pipeline Library",
    [UI_MENU_GPL_DESC] = "Nutzt die Vulkan-Erweiterung für Pipeline-Bibliotheken; schalte sie aus, wenn dein Treiber damit Probleme hat.",
    [UI_MENU_ASTC] = "ASTC-Texturdekodierung", [UI_MENU_ASTC_DESC] = "Wo komprimierte ASTC-Texturen dekodiert werden.",
    [UI_MENU_ASTC_AUTO] = "Automatisch", [UI_MENU_ASTC_GPU] = "GPU-Shader", [UI_MENU_ASTC_NATIVE] = "Nativ",
    [UI_MENU_ASTC_CPU] = "CPU",
    [UI_MENU_MEMORY] = "GPU-Speichermodus", [UI_MENU_MEMORY_DESC] = "Wie der Spielspeicher mit der GPU geteilt wird.",
    [UI_MENU_MEMORY_AUTO] = "Automatisch", [UI_MENU_MEMORY_IMPORT] = "Importieren", [UI_MENU_MEMORY_MIRROR] = "Spiegeln",
    [UI_MENU_MEMORY_VERIFY] = "Prüfen",
    [UI_MENU_TEX_ALIAS] = "Textur-Aliasing", [UI_MENU_TEX_ALIAS_DESC] = "Eine neue Textur im Speicher einer lebenden Textur beginnt mit deren Inhalt statt leer.",
    [UI_MENU_TEX_VRAM_PRESSURE] = "Grenze für Texturspeicher", [UI_MENU_TEX_VRAM_PRESSURE_DESC] = "Prozent des Grafikspeichers, ab dem vom Spiel freigegebene Texturen verworfen werden (bei Bedarf kommen sie vom Speicher zurück). 0 behält sie.",
    [UI_MENU_ASTC_BC] = "ASTC zu BC3 komprimieren", [UI_MENU_ASTC_BC_DESC] = "Mit dem GPU-Dekoder: kodiert dekodierte ASTC-Texturen zu BC3 um, ein Viertel des Grafikspeichers. Für GPUs mit 4 GB oder weniger; Texturen wirken etwas blockiger.",
    [UI_MENU_LOD_GUARD] = "LOD-Schutz", [UI_MENU_LOD_GUARD_DESC] = "Verhindert, dass Objekte für ein Bild an der falschen Stelle aufblitzen oder die Ferne flach wirkt, indem auf das Spiel gewartet wird, bis es die Detailstufen-Daten fertig geschrieben hat.",
    [UI_MENU_FIX_C0] = "Konstantenpuffer-Korrektur",
    [UI_MENU_FIX_C0_DESC] = "Füllt die vom Treiber reservierte Konstantenbank c0 (Speichergrößen, Dispatch-Größe, Punktgröße, Basisinstanz), statt Nullen zu lesen.",
    [UI_MENU_FIX_ZERO] = "Neuen GPU-Speicher nullen",
    [UI_MENU_FIX_ZERO_DESC] = "Neue GPU-Speicherpools beginnen mit Nullen, wie Switch-Speicher.",
    [UI_MENU_LIST_RESTART] = "Primitive Restart bei Listen",
    [UI_MENU_LIST_RESTART_DESC] = "Wendet den Primitive Restart des Spiels auf Dreieckslisten an, wie die Switch. Ohne ihn werden die Restart-Indizes als Eckpunkte gezeichnet: lange gestreckte Dreiecke, etwa auf der Karte. Braucht eine GPU mit primitiveTopologyListRestart.",
    [UI_MENU_BIND_SKIP] = "Wiederholte Bindungen überspringen",
    [UI_MENU_BIND_SKIP_DESC] = "Optimierung: überspringt Bindungen, die sich nicht geändert haben.",
    [UI_MENU_SHADER_MEMO] = "Shader-Merker", [UI_MENU_SHADER_MEMO_DESC] = "Optimierung: merkt sich Shader-Suchen.",
    [UI_MENU_PIPE_MEMO] = "Pipeline-Merker", [UI_MENU_PIPE_MEMO_DESC] = "Optimierung: nutzt die Pipeline des vorigen Draws erneut, wenn sich sein Zustand wiederholt.",
    [UI_MENU_TEX_MEMO] = "Texturbindungs-Merker", [UI_MENU_TEX_MEMO_DESC] = "Optimierung: merkt sich Textur-Deskriptoren zwischen Draws.",
    [UI_MENU_DESC_TEMPLATE] = "Deskriptor-Vorlagen",
    [UI_MENU_DESC_TEMPLATE_DESC] = "Optimierung: aktualisiert Deskriptoren gebündelt.",
    [UI_MENU_PUSH_SKIP] = "Wiederholte Push-Konstanten überspringen",
    [UI_MENU_PUSH_SKIP_DESC] = "Optimierung: überspringt Push-Konstanten, die sich nicht geändert haben.",
    [UI_MENU_SPLIT_DRAWS] = "Frühe GPU-Übergabe",
    [UI_MENU_SPLIT_DRAWS_DESC] = "Übergibt GPU-Arbeit alle N Draws zwischen Durchgängen in Teilen, damit die GPU früher anfängt; 0 schaltet es aus.",
    [UI_MENU_VK_VALIDATION] = "Validierungsschicht",
    [UI_MENU_VK_VALIDATION_DESC] = "Prüft jeden Vulkan-Aufruf und verlangsamt das Spiel stark.",
    [UI_MENU_VK_VALIDATION_MISSING] = "Die Validierungsschicht ist nicht installiert.",
    [UI_MENU_VK_DYN_VERTEX] = "Dynamischer Vertex-Zustand",
    [UI_MENU_VK_DYN_VERTEX_DESC] = "Erzwingt dynamische Vertex-Eingabe an oder aus; der Standard hängt von der GPU ab.",
    [UI_MENU_VK_DYN_A2C] = "Dynamisches Alpha-to-Coverage",
    [UI_MENU_VK_DYN_A2C_DESC] = "Erzwingt dynamisches Alpha-to-Coverage an oder aus. Standard: aus beim AMD-Treiber für RDNA 2 und "
                                "älter, der Figuren damit schwarz zeichnet, sonst an.",
    [UI_MENU_VK_SUBGROUP_PIN] = "Subgroup-Größe festlegen", [UI_MENU_VK_SUBGROUP_PIN_DESC] = "Legt die Shader-Subgroup-Größe fest.",
    [UI_MENU_TRACE] = "Leistungsprotokoll", [UI_MENU_TRACE_DESC] = "Schreibt Bildzeiten in eine Datei.",
    [UI_MENU_FLIGHT] = "Flugschreiber",
    [UI_MENU_FLIGHT_DESC] = "Behält einen kurzen Verlauf und speichert ihn, wenn ein Bild hängt.",
    [UI_MENU_FLIGHT_SPIKE_MS] = "Hänger-Schwelle (ms)",
    [UI_MENU_FLIGHT_SPIKE_MS_DESC] = "Bildzeit, die als Hänger zählt.",
    [UI_MENU_INPUT_RECORD] = "Eingaben aufzeichnen",
    [UI_MENU_INPUT_RECORD_DESC] = "Schreibt deine Eingaben für Fehlerberichte in eine Datei.",
    [UI_MENU_OPEN_LOG] = "Protokollordner öffnen", [UI_MENU_OPEN_LOG_DESC] = "Zeigt den Ordner mit den Protokolldateien.",
    [UI_MENU_COPY_INFO] = "Systeminfo kopieren",
    [UI_MENU_COPY_INFO_DESC] = "Kopiert GPU, Treiber, CPU, RAM und geänderte Einstellungen in die Zwischenablage.",
    [UI_MENU_COPY_INFO_DONE] = "Kopiert", [UI_MENU_ACTION_FAILED] = "hat nicht geklappt",
    [UI_MENU_RESET] = "Alle Einstellungen zurücksetzen", [UI_MENU_RESET_DESC] = "Gilt für alle Reiter.",
    [UI_MENU_RESET_CONFIRM] = "Alle Einstellungen auf Standard zurücksetzen?", [UI_MENU_RESET_YES] = "Zurücksetzen",
    [UI_MENU_RESET_NO] = "Abbrechen",

    [UI_MENU_OV_FPS] = "%.0f FPS", [UI_MENU_OV_FRAME_MS] = "%.1f ms", [UI_MENU_OV_GAME_PCT] = "Spiel %.0f%%",
    [UI_MENU_OV_MAX_MS] = "max. %.1f ms",
    [UI_MENU_OV_GPU] = "GPU", [UI_MENU_OV_CPU] = "CPU", [UI_MENU_OV_RAM] = "RAM", [UI_MENU_OV_VRAM] = "VRAM",
    [UI_MENU_OV_TEMP] = "Temperatur", [UI_MENU_OV_POWER] = "Leistung", [UI_MENU_OV_CLOCK] = "Takt",
    [UI_MENU_OV_NONE] = "-",
    [UI_MENU_OV_REPLAY] = "GPU-Bildvorbereitung", [UI_MENU_OV_GPU_TIME] = "GPU-Last pro Bild",
    [UI_MENU_OV_PIPE_WAIT] = "Pipeline-Wartezeiten",
    [UI_MENU_OV_SHADER_WAIT] = "Shader-Wartezeiten", [UI_MENU_OV_TEX_UPLOADS] = "Textur-Uploads",
    [UI_MENU_OV_FMT_PCT] = "%.0f%%", [UI_MENU_OV_FMT_TEMP] = "%.0f °C", [UI_MENU_OV_FMT_POWER] = "%.0f W",
    [UI_MENU_OV_FMT_MHZ] = "%.0f MHz", [UI_MENU_OV_FMT_GB] = "%.1f GB", [UI_MENU_OV_FMT_GB_OF] = "/ %.1f GB",
    [UI_MENU_OV_GAME_GB] = "Spiel %.1f GB", [UI_MENU_OV_FMT_MS] = "%.2f ms", [UI_MENU_OV_PER_FRAME] = "%.1f pro Bild",
    [UI_MENU_OV_WINDOW] = "Fenster", [UI_MENU_OV_OUTPUT] = "Spielbild", [UI_MENU_OV_SCENE] = "3D-Szene",
    [UI_MENU_OV_SCALING] = "Skalierung", [UI_MENU_OV_LIMIT] = "Bildlimit", [UI_MENU_OV_PRESENT] = "Bildausgabe",
    [UI_MENU_OV_FMT_SIZE] = "%ux%u", [UI_MENU_OV_FMT_SCENE] = "%ux%u, %.0f%%",
    [UI_MENU_OV_FMT_UPSCALE] = "%ux%u -> %ux%u",
    [UI_MENU_OV_NO_UPSCALER] = "kein Upscaler",
    [UI_MENU_OV_FMT_FSR] = "AMD FSR 1, Schärfe %d%%", [UI_MENU_OV_SCALE_NONE] = "keine (1:1)",
    [UI_MENU_OV_SCALE_BILINEAR] = "bilinear",
};

#define SWKBD_ONLY(name, ...) static const char *const name[UI_TEXT_COUNT] = {SWKBD(__VA_ARGS__)}
SWKBD_ONLY(text_ja, "文字入力", "名前を入力してください", "ウマ", "OK", "キャンセル", "コントローラー: A = OK、B = キャンセル");
SWKBD_ONLY(text_fr, "Saisie de texte", "Entrez un nom", "Cheval", "OK", "Annuler", "Manette : A = OK, B = Annuler");
SWKBD_ONLY(text_it, "Inserimento testo", "Inserisci un nome", "Cavallo", "OK", "Annulla", "Controller: A = OK, B = Annulla");
SWKBD_ONLY(text_es, "Introducir texto", "Introduce un nombre", "Caballo", "Aceptar", "Cancelar",
           "Mando: A = Aceptar, B = Cancelar");
SWKBD_ONLY(text_nl, "Tekstinvoer", "Voer een naam in", "Paard", "OK", "Annuleren", "Controller: A = OK, B = Annuleren");
SWKBD_ONLY(text_pt, "Inserir texto", "Introduz um nome", "Cavalo", "OK", "Cancelar", "Comando: A = OK, B = Cancelar");
SWKBD_ONLY(text_zh_hans, "文字输入", "请输入名字", "马儿", "确定", "取消", "手柄：A 确定，B 取消");
SWKBD_ONLY(text_zh_hant, "文字輸入", "請輸入名字", "馬兒", "確定", "取消", "手把：A 確定，B 取消");
SWKBD_ONLY(text_ko, "문자 입력", "이름을 입력하세요", "말", "확인", "취소", "컨트롤러: A 확인, B 취소");

static const Row rows[] = {
    {"en", text_en}, {"ja", text_ja}, {"fr", text_fr}, {"de", text_de}, {"it", text_it}, {"es", text_es},
    {"nl", text_nl}, {"pt", text_pt}, {"ru", text_ru}, {"zh-Hans", text_zh_hans}, {"zh-Hant", text_zh_hant},
    {"ko", text_ko},
};

const char *ui_text_lang(const char *code) {
    static const struct { const char *code, *lang; } map[] = {
        {"en-US", "en"}, {"en-GB", "en"}, {"fr-CA", "fr"}, {"es-419", "es"}, {"pt-BR", "pt"},
        {"zh-CN", "zh-Hans"}, {"zh-TW", "zh-Hant"},
    };
    for (size_t i = 0; i < sizeof map / sizeof *map; i++)
        if (!strcmp(code, map[i].code)) return map[i].lang;
    return code;
}

static const Row *find_row(const char *lang) {
    for (size_t i = 0; i < sizeof rows / sizeof *rows; i++)
        if (!strcmp(rows[i].lang, lang)) return &rows[i];
    return NULL;
}

int ui_text_defined(const char *lang, UiTextId id) {
    const Row *r = find_row(lang);
    return (unsigned)id < UI_TEXT_COUNT && r && r->text[id] && *r->text[id];
}

const char *ui_text_in(const char *lang, UiTextId id) {
    if ((unsigned)id >= UI_TEXT_COUNT) return "";
    return ui_text_defined(lang, id) ? find_row(lang)->text[id] : rows[0].text[id];
}

const char *ui_text(UiTextId id) { return ui_text_in(ui_text_lang(ov_language()), id); }

// The menu language, re-read when cfg_version changes. TOTK_MENU_LANG "en", "ru" or "de"; anything else (unset
// included) follows the system UI language: Russian for "ru", "ru-RU", ..., German for "de", "de-DE", ..., else English.
static const char *resolve_menu_lang(void) {
    const char *v = cfg_value("TOTK_MENU_LANG");
    if (v && (!strcmp(v, "ru") || !strcmp(v, "en") || !strcmp(v, "de"))) return v;
    const char *sys = plat_ui_language();
    if (sys && (!strcmp(sys, "ru") || !strncmp(sys, "ru-", 3))) return "ru";
    if (sys && (!strcmp(sys, "de") || !strncmp(sys, "de-", 3))) return "de";
    return "en";
}

const char *menu_text(UiTextId id) {
    static _Atomic(const char *) lang = "en";
    static _Atomic unsigned seen = ~0u;
    unsigned v = cfg_version();
    if (v != atomic_load_explicit(&seen, memory_order_relaxed)) {
        atomic_store_explicit(&lang, resolve_menu_lang(), memory_order_relaxed);
        atomic_store_explicit(&seen, v, memory_order_relaxed);
    }
    return ui_text_in(atomic_load_explicit(&lang, memory_order_relaxed), id);
}
