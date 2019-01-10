#pragma once

#include <cstddef>

#include "unix/ibus/input_method.h"
#include "unix/ibus/output_charset.h"

// The list of properties used in ibus-unikey.
struct InputMethodProperty {
    InputMethod input_method;
    const char *key;  // IBus property key for the mode.
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
    const char *label;  // text for the menu.
    const char *label_for_panel;  // text for the language panel.
};

// This pointer should be NULL when properties size is 0.
extern const OutputCharsetProperty *kOutputCharsetProperties;
extern const size_t kOutputCharsetPropertiesSize;
extern const OutputCharset kInitialOutputCharset;

// The list of charset used in ibus-unikey.
struct OptionProperty {
    const char *key;    // IBus property key.
    const char *label;  // text for the menu.
};

struct ToolProperty {
    const char *key;    // IBus property key for the UnikeyTool.
    const char *mode;   // command line passed as --mode=
    const char *label;  // text for the menu.
    const char *icon;   // icon
};

// This pointer should be NULL when properties size is 0.
extern const ToolProperty *kToolProperties;
extern const size_t kToolPropertiesSize;



