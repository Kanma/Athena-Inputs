/** @file   MacKeyboard.cpp
    @author Philip Abbet

    Implementation of the class 'Athena::Inputs::MacKeyboard'
*/

#include <Athena-Inputs/osx/MacKeyboard.h>
#include <Athena-Core/Log/LogManager.h>
#include <Athena-Inputs/IEventsListener.h>


using namespace Athena;
using namespace Athena::Inputs;
using namespace Athena::Log;
using namespace std;


/******************************************* CONSTANTS ********************************************/

static const char*  __CONTEXT__ = "Mac Keyboard";

// Events we subscribe to and remove from queue
const EventTypeSpec DownEvents[] =
{
    { kEventClassKeyboard, kEventRawKeyDown },  // Non-repeats
    { kEventClassKeyboard, kEventRawKeyRepeat } // Repeats
};

const EventTypeSpec UpEvents = { kEventClassKeyboard, kEventRawKeyUp };

const EventTypeSpec ModEvents = { kEventClassKeyboard, kEventRawKeyModifiersChanged };

const EventTypeSpec AllEvents[] =
{
    { kEventClassKeyboard, kEventRawKeyDown },
    { kEventClassKeyboard, kEventRawKeyRepeat },
    { kEventClassKeyboard, kEventRawKeyUp },
    { kEventClassKeyboard, kEventRawKeyModifiersChanged }
};


struct tKeysInfos
{
    tKey        key;
    const char* strName;
};


