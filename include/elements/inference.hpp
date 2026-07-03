#pragma once
#include <cstdint>
#include <string>
#include <string_view>

#include <gst/gst.h>

#include <nonstd/expected.hpp>

#include <gstreamer_raii.hpp>
#include <utils/error.hpp>

namespace ds {

struct NvInferConfig {
  std::string   config_file_path;
  std::uint32_t batch_size{1};
  std::uint32_t unique_id{1};
};

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

  [[nodiscard]] static nonstd::expected<PrimaryInfer, ElementError> create(const NvInferConfig& cfg, std::string_view name = {}) {
    auto result = create(name);
    if(!result) {
      return result;
    }
    if(!cfg.config_file_path.empty()) {
      result->config_file(cfg.config_file_path);
    }
    result->batch_size(cfg.batch_size).unique_id(cfg.unique_id);
    return result;
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

  [[nodiscard]] static nonstd::expected<SecondaryInfer, ElementError> create(const NvInferConfig& cfg, std::string_view name = {}) {
    auto result = create(name);
    if(!result) {
      return result;
    }
    if(!cfg.config_file_path.empty()) {
      result->config_file(cfg.config_file_path);
    }
    result->batch_size(cfg.batch_size).unique_id(cfg.unique_id);
    return result;
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

enum class InferMode : std::uint32_t { Primary = 1, Secondary = 2 };

class InferServer {
public:
  [[nodiscard]] static nonstd::expected<InferServer, ElementError> create(InferMode mode = InferMode::Primary,
                                                                          std::string_view name = {}) {
    GstElement* raw = gst_element_factory_make("nvinferserver", name.empty() ? nullptr : std::string(name).c_str());
    if(raw == nullptr) {
      return nonstd::make_unexpected(ElementError{ErrorKind::ElementCreation, "Failed to create 'nvinferserver' element"});
    }
    g_object_set(G_OBJECT(raw), "process-mode", static_cast<gint>(mode), nullptr);
    return InferServer{gst::raii::Element{raw}};
  }

  InferServer& config_file(std::string_view path) {
    g_object_set(G_OBJECT(mElement.get()), "config-file-path", std::string(path).c_str(), nullptr);
    return *this;
  }
  InferServer& batch_size(std::uint32_t size) {
    g_object_set(G_OBJECT(mElement.get()), "batch-size", static_cast<guint>(size), nullptr);
    return *this;
  }
  InferServer& unique_id(std::uint32_t id) {
    g_object_set(G_OBJECT(mElement.get()), "unique-id", static_cast<guint>(id), nullptr);
    return *this;
  }
  InferServer& infer_on_id(std::uint32_t gie_id) {
    g_object_set(G_OBJECT(mElement.get()), "infer-on-gie-id", static_cast<gint>(gie_id), nullptr);
    return *this;
  }

  [[nodiscard]] GstElement* get() const { return mElement.get(); }
  [[nodiscard]] GstElement* release() { return mElement.release(); }
  operator bool() const { return static_cast<bool>(mElement); }

  InferServer(InferServer&&) = default;
  InferServer& operator=(InferServer&&) = default;
  InferServer(const InferServer&) = delete;
  InferServer& operator=(const InferServer&) = delete;

private:
  explicit InferServer(gst::raii::Element element) : mElement(std::move(element)) {}
  gst::raii::Element mElement;
};

struct PreprocessConfig {
  std::string   config_file;
  std::uint32_t gpu_id{0};
};

class Preprocess {
public:
  [[nodiscard]] static nonstd::expected<Preprocess, ElementError> create(std::string_view name = {}) {
    GstElement* raw = gst_element_factory_make("nvdspreprocess", name.empty() ? nullptr : std::string(name).c_str());
    if(raw == nullptr) {
      return nonstd::make_unexpected(ElementError{ErrorKind::ElementCreation, "Failed to create 'nvdspreprocess' element"});
    }
    return Preprocess{gst::raii::Element{raw}};
  }

  [[nodiscard]] static nonstd::expected<Preprocess, ElementError> create(const PreprocessConfig& cfg,
                                                                         std::string_view name = {}) {
    auto result = create(name);
    if(!result) {
      return result;
    }
    if(!cfg.config_file.empty()) {
      result->config_file(cfg.config_file);
    }
    result->gpu_id(cfg.gpu_id);
    return result;
  }

  Preprocess& config_file(std::string_view path) {
    g_object_set(G_OBJECT(mElement.get()), "config-file", std::string(path).c_str(), nullptr);
    return *this;
  }
  Preprocess& gpu_id(std::uint32_t id) {
    g_object_set(G_OBJECT(mElement.get()), "gpu-id", static_cast<guint>(id), nullptr);
    return *this;
  }

  [[nodiscard]] GstElement* get() const { return mElement.get(); }
  [[nodiscard]] GstElement* release() { return mElement.release(); }
  operator bool() const { return static_cast<bool>(mElement); }

  Preprocess(Preprocess&&) = default;
  Preprocess& operator=(Preprocess&&) = default;
  Preprocess(const Preprocess&) = delete;
  Preprocess& operator=(const Preprocess&) = delete;

private:
  explicit Preprocess(gst::raii::Element element) : mElement(std::move(element)) {}
  gst::raii::Element mElement;
};

struct AnalyticsConfig {
  std::string config_file;
};

class Analytics {
public:
  [[nodiscard]] static nonstd::expected<Analytics, ElementError> create(std::string_view name = {}) {
    GstElement* raw = gst_element_factory_make("nvdsanalytics", name.empty() ? nullptr : std::string(name).c_str());
    if(raw == nullptr) {
      return nonstd::make_unexpected(ElementError{ErrorKind::ElementCreation, "Failed to create 'nvdsanalytics' element"});
    }
    return Analytics{gst::raii::Element{raw}};
  }

  [[nodiscard]] static nonstd::expected<Analytics, ElementError> create(const AnalyticsConfig& cfg,
                                                                        std::string_view name = {}) {
    auto result = create(name);
    if(!result) {
      return result;
    }
    if(!cfg.config_file.empty()) {
      result->config_file(cfg.config_file);
    }
    return result;
  }

  Analytics& config_file(std::string_view path) {
    g_object_set(G_OBJECT(mElement.get()), "config-file", std::string(path).c_str(), nullptr);
    return *this;
  }

  [[nodiscard]] GstElement* get() const { return mElement.get(); }
  [[nodiscard]] GstElement* release() { return mElement.release(); }
  operator bool() const { return static_cast<bool>(mElement); }

  Analytics(Analytics&&) = default;
  Analytics& operator=(Analytics&&) = default;
  Analytics(const Analytics&) = delete;
  Analytics& operator=(const Analytics&) = delete;

private:
  explicit Analytics(gst::raii::Element element) : mElement(std::move(element)) {}
  gst::raii::Element mElement;
};

}    // namespace ds
