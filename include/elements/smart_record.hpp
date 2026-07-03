#pragma once
#include <cstdint>
#include <string>
#include <string_view>

#include <glib.h>
#include <gst/gst.h>

namespace ds {

// Helper for triggering Smart Record on an nvurisrcbin element via signals.
// Smart Record must first be enabled on the source via UriSource::smart_record().
struct SmartRecord {
  enum class Mode : std::uint32_t { Off = 0, Cloud = 1, Multi = 2 };
  enum class Container : std::uint32_t { MP4 = 0, MKV = 1 };

  // Send "start-sr" signal to begin recording for source_id/session_id.
  // dir and prefix override the element's smart-rec-dir-path / smart-rec-file-prefix for this session.
  // timeout_sec=0 means use the element's default duration.
  static void start(GstElement* urisrcbin, std::uint32_t source_id, std::uint32_t session_id,
                    std::string_view dir = {}, std::string_view prefix = {}, std::uint32_t timeout_sec = 0) {
    const std::string dir_str(dir);
    const std::string prefix_str(prefix);
    const char*       dir_c    = dir.empty() ? nullptr : dir_str.c_str();
    const char*       prefix_c = prefix.empty() ? nullptr : prefix_str.c_str();
    gpointer    ret      = nullptr;
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg, hicpp-vararg)
    g_signal_emit_by_name(G_OBJECT(urisrcbin), "start-sr", static_cast<guint>(source_id),
                          static_cast<guint>(session_id), dir_c, prefix_c, static_cast<guint>(timeout_sec),
                          nullptr, &ret);
  }

  // Send "stop-sr" signal to stop recording for source_id/session_id.
  static void stop(GstElement* urisrcbin, std::uint32_t source_id, std::uint32_t session_id) {
    gpointer ret = nullptr;
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg, hicpp-vararg)
    g_signal_emit_by_name(G_OBJECT(urisrcbin), "stop-sr", static_cast<guint>(source_id),
                          static_cast<guint>(session_id), nullptr, &ret);
  }

  SmartRecord() = delete;
};

}    // namespace ds
