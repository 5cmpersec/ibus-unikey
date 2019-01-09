#include "unix/ibus/property_handler.h"

#include <string>

#include "base/logging.h"

#include "unix/ibus/unikey_engine_property.h"


namespace {

// A key which associates an IBusProperty object with InputMethodProperty.
const char kGObjectDataKey[] = "ibus-unikey-aux-data";

bool GetDisabled(IBusEngine *engine) {
    bool disabled = false;
#if defined(UNIKEY_ENABLE_IBUS_INPUT_PURPOSE)
    guint purpose = IBUS_INPUT_PURPOSE_FREE_FORM;
    guint hints = IBUS_INPUT_HINT_NONE;
    ibus_engine_get_content_type(engine, &purpose, &hints);
    disabled = (purpose == IBUS_INPUT_PURPOSE_PASSWORD ||
                            purpose == IBUS_INPUT_PURPOSE_PIN);
#endif  // UNIKEY_ENABLE_IBUS_INPUT_PURPOSE
    return disabled;
}

// Some users expect that IME is turned off by default on IBus 1.5.0 and later.
// On IBus 1.4.x, IBus expects that an IME should always be turned on and
// IME on/off keys are handled by IBus itself rather than each IME.
#if IBUS_CHECK_VERSION(1, 5, 0)
const bool kActivatedOnLaunch = false;
#else
const bool kActivatedOnLaunch = true;
#endif  // IBus>=1.5.0

}  // namespace

PropertyHandler::PropertyHandler()
        : prop_root_(ibus_prop_list_new()),
            prop_input_method_(nullptr),
            prop_output_charset_(nullptr),
            prop_unikey_tool_(nullptr),
            original_input_method_(kInitialInputMethod),
            original_output_charset_(kInitialOutputCharset),
            is_activated_(kActivatedOnLaunch),
            is_disabled_(false) {
    BLOG_DEBUG("PropertyHandler constructor start");
    if (is_activated_) {

    } else {

    }

    AppendInputMethodPropertyToPanel();
    AppendOutputCharsetPropertyToPanel();
    AppendToolPropertyToPanel();

    // We have to sink |prop_root_| as well so ibus_engine_register_properties()
    // in FocusIn() does not destruct it.
    g_object_ref_sink(prop_root_);
    BLOG_DEBUG("PropertyHandler constructor end");
}

PropertyHandler::~PropertyHandler() {
    if (prop_input_method_) {
        // The ref counter will drop to one.
        g_object_unref(prop_input_method_);
        prop_input_method_ = nullptr;
    }

    if (prop_output_charset_) {
        // The ref counter will drop to one.
        g_object_unref(prop_output_charset_);
        prop_output_charset_ = nullptr;
    }

    if (prop_unikey_tool_) {
        // The ref counter will drop to one.
        g_object_unref(prop_unikey_tool_);
        prop_unikey_tool_ = nullptr;
    }

    if (prop_root_) {
        // Destroy all objects under the root.
        g_object_unref(prop_root_);
        prop_root_ = nullptr;
    }
}


void PropertyHandler::Register(IBusEngine *engine) {
    BLOG_DEBUG("Register start");
    ibus_engine_register_properties(engine, prop_root_);
    UpdateContentType(engine);
    BLOG_DEBUG("Register end");
}


void PropertyHandler::ResetContentType(IBusEngine *engine) {
    UpdateContentTypeImpl(engine, false);
}


void PropertyHandler::UpdateContentType(IBusEngine *engine) {
    UpdateContentTypeImpl(engine, GetDisabled(engine));
}


void PropertyHandler::UpdateContentTypeImpl(IBusEngine *engine,
                                            bool disabled) {
    BLOG_DEBUG("UpdateContentTypeImpl start: disabled = {}, prev={}", disabled, is_disabled_);
    const bool prev_is_disabled = is_disabled_;
    is_disabled_ = disabled;

    if (prev_is_disabled == is_disabled_) {
        return;
    }

    const auto visible_method = original_input_method_;

    UpdateInputMethodIcon(engine, visible_method);
    BLOG_DEBUG("UpdateContentTypeImpl end");
}



