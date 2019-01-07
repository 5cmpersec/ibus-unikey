#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "engine_const.h"
#include "engine_private.h"

#include <string.h>
#include <cstdlib>

#include <sys/wait.h>

#include <libintl.h>
#include <ibus.h>

#include "utils.h"
#include "unikey.h"
#include "vnconv.h"

#include "base/logging.h"

#define _(string) gettext(string)

#define CONVERT_BUF_SIZE 1024

static unsigned char WordBreakSyms[] =
{
    ',', ';', ':', '.', '\"', '\'', '!', '?', ' ',
    '<', '>', '=', '+', '-', '*', '/', '\\',
    '_', '~', '`', '@', '#', '$', '%', '^', '&', '(', ')', '{', '}', '[', ']',
    '|'
};

static GSettings*       settings     = NULL;
static IBusUnikeyData g_data;


void ibus_unikey_init()
{
    BLOG_DEBUG("ibus_unikey_init");
    UnikeySetup();
    settings = g_settings_new("org.freedesktop.ibus.engine.unikey");

    g_signal_connect(settings, "changed", G_CALLBACK(ibus_unikey_config_value_changed), NULL);
}

void ibus_unikey_exit()
{
    BLOG_DEBUG("ibus_unikey_exit");
    UnikeyCleanup();
}


void ibus_unikey_engine_init()
{
    BLOG_DEBUG("ibus_unikey_engine_init");
    ibus_unikey_engine_load_config();

    g_data.preeditstr = new std::string();
    ibus_unikey_engine_create_property_list();
}

void ibus_unikey_engine_load_config()
{
    BLOG_DEBUG("ibus_unikey_engine_load_config");
    gchar* str;
    gboolean b;
    guint i;

    //set default options
    g_data.im = Unikey_IM[0];
    g_data.oc = Unikey_OC[0];
    g_data.ukopt.spellCheckEnabled     = DEFAULT_CONF_SPELLCHECK;
    g_data.ukopt.autoNonVnRestore      = DEFAULT_CONF_AUTONONVNRESTORE;
    g_data.ukopt.modernStyle           = DEFAULT_CONF_MODERNSTYLE;
    g_data.ukopt.freeMarking           = DEFAULT_CONF_FREEMARKING;
    g_data.ukopt.macroEnabled          = DEFAULT_CONF_MACROENABLED;
    g_data.process_w_at_begin          = DEFAULT_CONF_PROCESSWATBEGIN;

    if (ibus_unikey_config_get_string(settings, CONFIG_INPUTMETHOD, &str))
    {
        BLOG_DEBUG("load_config: im={}", str);
        for (i = 0; i < NUM_INPUTMETHOD; i++)
        {
            if (strcasecmp(str, Unikey_IMNames[i]) == 0)
            {
                g_data.im = Unikey_IM[i];
                break;
            }
        }
    }

    if (ibus_unikey_config_get_string(settings, CONFIG_OUTPUTCHARSET, &str))
    {
        BLOG_DEBUG("load_config: oc={}", str);
        for (i = 0; i < NUM_OUTPUTCHARSET; i++)
        {
            if (strcasecmp(str, Unikey_OCNames[i]) == 0)
            {
                g_data.oc = Unikey_OC[i];
                break;
            }
        }
    }

    if (ibus_unikey_config_get_boolean(settings, CONFIG_FREEMARKING, &b))
        g_data.ukopt.freeMarking = b;

    if (ibus_unikey_config_get_boolean(settings, CONFIG_MODERNSTYLE, &b))
        g_data.ukopt.modernStyle = b;

    if (ibus_unikey_config_get_boolean(settings, CONFIG_MACROENABLED, &b))
        g_data.ukopt.macroEnabled = b;

    if (ibus_unikey_config_get_boolean(settings, CONFIG_SPELLCHECK, &b))
        g_data.ukopt.spellCheckEnabled = b;

    if (ibus_unikey_config_get_boolean(settings, CONFIG_AUTORESTORENONVN, &b))
        g_data.ukopt.autoNonVnRestore = b;

    if (ibus_unikey_config_get_boolean(settings, CONFIG_PROCESSWATBEGIN, &b))
        g_data.process_w_at_begin = b;

    // load macro
    gchar* fn = get_macro_file();
    UnikeyLoadMacroTable(fn);
    g_free(fn);
}


