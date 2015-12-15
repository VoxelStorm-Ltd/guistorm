#ifndef GUISTORM_H_INCLUDED
#define GUISTORM_H_INCLUDED

/// Extensible, fast, customisable window and widget display toolkit version 2.0
/// by Eugene Hopkinson (SlowRiot) for VoxelStorm 2014
///
/// Defines: GUISTORM_AVOIDQUADS - use triangles instead of deprecated GL_QUADS primitives even if quads are cheaper
///          GUISTORM_UNBIND - unbind shader and buffers after rendering
///          GUISTORM_UNSAFEUTF - do not check UTF8 input for validity when iterating; this assumes you guarantee all strings are safe
///          GUISTORM_LOAD_MISSING_GLYPHS - add any new characters we encounter dynamically to the texture atlas (can be costly at runtime)
///          GUISTORM_ROUND_NEAREST_OUT - round screen positions and sizes to the nearest pixel when transforming to screen space
///          GUISTORM_ROUND_NEAREST_ALL - round all element screen positions and sizes to the nearest pixel at all stages
///          DEBUG_GUISTORM - draw outlines of hidden gui elements and show advanced debugging messages

/// Convenience wrapper header to include all top level types

#include "gui.h"

#include "button.h"
#include "graph_line.h"
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
