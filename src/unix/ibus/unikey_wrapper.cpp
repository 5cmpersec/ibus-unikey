// TODO: Refactor this file

#include "unikey_wrapper.h"

#include <libintl.h>
#include <ibus.h>

#include "unix/ibus/utils.h"

#include "third_party/libunikey/unikey.h"
#include "third_party/libunikey/vnconv.h"

#include "base/logging.h"

#define _(string) gettext(string)

namespace {

const int kBufferSize = 1024;

unsigned char kWordBreakSyms[] =
    {
        ',', ';', ':', '.', '\"', '\'', '!', '?', ' ',
        '<', '>', '=', '+', '-', '*', '/', '\\',
        '_', '~', '`', '@', '#', '$', '%', '^', '&', '(', ')', '{', '}', '[', ']',
        '|'
    };

} // namespace

void UnikeyWrapper::SetUp() {
    BLOG_DEBUG("UnikeyWrapper::SetUp");
    UnikeySetup();

    options_.spellCheckEnabled     = 1;
    options_.autoNonVnRestore      = 1;
    options_.modernStyle           = 0;
    options_.freeMarking           = 1;
    options_.macroEnabled          = 0;
    UnikeySetOptions(&options_);

    input_method_ = UkTelex;
    output_charset_ = 12;
}

void UnikeyWrapper::CleanUp() {
    BLOG_DEBUG("UnikeyWrapper::CleanUp");
    UnikeyCleanup();
}

void UnikeyWrapper::Reset(IBusEngine* engine) {
    BLOG_DEBUG("UnikeyWrapper::Reset");
    CleanBuffer(engine);
}

void UnikeyWrapper::SetInputMethod(InputMethod new_method) {
    BLOG_DEBUG("UnikeyWrapper::SetInputMethod: {}", (int)new_method);
    switch(new_method) {
        case InputMethod::TELEX:
            input_method_ = UkTelex;
            break;
        case InputMethod::VNI:
            input_method_ = UkVni;
            break;
        case InputMethod::STELEX:
            input_method_ = UkSimpleTelex;
            break;
        case InputMethod::STELEX2:
            input_method_ = UkSimpleTelex2;
            break;
        default:
            break;
    }
    UnikeySetInputMethod(input_method_);
}

void UnikeyWrapper::SetOutputCharset(OutputCharset new_charset) {
    BLOG_DEBUG("UnikeyWrapper::SetOutputCharset: {}", (int)new_charset);
    switch(new_charset) {
        case OutputCharset::UNICODE:
            output_charset_ = CONV_CHARSET_XUTF8;
            break;
        case OutputCharset::TCVN3:
            output_charset_ = CONV_CHARSET_TCVN3;
            break;
        case OutputCharset::VNI_WIN:
            output_charset_ = CONV_CHARSET_VNIWIN;
            break;
        case OutputCharset::VIQR:
            output_charset_ = CONV_CHARSET_VIQR;
            break;
        case OutputCharset::BK_HCM2:
            output_charset_ = CONV_CHARSET_BKHCM2;
            break;
        case OutputCharset::CSTRING:
            output_charset_ = CONV_CHARSET_UNI_CSTRING;
            break;
        case OutputCharset::NCR_DECIMAL:
            output_charset_ = CONV_CHARSET_UNIREF;
            break;
        case OutputCharset::NCR_HEX:
            output_charset_ = CONV_CHARSET_UNIREF_HEX;
            break;
        default:
            break;
    }
    UnikeySetOutputCharset(output_charset_);
}

void UnikeyWrapper::CleanBuffer(IBusEngine* engine) {
    BLOG_DEBUG("UnikeyWrapper::CleanBuffer");
    UnikeyResetBuf();
    buffer_.clear();
    ibus_engine_hide_preedit_text(engine);
}

void UnikeyWrapper::CommitPreedit(IBusEngine* engine) {
    BLOG_DEBUG("UnikeyWrapper::CommitPreedit");

    if (buffer_.length() > 0) {
        IBusText *text;

        text = ibus_text_new_from_static_string(buffer_.c_str());
        ibus_engine_commit_text(engine, text);
    }

    CleanBuffer(engine);  
}

void UnikeyWrapper::UpdatePreedit(IBusEngine* engine,
                                  const gchar *string,
                                  gboolean visible) {
    BLOG_DEBUG("UnikeyWrapper::UpdatePreedit");
    IBusText *text;

    text = ibus_text_new_from_static_string(string);

    // underline text
    ibus_text_append_attribute(text,
                               IBUS_ATTR_TYPE_UNDERLINE,
                               IBUS_ATTR_UNDERLINE_SINGLE,
                               0,
                               -1);

    // update and display text
    ibus_engine_update_preedit_text_with_mode(engine,
                                              text,
                                              ibus_text_get_length(text),
                                              visible,
                                              IBUS_ENGINE_PREEDIT_COMMIT);
}