void ibus_unikey_engine_destroy()
{
    BLOG_DEBUG("ibus_unikey_engine_destroy");
    delete g_data.preeditstr;
    g_object_unref(g_data.prop_list);
}


void ibus_unikey_engine_focus_in(IBusEngine* engine)
{
    BLOG_DEBUG("ibus_unikey_engine_focus_in");

    UnikeySetInputMethod(g_data.im);
    UnikeySetOutputCharset(g_data.oc);

    UnikeySetOptions(&g_data.ukopt);
    ibus_engine_register_properties(engine, g_data.prop_list);
}

void ibus_unikey_engine_focus_out(IBusEngine* engine)
{
    BLOG_DEBUG("ibus_unikey_engine_focus_out");
    ibus_unikey_engine_clean_buffer(engine);
}

void ibus_unikey_engine_reset(IBusEngine* engine)
{
    BLOG_DEBUG("ibus_unikey_engine_reset");
    ibus_unikey_engine_clean_buffer(engine);
}

void ibus_unikey_engine_enable(IBusEngine* engine)
{
    BLOG_DEBUG("ibus_unikey_engine_enable");
}

void ibus_unikey_engine_disable(IBusEngine* engine)
{
    BLOG_DEBUG("ibus_unikey_engine_disable");
}

void ibus_unikey_config_value_changed(GSettings *settings,
                                             const gchar *key,
                                             gpointer    user_data)
{
    BLOG_DEBUG("ibus_unikey_config_value_changed: key={}", key);
    // TODO: Should update for the key only.
    ibus_unikey_engine_load_config();
    ibus_unikey_engine_create_property_list();
}