const tKeysInfos KEY_INFOS[] =
{
    { KEY_A,            "A" },              // kVK_ANSI_A, 0x00
    { KEY_S,            "S" },              // kVK_ANSI_S, 0x01
    { KEY_D,            "D" },              // kVK_ANSI_D, 0x02
    { KEY_F,            "F" },              // kVK_ANSI_F, 0x03
    { KEY_H,            "H" },              // kVK_ANSI_H, 0x04
    { KEY_G,            "G" },              // kVK_ANSI_G, 0x05
    { KEY_Z,            "Z" },              // kVK_ANSI_Z, 0x06
    { KEY_X,            "X" },              // kVK_ANSI_X, 0x07
    { KEY_C,            "C" },              // kVK_ANSI_C, 0x08
    { KEY_V,            "V" },              // kVK_ANSI_V, 0x09
    { 0, 0 },                               // ???, 0x0A
    { KEY_B,            "B" },              // kVK_ANSI_B, 0x0B
    { KEY_Q,            "Q" },              // kVK_ANSI_Q, 0x0C
    { KEY_W,            "W" },              // kVK_ANSI_W, 0x0D
    { KEY_E,            "E" },              // kVK_ANSI_E, 0x0E
    { KEY_R,            "R" },              // kVK_ANSI_R, 0x0F
    { KEY_Y,            "Y" },              // kVK_ANSI_Y, 0x10
    { KEY_T,            "T" },              // kVK_ANSI_T, 0x11
    { KEY_1,            "1" },              // kVK_ANSI_1, 0x12
    { KEY_2,            "2" },              // kVK_ANSI_2, 0x13
    { KEY_3,            "3" },              // kVK_ANSI_3, 0x14
    { KEY_4,            "4" },              // kVK_ANSI_4, 0x15
    { KEY_6,            "6" },              // kVK_ANSI_6, 0x16
    { KEY_5,            "5" },              // kVK_ANSI_5, 0x17
    { KEY_EQUALS,       "=" },              // kVK_ANSI_Equal, 0x18
    { KEY_9,            "9" },              // kVK_ANSI_9, 0x19
    { KEY_7,            "7" },              // kVK_ANSI_7, 0x1A
    { KEY_MINUS,        "-" },              // kVK_ANSI_Minus, 0x1B
    { KEY_8,            "8" },              // kVK_ANSI_8, 0x1C
    { KEY_0,            "0" },              // kVK_ANSI_0, 0x1D
    { KEY_RBRACKET,     "]" },              // kVK_ANSI_RightBracket, 0x1E
    { KEY_O,            "O" },              // kVK_ANSI_O, 0x1F
    { KEY_U,            "U" },              // kVK_ANSI_U, 0x20
    { KEY_LBRACKET,     "[" },              // kVK_ANSI_LeftBracket, 0x21
    { KEY_I,            "I" },              // kVK_ANSI_I, 0x22
    { KEY_P,            "P" },              // kVK_ANSI_P, 0x23
    { KEY_RETURN,       "Return" },         // kVK_Return, 0x24
    { KEY_L,            "L" },              // kVK_ANSI_L, 0x25
    { KEY_J,            "J" },              // kVK_ANSI_J, 0x26
    { KEY_APOSTROPHE,   "\"" },             // kVK_ANSI_Quote, 0x27
    { KEY_K,            "K" },              // kVK_ANSI_K, 0x28
    { KEY_SEMICOLON,    ";" },              // kVK_ANSI_Semicolon, 0x29
    { KEY_BACKSLASH,    "\\" },             // kVK_ANSI_Backslash, 0x2A
    { KEY_COMMA,        "," },              // kVK_ANSI_Comma    , 0x2B
    { KEY_SLASH,        "/" },              // kVK_ANSI_Slash    , 0x2C
    { KEY_N,            "N" },              // kVK_ANSI_N, 0x2D
    { KEY_M,            "M" },              // kVK_ANSI_M, 0x2E
    { KEY_POINT,        "." },              // kVK_ANSI_Period, 0x2F
    { KEY_TAB,          "Tab" },            // kVK_Tab, 0x30
    { KEY_SPACE,        "Space" },          // kVK_Space, 0x31
    { KEY_GRAVE,        "`" },              // kVK_ANSI_Grave, 0x32
    { KEY_BACKSPACE,    "Backspace" },      // kVK_Delete, 0x33
    { 0, 0 },                               // ???, 0x34
    { KEY_ESCAPE,       "Escape" },         // kVK_Escape, 0x35
    { 0, 0 },                               // ???, 0x36
    { KEY_LWIN,         "Command" },        // kVK_Command, 0x37
    { KEY_LSHIFT,       "Shift" },          // kVK_Shift, 0x38
    { KEY_CAPITAL,      "Caps Lock" },      // kVK_CapsLock, 0x39
    { KEY_LALT,         "Option" },         // kVK_Option, 0x3A
    { KEY_LCONTROL,     "Control" },        // kVK_Control, 0x3B
    { KEY_RSHIFT,       "Right shift" },    // kVK_RightShift, 0x3C
    { KEY_RALT,         "Right option" },   // kVK_RightOption, 0x3D
    { KEY_RCONTROL,     "Right control" },  // kVK_RightControl, 0x3E
    { KEY_APPS,         "fn" },             // kVK_Function, 0x3F
    { KEY_F17,          "F17" },            // kVK_F17, 0x40
    { KEY_NUMPADPOINT,  ". (Num)" },        // kVK_ANSI_KeypadDecimal, 0x41
    { 0, 0 },                               // ???, 0x42
    { KEY_NUMPADSTAR,   "* (Num)" },        // kVK_ANSI_KeypadMultiply, 0x43
    { 0, 0 },                               // ???, 0x44
    { KEY_NUMPADPLUS,   "+ (Num)" },        // kVK_ANSI_KeypadPlus, 0x45
    { 0, 0 },                               // ???, 0x46
    { 0, 0 },                               // kVK_ANSI_KeypadClear, 0x47
    { KEY_VOLUMEUP,     "Volume up" },      // kVK_VolumeUp, 0x48
    { KEY_VOLUMEDOWN,   "Volume down" },    // kVK_VolumeDown, 0x49
    { KEY_MUTE,         "Mute" },           // kVK_Mute, 0x4A
    { KEY_NUMPADSLASH,  "/ (Num)" },        // kVK_ANSI_KeypadDivide, 0x4B
    { KEY_NUMPADENTER,  "Enter (Num)" },    // kVK_ANSI_KeypadEnter, 0x4C
    { 0, 0 },                               // ???, 0x4D
    { KEY_NUMPADMINUS,  "- (Num)" },        // kVK_ANSI_KeypadMinus, 0x4E
    { KEY_F18,          "F18" },            // kVK_F18, 0x4F
    { KEY_F19,          "F19" },            // kVK_F19, 0x50
    { KEY_EQUALS,       "=" },              // kVK_ANSI_KeypadEquals, 0x51
    { KEY_NUMPAD0,      "0 (Num)" },        // kVK_ANSI_Keypad0, 0x52
    { KEY_NUMPAD1,      "1 (Num)" },        // kVK_ANSI_Keypad1, 0x53
    { KEY_NUMPAD2,      "2 (Num)" },        // kVK_ANSI_Keypad2, 0x54
    { KEY_NUMPAD3,      "3 (Num)" },        // kVK_ANSI_Keypad3, 0x55
    { KEY_NUMPAD4,      "4 (Num)" },        // kVK_ANSI_Keypad4, 0x56
    { KEY_NUMPAD5,      "5 (Num)" },        // kVK_ANSI_Keypad5, 0x57
    { KEY_NUMPAD6,      "6 (Num)" },        // kVK_ANSI_Keypad6, 0x58
    { KEY_NUMPAD7,      "7 (Num)" },        // kVK_ANSI_Keypad7, 0x59
    { KEY_F20,          "F20" },            // kVK_F20, 0x5A
    { KEY_NUMPAD8,      "8 (Num)" },        // kVK_ANSI_Keypad8, 0x5B
    { KEY_NUMPAD9,      "9 (Num)" },        // kVK_ANSI_Keypad9, 0x5C
    { 0, 0 },                               // ???, 0x5D
    { 0, 0 },                               // ???, 0x5E
    { 0, 0 },                               // ???, 0x5F
    { KEY_F5,           "F5" },             // kVK_F5, 0x60
    { KEY_F6,           "F6" },             // kVK_F6, 0x61
    { KEY_F7,           "F7" },             // kVK_F7, 0x62
    { KEY_F3,           "F3" },             // kVK_F3, 0x63
    { KEY_F8,           "F8" },             // kVK_F8, 0x64
    { KEY_F9,           "F9" },             // kVK_F9, 0x65
    { 0, 0 },                               // ???, 0x66
    { KEY_F11,          "F11" },            // kVK_F11, 0x67
    { 0, 0 },                               // ???, 0x68
    { KEY_F13,          "F13" },            // kVK_F13, 0x69
    { KEY_F16,          "F16" },            // kVK_F16, 0x6A
    { KEY_F14,          "F14" },            // kVK_F14, 0x6B
    { 0, 0 },                               // ???, 0x6C
    { KEY_F10,          "F10" },            // kVK_F10, 0x6D
    { 0, 0 },                               // ???, 0x6E
    { KEY_F12,          "F12" },            // kVK_F12, 0x6F
    { 0, 0 },                               // ???, 0x70
    { KEY_F15,          "F15" },            // kVK_F15, 0x71
    { 0, 0 },                               // kVK_Help, 0x72
    { KEY_HOME,         "Home" },           // kVK_Home, 0x73
    { KEY_PGUP,         "PageUp" },         // kVK_PageUp, 0x74
    { KEY_DELETE,       "Delete" },         // kVK_ForwardDelete, 0x75
    { KEY_F4,           "F4" },             // kVK_F4, 0x76
    { KEY_END,          "End" },            // kVK_End, 0x77
    { KEY_F2,           "F2" },             // kVK_F2, 0x78
    { KEY_PGDOWN,       "PageDown" },       // kVK_PageDown, 0x79
    { KEY_F1,           "F1" },             // kVK_F1, 0x7A
    { KEY_LEFT,         "Left" },           // kVK_LeftArrow, 0x7B
    { KEY_RIGHT,        "Right" },          // kVK_RightArrow, 0x7C
    { KEY_DOWN,         "Down" },           // kVK_DownArrow, 0x7D
    { KEY_UP,           "Up" },             // kVK_UpArrow, 0x7E
};


