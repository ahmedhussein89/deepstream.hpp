#pragma once
#include <cstdint>
#include <string>
#include <string_view>

#include <gst/gst.h>

#include <nonstd/expected.hpp>

#include <gstreamer.hpp>
#include <utils/error.hpp>

namespace ds {

class WindowSink {
public:
  [[nodiscard]] static nonstd::expected<WindowSink, ElementError> create(std::string_view name = {}) {
    GstElement* raw = gst_element_factory_make("nveglglessink", name.empty() ? nullptr : std::string(name).c_str());
    if(raw == nullptr) {
      return nonstd::make_unexpected(ElementError{ErrorKind::ElementCreation, "Failed to create 'nveglglessink' element"});
    }
    return WindowSink{gst::Element{raw}};
  }

  WindowSink& sync(bool enable) {
    g_object_set(G_OBJECT(mElement.get()), "sync", static_cast<gboolean>(enable), nullptr);
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
  explicit WindowSink(gst::Element element) : mElement(std::move(element)) {}
  gst::Element mElement;
};

class FileSink {
public:
  [[nodiscard]] static nonstd::expected<FileSink, ElementError> create(std::string_view name = {}) {
    GstElement* raw = gst_element_factory_make("filesink", name.empty() ? nullptr : std::string(name).c_str());
    if(raw == nullptr) {
      return nonstd::make_unexpected(ElementError{ErrorKind::ElementCreation, "Failed to create 'filesink' element"});
    }
    return FileSink{gst::Element{raw}};
  }

  FileSink& location(std::string_view path) {
    g_object_set(G_OBJECT(mElement.get()), "location", std::string(path).c_str(), nullptr);
    return *this;
  }

  FileSink& sync(bool enable) {
    g_object_set(G_OBJECT(mElement.get()), "sync", static_cast<gboolean>(enable), nullptr);
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
  explicit FileSink(gst::Element element) : mElement(std::move(element)) {}
  gst::Element mElement;
};

}    // namespace ds