void ibus_unikey_engine_property_activate(IBusEngine* engine,
                                                 const gchar* prop_name,
                                                 guint prop_state)
{
    BLOG_DEBUG("ibus_unikey_engine_property_activate: {}, {}", prop_name, prop_state);
    IBusProperty* prop;
    IBusText* label;
    guint i, j;

    // input method active
    if (strncmp(prop_name, CONFIG_INPUTMETHOD, strlen(CONFIG_INPUTMETHOD)) == 0)
    {
        for (i=0; i<NUM_INPUTMETHOD; i++)
        {
            if (strcmp(prop_name + strlen(CONFIG_INPUTMETHOD)+1,
                       Unikey_IMNames[i]) == 0)
            {
                g_data.im = Unikey_IM[i];
                ibus_unikey_config_set_string(settings, CONFIG_INPUTMETHOD, Unikey_IMNames[i]);

                // update label
                for (j=0; j<g_data.prop_list->properties->len; j++)
                {
                    prop = ibus_prop_list_get(g_data.prop_list, j);
                    if (prop==NULL)
                        return;
                    else if (strcmp(ibus_property_get_key(prop), CONFIG_INPUTMETHOD) == 0)
                    {
                        label = ibus_text_new_from_static_string(Unikey_IMNames[i]);
                        ibus_property_set_label(prop, label);
                        break;
                    }
                } // end update label

                // update property state
                for (j=0; j<g_data.menu_im->properties->len; j++)
                {
                    prop = ibus_prop_list_get(g_data.menu_im, j);
                    if (prop==NULL)
                        return;
                    else if (strcmp(ibus_property_get_key(prop), prop_name)==0)
                        ibus_property_set_state(prop, PROP_STATE_CHECKED);
                    else
                        ibus_property_set_state(prop, PROP_STATE_UNCHECKED);
                } // end update property state

                break;
            }
        }
    } // end input method active

    // output charset active
    else if (strncmp(prop_name, CONFIG_OUTPUTCHARSET, strlen(CONFIG_OUTPUTCHARSET)) == 0)
    {
        for (i=0; i<NUM_OUTPUTCHARSET; i++)
        {
            if (strcmp(prop_name+strlen(CONFIG_OUTPUTCHARSET)+1,
                       Unikey_OCNames[i]) == 0)
            {
                g_data.oc = Unikey_OC[i];
                ibus_unikey_config_set_string(settings, CONFIG_OUTPUTCHARSET, Unikey_OCNames[i]);

                // update label
                for (j=0; j<g_data.prop_list->properties->len; j++)
                {
                    prop = ibus_prop_list_get(g_data.prop_list, j);
                    if (prop==NULL)
                        return;
                    else if (strcmp(ibus_property_get_key(prop), CONFIG_OUTPUTCHARSET)==0)
                    {
                        label = ibus_text_new_from_static_string(Unikey_OCNames[i]);
                        ibus_property_set_label(prop, label);
                        break;
                    }
                } // end update label

                // update property state
                for (j=0; j<g_data.menu_oc->properties->len; j++)
                {
                    prop = ibus_prop_list_get(g_data.menu_oc, j);
                    if (prop==NULL)
                        return;
                    else if (strcmp(ibus_property_get_key(prop), prop_name) == 0)
                        ibus_property_set_state(prop, PROP_STATE_CHECKED);
                    else
                        ibus_property_set_state(prop, PROP_STATE_UNCHECKED);
                } // end update property state

                break;
            }
        }
    } // end output charset active

    // spellcheck active
    else if (strcmp(prop_name, CONFIG_SPELLCHECK) == 0)
    {
        g_data.ukopt.spellCheckEnabled = !g_data.ukopt.spellCheckEnabled;
        ibus_unikey_config_set_boolean(settings, CONFIG_SPELLCHECK, (g_data.ukopt.spellCheckEnabled == 1));

        // update state
        for (j = 0; j < g_data.menu_opt->properties->len ; j++)
        {
            prop = ibus_prop_list_get(g_data.menu_opt, j);
            if (prop == NULL)
                return;

            else if (strcmp(ibus_property_get_key(prop), CONFIG_SPELLCHECK) == 0)
            {
                ibus_property_set_state(prop, (g_data.ukopt.spellCheckEnabled == 1)?
                    PROP_STATE_CHECKED:PROP_STATE_UNCHECKED);
                break;
            }
        } // end update state
    } // end spellcheck active

    // MacroEnabled active
    else if (strcmp(prop_name, CONFIG_MACROENABLED) == 0)
    {
        g_data.ukopt.macroEnabled = !g_data.ukopt.macroEnabled;
        ibus_unikey_config_set_boolean(settings, CONFIG_MACROENABLED, (g_data.ukopt.macroEnabled == 1));

        // update state
        for (j = 0; j < g_data.menu_opt->properties->len ; j++)
        {
            prop = ibus_prop_list_get(g_data.menu_opt, j);
            if (prop == NULL)
                return;

            else if (strcmp(ibus_property_get_key(prop), CONFIG_MACROENABLED) == 0)
            {
                ibus_property_set_state(prop, (g_data.ukopt.macroEnabled == 1)?
                    PROP_STATE_CHECKED:PROP_STATE_UNCHECKED);
                break;
            }
        } // end update state
    } // end MacroEnabled active

    // if Run setup
    else if (strcmp(prop_name, "RunSetupGUI") == 0)
    {
        int ret = system(LIBEXECDIR "/ibus-setup-unikey &");
        if (ret == -1)
            return;
    } // END Run setup

    UnikeySetInputMethod(g_data.im);
    UnikeySetOutputCharset(g_data.oc);
    UnikeySetOptions(&g_data.ukopt);
}

