#pragma once

#include "unix/ibus/ibus_header.h"


class IEngine;

  // Provides functions to register/unregister interface functions implemented by
  // a concrete class of IEngine as signal handlers of IBusEngine.
class EngineRegistrar {
public:
    // Registers signal handlers implemented by |engine| to the ibus framework.
    static bool Register(IEngine *engine,
                         IBusEngineClass *engine_class);

    // Unregisters all signal handlers registered to the ibus framework.
    // Returns an engine interface registered previously.
    static IEngine *Unregister(IBusEngineClass *engine_class);

private:
    EngineRegistrar();
    ~EngineRegistrar();

    static void CandidateClicked(IBusEngine *engine,
                                 guint index,
                                 guint button,
                                 guint state);
    static void CursorDown(IBusEngine *engine);
    static void CursorUp(IBusEngine *engine);
    static void Disable(IBusEngine *engine);
    static void Enable(IBusEngine *engine);
    static void FocusIn(IBusEngine *engine);
    static void FocusOut(IBusEngine *engine);
    static void PageDown(IBusEngine *engine);
    static void PageUp(IBusEngine *engine);
    static gboolean ProcessKeyEvent(IBusEngine *engine,
                                    guint keyval,
                                    guint keycode,
                                    guint state);
    static void PropertyActivate(IBusEngine *engine,
                                 const gchar *property_name,
                                 guint property_state);
    static void PropertyHide(IBusEngine *engine,
                             const gchar *property_name);
    static void PropertyShow(IBusEngine *engine,
                             const gchar *property_name);
    static void Reset(IBusEngine *engine);
    static void SetCapabilities(IBusEngine *engine,
                                guint capabilities);
    static void SetCursorLocation(IBusEngine *engine,
                                  gint x,
                                  gint y,
                                  gint w,
                                  gint h);
    static void SetContentType(IBusEngine *engine,
                               guint purpose,
                               guint hints);
};
