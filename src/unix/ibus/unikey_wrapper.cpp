#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "unikey_wrapper.h"

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

static IBusUnikeyData g_data;


void ibus_unikey_init()
{
    BLOG_DEBUG("ibus_unikey_init");
    UnikeySetup();

    UnikeyOptions ukopt;
    ukopt.spellCheckEnabled     = 1;
    ukopt.autoNonVnRestore      = 1;
    ukopt.modernStyle           = 0;
    ukopt.freeMarking           = 1;
    ukopt.macroEnabled          = 0;
    UnikeySetOptions(&ukopt);

    g_data.im = UkTelex;
    g_data.oc = 12;

}

void ibus_unikey_exit()
{
    BLOG_DEBUG("ibus_unikey_exit");
    UnikeyCleanup();
}


void ibus_unikey_engine_focus_in(IBusEngine* engine)
{
    BLOG_DEBUG("ibus_unikey_engine_focus_in");

    UnikeySetInputMethod(g_data.im);
    UnikeySetOutputCharset(g_data.oc);

    UnikeySetOptions(&g_data.ukopt);
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

    for ( i = g_data.preeditstr.length()-1; i >= 0 && k > 0; i--)
    {
        c = g_data.preeditstr.at(i);

        // count down if byte is begin byte of utf-8 char
        if (c < (guchar)'\x80' || c >= (guchar)'\xC0')
        {
            k--;
        }
    }

    g_data.preeditstr.erase(i+1);
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

        if (UnikeyBackspaces == 0 || g_data.preeditstr.empty())
        {
            return false;
        }
        else
        {
            if (g_data.preeditstr.length() <= (guint)UnikeyBackspaces)
            {
                g_data.preeditstr.clear();
                ibus_engine_hide_preedit_text(engine);
            }
            else
            {
                ibus_unikey_engine_erase_chars(UnikeyBackspaces);
                ibus_unikey_engine_update_preedit_string(engine, g_data.preeditstr.c_str(), true);
            }

            // change tone position after press backspace
            if (UnikeyBufChars > 0)
            {
                if (g_data.oc == CONV_CHARSET_XUTF8)
                {
                    g_data.preeditstr.append((const gchar*)UnikeyBuf, UnikeyBufChars);
                }
                else
                {
                    static unsigned char buf[CONVERT_BUF_SIZE];
                    int bufSize = CONVERT_BUF_SIZE;

                    latinToUtf(buf, UnikeyBuf, UnikeyBufChars, &bufSize);
                    g_data.preeditstr.append((const gchar*)buf, CONVERT_BUF_SIZE - bufSize);
                }

                ibus_unikey_engine_update_preedit_string(engine, g_data.preeditstr.c_str(), true);
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
                g_data.preeditstr.append(keyval==IBUS_w?"w":"W");
                ibus_unikey_engine_update_preedit_string(engine, g_data.preeditstr.c_str(), true);
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
            if (g_data.preeditstr.length() <= (guint)UnikeyBackspaces)
            {
                g_data.preeditstr.clear();
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
                g_data.preeditstr.append((const gchar*)UnikeyBuf, UnikeyBufChars);
            }
            else
            {
                static unsigned char buf[CONVERT_BUF_SIZE];
                int bufSize = CONVERT_BUF_SIZE;

                latinToUtf(buf, UnikeyBuf, UnikeyBufChars, &bufSize);
                g_data.preeditstr.append((const gchar*)buf, CONVERT_BUF_SIZE - bufSize);
            }
        }
        else if (keyval != IBUS_Shift_L && keyval != IBUS_Shift_R) // if ukengine not process
        {
            static int n;
            static char s[6];

            n = g_unichar_to_utf8(keyval, s); // convert ucs4 to utf8 char
            g_data.preeditstr.append(s, n);
        }
        // end process result of ukengine

        // commit string: if need
        if (g_data.preeditstr.length() > 0)
        {
            static guint i;
            for (i = 0; i < sizeof(WordBreakSyms); i++)
            {
                if (WordBreakSyms[i] == g_data.preeditstr.at(g_data.preeditstr.length()-1)
                    && WordBreakSyms[i] == keyval)
                {
                    ibus_unikey_engine_commit(engine);
                    return true;
                }
            }
        }
        // end commit string

        ibus_unikey_engine_update_preedit_string(engine, g_data.preeditstr.c_str(), true);
        return true;
    } //end capture printable char

    // non process key
    ibus_unikey_engine_commit(engine);
    return false;
}

void ibus_unikey_engine_clean_buffer(IBusEngine* engine) {
    BLOG_DEBUG("ibus_unikey_engine_clean_buffer");
    UnikeyResetBuf();
    g_data.preeditstr.clear();
    ibus_engine_hide_preedit_text(engine);    
}

void ibus_unikey_engine_commit(IBusEngine* engine) {
    BLOG_DEBUG("ibus_unikey_engine_commit");

    if (g_data.preeditstr.length() > 0)
    {
        IBusText *text;

        text = ibus_text_new_from_static_string(g_data.preeditstr.c_str());
        ibus_engine_commit_text(engine, text);        
    }
    
    ibus_unikey_engine_clean_buffer(engine);        
}