/***************************** CONSTRUCTION / DESTRUCTION ******************************/

MacKeyboard::MacKeyboard(io_object_t hidDevice)
: MacController(hidDevice, CONTROLLER_KEYBOARD, 1), m_previousModMask(0), m_keyDownUPP(0),
  m_keyDownRef(0), m_keyUpUPP(0), m_keyUpRef(0), m_keyModUPP(0), m_keyModRef(0)
{
    // Get a "Universal procedure pointer" for our callbacks
    m_keyDownUPP    = NewEventHandlerUPP(KeyDownWrapper);
    m_keyUpUPP      = NewEventHandlerUPP(KeyUpWrapper);
    m_keyModUPP     = NewEventHandlerUPP(KeyModWrapper);
}

//-----------------------------------------------------------------------

MacKeyboard::~MacKeyboard()
{
    ATHENA_LOG_EVENT("Destruction");

    if (m_keyDownRef)
        RemoveEventHandler(m_keyDownRef);

    if (m_keyUpRef)
        RemoveEventHandler(m_keyUpRef);

    if (m_keyModRef)
        RemoveEventHandler(m_keyModRef);

    DisposeEventHandlerUPP(m_keyDownUPP);
    DisposeEventHandlerUPP(m_keyUpUPP);
    DisposeEventHandlerUPP(m_keyModUPP);
}


/****************************** MANAGEMENT OF THE MOUSE ********************************/

