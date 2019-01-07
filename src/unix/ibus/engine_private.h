#ifndef __ENGINE_PRIVATE_H__
#define __ENGINE_PRIVATE_H__

#include <string>
#include <ibus.h>

#include <gio/gio.h>

#include "unikey.h"
#include "vnconv.h"

typedef struct _IBusUnikeyData         IBusUnikeyData;

// libunikey internal data
struct _IBusUnikeyData
{
    IBusPropList* prop_list;
    IBusPropList* menu_im;
    IBusPropList* menu_oc;
    IBusPropList* menu_opt;

    UkInputMethod im; // input method
    unsigned int  oc; // output charset
    UnikeyOptions ukopt;
    gboolean process_w_at_begin;

    gboolean last_key_with_shift;

    std::string* preeditstr;
};

void ibus_unikey_init();
void ibus_unikey_exit();

// prototype
void ibus_unikey_engine_init();

void ibus_unikey_engine_destroy();
gboolean ibus_unikey_engine_process_key_event(IBusEngine* engine,
                                                     guint keyval,
                                                     guint keycode,
                                                     guint modifiers);

void ibus_unikey_engine_focus_in(IBusEngine* engine);
void ibus_unikey_engine_focus_out(IBusEngine* engine);
void ibus_unikey_engine_reset(IBusEngine* engine);
void ibus_unikey_engine_enable(IBusEngine* engine);
void ibus_unikey_engine_disable(IBusEngine* engine);
void ibus_unikey_engine_load_config();
void ibus_unikey_config_value_changed(GSettings *settings,
                                             const gchar *key,
                                             gpointer    user_data);
void ibus_unikey_engine_property_activate(IBusEngine* engine,
                                                 const gchar* prop_name,
                                                 guint prop_state);

gboolean ibus_unikey_engine_process_key_event_preedit(IBusEngine* engine,
                                                             guint keyval,
                                                             guint keycode,
                                                             guint modifiers);

void ibus_unikey_engine_create_property_list();

void ibus_unikey_engine_update_preedit_string(IBusEngine *engine, const gchar *string, gboolean visible);
void ibus_unikey_engine_erase_chars(int num_chars);

void ibus_unikey_engine_clean_buffer(IBusEngine* engine);

void ibus_unikey_engine_commit(IBusEngine* engine);

#endif // __ENGINE_PRIVATE_H__

