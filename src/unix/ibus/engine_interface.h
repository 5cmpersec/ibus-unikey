#pragma once

#include "unix/ibus/ibus_header.h"


class IEngine {
public:
    IEngine() {}
    virtual ~IEngine() {}

    // The interface function for the "candidate-clicked" signal
    virtual void CandidateClicked(IBusEngine *engine,
                                  guint index,
                                  guint button,
                                  guint state) = 0;

    // The interface function for the "cursor-down" signal
    virtual void CursorDown(IBusEngine *engine) = 0;

    // The interface function for the "cursor-up" signal
    virtual void CursorUp(IBusEngine *engine) = 0;

    // The interface function for the "disable" signal
    virtual void Disable(IBusEngine *engine) = 0;

    // The interface function for the "enable" signal
    virtual void Enable(IBusEngine *engine) = 0;

    // The interface function for the "focus-in" signal
    virtual void FocusIn(IBusEngine *engine) = 0;

    // The interface function for the "focus-out" signal
    virtual void FocusOut(IBusEngine *engine) = 0;

    // The interface function for the "page-down" signal
    virtual void PageDown(IBusEngine *engine) = 0;

    // The interface function for the "page-up" signal
    virtual void PageUp(IBusEngine *engine) = 0;

    // The interface function for the "process-key-event" signal
    virtual gboolean ProcessKeyEvent(IBusEngine *engine,
                                     guint keyval,
                                     guint keycode,
                                     guint state) = 0;

    // The interface function for the "property-activate" signal
    virtual void PropertyActivate(IBusEngine *engine,
                                  const gchar *property_name,
                                  guint property_state) = 0;

    // The interface function for the "property-hide" signal
    virtual void PropertyHide(IBusEngine *engine,
                              const gchar *property_name) = 0;

    // The interface function for the "property-show" signal
    virtual void PropertyShow(IBusEngine *engine,
                              const gchar *property_name) = 0;

    // The interface function for the "reset" signal
    virtual void Reset(IBusEngine *engine) = 0;

    // The interface function for the "set-capabilities" signal
    virtual void SetCapabilities(IBusEngine *engine,
                                 guint capabilities) = 0;

    // The interface function for the "set-cursor-location" signal
    virtual void SetCursorLocation(IBusEngine *engine,
                                   gint x,
                                   gint y,
                                   gint w,
                                   gint h) = 0;

    // The interface function for the "set-content-type" signal
    virtual void SetContentType(IBusEngine *engine,
                                guint purpose,
                                guint hints) = 0;
};
