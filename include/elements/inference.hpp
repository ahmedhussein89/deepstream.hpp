#pragma once
#include <cstdint>
#include <string>
#include <string_view>

#include <gst/gst.h>

#include <nonstd/expected.hpp>

#include <gstreamer_raii.hpp>
#include <utils/error.hpp>

namespace ds {

class PrimaryInfer {
public:
  [[nodiscard]] static nonstd::expected<PrimaryInfer, ElementError> create(std::string_view name = {}) {
    GstElement* raw = gst_element_factory_make("nvinfer", name.empty() ? nullptr : std::string(name).c_str());
    if(raw == nullptr) {
      return nonstd::make_unexpected(ElementError{ErrorKind::ElementCreation, "Failed to create 'nvinfer' element"});
    }
    g_object_set(G_OBJECT(raw), "process-mode", 1, nullptr);
    return PrimaryInfer{gst::raii::Element{raw}};
  }

  PrimaryInfer& config_file(std::string_view path) {
    g_object_set(G_OBJECT(mElement.get()), "config-file-path", std::string(path).c_str(), nullptr);
    return *this;
  }

  PrimaryInfer& batch_size(std::uint32_t size) {
    g_object_set(G_OBJECT(mElement.get()), "batch-size", static_cast<guint>(size), nullptr);
    return *this;
  }

  PrimaryInfer& unique_id(std::uint32_t id) {
    g_object_set(G_OBJECT(mElement.get()), "unique-id", static_cast<guint>(id), nullptr);
    return *this;
  }

  [[nodiscard]] GstElement* get() const { return mElement.get(); }

  [[nodiscard]] GstElement* release() { return mElement.release(); }
  operator bool() const { return static_cast<bool>(mElement); }

  PrimaryInfer(PrimaryInfer&&) = default;
  PrimaryInfer& operator=(PrimaryInfer&&) = default;
  PrimaryInfer(const PrimaryInfer&) = delete;
  PrimaryInfer& operator=(const PrimaryInfer&) = delete;

private:
  explicit PrimaryInfer(gst::raii::Element element) : mElement(std::move(element)) {}
  gst::raii::Element mElement;
};

class SecondaryInfer {
public:
  [[nodiscard]] static nonstd::expected<SecondaryInfer, ElementError> create(std::string_view name = {}) {
    GstElement* raw = gst_element_factory_make("nvinfer", name.empty() ? nullptr : std::string(name).c_str());
    if(raw == nullptr) {
      return nonstd::make_unexpected(ElementError{ErrorKind::ElementCreation, "Failed to create 'nvinfer' element"});
    }
    g_object_set(G_OBJECT(raw), "process-mode", 2, nullptr);
    return SecondaryInfer{gst::raii::Element{raw}};
  }

  SecondaryInfer& config_file(std::string_view path) {
    g_object_set(G_OBJECT(mElement.get()), "config-file-path", std::string(path).c_str(), nullptr);
    return *this;
  }

  SecondaryInfer& batch_size(std::uint32_t size) {
    g_object_set(G_OBJECT(mElement.get()), "batch-size", static_cast<guint>(size), nullptr);
    return *this;
  }

  SecondaryInfer& unique_id(std::uint32_t id) {
    g_object_set(G_OBJECT(mElement.get()), "unique-id", static_cast<guint>(id), nullptr);
    return *this;
  }

  SecondaryInfer& infer_on_id(std::uint32_t gie_id) {
    g_object_set(G_OBJECT(mElement.get()), "infer-on-gie-id", static_cast<gint>(gie_id), nullptr);
    return *this;
  }

  [[nodiscard]] GstElement* get() const { return mElement.get(); }

  [[nodiscard]] GstElement* release() { return mElement.release(); }
  operator bool() const { return static_cast<bool>(mElement); }

  SecondaryInfer(SecondaryInfer&&) = default;
  SecondaryInfer& operator=(SecondaryInfer&&) = default;
  SecondaryInfer(const SecondaryInfer&) = delete;
  SecondaryInfer& operator=(const SecondaryInfer&) = delete;

private:
  explicit SecondaryInfer(gst::raii::Element element) : mElement(std::move(element)) {}
  gst::raii::Element mElement;
};

}    // namespace ds
