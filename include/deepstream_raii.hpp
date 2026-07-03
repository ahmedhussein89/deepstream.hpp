#pragma once
// Layer 2: owning ds:: wrappers (typed element factories + pipeline builder).
// Mirrors gstreamer_raii.hpp — include this to create and own DeepStream
// pipeline elements. Transitively includes deepstream.hpp (layer 1).
#include <deepstream.hpp>
#include <builder.hpp>
#include <elements.hpp>