void PropertyHandler::AppendInputMethodPropertyToPanel() {
    BLOG_DEBUG("AppendInputMethodPropertyToPanel start");
    if (kInputMethodProperties == nullptr || kInputMethodPropertiesSize == 0) {
        return;
    }

    IBusPropList *sub_prop_list = ibus_prop_list_new();
    const InputMethod initial_method = original_input_method_;

    std::string icon_path_for_panel;
    const char *method_symbol = nullptr;

    for (size_t i = 0; i < kInputMethodPropertiesSize; ++i) {
        const InputMethodProperty &entry = kInputMethodProperties[i];
        IBusText *label = ibus_text_new_from_string(entry.label);
        IBusPropState state = PROP_STATE_UNCHECKED;

        if (entry.input_method == initial_method) {
            state = PROP_STATE_CHECKED;
            method_symbol = entry.label_for_panel;
        }

        IBusProperty *item = ibus_property_new(entry.key,
                                             PROP_TYPE_RADIO,
                                             label,
                                             nullptr /* icon */,
                                             nullptr /* tooltip */,
                                             TRUE /* sensitive */,
                                             TRUE /* visible */,
                                             state,
                                             nullptr /* sub props */);
        g_object_set_data(G_OBJECT(item), kGObjectDataKey, (gpointer)&entry);
        ibus_prop_list_append(sub_prop_list, item);
        // |sub_prop_list| owns |item| by calling g_object_ref_sink for the |item|.
    }

    const std::string &method_label = "Input Method (" + std::string(method_symbol) + ")";
    IBusText *label = ibus_text_new_from_string(method_label.c_str());

    prop_input_method_ = ibus_property_new("InputMethod",
                                             PROP_TYPE_MENU,
                                             label,
                                             nullptr,
                                             nullptr /* tooltip */,
                                             TRUE /* sensitive */,
                                             TRUE /* visible */,
                                             PROP_STATE_UNCHECKED,
                                             sub_prop_list);
#ifdef UNIKEY_IBUS_HAS_SYMBOL
    IBusText *symbol = ibus_text_new_from_static_string(method_symbol);
    ibus_property_set_symbol(prop_input_method_, symbol);
#endif  // UNIKEY_IBUS_HAS_SYMBOL

    g_object_ref_sink(prop_input_method_);

    ibus_prop_list_append(prop_root_, prop_input_method_);
    BLOG_DEBUG("AppendInputMethodPropertyToPanel end");
}

void PropertyHandler::AppendOutputCharsetPropertyToPanel() {
    BLOG_DEBUG("AppendOutputCharsetPropertyToPanel");
    if (kOutputCharsetProperties == nullptr || kOutputCharsetPropertiesSize == 0) {
        return;
    }

    IBusPropList *sub_prop_list = ibus_prop_list_new();
    const OutputCharset initial_charset = original_output_charset_;

    const char *charset_symbol = nullptr;

    for (size_t i = 0; i < kOutputCharsetPropertiesSize; ++i) {
        const OutputCharsetProperty &entry = kOutputCharsetProperties[i];
        IBusText *label = ibus_text_new_from_string(entry.label);
        IBusPropState state = PROP_STATE_UNCHECKED;

        if (entry.output_charset == initial_charset) {
            state = PROP_STATE_CHECKED;
            charset_symbol = entry.label;
        }

        IBusProperty *item = ibus_property_new(entry.key,
                                             PROP_TYPE_RADIO,
                                             label,
                                             nullptr /* icon */,
                                             nullptr /* tooltip */,
                                             TRUE /* sensitive */,
                                             TRUE /* visible */,
                                             state,
                                             nullptr /* sub props */);
        g_object_set_data(G_OBJECT(item), kGObjectDataKey, (gpointer)&entry);
        ibus_prop_list_append(sub_prop_list, item);
        // |sub_prop_list| owns |item| by calling g_object_ref_sink for the |item|.
    }

    const std::string &charset_label = "Output Charset (" + std::string(charset_symbol) + ")";
    IBusText *label = ibus_text_new_from_string(charset_label.c_str());

    prop_output_charset_ = ibus_property_new("OutputCharset",
                                             PROP_TYPE_MENU,
                                             label,
                                             nullptr,
                                             nullptr /* tooltip */,
                                             TRUE /* sensitive */,
                                             TRUE /* visible */,
                                             PROP_STATE_UNCHECKED,
                                             sub_prop_list);
#ifdef UNIKEY_IBUS_HAS_SYMBOL
    IBusText *symbol = ibus_text_new_from_static_string(charset_symbol);
    ibus_property_set_symbol(prop_output_charset_, symbol);
#endif  // UNIKEY_IBUS_HAS_SYMBOL

    g_object_ref_sink(prop_output_charset_);

    ibus_prop_list_append(prop_root_, prop_output_charset_);
}


