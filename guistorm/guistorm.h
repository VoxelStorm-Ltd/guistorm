#ifndef GUISTORM_H_INCLUDED
#define GUISTORM_H_INCLUDED

/// Extensible, fast, customisable window and widget display toolkit version 2.0
/// by Eugene Hopkinson (SlowRiot) for VoxelStorm 2014
///
/// Defines: GUISTORM_AVOIDQUADS - use triangles instead of deprecated GL_QUADS primitives even if quads are cheaper
///          GUISTORM_SINGLETHREADED - omit all thread safety features
///          GUISTORM_UNBIND - unbind shader and buffers after rendering
///          GUISTORM_NO_UTF - do not use utf8 and utf32 at all, limit all characters to ascii
///          GUISTORM_UNSAFEUTF - do not check UTF8 input for validity when iterating; this assumes you guarantee all strings are safe
///          GUISTORM_LOAD_MISSING_GLYPHS - add any new characters we encounter dynamically to the texture atlas (can be costly at runtime)
///          GUISTORM_NO_TEXT - do not enable any text rendering components at all; removes all dependencies on freetype
///          GUISTORM_ROUND_NEAREST_OUT - round screen positions and sizes to the nearest pixel when transforming to screen space
///          GUISTORM_ROUND_NEAREST_ALL - round all element screen positions and sizes to the nearest pixel at all stages
///            GUISTORM_ROUND_NEARBYINT - when rounding use std::nearbyint
///            GUISTORM_ROUND_RINT - when rounding use std::rint
///            GUISTORM_ROUND_ROUND - when rounding use std::round
///            GUISTORM_ROUND_CAST - when rounding use cast back and forth
///            GUISTORM_ROUND_REAL2INT - when rounding use a custom magic-number based function
///            GUISTORM_ROUND_FISTP - when rounding use the fistp assembly instruction
///            GUISTORM_ROUND_FISTP2 - when rounding use an alternative fistp assembly implementation
///            GUISTORM_ROUND_STOREINT - instead of rounding, just store all coordinates in ints (may require -Wno-narrowing)
///          DEBUG_GUISTORM - draw outlines of hidden gui elements and show advanced debugging messages

/// Convenience wrapper header to include all top level types

#include "gui.h"

#include "button.h"
#include "graph_line.h"
#include "graph_ringbuffer_line.h"
#include "group.h"
#include "input_text.h"
#include "label.h"
#include "line.h"
#include "lineshape.h"
#include "progressbar.h"
#include "widget.h"
#include "window.h"

#include "colourgroup.h"
#include "colourset.h"
#include "font.h"
#include "types.h"

#endif // GUISTORM_H_INCLUDED