bool MacKeyboard::readInputs()
{
    // Declarations
    vector<tKeyboardEvent>::iterator    eventsIter, eventsIterEnd;
    vector<IEventsListener*>::iterator  listenersIter, listenersIterEnd;

    // Check that the keyboard is active
    if (!m_bActive)
        return false;

    // Process each event
    for (eventsIter = m_events.begin(), eventsIterEnd = m_events.end();
         eventsIter != eventsIterEnd; ++eventsIter)
    {
        tKeyboardEvent event = *eventsIter;

        if (event.bCharacter)
        {
            if (!m_bExclusiveMode)
            {
                for (listenersIter = m_listeners.begin(), listenersIterEnd = m_listeners.end();
                     listenersIter != listenersIterEnd; ++listenersIter)
                {
                    (*listenersIter)->onCharacter(event.strUTF8Character);
                }
            }
        }
        else
        {
            for (listenersIter = m_listeners.begin(), listenersIterEnd = m_listeners.end();
                 listenersIter != listenersIterEnd; ++listenersIter)
            {
                (*listenersIter)->onEvent(&event.event);
            }
        }
    }

    m_events.clear();

    return true;
}

//-----------------------------------------------------------------------

const string MacKeyboard::getNameOfKey(tKey key)
{
    if (KEY_INFOS[key].strName)
        return KEY_INFOS[key].strName;

    return "";
}

//-----------------------------------------------------------------------

bool MacKeyboard::acquire(bool bAcquire)
{
    // Start listening for events
    EventTargetRef event = GetApplicationEventTarget();

    if (m_keyDownRef)
    {
        RemoveEventHandler(m_keyDownRef);
        m_keyDownRef = 0;
    }

    if (m_keyUpRef)
    {
        RemoveEventHandler(m_keyUpRef);
        m_keyUpRef = 0;
    }

    if (m_keyModRef)
    {
        RemoveEventHandler(m_keyModRef);
        m_keyModRef = 0;
    }


    if (bAcquire)
    {
        if (InstallEventHandler(event, m_keyDownUPP, (!m_bExclusiveMode ? 2 : 1), DownEvents, this, &m_keyDownRef) != noErr)
        {
            ATHENA_LOG_ERROR("Failed to install the key down event handler");
            return false;
        }

        if (InstallEventHandler(event, m_keyUpUPP, 1, &UpEvents, this, &m_keyUpRef) != noErr)
        {
            ATHENA_LOG_ERROR("Failed to install the key up event handler");
            return false;
        }

        if (InstallEventHandler(event, m_keyModUPP, 1, &ModEvents, this, &m_keyModRef) != noErr)
        {
            ATHENA_LOG_ERROR("Failed to install the key mod event handler");
            return false;
        }
    }

    return true;
}


/***************************************** EVENT HANDLERS *****************************************/

OSStatus MacKeyboard::KeyDownWrapper(EventHandlerCallRef nextHandler, EventRef theEvent, void* macKeyboard)
{
    MacKeyboard* pKeyboard = static_cast<MacKeyboard*>(macKeyboard);
    if (pKeyboard)
    {
        UInt32 virtualKey;
        GetEventParameter(theEvent, kEventParamKeyCode, typeUInt32, NULL, sizeof(UInt32), NULL, &virtualKey);

        tKey key = KEY_INFOS[virtualKey].key;

        tKeyboardEvent event;

        event.bCharacter            = false;
        event.event.part            = PART_KEY;
        event.event.partID.key      = key;
        event.event.value.bPressed  = true;
        event.event.pController     = pKeyboard;
        event.event.dwTimeStamp     = (unsigned int) GetEventTime(theEvent);
        event.event.dwSequence      = pKeyboard->m_events.size();

        pKeyboard->m_events.push_back(event);


        if (!pKeyboard->m_bExclusiveMode &&
            (key != KEY_DELETE) && (key != KEY_ESCAPE) && (key != KEY_LWIN) && (key != KEY_LSHIFT) &&
            (key != KEY_CAPITAL) && (key != KEY_LALT) && (key != KEY_LCONTROL) && (key != KEY_RSHIFT) &&
            (key != KEY_RALT) && (key != KEY_RCONTROL) && (key != KEY_APPS) && (key != KEY_F17) &&
            (key != KEY_VOLUMEUP) && (key != KEY_VOLUMEDOWN) && (key != KEY_MUTE) && (key != KEY_F18) &&
            (key != KEY_F19) && (key != KEY_F20) && (key != KEY_F5) && (key != KEY_F6) &&
            (key != KEY_F7) && (key != KEY_F3) && (key != KEY_F8) && (key != KEY_F9) &&
            (key != KEY_F11) && (key != KEY_F13) && (key != KEY_F16) && (key != KEY_F14) &&
            (key != KEY_F10) && (key != KEY_F12) && (key != KEY_F15) && (key != KEY_HOME) &&
            (key != KEY_PGUP) && (key != KEY_BACKSPACE) && (key != KEY_F4) && (key != KEY_END) &&
            (key != KEY_F2) && (key != KEY_PGDOWN) && (key != KEY_F1) && (key != KEY_LEFT) &&
            (key != KEY_RIGHT) && (key != KEY_DOWN) && (key != KEY_UP))
        {
            UniChar text[10];
            UInt32 size;

            GetEventParameter(theEvent, kEventParamKeyUnicodes, typeUnicodeText, NULL,
                              sizeof(text), (ByteCount*) &size, &text);

            if (size > 1)
            {
                // Send an event for each unicode character
                for (int i = 0; i < size - 1; ++i)
                {
                    tKeyboardEvent event;

                    event.bCharacter        = true;
                    event.strUTF8Character  = (unsigned int) text[i];

                    pKeyboard->m_events.push_back(event);
                }
            }
        }
    }

    // Propagate the event down the chain
    return CallNextEventHandler(nextHandler, theEvent);
}