void PropertyHandler::AppendToolPropertyToPanel() {
    BLOG_DEBUG("AppendToolPropertyToPanel");
    if (kToolProperties == nullptr || kToolPropertiesSize == 0) {
        return;
    }


    IBusPropList *sub_prop_list = ibus_prop_list_new();

    for (size_t i = 0; i < kToolPropertiesSize; ++i) {
        const ToolProperty &entry = kToolProperties[i];
        IBusText *label = ibus_text_new_from_string(entry.label);
        IBusProperty *item = ibus_property_new(entry.mode,
                                               PROP_TYPE_NORMAL,
                                               label,
                                               nullptr /* icon */,
                                               nullptr /* tooltip */,
                                               TRUE,
                                               TRUE,
                                               PROP_STATE_UNCHECKED,
                                               nullptr);
        g_object_set_data(G_OBJECT(item), kGObjectDataKey, (gpointer)&entry);
        ibus_prop_list_append(sub_prop_list, item);
    }

    IBusText *tool_label = ibus_text_new_from_string("Tools");
    prop_unikey_tool_ = ibus_property_new("Tool",
                                          PROP_TYPE_MENU,
                                          tool_label,
                                          nullptr,
                                          nullptr /* tooltip */,
                                          TRUE /* sensitive */,
                                          TRUE /* visible */,
                                          PROP_STATE_UNCHECKED,
                                          sub_prop_list);

      g_object_ref_sink(prop_unikey_tool_);

      ibus_prop_list_append(prop_root_, prop_unikey_tool_);
}

void PropertyHandler::Update(IBusEngine *engine) {
    BLOG_DEBUG("Update");

    if (IsDisabled()) {
        return;
    }

}

void PropertyHandler::ProcessPropertyActivate(IBusEngine *engine,
                                            const gchar *property_name,
                                            guint property_state) {
    BLOG_DEBUG("ProcessPropertyActivate: name={}, state={}", property_name, property_state);

    if (IsDisabled()) {
        return;
    }

    if (property_state != PROP_STATE_CHECKED) {
        return;
    }

    if (prop_input_method_) {
        for (guint prop_index = 0; ; ++prop_index) {
            IBusProperty *prop = ibus_prop_list_get(
                    ibus_property_get_sub_props(prop_input_method_), prop_index);
            if (prop == nullptr) {
                break;
            }
            if (!g_strcmp0(property_name, ibus_property_get_key(prop))) {
                const InputMethodProperty *entry =
                        reinterpret_cast<const InputMethodProperty*>(
                                g_object_get_data(G_OBJECT(prop), kGObjectDataKey));
                SetInputMethod(engine, entry->input_method);
                UpdateInputMethodIcon(engine, entry->input_method);
                break;
            }
        }
    }

    if (prop_output_charset_) {
        for (guint prop_index = 0; ; ++prop_index) {
            IBusProperty *prop = ibus_prop_list_get(
                    ibus_property_get_sub_props(prop_output_charset_), prop_index);
            if (prop == nullptr) {
                break;
            }
            if (!g_strcmp0(property_name, ibus_property_get_key(prop))) {
                const OutputCharsetProperty *entry =
                        reinterpret_cast<const OutputCharsetProperty*>(
                                g_object_get_data(G_OBJECT(prop), kGObjectDataKey));
                SetOutputCharset(engine, entry->output_charset);
                UpdateOutputCharsetIcon(engine, entry->output_charset);
                break;
            }
        }
    }
}

bool PropertyHandler::IsActivated() const {
    return is_activated_;
}

bool PropertyHandler::IsDisabled() const {
    return is_disabled_;
}

