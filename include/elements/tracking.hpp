#pragma once
#include <cstdint>
#include <string>
#include <string_view>

#include <gst/gst.h>

#include <nonstd/expected.hpp>

#include <gstreamer_raii.hpp>
#include <utils/error.hpp>

namespace ds {

class Tracker {
public:
  [[nodiscard]] static nonstd::expected<Tracker, ElementError> create(std::string_view name = {}) {
    GstElement* raw = gst_element_factory_make("nvtracker", name.empty() ? nullptr : std::string(name).c_str());
    if(raw == nullptr) {
      return nonstd::make_unexpected(ElementError{ErrorKind::ElementCreation, "Failed to create 'nvtracker' element"});
    }
    return Tracker{gst::raii::Element{raw}};
  }

  Tracker& lib_file(std::string_view path) {
    g_object_set(G_OBJECT(mElement.get()), "ll-lib-file", std::string(path).c_str(), nullptr);
    return *this;
  }

  Tracker& config_file(std::string_view path) {
    g_object_set(G_OBJECT(mElement.get()), "ll-config-file", std::string(path).c_str(), nullptr);
    return *this;
  }

  Tracker& tracker_width(std::uint32_t w) {
    g_object_set(G_OBJECT(mElement.get()), "tracker-width", static_cast<guint>(w), nullptr);
    return *this;
  }

  Tracker& tracker_height(std::uint32_t h) {
    g_object_set(G_OBJECT(mElement.get()), "tracker-height", static_cast<guint>(h), nullptr);
    return *this;
  }

  [[nodiscard]] GstElement* get() const { return mElement.get(); }

  [[nodiscard]] GstElement* release() { return mElement.release(); }
  operator bool() const { return static_cast<bool>(mElement); }

  Tracker(Tracker&&) = default;
  Tracker& operator=(Tracker&&) = default;
  Tracker(const Tracker&) = delete;
  Tracker& operator=(const Tracker&) = delete;

private:
  explicit Tracker(gst::raii::Element element) : mElement(std::move(element)) {}
  gst::raii::Element mElement;
};

}    // namespace ds