void ibus_unikey_engine_create_property_list()
{
    BLOG_DEBUG("ibus_unikey_engine_create_property_list");
    IBusProperty* prop;
    IBusText* label,* tooltip;
    gchar name[32];
    guint i;

    if (g_data.prop_list == NULL)
    {
        g_data.prop_list = ibus_prop_list_new();
        g_data.menu_im   = ibus_prop_list_new();
        g_data.menu_oc   = ibus_prop_list_new();
        g_data.menu_opt  = ibus_prop_list_new();

        g_object_ref_sink(g_data.prop_list);
    }

// create input method menu
    // add item
    for (i = 0; i < NUM_INPUTMETHOD; i++)
    {
        label = ibus_text_new_from_static_string(Unikey_IMNames[i]);
        tooltip = ibus_text_new_from_static_string(""); // ?
        sprintf(name, CONFIG_INPUTMETHOD"_%s", Unikey_IMNames[i]);
        prop = ibus_property_new(name,
                                 PROP_TYPE_RADIO,
                                 label,
                                 "",
                                 tooltip,
                                 TRUE,
                                 TRUE,
                                 Unikey_IM[i]==g_data.im?PROP_STATE_CHECKED:PROP_STATE_UNCHECKED,
                                 NULL);

        if (ibus_prop_list_update_property(g_data.menu_im, prop) == false)
            ibus_prop_list_append(g_data.menu_im, prop);
    }
// END create input method menu

// create output charset menu
    // add item
    for (i = 0; i < NUM_OUTPUTCHARSET; i++)
    {
        label = ibus_text_new_from_static_string(Unikey_OCNames[i]);
        tooltip = ibus_text_new_from_static_string(""); // ?
        sprintf(name, CONFIG_OUTPUTCHARSET"_%s", Unikey_OCNames[i]);
        prop = ibus_property_new(name,
                                 PROP_TYPE_RADIO,
                                 label,
                                 "",
                                 tooltip,
                                 TRUE,
                                 TRUE,
                                 Unikey_OC[i]==g_data.oc?PROP_STATE_CHECKED:PROP_STATE_UNCHECKED,
                                 NULL);

        if (ibus_prop_list_update_property(g_data.menu_oc, prop) == false)
            ibus_prop_list_append(g_data.menu_oc, prop);
    }
// END create output charset menu

// create option menu (for configure unikey)
    // add option property

    // --create and add spellcheck property
    label = ibus_text_new_from_static_string(_("Enable spell check"));
    tooltip = ibus_text_new_from_static_string(_("If enable, you can decrease mistake when typing"));
    prop = ibus_property_new(CONFIG_SPELLCHECK,
                             PROP_TYPE_TOGGLE,
                             label,
                             "",
                             tooltip,
                             TRUE,
                             TRUE,
                             (g_data.ukopt.spellCheckEnabled==1)?
                             PROP_STATE_CHECKED:PROP_STATE_UNCHECKED,
                             NULL);

    if (ibus_prop_list_update_property(g_data.menu_opt, prop) == false)
        ibus_prop_list_append(g_data.menu_opt, prop);

    // --create and add macroEnabled property
    label = ibus_text_new_from_static_string(_("Enable Macro"));
    tooltip = ibus_text_new_from_static_string("");
    prop = ibus_property_new(CONFIG_MACROENABLED,
                             PROP_TYPE_TOGGLE,
                             label,
                             "",
                             tooltip,
                             TRUE,
                             TRUE,
                             (g_data.ukopt.macroEnabled==1)?
                             PROP_STATE_CHECKED:PROP_STATE_UNCHECKED,
                             NULL);

    if (ibus_prop_list_update_property(g_data.menu_opt, prop) == false)
        ibus_prop_list_append(g_data.menu_opt, prop);

    // --separator
    prop = ibus_property_new("", PROP_TYPE_SEPARATOR,
                             NULL, "", NULL, TRUE, TRUE,
                             PROP_STATE_UNCHECKED, NULL);
    if (ibus_prop_list_update_property(g_data.menu_opt, prop) == false)
        ibus_prop_list_append(g_data.menu_opt, prop);

    // --create and add Launch Setup GUI property
    label = ibus_text_new_from_static_string(_("Full setup..."));
    tooltip = ibus_text_new_from_static_string(_("Full setup utility for IBus-Unikey"));
    prop = ibus_property_new("RunSetupGUI",
                             PROP_TYPE_NORMAL,
                             label,
                             "",
                             tooltip,
                             TRUE,
                             TRUE,
                             PROP_STATE_UNCHECKED,
                             NULL);

    if (ibus_prop_list_update_property(g_data.menu_opt, prop) == false)
        ibus_prop_list_append(g_data.menu_opt, prop);
// END create option menu

// create top menu
    // add item
    // -- add input method menu
    for (i = 0; i < NUM_INPUTMETHOD; i++)
    {
        if (Unikey_IM[i] == g_data.im)
            break;
    }
    label = ibus_text_new_from_static_string(Unikey_IMNames[i]);
    tooltip = ibus_text_new_from_static_string(_("Choose input method"));
    prop = ibus_property_new(CONFIG_INPUTMETHOD,
                             PROP_TYPE_MENU,
                             label,
                             "",
                             tooltip,
                             TRUE,
                             TRUE,
                             PROP_STATE_UNCHECKED,
                             g_data.menu_im);

    if (ibus_prop_list_update_property(g_data.prop_list, prop) == false)
        ibus_prop_list_append(g_data.prop_list, prop);

    // -- add output charset menu
    for (i = 0; i < NUM_OUTPUTCHARSET; i++)
    {
        if (Unikey_OC[i] == g_data.oc)
            break;
    }
    label = ibus_text_new_from_static_string(Unikey_OCNames[i]);
    tooltip = ibus_text_new_from_static_string(_("Choose output charset"));
    prop = ibus_property_new(CONFIG_OUTPUTCHARSET,
                             PROP_TYPE_MENU,
                             label,
                             "",
                             tooltip,
                             TRUE,
                             TRUE,
                             PROP_STATE_UNCHECKED,
                             g_data.menu_oc);

    if (ibus_prop_list_update_property(g_data.prop_list, prop) == false)
        ibus_prop_list_append(g_data.prop_list, prop);

    // -- add option menu
    label = ibus_text_new_from_static_string(_("Options"));
    tooltip = ibus_text_new_from_static_string(_("Options for Unikey"));
    prop = ibus_property_new("Options",
                             PROP_TYPE_MENU,
                             label,
                             "gtk-preferences",
                             tooltip,
                             TRUE,
                             TRUE,
                             PROP_STATE_UNCHECKED,
                             g_data.menu_opt);

    if (ibus_prop_list_update_property(g_data.prop_list, prop) == false)
        ibus_prop_list_append(g_data.prop_list, prop);
// end top menu
}

