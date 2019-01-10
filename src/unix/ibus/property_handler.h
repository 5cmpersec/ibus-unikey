#pragma once

#include <memory>
#include <vector>

#include "base/port.h"
#include "unix/ibus/ibus_header.h"
#include "unix/ibus/input_method.h"
#include "unix/ibus/output_charset.h"
#include "unix/ibus/property_handler_interface.h"


class PropertyHandler : public IPropertyHandler {
public:
    PropertyHandler();
    virtual ~PropertyHandler();

    virtual void Register(IBusEngine *engine);
    virtual void ResetContentType(IBusEngine *engine);
    virtual void UpdateContentType(IBusEngine *engine);
    virtual void Update(IBusEngine *engine);
    virtual void ProcessPropertyActivate(IBusEngine *engine,
                                         const gchar *property_name,
                                         guint property_state);
    virtual bool IsActivated() const;
    virtual bool IsDisabled() const;

private:
    void UpdateContentTypeImpl(IBusEngine *engine, bool disabled);
    // Appends input method properties into panel
    void AppendInputMethodPropertyToPanel();
    // Appends output charset properties into panel
    void AppendOutputCharsetPropertyToPanel();
    // Appends tool properties into panel
    void AppendToolPropertyToPanel();
    // Appends switch properties into panel
    void UpdateInputMethodIcon(IBusEngine* engine, const InputMethod new_input_method);
    void SetInputMethod(IBusEngine *engine, const InputMethod new_input_method);
    void UpdateOutputCharsetIcon(IBusEngine* engine, const OutputCharset new_charset);
    void SetOutputCharset(IBusEngine *engine, const OutputCharset new_charset);

    IBusPropList *prop_root_;
    IBusProperty *prop_input_method_;
    IBusProperty *prop_output_charset_;
    IBusProperty *prop_unikey_tool_;

    InputMethod original_input_method_;
    OutputCharset original_output_charset_;
    bool is_activated_;
    bool is_disabled_;
};
