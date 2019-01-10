#pragma once

#include <ibus.h>

#if !IBUS_CHECK_VERSION(1, 4, 1)
#error "ibus-unikey now requires IBus>=1.4.1"
#endif  // libibus (<1.4.1)

#if IBUS_CHECK_VERSION(1, 5, 4)
#if !defined(UNIKEY_ENABLE_IBUS_INPUT_PURPOSE)
#define UNIKEY_ENABLE_IBUS_INPUT_PURPOSE
#endif  // !UNIKEY_ENABLE_IBUS_INPUT_PURPOSE
#endif  // libibus (>=1.5.4)

#if IBUS_CHECK_VERSION(1, 5, 0)
#define UNIKEY_IBUS_HAS_SYMBOL
#endif  // IBus >= 1.5