void PropertyHandler::UpdateInputMethodIcon(IBusEngine* engine,
                                            const InputMethod new_input_method) {
    BLOG_DEBUG("UpdateInputMethodIcon start");

    if (prop_input_method_ == nullptr) {
        return;
    }

    const InputMethodProperty *entry = nullptr;
    for (size_t i = 0; i < kInputMethodPropertiesSize; ++i) {
        if (kInputMethodProperties[i].input_method == new_input_method) {
          entry = &(kInputMethodProperties[i]);
          break;
        }
    }

    for (guint prop_index = 0; ; ++prop_index) {
        IBusProperty *prop = ibus_prop_list_get(
            ibus_property_get_sub_props(prop_input_method_), prop_index);
        if (prop == nullptr) {
            break;
        }
        if (!g_strcmp0(entry->key, ibus_property_get_key(prop))) {
            // Update the language panel.
            ibus_property_set_icon(prop_input_method_, entry->icon);
            // Update the radio menu item.
            ibus_property_set_state(prop, PROP_STATE_CHECKED);
        } else {
            ibus_property_set_state(prop, PROP_STATE_UNCHECKED);
        }
        // No need to call unref since ibus_prop_list_get does not add ref.
    }

    const char *mode_symbol = entry->label_for_panel;
// Update the text icon for Gnome shell.
#ifdef UNIKEY_IBUS_HAS_SYMBOL
    IBusText *symbol = ibus_text_new_from_static_string(mode_symbol);
    ibus_property_set_symbol(prop_input_method_, symbol);
#endif  // UNIKEY_IBUS_HAS_SYMBOL

    const std::string &mode_label = "Input Method (" + std::string(mode_symbol) + ")";
    IBusText *label = ibus_text_new_from_string(mode_label.c_str());
    ibus_property_set_label(prop_input_method_, label);

    ibus_engine_update_property(engine, prop_input_method_);
    BLOG_DEBUG("UpdateInputMethodIcon end");
}

void PropertyHandler::SetInputMethod(IBusEngine *engine, 
                                     const InputMethod new_input_method) {
    BLOG_DEBUG("SetInputMethod");
    original_input_method_ = new_input_method;
}

void PropertyHandler::UpdateOutputCharsetIcon(IBusEngine* engine,
                                        const OutputCharset new_charset) {
    BLOG_DEBUG("UpdateOutputCharsetIcon start");

    if (prop_output_charset_ == nullptr) {
        return;
    }

    const OutputCharsetProperty *entry = nullptr;
    for (size_t i = 0; i < kOutputCharsetPropertiesSize; ++i) {
        if (kOutputCharsetProperties[i].output_charset == new_charset) {
          entry = &(kOutputCharsetProperties[i]);
          break;
        }
    }

    for (guint prop_index = 0; ; ++prop_index) {
        IBusProperty *prop = ibus_prop_list_get(
            ibus_property_get_sub_props(prop_output_charset_), prop_index);
        if (prop == nullptr) {
            break;
        }
        if (!g_strcmp0(entry->key, ibus_property_get_key(prop))) {
            // Update the radio menu item.
            ibus_property_set_state(prop, PROP_STATE_CHECKED);
        } else {
            ibus_property_set_state(prop, PROP_STATE_UNCHECKED);
        }
        // No need to call unref since ibus_prop_list_get does not add ref.
    }

    const char *charset_symbol = entry->label;
// Update the text icon for Gnome shell.
#ifdef UNIKEY_IBUS_HAS_SYMBOL
    IBusText *symbol = ibus_text_new_from_static_string(charset_symbol);
    ibus_property_set_symbol(prop_output_charset_, symbol);
#endif  // UNIKEY_IBUS_HAS_SYMBOL

    const std::string &charset_label = "Output Charset (" + std::string(charset_symbol) + ")";
    IBusText *label = ibus_text_new_from_string(charset_label.c_str());
    ibus_property_set_label(prop_output_charset_, label);

    ibus_engine_update_property(engine, prop_output_charset_);
    BLOG_DEBUG("UpdateOutputCharsetIcon end");
}

void PropertyHandler::SetOutputCharset(IBusEngine *engine, 
                                       const OutputCharset new_charset) {
    BLOG_DEBUG("SetOutputCharset");
    original_output_charset_ = new_charset;

}