#include "unix/ibus/unikey_engine_property.h"

#include "base/port.h"


namespace {
// The list of properties used in ibus-unikey.
const InputMethodProperty kInputMethodPropertiesArray[] = {
    {
        InputMethod::TELEX,
        "InputMethod.Telex",
        "telex",
        "Telex",
        "Telex",
        "telex.png",
    },
    {
        InputMethod::VNI,
        "InputMethod.Vni",
        "vni",
        "VNI",
        "VNI",
        "vni.png",
    },
    {
        InputMethod::STELEX,
        "InputMethod.STelex",
        "stelex",
        "Simple Telex",
        "STelex",
        "stelex.png",
    },
    {
        InputMethod::STELEX2,
        "InputMethod.STelex2",
        "stelex2",
        "Simple Telex 2",
        "STelex2",
        "stelex2.png",
    },
};

const OutputCharsetProperty kOutputCharsetPropertiesArray[] = {
    {
        OutputCharset::UNICODE,
        "OutputCharset.Unicode",
        "unicode",
        "Unicode",
        "Unicode",
    },
    {
        OutputCharset::TCVN3,
        "OutputCharset.TCVN3",
        "tcvn3",
        "TCVN3",
        "TCVN3",
    },
    {
        OutputCharset::VNI_WIN,
        "OutputCharset.VNI_WIN",
        "vni-win",
        "VNI Win",
        "VNI Win",
    },
    {
        OutputCharset::VIQR,
        "OutputCharset.VIQR",
        "viqr",
        "VIQR",
        "VIQR",
    },
    {
        OutputCharset::BK_HCM2,
        "OutputCharset.BK_HCM2",
        "bk-hcm2",
        "BK HCM 2",
        "BK HCM 2",
    },
    {
        OutputCharset::CSTRING,
        "OutputCharset.CSTRING",
        "cstr",
        "CString",
        "CString",
    },
    {
        OutputCharset::NCR_DECIMAL,
        "OutputCharset.NCR_DECIMAL",
        "ncr-dec",
        "NCR Decimal",
        "NCR Decimal",
    },
    {
        OutputCharset::NCR_HEX,
        "OutputCharset.NCR_HEX",
        "ncr-hex",
        "NCR Hex",
        "NCR Hex",
    },
};

const OptionProperty kOptionPropertiesArray[] = {
    {
        "Option.SpellCheck",
        "spell-check",
        "Spell Check",
    },
    {
        "Option.FreeMarking",
        "free-marking",
        "Free Marking",
    },
    {
        "Option.ModernStyle",
        "modern-style",
        "Modern Style",
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

const size_t kOptionPropertiesSize = arraysize(kOptionPropertiesArray);

const OptionProperty *kOptionProperties = &kOptionPropertiesArray[0];