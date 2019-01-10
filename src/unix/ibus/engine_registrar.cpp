#include "unix/ibus/engine_registrar.h"

#include "base/logging.h"
#include "unix/ibus/engine_interface.h"

namespace {
IEngine *g_engine = nullptr;
}

bool EngineRegistrar::Register(IEngine *engine,
                               IBusEngineClass *engine_class) {

    if (g_engine) {
        BLOG_INFO("engine is registered");
    }

    g_engine = engine;

    engine_class->cursor_down = CursorDown;
    engine_class->candidate_clicked = CandidateClicked;
    engine_class->cursor_down = CursorDown;
    engine_class->cursor_up = CursorUp;
    engine_class->disable = Disable;
    engine_class->enable = Enable;
    engine_class->focus_in = FocusIn;
    engine_class->focus_out = FocusOut;
    engine_class->page_down = PageDown;
    engine_class->page_up = PageUp;
    engine_class->process_key_event = ProcessKeyEvent;
    engine_class->property_activate = PropertyActivate;
    engine_class->property_hide = PropertyHide;
    engine_class->property_show = PropertyShow;
    engine_class->reset = Reset;
    engine_class->set_capabilities = SetCapabilities;
    engine_class->set_cursor_location = SetCursorLocation;
#if defined(UNIKEY_ENABLE_IBUS_INPUT_PURPOSE)
    engine_class->set_content_type = SetContentType;
#endif  // unikey_ENABLE_IBUS_INPUT_PURPOSE
    return true;
}

IEngine *EngineRegistrar::Unregister(IBusEngineClass *engine_class) {
    if (g_engine) {
        BLOG_INFO("engine is registered");
    }

    engine_class->cursor_down = nullptr;
    engine_class->candidate_clicked = nullptr;
    engine_class->cursor_down = nullptr;
    engine_class->cursor_up = nullptr;
    engine_class->disable = nullptr;
    engine_class->enable = nullptr;
    engine_class->focus_in = nullptr;
    engine_class->focus_out = nullptr;
    engine_class->page_down = nullptr;
    engine_class->page_up = nullptr;
    engine_class->process_key_event = nullptr;
    engine_class->property_activate = nullptr;
    engine_class->property_hide = nullptr;
    engine_class->property_show = nullptr;
    engine_class->reset = nullptr;
    engine_class->set_capabilities = nullptr;
    engine_class->set_cursor_location = nullptr;
#if defined(UNIKEY_ENABLE_IBUS_INPUT_PURPOSE)
    engine_class->set_content_type = nullptr;
#endif  // UNIKEY_ENABLE_IBUS_INPUT_PURPOSE

    IEngine *previous = g_engine;
    g_engine = nullptr;
    return previous;
}

void EngineRegistrar::CandidateClicked(
    IBusEngine *engine,
    guint index,
    guint button,
    guint state) {
    g_engine->CandidateClicked(engine, index, button, state);
}

void EngineRegistrar::CursorDown(IBusEngine *engine) {
    g_engine->CursorDown(engine);
}

void EngineRegistrar::CursorUp(IBusEngine *engine) {
    g_engine->CursorUp(engine);
}

void EngineRegistrar::Disable(IBusEngine *engine) {
    g_engine->Disable(engine);
}

void EngineRegistrar::Enable(IBusEngine *engine) {
    g_engine->Enable(engine);
}

void EngineRegistrar::FocusIn(IBusEngine *engine) {
    g_engine->FocusIn(engine);
}

void EngineRegistrar::FocusOut(IBusEngine *engine) {
    g_engine->FocusOut(engine);
}

void EngineRegistrar::PageDown(IBusEngine *engine) {
    g_engine->PageDown(engine);
}

void EngineRegistrar::PageUp(IBusEngine *engine) {
    g_engine->PageUp(engine);
}

gboolean EngineRegistrar::ProcessKeyEvent(
    IBusEngine *engine,
    guint keyval,
    guint keycode,
    guint state) {
    return g_engine->ProcessKeyEvent(engine, keyval, keycode, state);
}

void EngineRegistrar::PropertyActivate(
    IBusEngine *engine,
    const gchar *property_name,
    guint property_state) {
    g_engine->PropertyActivate(engine, property_name, property_state);
}

void EngineRegistrar::PropertyHide(
    IBusEngine *engine,
    const gchar *property_name) {
    g_engine->PropertyHide(engine, property_name);
}

void EngineRegistrar::PropertyShow(
    IBusEngine *engine,
    const gchar *property_name) {
    g_engine->PropertyShow(engine, property_name);
}

void EngineRegistrar::Reset(IBusEngine *engine) {
    g_engine->Reset(engine);
}

void EngineRegistrar::SetCapabilities(
    IBusEngine *engine,
    guint capabilities) {
    g_engine->SetCapabilities(engine, capabilities);
}

void EngineRegistrar::SetCursorLocation(
    IBusEngine *engine,
    gint x,
    gint y,
    gint w,
    gint h) {
    g_engine->SetCursorLocation(engine, x, y, w, h);
}

void EngineRegistrar::SetContentType(
    IBusEngine *engine,
    guint purpose,
    guint hints) {
    g_engine->SetContentType(engine, purpose, hints);
}

