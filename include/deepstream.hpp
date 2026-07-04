#pragma once
// Layer 1: non-owning ds:: wrappers (metadata views).
// Mirrors gstreamer.hpp — include this when you only need to read NvDs
// metadata without creating or owning any DeepStream pipeline elements.
//
// Requires DeepStream SDK headers (DS_HAS_DEEPSTREAM is defined by the
// ds::hpp CMake target when the SDK is present).
#ifdef DS_HAS_DEEPSTREAM
#  include <metadata.hpp>
#endif
