#include "unix/ibus/unikey_engine_property.h"

#include "base/port.h"


namespace {
// The list of properties used in ibus-unikey.
const InputMethodProperty kInputMethodPropertiesArray[] = {
    {
        InputMethod::TELEX,
        "InputMethod.Telex",
        "Telex",
        "Telex",
        "telex.png",
    },
    {
        InputMethod::VNI,
        "InputMethod.Vni",
        "VNI",
        "VNI",
        "vni.png",
    },
    {
        InputMethod::STELEX,
        "InputMethod.STelex",
        "STelex",
        "STelex",
        "stelex.png",
    },
    {
        InputMethod::STELEX2,
        "InputMethod.STelex2",
        "STelex2",
        "STelex2",
        "stelex2.png",
    },
};

const OutputCharsetProperty kOutputCharsetPropertiesArray[] = {
    {
        OutputCharset::UNICODE,
        "OutputCharset.Unicode",
        "Unicode",
    },
    {
        OutputCharset::TCVN3,
        "OutputCharset.TCVN3",
        "TCVN3",
    },
    {
        OutputCharset::VNI_WIN,
        "OutputCharset.VNI_WIN",
        "VNI_WIN",
    },
    {
        OutputCharset::VIQR,
        "OutputCharset.VIQR",
        "VIQR",
    },
    {
        OutputCharset::BK_HCM2,
        "OutputCharset.BK_HCM2",
        "BK_HCM2",
    },
    {
        OutputCharset::CSTRING,
        "OutputCharset.CSTRING",
        "CSTRING",
    },
    {
        OutputCharset::NCR_DECIMAL,
        "OutputCharset.NCR_DECIMAL",
        "NCR_DECIMAL",
    },
    {
        OutputCharset::NCR_HEX,
        "OutputCharset.NCR_HEX",
        "NCR_HEX",
    },
};

const ToolProperty kToolPropertiesArray[] = {
    {
        "Tool.AboutDialog",
        "about_dialog",
        "About Unikey",
        nullptr,
    },
};

}  // namespace

// The list of properties used in ibus-unikey.
const InputMethodProperty *kInputMethodProperties = &kInputMethodPropertiesArray[0];

const size_t kInputMethodPropertiesSize = arraysize(kInputMethodPropertiesArray);

const InputMethod kInitialInputMethod = InputMethod::TELEX;

const OutputCharsetProperty *kOutputCharsetProperties = &kOutputCharsetPropertiesArray[0];

const size_t kOutputCharsetPropertiesSize = arraysize(kOutputCharsetPropertiesArray);

const OutputCharset kInitialOutputCharset = OutputCharset::UNICODE;

const ToolProperty *kToolProperties = &kToolPropertiesArray[0];

const size_t kToolPropertiesSize = arraysize(kToolPropertiesArray);

