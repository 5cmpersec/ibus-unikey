#pragma once

#include <string>
#include <ibus.h>

#include "base/port.h"
#include "third_party/libunikey/unikey.h"
#include "third_party/libunikey/vnconv.h"


class UnikeyWrapper {

public:
    UnikeyWrapper() {}
    virtual ~UnikeyWrapper() {}

    void SetUp();
    void CleanUp();
    void Reset(IBusEngine* engine);

    gboolean ProcessKeyEvent(IBusEngine* engine,
                             guint keyval,
                             guint keycode,
                             guint modifiers);
private:
    void CleanBuffer(IBusEngine* engine);
    void UpdatePreedit(IBusEngine* engine,
                       const gchar *string,
                       gboolean visible);
    void CommitPreedit(IBusEngine* engine);
    gboolean ProcessKeyEventPreedit(IBusEngine* engine,
                                    guint keyval,
                                    guint keycode,
                                    guint modifiers);

private:
    std::string buffer_;
    UkInputMethod input_method_;
    unsigned int output_charset_;
    UnikeyOptions options_;
    gboolean process_w_at_begin_;
    gboolean last_key_with_shift_;

    DISALLOW_COPY_AND_ASSIGN(UnikeyWrapper);
};