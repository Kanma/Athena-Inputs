/** @file       Declarations.h
    @author     Philip Abbet

    Declaration of constants and type related to the inputs
*/

#ifndef _ATHENA_INPUTS_DECLARATIONS_H_
#define _ATHENA_INPUTS_DECLARATIONS_H_

#include <Athena-Inputs/Prerequisites.h>
#include <OIS/OISPrereqs.h>
#include <string>


namespace Athena {
namespace Inputs {


/************************************** TYPES ******************************************/

typedef unsigned int tVirtualID;        ///< Represents a part of a virtual controller
typedef unsigned char tKey;             ///< Represents a key
typedef unsigned char tAxis;            ///< Represents a axis
typedef unsigned char tPOV;             ///< Represents a point-of-view (POV)
typedef unsigned char tPOVPosition;     ///< Represents a POV position


//---------------------------------------------------------------------------------------
/// @brief  Enumerates the parts of a controller
//---------------------------------------------------------------------------------------
enum tControllerPart
{
    PART_KEY,           ///< Key (Keyboard, Mouse, Gamepad)
    PART_AXIS,          ///< Axis (Mouse, Gamepad)
    PART_POV,           ///< Point-of-view (Gamepad)
};


union tInputEventPart
{
    tKey        key;                ///< Key ID (if part == PART_KEY)
    tAxis       axis;               ///< Axis ID (if part == PART_AXIS)
    tPOV        pov;                ///< POV ID (if part == PART_POV)
};


union tInputEventValue
{
    bool            bPressed;       ///< Indicates if the key is pressed or not
    int             iValue;         ///< Value of the axis
    tPOVPosition    position;       ///< Position of the POV
};


//---------------------------------------------------------------------------------------
/// @brief  Represent an event coming from a controller
//---------------------------------------------------------------------------------------
struct tInputEvent
{
    Controller*         pController;        ///< The controller
    tControllerPart     part;               ///< Part on the controller
    tInputEventPart     partID;
    tInputEventValue    value;
    unsigned long       ulTimeStamp;        ///< Timestamp of the event
};


union tVirtualEventValue
{
    bool            bPressed;       ///< Indicates if the virtual key is pressed or not
    int             iValue;         ///< Value of the virtual axis
    tPOVPosition    position;       ///< Position of the virtual POV
};


//-----------------------------------------------------------------------------------
/// @brief  Represents a virtual event (used by a virtual controller)
//-----------------------------------------------------------------------------------
struct tVirtualEvent
{
    tVirtualID          virtualID;      ///< Virtual ID on which occured the event
    tControllerPart     part;           ///< Part type of the virtual ID
    unsigned long       ulTimestamp;    ///< Timestamp of the event
    tVirtualEventValue  value;
};


//-----------------------------------------------------------------------------------
/// @brief  Represents a virtual key (on a virtual controller)
//-----------------------------------------------------------------------------------
struct tVirtualKey
{
    Controller*     pController;        ///< The real controller
    tKey            key;                ///< The real key
    bool            bPressed;           ///< Indicates if the virtual key is pressed or not
    bool            bToggled;           ///< Indicates if the virtual key was just toggled
    bool            bHasShortcut;       ///< Indicates if the virtual key has a shortcut
    unsigned long   ulPressTimestamp;   ///< Timestamp of the last press of the key
    unsigned long   ulReleaseTimestamp; ///< Timestamp of the last release of the key
};


struct tVirtualAxisRealPartPOV
{
    tPOV    pov;            ///< The real POV used to make the virtual axis
    bool    bUpDown;        ///< Indicates if the axis is on the Up/Down part of the POV
};                          ///< The real POV used to make the virtual axis


struct tVirtualAxisRealPartKeys
{
    tKey    keyMin;         ///< The real key mapped to the minimum side of the virtual axis
    tKey    keyMax;         ///< The real key mapped to the maximum side of the virtual axis
};


union tVirtualAxisRealPart
{
    tAxis                       axis;   ///< The real axis
    tVirtualAxisRealPartPOV     pov;    ///< The real POV used to make the virtual axis
    tVirtualAxisRealPartKeys    keys;   ///< The real keys used to make the virtual axis
};


//-----------------------------------------------------------------------------------
/// @brief  Represents a virtual axis (on a virtual controller)
///
/// A virtual axis can be made from:
///    - A real axis
///    - A POV (either the up/down part, or the left/right one)
///    - Two keys
//-----------------------------------------------------------------------------------
struct tVirtualAxis
{
    Controller*             pController;        ///< The real controller
    tControllerPart         part;               ///< Indicates from which part(s) this virtual axis is made
    bool                    bChanged;           ///< Indicates if the axis value has changed
    tVirtualAxisRealPart    realPart;
    int                     iValue;             ///< Value of the axis
    unsigned long           ulTimestamp;        ///< Timestamp of the last change
};


struct tVirtualPOVShortcuts
{
    std::string strShortcutUp;          ///< Shortcut for the UP position
    std::string strShortcutDown;        ///< Shortcut for the DOWN position
    std::string strShortcutLeft;        ///< Shortcut for the LEFT position
    std::string strShortcutRight;       ///< Shortcut for the RIGHT position
};


struct tVirtualPOVRealPartKeys
{
    tKey    keyUp;                      ///< The real key mapped to the UP side of the virtual POV
    tKey    keyDown;                    ///< The real key mapped to the DOWN side of the virtual POV
    tKey    keyLeft;                    ///< The real key mapped to the LEFT side of the virtual POV
    tKey    keyRight;                   ///< The real key mapped to the RIGHT side of the virtual POV
};


struct tVirtualPOVRealPartAxes
{
    tAxis           axisUpDown;         ///< The real axis mapped to the UP/DOWN part of the virtual POV
    tAxis           axisLeftRight;      ///< The real axis mapped to the LEFT/RIGHT part of the virtual POV
    int             iUpDownPos;         ///< Store the UP/DOWN axis position
    int             iLeftRightPos;      ///< Store the ULEFT/RIGHT axis position
    tPOVPosition    tempPosition;       ///< Temporary value used by the computation of the POV position
};


union tVirtualPOVRealPart
{
    tPOV                    pov;        ///< The real POV
    tVirtualPOVRealPartKeys keys;       ///< The real keys used to make the virtual POV
    tVirtualPOVRealPartAxes axes;       ///< The real axes used to make the virtual POV
};


//-----------------------------------------------------------------------------------
/// @brief  Represents a virtual POV (on a virtual controller)
///
/// A virtual POV can be made from:
///    - A real POV
///    - Two axes
///    - Four keys
///
/// The shortcuts of a POV can be combined to make 'diagonals' shortcuts.
//-----------------------------------------------------------------------------------
struct tVirtualPOV
{
    Controller*             pController;                ///< The real controller
    tControllerPart         part;                       ///< Indicates from which part(s) this virtual POV is made
    bool                    bChanged;                   ///< Indicates if the POV value has changed
    tVirtualPOVRealPart     realPart;
    tVirtualPOVShortcuts    shortcuts;                  ///< Shortcut of the virtual POV
    tPOVPosition            position;                   ///< Current position of the virtual POV
    tPOVPosition            previousPosition;           ///< Previous position of the virtual POV
    unsigned long           ulLastChangeTimestamp;      ///< Timestamp of the last change of position
    unsigned long           ulPreviousChangeTimestamp;  ///< Timestamp of the previous change of position
};


/************************************** CONSTANTS **************************************/

// Possible positions for a point-of-view
const tPOVPosition POV_CENTER       = 0;                    ///< Center position of a POV
const tPOVPosition POV_UP           = 1;                    ///< Up position of a POV
const tPOVPosition POV_DOWN         = 2;                    ///< Down position of a POV
const tPOVPosition POV_RIGHT        = 4;                    ///< Right position of a POV
const tPOVPosition POV_LEFT         = 8;                    ///< Left position of a POV
const tPOVPosition POV_UPLEFT       = POV_UP | POV_LEFT;    ///< Up/Left position of a POV
const tPOVPosition POV_UPRIGHT      = POV_UP | POV_RIGHT;   ///< Up/Right position of a POV
const tPOVPosition POV_DOWNLEFT     = POV_DOWN | POV_LEFT;  ///< Down/Left position of a POV
const tPOVPosition POV_DOWNRIGHT    = POV_DOWN | POV_RIGHT; ///< Down/Right position of a POV

// Axes
const tAxis AXIS_X          = 1;                            ///< X-axis
const tAxis AXIS_Y          = 2;                            ///< Y-axis
const tAxis AXIS_Z          = 4;                            ///< Z-axis
const tAxis AXIS_X_ROT      = 8;                            ///< X rotation axis
const tAxis AXIS_Y_ROT      = 16;                           ///< Y rotation axis
const tAxis AXIS_Z_ROT      = 32;                           ///< Z rotation axis
const tAxis SLIDER_0        = 64;                           ///< Slider 0
const tAxis SLIDER_1        = 128;                          ///< Slider 1
const tAxis AXIS_ALL        = 255;                          ///< All the axes

// Mouse keys
const tKey MOUSEKEY_LEFT    = 0x01;                         ///< Left mouse key
const tKey MOUSEKEY_RIGHT   = 0x02;                         ///< Right mouse key
const tKey MOUSEKEY_CENTER  = 0x03;                         ///< Center mouse key
const tKey MOUSEKEY_X1      = 0x04;                         ///< X1 mouse key
const tKey MOUSEKEY_X2      = 0x05;                         ///< X2 mouse key
const tKey MOUSEKEY_OTHER   = 0x06;                         ///< Use 'MOUSEKEY_OTHER + X' to reference keys above MOUSEKEY_X2

// Keyboards keys
const tKey KEY_ESCAPE       = 0x01;                         ///< Escape key
const tKey KEY_1            = 0x02;                         ///< Key '1' (on main keyboard)
const tKey KEY_2            = 0x03;                         ///< Key '2' (on main keyboard)
const tKey KEY_3            = 0x04;                         ///< Key '3' (on main keyboard)
const tKey KEY_4            = 0x05;                         ///< Key '4' (on main keyboard)
const tKey KEY_5            = 0x06;                         ///< Key '5' (on main keyboard)
const tKey KEY_6            = 0x07;                         ///< Key '6' (on main keyboard)
const tKey KEY_7            = 0x08;                         ///< Key '7' (on main keyboard)
const tKey KEY_8            = 0x09;                         ///< Key '8' (on main keyboard)
const tKey KEY_9            = 0x0A;                         ///< Key '9' (on main keyboard)
const tKey KEY_0            = 0x0B;                         ///< Key '0' (on main keyboard)
const tKey KEY_MINUS        = 0x0C;                         ///< Minus key (on main keyboard)
const tKey KEY_EQUALS       = 0x0D;                         ///< Equals key
const tKey KEY_BACKSPACE    = 0x0E;                         ///< Backspace key
const tKey KEY_TAB          = 0x0F;                         ///< Tabulation key
const tKey KEY_Q            = 0x10;                         ///< Key 'Q'
const tKey KEY_W            = 0x11;                         ///< Key 'W'
const tKey KEY_E            = 0x12;                         ///< Key 'E'
const tKey KEY_R            = 0x13;                         ///< Key 'R'
const tKey KEY_T            = 0x14;                         ///< Key 'T'
const tKey KEY_Y            = 0x15;                         ///< Key 'Y'
const tKey KEY_U            = 0x16;                         ///< Key 'U'
const tKey KEY_I            = 0x17;                         ///< Key 'I'
const tKey KEY_O            = 0x18;                         ///< Key 'O'
const tKey KEY_P            = 0x19;                         ///< Key 'P'
const tKey KEY_LBRACKET     = 0x1A;                         ///< Key '('
const tKey KEY_RBRACKET     = 0x1B;                         ///< Key ')'
const tKey KEY_RETURN       = 0x1C;                         ///< Enter key (on main keyboard)
const tKey KEY_LCONTROL     = 0x1D;                         ///< Left Ctrl key
const tKey KEY_A            = 0x1E;                         ///< Key 'A'
const tKey KEY_S            = 0x1F;                         ///< Key 'S'
const tKey KEY_D            = 0x20;                         ///< Key 'D'
const tKey KEY_F            = 0x21;                         ///< Key 'F'
const tKey KEY_G            = 0x22;                         ///< Key 'G'
const tKey KEY_H            = 0x23;                         ///< Key 'H'
const tKey KEY_J            = 0x24;                         ///< Key 'J'
const tKey KEY_K            = 0x25;                         ///< Key 'K'
const tKey KEY_L            = 0x26;                         ///< Key 'L'
const tKey KEY_SEMICOLON    = 0x27;                         ///< Key ')'
const tKey KEY_APOSTROPHE   = 0x28;                         ///< Key ';'
const tKey KEY_GRAVE        = 0x29;                         ///< Key 'È'
const tKey KEY_LSHIFT       = 0x2A;                         ///< Left shift key
const tKey KEY_BACKSLASH    = 0x2B;                         ///< Key '\'
const tKey KEY_Z            = 0x2C;                         ///< Key 'Z'
const tKey KEY_X            = 0x2D;                         ///< Key 'X'
const tKey KEY_C            = 0x2E;                         ///< Key 'C'
const tKey KEY_V            = 0x2F;                         ///< Key 'V'
const tKey KEY_B            = 0x30;                         ///< Key 'B'
const tKey KEY_N            = 0x31;                         ///< Key 'N'
const tKey KEY_M            = 0x32;                         ///< Key 'M'
const tKey KEY_COMMA        = 0x33;                         ///< Key ','
const tKey KEY_POINT        = 0x34;                         ///< Key '.' (on main keyboard)
const tKey KEY_SLASH        = 0x35;                         ///< Key '/' (on main keyboard)
const tKey KEY_RSHIFT       = 0x36;                         ///< Right shift key
const tKey KEY_NUMPADSTAR   = 0x37;                         ///< Key '*' (on the numpad)
const tKey KEY_LALT         = 0x38;                         ///< Left Alt key
const tKey KEY_SPACE        = 0x39;                         ///< Space key
const tKey KEY_CAPITAL      = 0x3A;                         ///< Caps lock key
const tKey KEY_F1           = 0x3B;                         ///< Key 'F1'
const tKey KEY_F2           = 0x3C;                         ///< Key 'F2'
const tKey KEY_F3           = 0x3D;                         ///< Key 'F3'
const tKey KEY_F4           = 0x3E;                         ///< Key 'F4'
const tKey KEY_F5           = 0x3F;                         ///< Key 'F5'
const tKey KEY_F6           = 0x40;                         ///< Key 'F6'
const tKey KEY_F7           = 0x41;                         ///< Key 'F7'
const tKey KEY_F8           = 0x42;                         ///< Key 'F8'
const tKey KEY_F9           = 0x43;                         ///< Key 'F9'
const tKey KEY_F10          = 0x44;                         ///< Key 'F10'
const tKey KEY_NUMLOCK      = 0x45;                         ///< Num lock key
const tKey KEY_SCROLL_LOCK  = 0x46;                         ///< Scroll lock key
const tKey KEY_NUMPAD7      = 0x47;                         ///< Key '7' (on the numpad)
const tKey KEY_NUMPAD8      = 0x48;                         ///< Key '8' (on the numpad)
const tKey KEY_NUMPAD9      = 0x49;                         ///< Key '9' (on the numpad)
const tKey KEY_NUMPADMINUS  = 0x4A;                         ///< Minus key (on the numpad)
const tKey KEY_NUMPAD4      = 0x4B;                         ///< Key '4' (on the numpad)
const tKey KEY_NUMPAD5      = 0x4C;                         ///< Key '5' (on the numpad)
const tKey KEY_NUMPAD6      = 0x4D;                         ///< Key '6' (on the numpad)
const tKey KEY_NUMPADPLUS   = 0x4E;                         ///< Plus key (on the numpad)
const tKey KEY_NUMPAD1      = 0x4F;                         ///< Key '1' (on the numpad)
const tKey KEY_NUMPAD2      = 0x50;                         ///< Key '2' (on the numpad)
const tKey KEY_NUMPAD3      = 0x51;                         ///< Key '3' (on the numpad)
const tKey KEY_NUMPAD0      = 0x52;                         ///< Key '0' (on the numpad)
const tKey KEY_NUMPADPOINT  = 0x53;                         ///< Key '.' (on the numpad)
const tKey KEY_F11          = 0x57;                         ///< Key 'F11'
const tKey KEY_F12          = 0x58;                         ///< Key 'F12'
const tKey KEY_NUMPADENTER  = 0x9C;                         ///< Enter key (on the numpad)
const tKey KEY_RCONTROL     = 0x9D;                         ///< Right Ctrl key
const tKey KEY_NUMPADSLASH  = 0xB5;                         ///< Key '/' (on the numpad)
const tKey KEY_SYSRQ        = 0xB7;                         ///< SysRq key
const tKey KEY_RALT         = 0xB8;                         ///< Right Alt key
const tKey KEY_PAUSE        = 0xC5;                         ///< Pause key
const tKey KEY_HOME         = 0xC7;                         ///< Home key
const tKey KEY_UP           = 0xC8;                         ///< Up arrow key
const tKey KEY_PGUP         = 0xC9;                         ///< Page up key
const tKey KEY_LEFT         = 0xCB;                         ///< Left arrow key
const tKey KEY_RIGHT        = 0xCD;                         ///< Right arrow key
const tKey KEY_END          = 0xCF;                         ///< End key
const tKey KEY_DOWN         = 0xD0;                         ///< Down arrow key
const tKey KEY_PGDOWN       = 0xD1;                         ///< Page down key
const tKey KEY_INSERT       = 0xD2;                         ///< Insert key
const tKey KEY_DELETE       = 0xD3;                         ///< Delete key
const tKey KEY_LWIN         = 0xDB;                         ///< Left Windows key
const tKey KEY_RWIN         = 0xDC;                         ///< Right Windows key
const tKey KEY_APPS         = 0xDD;                         ///< AppMenu key

// The following keys are only found on Mac keyboards
const tKey KEY_F13          = 0xF0;                         ///< Key 'F13'
const tKey KEY_F14          = 0xF1;                         ///< Key 'F14'
const tKey KEY_F15          = 0xF2;                         ///< Key 'F15'
const tKey KEY_F16          = 0xF3;                         ///< Key 'F16'
const tKey KEY_F17          = 0xF4;                         ///< Key 'F17'
const tKey KEY_F18          = 0xF5;                         ///< Key 'F18'
const tKey KEY_F19          = 0xF6;                         ///< Key 'F19'
const tKey KEY_F20          = 0xF7;                         ///< Key 'F20'
const tKey KEY_VOLUMEUP     = 0xF8;                         ///< Volume up
const tKey KEY_VOLUMEDOWN   = 0xF9;                         ///< Volume down
const tKey KEY_MUTE         = 0xFA;                         ///< Mute

}
}

#endif