gboolean UnikeyWrapper::ProcessKeyEvent(IBusEngine* engine,
                                        guint keyval,
                                        guint keycode,
                                        guint modifiers) {
    BLOG_DEBUG("UnikeyWrapper::ProcessKeyEvent");

    gboolean tmp = ProcessKeyEventPreedit(engine, keyval, keycode, modifiers);

    // check last keyevent with shift
    if (keyval >= IBUS_space && keyval <=IBUS_asciitilde)
    {
        last_key_with_shift_ = modifiers & IBUS_SHIFT_MASK;
    }
    else
    {
        last_key_with_shift_ = false;
    } // end check last keyevent with shift

    return tmp;
}


gboolean UnikeyWrapper::ProcessKeyEventPreedit(IBusEngine* engine,
                                           guint keyval,
                                           guint keycode,
                                           guint modifiers) {
    BLOG_DEBUG("UnikeyWrapper::ProcessKeyEventPreedit");
    if (modifiers & IBUS_RELEASE_MASK) {
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
        CommitPreedit(engine);
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
        CommitPreedit(engine);
        return false;
    }

    // capture BackSpace
    else if (keyval == IBUS_BackSpace)
    {
        UnikeyBackspacePress();

        if (UnikeyBackspaces == 0 || buffer_.empty())
        {
            return false;
        }
        else
        {
            if (buffer_.length() <= (guint)UnikeyBackspaces)
            {
                buffer_.clear();
                ibus_engine_hide_preedit_text(engine);
            }
            else
            {
                utils::EraseCharsUtf8(buffer_, UnikeyBackspaces);
                UpdatePreedit(engine, buffer_.c_str(), true);
            }

            // change tone position after press backspace
            if (UnikeyBufChars > 0)
            {
                if (output_charset_ == CONV_CHARSET_XUTF8)
                {
                    buffer_.append((const gchar*)UnikeyBuf, UnikeyBufChars);
                }
                else
                {
                    static unsigned char buf[kBufferSize];
                    int bufSize = kBufferSize;

                    utils::LatinToUtf(buf, UnikeyBuf, UnikeyBufChars, &bufSize);
                    buffer_.append((const gchar*)buf, kBufferSize - bufSize);
                }

                UpdatePreedit(engine, buffer_.c_str(), true);
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

        if ((input_method_ == UkTelex || input_method_ == UkSimpleTelex2)
            && process_w_at_begin_ == false
            && UnikeyAtWordBeginning()
            && (keyval == IBUS_w || keyval == IBUS_W))
        {
            UnikeyPutChar(keyval);
            if (options_.macroEnabled == 0)
            {
                return false;
            }
            else
            {
                buffer_.append(keyval==IBUS_w?"w":"W");
                UpdatePreedit(engine, buffer_.c_str(), true);
                return true;
            }
        }

        // shift + space, shift + shift event
        if ((last_key_with_shift_ == false && modifiers & IBUS_SHIFT_MASK
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
            if (buffer_.length() <= (guint)UnikeyBackspaces)
            {
                buffer_.clear();
            }
            else
            {
                utils::EraseCharsUtf8(buffer_, UnikeyBackspaces);
            }
        }

        if (UnikeyBufChars > 0)
        {
            if (output_charset_ == CONV_CHARSET_XUTF8)
            {
                buffer_.append((const gchar*)UnikeyBuf, UnikeyBufChars);
            }
            else
            {
                static unsigned char buf[kBufferSize];
                int bufSize = kBufferSize;

                utils::LatinToUtf(buf, UnikeyBuf, UnikeyBufChars, &bufSize);
                buffer_.append((const gchar*)buf, kBufferSize - bufSize);
            }
        }
        else if (keyval != IBUS_Shift_L && keyval != IBUS_Shift_R) // if ukengine not process
        {
            static int n;
            static char s[6];

            n = g_unichar_to_utf8(keyval, s); // convert ucs4 to utf8 char
            buffer_.append(s, n);
        }
        // end process result of ukengine

        // commit string: if need
        if (buffer_.length() > 0)
        {
            static guint i;
            for (i = 0; i < sizeof(kWordBreakSyms); i++)
            {
                if (kWordBreakSyms[i] == buffer_.at(buffer_.length()-1)
                    && kWordBreakSyms[i] == keyval)
                {
                    CommitPreedit(engine);
                    return true;
                }
            }
        }
        // end commit string

        UpdatePreedit(engine, buffer_.c_str(), true);
        return true;
    } //end capture printable char

    // non process key
    CommitPreedit(engine);
    return false;
}
