#pragma once
#include <cstdint>
#include <string>
#include <string_view>

#include <gst/gst.h>

#include <nonstd/expected.hpp>

#include <gstreamer_raii.hpp>
#include <utils/error.hpp>

namespace ds {

class MsgConv {
public:
  [[nodiscard]] static nonstd::expected<MsgConv, ElementError> create(std::string_view name = {}) {
    GstElement* raw = gst_element_factory_make("nvmsgconv", name.empty() ? nullptr : std::string(name).c_str());
    if(raw == nullptr) {
      return nonstd::make_unexpected(ElementError{ErrorKind::ElementCreation, "Failed to create 'nvmsgconv' element"});
    }
    return MsgConv{gst::raii::Element{raw}};
  }

  MsgConv& config(std::string_view path) {
    g_object_set(G_OBJECT(mElement.get()), "config", std::string(path).c_str(), nullptr);
    return *this;
  }
  MsgConv& sensor_id(std::int32_t id) {
    g_object_set(G_OBJECT(mElement.get()), "sensor-id", static_cast<gint>(id), nullptr);
    return *this;
  }
  MsgConv& payload_type(std::uint32_t type) {
    g_object_set(G_OBJECT(mElement.get()), "payload-type", static_cast<guint>(type), nullptr);
    return *this;
  }
  MsgConv& comp_id(std::uint32_t id) {
    g_object_set(G_OBJECT(mElement.get()), "comp-id", static_cast<guint>(id), nullptr);
    return *this;
  }

  [[nodiscard]] GstElement* get() const { return mElement.get(); }
  [[nodiscard]] GstElement* release() { return mElement.release(); }
  operator bool() const { return static_cast<bool>(mElement); }

  MsgConv(MsgConv&&) = default;
  MsgConv& operator=(MsgConv&&) = default;
  MsgConv(const MsgConv&) = delete;
  MsgConv& operator=(const MsgConv&) = delete;

private:
  explicit MsgConv(gst::raii::Element element) : mElement(std::move(element)) {}
  gst::raii::Element mElement;
};

class MsgBroker {
public:
  [[nodiscard]] static nonstd::expected<MsgBroker, ElementError> create(std::string_view name = {}) {
    GstElement* raw = gst_element_factory_make("nvmsgbroker", name.empty() ? nullptr : std::string(name).c_str());
    if(raw == nullptr) {
      return nonstd::make_unexpected(ElementError{ErrorKind::ElementCreation, "Failed to create 'nvmsgbroker' element"});
    }
    return MsgBroker{gst::raii::Element{raw}};
  }

  MsgBroker& proto_lib(std::string_view path) {
    g_object_set(G_OBJECT(mElement.get()), "proto-lib", std::string(path).c_str(), nullptr);
    return *this;
  }
  MsgBroker& conn_str(std::string_view connection) {
    g_object_set(G_OBJECT(mElement.get()), "conn-str", std::string(connection).c_str(), nullptr);
    return *this;
  }
  MsgBroker& topic(std::string_view t) {
    g_object_set(G_OBJECT(mElement.get()), "topic", std::string(t).c_str(), nullptr);
    return *this;
  }
  MsgBroker& config(std::string_view path) {
    g_object_set(G_OBJECT(mElement.get()), "config", std::string(path).c_str(), nullptr);
    return *this;
  }
  MsgBroker& sync(bool enable) {
    g_object_set(G_OBJECT(mElement.get()), "sync", static_cast<gboolean>(enable), nullptr);
    return *this;
  }
  MsgBroker& async(bool enable) {
    g_object_set(G_OBJECT(mElement.get()), "async", static_cast<gboolean>(enable), nullptr);
    return *this;
  }

  [[nodiscard]] GstElement* get() const { return mElement.get(); }
  [[nodiscard]] GstElement* release() { return mElement.release(); }
  operator bool() const { return static_cast<bool>(mElement); }

  MsgBroker(MsgBroker&&) = default;
  MsgBroker& operator=(MsgBroker&&) = default;
  MsgBroker(const MsgBroker&) = delete;
  MsgBroker& operator=(const MsgBroker&) = delete;

private:
  explicit MsgBroker(gst::raii::Element element) : mElement(std::move(element)) {}
  gst::raii::Element mElement;
};

}    // namespace ds
