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
    detail::set_property(mElement.get(), "config", path);
    return *this;
  }
  MsgConv& sensor_id(std::int32_t id) {
    detail::set_property(mElement.get(), "sensor-id", static_cast<gint>(id));
    return *this;
  }
  MsgConv& payload_type(std::uint32_t type) {
    detail::set_property(mElement.get(), "payload-type", static_cast<guint>(type));
    return *this;
  }
  MsgConv& comp_id(std::uint32_t id) {
    detail::set_property(mElement.get(), "comp-id", static_cast<guint>(id));
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
    detail::set_property(mElement.get(), "proto-lib", path);
    return *this;
  }
  MsgBroker& conn_str(std::string_view connection) {
    detail::set_property(mElement.get(), "conn-str", connection);
    return *this;
  }
  MsgBroker& topic(std::string_view topic_name) {
    detail::set_property(mElement.get(), "topic", topic_name);
    return *this;
  }
  MsgBroker& config(std::string_view path) {
    detail::set_property(mElement.get(), "config", path);
    return *this;
  }
  MsgBroker& sync(bool enable) {
    detail::set_property(mElement.get(), "sync", static_cast<gboolean>(enable));
    return *this;
  }
  MsgBroker& async(bool enable) {
    detail::set_property(mElement.get(), "async", static_cast<gboolean>(enable));
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
