#pragma once
#include <concepts>
#include <string>
#include <string_view>
#include <type_traits>

#include <gst/gst.h>

namespace ds::detail {

// GObjectScalar<T>: T is a scalar (numeric, pointer, or enum) type that can
// be passed directly through g_object_set's variadic argument list.
template <typename T>
concept GObjectScalar = std::is_scalar_v<T>;

// Typed single-property setter — consolidates all g_object_set calls into one
// place so the unavoidable C vararg call needs only one NOLINT annotation.
template <GObjectScalar T>
void set_property(GstElement* elem, const char* prop, T value) noexcept {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg, hicpp-vararg)
  g_object_set(G_OBJECT(elem), prop, value, nullptr);
}

inline void set_property(GstElement* elem, const char* prop, std::string_view value) noexcept {
  const std::string val{value};
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg, hicpp-vararg)
  g_object_set(G_OBJECT(elem), prop, val.c_str(), nullptr);
}

}  // namespace ds::detail