void ibus_unikey_engine_update_preedit_string(IBusEngine *engine, const gchar *string, gboolean visible)
{
    BLOG_DEBUG("ibus_unikey_engine_update_preedit_string");
    IBusText *text;

    text = ibus_text_new_from_static_string(string);

    // underline text
    ibus_text_append_attribute(text, IBUS_ATTR_TYPE_UNDERLINE, IBUS_ATTR_UNDERLINE_SINGLE, 0, -1);

    // update and display text
    ibus_engine_update_preedit_text_with_mode(engine, text, ibus_text_get_length(text), visible, IBUS_ENGINE_PREEDIT_COMMIT);
}

void ibus_unikey_engine_erase_chars(int num_chars)
{
    BLOG_DEBUG("ibus_unikey_engine_erase_chars");
    int i, k;
    guchar c;

    k = num_chars;

    for ( i = g_data.preeditstr->length()-1; i >= 0 && k > 0; i--)
    {
        c = g_data.preeditstr->at(i);

        // count down if byte is begin byte of utf-8 char
        if (c < (guchar)'\x80' || c >= (guchar)'\xC0')
        {
            k--;
        }
    }

    g_data.preeditstr->erase(i+1);
}

gboolean ibus_unikey_engine_process_key_event(IBusEngine* engine,
                                                     guint keyval,
                                                     guint keycode,
                                                     guint modifiers)
{
    BLOG_DEBUG("ibus_unikey_engine_process_key_event");
    static gboolean tmp;

    tmp = ibus_unikey_engine_process_key_event_preedit(engine, keyval, keycode, modifiers);

    // check last keyevent with shift
    if (keyval >= IBUS_space && keyval <=IBUS_asciitilde)
    {
        g_data.last_key_with_shift = modifiers & IBUS_SHIFT_MASK;
    }
    else
    {
        g_data.last_key_with_shift = false;
    } // end check last keyevent with shift

    return tmp;
}

