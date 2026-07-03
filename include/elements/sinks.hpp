#pragma once
#include <cstdint>
#include <string>
#include <string_view>

#include <gst/gst.h>

#include <nonstd/expected.hpp>

#include <elements/detail.hpp>
#include <gstreamer_raii.hpp>
#include <utils/error.hpp>

namespace ds {

class WindowSink {
public:
  [[nodiscard]] static nonstd::expected<WindowSink, ElementError> create(std::string_view name = {}) {
    GstElement* raw = gst_element_factory_make("nveglglessink", name.empty() ? nullptr : std::string(name).c_str());
    if(raw == nullptr) {
      return nonstd::make_unexpected(ElementError{ErrorKind::ElementCreation, "Failed to create 'nveglglessink' element"});
    }
    return WindowSink{gst::raii::Element{raw}};
  }

  WindowSink& sync(bool enable) {
    detail::set_property(mElement.get(), "sync", static_cast<gboolean>(enable));
    return *this;
  }

  [[nodiscard]] GstElement* get() const { return mElement.get(); }

  [[nodiscard]] GstElement* release() { return mElement.release(); }
  operator bool() const { return static_cast<bool>(mElement); }

  WindowSink(WindowSink&&) = default;
  WindowSink& operator=(WindowSink&&) = default;
  WindowSink(const WindowSink&) = delete;
  WindowSink& operator=(const WindowSink&) = delete;

private:
  explicit WindowSink(gst::raii::Element element) : mElement(std::move(element)) {}
  gst::raii::Element mElement;
};

class FileSink {
public:
  [[nodiscard]] static nonstd::expected<FileSink, ElementError> create(std::string_view name = {}) {
    GstElement* raw = gst_element_factory_make("filesink", name.empty() ? nullptr : std::string(name).c_str());
    if(raw == nullptr) {
      return nonstd::make_unexpected(ElementError{ErrorKind::ElementCreation, "Failed to create 'filesink' element"});
    }
    return FileSink{gst::raii::Element{raw}};
  }

  FileSink& location(std::string_view path) {
    detail::set_property(mElement.get(), "location", path);
    return *this;
  }

  FileSink& sync(bool enable) {
    detail::set_property(mElement.get(), "sync", static_cast<gboolean>(enable));
    return *this;
  }

  [[nodiscard]] GstElement* get() const { return mElement.get(); }

  [[nodiscard]] GstElement* release() { return mElement.release(); }
  operator bool() const { return static_cast<bool>(mElement); }

  FileSink(FileSink&&) = default;
  FileSink& operator=(FileSink&&) = default;
  FileSink(const FileSink&) = delete;
  FileSink& operator=(const FileSink&) = delete;

private:
  explicit FileSink(gst::raii::Element element) : mElement(std::move(element)) {}
  gst::raii::Element mElement;
};

}    // namespace ds
