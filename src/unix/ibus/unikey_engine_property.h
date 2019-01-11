#pragma once

#include <cstddef>

#include "unix/ibus/input_method.h"
#include "unix/ibus/output_charset.h"

// The list of properties used in ibus-unikey.
struct InputMethodProperty {
    InputMethod input_method;
    const char *key;  // IBus property key for the mode.
    const char *key_for_gsettings;
    const char *label;  // text for the radio menu.
    const char *label_for_panel;  // text for the language panel.
    const char *icon;
};

// This pointer should be NULL when properties size is 0.
extern const InputMethodProperty *kInputMethodProperties;
extern const size_t kInputMethodPropertiesSize;
extern const InputMethod kInitialInputMethod;

// The list of charset used in ibus-unikey.
struct OutputCharsetProperty {
    OutputCharset output_charset;
    const char *key;  // IBus property key.
    const char *key_for_gsettings;
    const char *label;  // text for the menu.
    const char *label_for_panel;  // text for the language panel.
};


extern const OutputCharsetProperty *kOutputCharsetProperties;
extern const size_t kOutputCharsetPropertiesSize;
extern const OutputCharset kInitialOutputCharset;

struct OptionProperty {
    const char *key;    // IBus property key.
    const char *label;  // text for the menu.
    const bool default_enabled;
};

extern const OptionProperty *kOptionProperties;
extern const size_t kOptionPropertiesSize;

struct ToolProperty {
    const char *key;    // IBus property key for the UnikeyTool.
    const char *mode;   // command line passed as --mode=
    const char *label;  // text for the menu.
    const char *icon;   // icon
};

extern const ToolProperty *kToolProperties;
extern const size_t kToolPropertiesSize;



