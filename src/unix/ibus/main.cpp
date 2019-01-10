#include <cstddef>
#include <cstdio>

#include "base/const.h"
#include "base/logging.h"
#include "base/version.h"

#include "unix/ibus/unikey_engine.h"

namespace {

IBusBus *g_bus = nullptr;

// Creates a IBusComponent object and add engine(s) to the object.
IBusComponent *GetIBusComponent() {
    IBusComponent *component = ibus_component_new(
        kComponentName,
        kComponentDescription,
        Version::GetUnikeyVersion().c_str(),
        kComponentLicense,
        kComponentAuthor,
        kComponentHomepage,
        "",
        kComponentTextdomain);

    for (size_t i = 0; i < kEngineArrayLen; ++i) {
        ibus_component_add_engine(component,
                                  ibus_engine_desc_new(kEngineNameArray[i],
                                                       kEngineLongnameArray[i],
                                                       kEngineDescription,
                                                       kEngineLanguage,
                                                       kComponentLicense,
                                                       kComponentAuthor,
                                                       kEngineIcon,
                                                       kEngineLayoutArray[i]));
    }
    return component;
}

// Initializes ibus components and adds Unikey engine.
void InitIBusComponent(bool executed_by_ibus_daemon) {
    BLOG_DEBUG("InitIBusComponent started: {}", executed_by_ibus_daemon);
    g_bus = ibus_bus_new();
    BLOG_DEBUG("ibus_bus_new: ");
    g_signal_connect(g_bus,
                     "disconnected",
                     G_CALLBACK(UnikeyEngine::Disconnected),
                     nullptr);

    IBusComponent *component = GetIBusComponent();
    IBusFactory *factory = ibus_factory_new(ibus_bus_get_connection(g_bus));
    GList *engines = ibus_component_get_engines(component);
    for (GList *p = engines; p; p = p->next) {
        IBusEngineDesc *engine = reinterpret_cast<IBusEngineDesc*>(p->data);
        const gchar * const engine_name = ibus_engine_desc_get_name(engine);
        BLOG_DEBUG("engine_name: {}", engine_name);
        ibus_factory_add_engine(
            factory, engine_name, UnikeyEngine::GetType());
    }

    if (executed_by_ibus_daemon) {
        ibus_bus_request_name(g_bus, kComponentName, 0);
    } else {
        ibus_bus_register_component(g_bus, component);
    }
    g_object_unref(component);
    BLOG_DEBUG("InitIBusComponent ended");
}

}  // namespace

int main(gint argc, gchar **argv) {
    BLOG_DEBUG("main started");
    ibus_init();
    InitIBusComponent(true);
    ibus_main();
    return 0;
}