//-----------------------------------------------------------------------

OSStatus MacKeyboard::KeyUpWrapper(EventHandlerCallRef nextHandler, EventRef theEvent, void* macKeyboard)
{
    MacKeyboard* pKeyboard = static_cast<MacKeyboard*>(macKeyboard);
    if (pKeyboard)
    {
        UInt32 virtualKey;
        GetEventParameter(theEvent, kEventParamKeyCode, typeUInt32, NULL, sizeof(UInt32), NULL, &virtualKey);

        tKey key = KEY_INFOS[virtualKey].key;

        tKeyboardEvent event;

        event.bCharacter            = false;
        event.event.part            = PART_KEY;
        event.event.partID.key      = key;
        event.event.value.bPressed  = false;
        event.event.pController     = pKeyboard;
        event.event.dwTimeStamp     = (unsigned int) GetEventTime(theEvent);
        event.event.dwSequence      = pKeyboard->m_events.size();

        pKeyboard->m_events.push_back(event);
    }

    // Propagate the event down the chain
    return CallNextEventHandler(nextHandler, theEvent);
}

//-----------------------------------------------------------------------

OSStatus MacKeyboard::KeyModWrapper(EventHandlerCallRef nextHandler, EventRef theEvent, void* macKeyboard)
{
    MacKeyboard* pKeyboard = static_cast<MacKeyboard*>(macKeyboard);
    if (pKeyboard)
    {
        UInt32 mods;
        GetEventParameter(theEvent, kEventParamKeyModifiers, typeUInt32, NULL, sizeof(UInt32), NULL, &mods);

        // Find the changed bit
        UInt32 change = pKeyboard->m_previousModMask ^ mods;

        tKeyboardEvent event;

        event.bCharacter            = false;
        event.event.part            = PART_KEY;
        event.event.partID.key      = 0;
        event.event.value.bPressed  = ((change & pKeyboard->m_previousModMask) == 0);
        event.event.pController     = pKeyboard;
        event.event.dwTimeStamp     = (unsigned int) GetEventTime(theEvent);
        event.event.dwSequence      = pKeyboard->m_events.size();

        switch (change)
        {
            // Shift
            case (shiftKey):
                event.event.partID.key = KEY_LSHIFT;
                break;

            // Option (alt)
            case (optionKey):
                event.event.partID.key = KEY_LALT;
                break;

            // Ctrl
            case (controlKey):
                event.event.partID.key = KEY_LCONTROL;
                break;

            // Apple
            case (cmdKey):
                event.event.partID.key = KEY_LWIN;
                break;

            // fn key
            case (kEventKeyModifierFnMask):
                event.event.partID.key = KEY_APPS;
                break;

            // Numlock
            case (kEventKeyModifierNumLockMask):
                event.event.partID.key = KEY_NUMLOCK;
                break;

            // Caps lock
            case (alphaLock):
                event.event.partID.key = KEY_CAPITAL;
                break;
        }

        if (event.event.partID.key != 0)
            pKeyboard->m_events.push_back(event);

        pKeyboard->m_previousModMask = mods;
    }

    // Propagate the event down the chain
    return CallNextEventHandler(nextHandler, theEvent);
}
