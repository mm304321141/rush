#pragma once

#ifdef _MSC_VER
#include "cpp/xf.hpp"
#endif

#ifdef __GNUC__
#include "cpp/xf.hpp"
#endif

#ifdef _RJIT
#ifdef _RS64
#include "rs64/xf.rs"
#else
#include "rs/xf.rs"
#endif
#endif

#ifdef _RNASM
#ifdef _RS64
#include "rs64/xf.rs"
#else
#include "rs/xf.rs"
#endif
#endif

#ifdef _RVM
#include "rs/xf.rs"
#endif

#ifdef _RGPP
#include "gpp/xf.rs"
#endif
