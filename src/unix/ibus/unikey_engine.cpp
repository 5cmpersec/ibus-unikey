#include "unix/ibus/unikey_engine.h"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <map>
#include <sstream>
#include <string>

#include "base/singleton.h"
#include "base/logging.h"

#include "unix/ibus/engine_registrar.h"
#include "unix/ibus/engine_private.h"

#include "unix/ibus/property_handler.h"


namespace {

struct IBusUnikeyEngineClass {
    IBusEngineClass parent;
};

struct IBusUnikeyEngine {
    IBusEngine parent;
    UnikeyEngine *engine;
};

IBusEngineClass *g_parent_class = nullptr;

GObject *UnikeyEngineClassConstructor(
    GType type,
    guint n_construct_properties,
    GObjectConstructParam *construct_properties) {
    BLOG_DEBUG("UnikeyEngineClassConstructor");

    ibus_unikey_init();
    ibus_unikey_engine_init();


    return G_OBJECT_CLASS(g_parent_class)->constructor(type,
                                                       n_construct_properties,
                                                       construct_properties);
}

void UnikeyEngineClassDestroy(IBusObject *engine) {
    BLOG_DEBUG("UnikeyEngineClassDestroy");
    IBUS_OBJECT_CLASS(g_parent_class)->destroy(engine);

    ibus_unikey_engine_destroy();
    ibus_unikey_exit();
}

void UnikeyEngineClassInit(gpointer klass, gpointer class_data) {
    BLOG_DEBUG("UnikeyEngineClassInit start");
    IBusEngineClass *engine_class = IBUS_ENGINE_CLASS(klass);

    BLOG_DEBUG("UnikeyEngineClassInit is called");
    EngineRegistrar::Register(
        Singleton<UnikeyEngine>::get(), engine_class);

    g_parent_class = reinterpret_cast<IBusEngineClass*>(
        g_type_class_peek_parent(klass));

    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    object_class->constructor = UnikeyEngineClassConstructor;
    IBusObjectClass *ibus_object_class = IBUS_OBJECT_CLASS(klass);
    ibus_object_class->destroy = UnikeyEngineClassDestroy;
    BLOG_DEBUG("UnikeyEngineClassInit end");
}

void UnikeyEngineInstanceInit(GTypeInstance *instance, gpointer klass) {
    BLOG_DEBUG("UnikeyEngineInstanceInit is called");
    IBusUnikeyEngine *engine = reinterpret_cast<IBusUnikeyEngine*>(instance);
    engine->engine = Singleton<UnikeyEngine>::get();
}

}  // namespace


UnikeyEngine::UnikeyEngine() 
    : property_handler_(new PropertyHandler()) {
}

UnikeyEngine::~UnikeyEngine() {

}

void UnikeyEngine::CandidateClicked(
    IBusEngine *engine,
    guint index,
    guint button,
    guint state) {
    BLOG_DEBUG("CandidateClicked");

}

void UnikeyEngine::CursorDown(IBusEngine *engine) {
    BLOG_DEBUG("CursorDown");

    g_parent_class->cursor_down(engine);
}

void UnikeyEngine::CursorUp(IBusEngine *engine) {
    BLOG_DEBUG("CursorUp");

    g_parent_class->cursor_up(engine);
}

void UnikeyEngine::Disable(IBusEngine *engine) {
    BLOG_DEBUG("Disable");

    ibus_unikey_engine_disable(engine);

    g_parent_class->disable(engine);
}

void UnikeyEngine::Enable(IBusEngine *engine) {
    BLOG_DEBUG("Enable");
    // If engine wants to use surrounding text, we should call
    // ibus_engine_get_surrounding_text once when the engine enabled.
    //ibus_engine_get_surrounding_text(engine, nullptr, nullptr, nullptr);

    ibus_unikey_engine_enable(engine);

    g_parent_class->enable(engine);
}

void UnikeyEngine::FocusIn(IBusEngine *engine) {
    BLOG_DEBUG("FocusIn");
    property_handler_->Register(engine);

    ibus_unikey_engine_focus_in(engine);

    g_parent_class->focus_in(engine);
}

void UnikeyEngine::FocusOut(IBusEngine *engine) {
    BLOG_DEBUG("FocusOut");
    property_handler_->ResetContentType(engine);

    ibus_unikey_engine_focus_out(engine);

    g_parent_class->focus_out(engine);
}

void UnikeyEngine::PageDown(IBusEngine *engine) {
    BLOG_DEBUG("PageDown");

    g_parent_class->page_down(engine);
}

void UnikeyEngine::PageUp(IBusEngine *engine) {
    BLOG_DEBUG("PageUp");

    g_parent_class->page_up(engine);
}

gboolean UnikeyEngine::ProcessKeyEvent(
    IBusEngine *engine,
    guint keyval,
    guint keycode,
    guint modifiers) {
    BLOG_DEBUG("keyval: {}, keycode: {}, modifiers:  {}", keyval, keycode, modifiers);

    return ibus_unikey_engine_process_key_event(engine, keyval, keycode, modifiers);
}

void UnikeyEngine::PropertyActivate(IBusEngine *engine,
                                  const gchar *property_name,
                                  guint property_state) {
    BLOG_DEBUG("PropertyActivate: name={}, state={}", property_name, property_state);

    property_handler_->ProcessPropertyActivate(engine, property_name, property_state);
    // ibus_unikey_engine_property_activate(engine, property_name, property_state);
}

void UnikeyEngine::PropertyHide(IBusEngine *engine,
                              const gchar *property_name) {
  // We can ignore the signal.
}

void UnikeyEngine::PropertyShow(IBusEngine *engine,
                              const gchar *property_name) {
  // We can ignore the signal.
}

void UnikeyEngine::Reset(IBusEngine *engine) {
    BLOG_DEBUG("Reset");

    ibus_unikey_engine_reset(engine);
}

void UnikeyEngine::SetCapabilities(IBusEngine *engine,
                                 guint capabilities) {
  // Do nothing.
}

void UnikeyEngine::SetCursorLocation(IBusEngine *engine,
                                   gint x,
                                   gint y,
                                   gint w,
                                   gint h) {

}

void UnikeyEngine::SetContentType(IBusEngine *engine,
                                guint purpose,
                                guint hints) {

}

GType UnikeyEngine::GetType() {
    BLOG_DEBUG("GetType start");
    static GType type = 0;

    static const GTypeInfo type_info = {
        sizeof(IBusUnikeyEngineClass),
        nullptr,
        nullptr,
        UnikeyEngineClassInit,
        nullptr,
        nullptr,
        sizeof(IBusUnikeyEngine),
        0,
        UnikeyEngineInstanceInit,
    };

    if (type == 0) {
        type = g_type_register_static(IBUS_TYPE_ENGINE,
                                      "IBusUnikeyEngine",
                                      &type_info,
                                      static_cast<GTypeFlags>(0));
        if (type == 0) {
            BLOG_DEBUG("g_type_register_static failed");
        }
    }

    BLOG_DEBUG("GetType end");
    return type;
}

// static
void UnikeyEngine::Disconnected(IBusBus *bus, gpointer user_data) {
    BLOG_DEBUG("Disconnected");
    ibus_quit();
}