gboolean ibus_unikey_engine_process_key_event_preedit(IBusEngine* engine,
                                                             guint keyval,
                                                             guint keycode,
                                                             guint modifiers)
{
    BLOG_DEBUG("ibus_unikey_engine_process_key_event_preedit");
    if (modifiers & IBUS_RELEASE_MASK)
    {
        return false;
    }

    else if (modifiers & IBUS_CONTROL_MASK
             || modifiers & IBUS_MOD1_MASK // alternate mask
             || keyval == IBUS_Control_L
             || keyval == IBUS_Control_R
             || keyval == IBUS_Tab
             || keyval == IBUS_Return
             || keyval == IBUS_Delete
             || keyval == IBUS_KP_Enter
             || (keyval >= IBUS_Home && keyval <= IBUS_Insert)
             || (keyval >= IBUS_KP_Home && keyval <= IBUS_KP_Delete)
        )
    {
        ibus_unikey_engine_commit(engine);
        return false;
    }

    else if ((keyval >= IBUS_Caps_Lock && keyval <= IBUS_Hyper_R)
            || (!(modifiers & IBUS_SHIFT_MASK) && (keyval == IBUS_Shift_L || keyval == IBUS_Shift_R))  // when press one shift key
        )
    {
        return false;
    }

    else if (keyval >=IBUS_KP_Multiply && keyval <=IBUS_KP_9)
    {
        ibus_unikey_engine_commit(engine);
        return false;
    }

    // capture BackSpace
    else if (keyval == IBUS_BackSpace)
    {
        UnikeyBackspacePress();

        if (UnikeyBackspaces == 0 || g_data.preeditstr->empty())
        {
            return false;
        }
        else
        {
            if (g_data.preeditstr->length() <= (guint)UnikeyBackspaces)
            {
                g_data.preeditstr->clear();
                ibus_engine_hide_preedit_text(engine);
            }
            else
            {
                ibus_unikey_engine_erase_chars(UnikeyBackspaces);
                ibus_unikey_engine_update_preedit_string(engine, g_data.preeditstr->c_str(), true);
            }

            // change tone position after press backspace
            if (UnikeyBufChars > 0)
            {
                if (g_data.oc == CONV_CHARSET_XUTF8)
                {
                    g_data.preeditstr->append((const gchar*)UnikeyBuf, UnikeyBufChars);
                }
                else
                {
                    static unsigned char buf[CONVERT_BUF_SIZE];
                    int bufSize = CONVERT_BUF_SIZE;

                    latinToUtf(buf, UnikeyBuf, UnikeyBufChars, &bufSize);
                    g_data.preeditstr->append((const gchar*)buf, CONVERT_BUF_SIZE - bufSize);
                }

                ibus_unikey_engine_update_preedit_string(engine, g_data.preeditstr->c_str(), true);
            }
        }
        return true;
    } // end capture BackSpace

    // capture ascii printable char
    else if ((keyval >= IBUS_space && keyval <=IBUS_asciitilde)
            || keyval == IBUS_Shift_L || keyval == IBUS_Shift_R) // sure this have IBUS_SHIFT_MASK
    {
        UnikeySetCapsState(modifiers & IBUS_SHIFT_MASK, modifiers & IBUS_LOCK_MASK);

        // process keyval

        if ((g_data.im == UkTelex || g_data.im == UkSimpleTelex2)
            && g_data.process_w_at_begin == false
            && UnikeyAtWordBeginning()
            && (keyval == IBUS_w || keyval == IBUS_W))
        {
            UnikeyPutChar(keyval);
            if (g_data.ukopt.macroEnabled == 0)
            {
                return false;
            }
            else
            {
                g_data.preeditstr->append(keyval==IBUS_w?"w":"W");
                ibus_unikey_engine_update_preedit_string(engine, g_data.preeditstr->c_str(), true);
                return true;
            }
        }

        // shift + space, shift + shift event
        if ((g_data.last_key_with_shift == false && modifiers & IBUS_SHIFT_MASK
                    && keyval == IBUS_space && !UnikeyAtWordBeginning())
            || (keyval == IBUS_Shift_L || keyval == IBUS_Shift_R) // (&& modifiers & IBUS_SHIFT_MASK), sure this have IBUS_SHIFT_MASK
           )
        {
            UnikeyRestoreKeyStrokes();
        } // end shift + space, shift + shift event

        else
        {
            UnikeyFilter(keyval);
        }
        // end process keyval

        // process result of ukengine
        if (UnikeyBackspaces > 0)
        {
            if (g_data.preeditstr->length() <= (guint)UnikeyBackspaces)
            {
                g_data.preeditstr->clear();
            }
            else
            {
                ibus_unikey_engine_erase_chars(UnikeyBackspaces);
            }
        }

        if (UnikeyBufChars > 0)
        {
            if (g_data.oc == CONV_CHARSET_XUTF8)
            {
                g_data.preeditstr->append((const gchar*)UnikeyBuf, UnikeyBufChars);
            }
            else
            {
                static unsigned char buf[CONVERT_BUF_SIZE];
                int bufSize = CONVERT_BUF_SIZE;

                latinToUtf(buf, UnikeyBuf, UnikeyBufChars, &bufSize);
                g_data.preeditstr->append((const gchar*)buf, CONVERT_BUF_SIZE - bufSize);
            }
        }
        else if (keyval != IBUS_Shift_L && keyval != IBUS_Shift_R) // if ukengine not process
        {
            static int n;
            static char s[6];

            n = g_unichar_to_utf8(keyval, s); // convert ucs4 to utf8 char
            g_data.preeditstr->append(s, n);
        }
        // end process result of ukengine

        // commit string: if need
        if (g_data.preeditstr->length() > 0)
        {
            static guint i;
            for (i = 0; i < sizeof(WordBreakSyms); i++)
            {
                if (WordBreakSyms[i] == g_data.preeditstr->at(g_data.preeditstr->length()-1)
                    && WordBreakSyms[i] == keyval)
                {
                    ibus_unikey_engine_commit(engine);
                    return true;
                }
            }
        }
        // end commit string

        ibus_unikey_engine_update_preedit_string(engine, g_data.preeditstr->c_str(), true);
        return true;
    } //end capture printable char

    // non process key
    ibus_unikey_engine_commit(engine);
    return false;
}

void ibus_unikey_engine_clean_buffer(IBusEngine* engine) {
    BLOG_DEBUG("ibus_unikey_engine_clean_buffer");
    UnikeyResetBuf();
    g_data.preeditstr->clear();
    ibus_engine_hide_preedit_text(engine);    
}

void ibus_unikey_engine_commit(IBusEngine* engine) {
    BLOG_DEBUG("ibus_unikey_engine_commit");

    if (g_data.preeditstr->length() > 0)
    {
        IBusText *text;

        text = ibus_text_new_from_static_string(g_data.preeditstr->c_str());
        ibus_engine_commit_text(engine, text);        
    }
    
    ibus_unikey_engine_clean_buffer(engine);        
}
