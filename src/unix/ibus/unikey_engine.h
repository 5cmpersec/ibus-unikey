#pragma once

#include <memory>
#include <set>
#include <vector>

#include "base/port.h"
#include "unix/ibus/engine_interface.h"

class IPropertyHandler;

class UnikeyEngine : public IEngine {
public:
    UnikeyEngine();
    virtual ~UnikeyEngine();

    // EngineInterface functions
    void CandidateClicked(IBusEngine *engine,
                          guint index,
                          guint button,
                          guint state);
    void CursorDown(IBusEngine *engine);
    void CursorUp(IBusEngine *engine);
    void Disable(IBusEngine *engine);
    void Enable(IBusEngine *engine);
    void FocusIn(IBusEngine *engine);
    void FocusOut(IBusEngine *engine);
    void PageDown(IBusEngine *engine);
    void PageUp(IBusEngine *engine);
    gboolean ProcessKeyEvent(IBusEngine *engine,
                             guint keyval,
                             guint keycode,
                             guint state);
    void PropertyActivate(IBusEngine *engine,
                          const gchar *property_name,
                          guint property_state);
    void PropertyHide(IBusEngine *engine,
                      const gchar *property_name);
    void PropertyShow(IBusEngine *engine,
                      const gchar *property_name);
    void Reset(IBusEngine *engine);
    void SetCapabilities(IBusEngine *engine,
                         guint capabilities);
    void SetCursorLocation(IBusEngine *engine,
                           gint x,
                           gint y,
                           gint w,
                           gint h);
    void SetContentType(IBusEngine *engine,
                        guint purpose,
                        guint hints);

    // Returns the GType which this class represents.
    static GType GetType();
    // The callback function to the "disconnected" signal to the bus object.
    static void Disconnected(IBusBus *bus, gpointer user_data);

private:
    std::unique_ptr<IPropertyHandler> property_handler_;

    DISALLOW_COPY_AND_ASSIGN(UnikeyEngine);
};

