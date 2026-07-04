#pragma once
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <string_view>

#include <fmt/format.h>

#include <spdlog/spdlog.h>
#include <utils/error.hpp>

namespace ds {

enum class DebugLevel : int {
  Debug = 0,
  Info = 1,
  Warn = 2,
  Error = 3,
  Off = 4,    // suppress all output
};

[[nodiscard]] inline std::string_view debug_level_str(DebugLevel lvl) noexcept {
  switch(lvl) {
  case DebugLevel::Debug:
    return "DEBUG";
  case DebugLevel::Info:
    return "INFO";
  case DebugLevel::Warn:
    return "WARN";
  case DebugLevel::Error:
    return "ERROR";
  case DebugLevel::Off:
    return "OFF";
  }
  return "?";
}

// Structured diagnostic message delivered to the validation callback and/or spdlog.
struct DebugMessage {
  DebugLevel level{DebugLevel::Debug};
  ErrorKind kind{ErrorKind::Unknown};
  std::string message;
  std::string file;
  int line{0};

  [[nodiscard]] std::string_view level_str() const noexcept {
    return debug_level_str(level);
  }
  [[nodiscard]] std::string_view kind_str() const noexcept {
    return error_kind_str(kind);
  }
};

// Vulkan-style validation / debug layer.
//
// Lifecycle:
//   ds::DebugLayer::instance().set_callback([](const ds::DebugMessage& m) {
//       std::cerr << "[" << m.level_str() << "] " << m.message << "\n";
//   });
//
// Optionally attach a spdlog logger:
//   ds::DebugLayer::instance().set_logger(spdlog::stdout_logger_mt("ds"));
//
// Minimum level filter (default: Debug — all messages pass):
//   ds::DebugLayer::instance().set_min_level(ds::DebugLevel::Warn);
class DebugLayer {
public:
  using Callback = std::function<void(const DebugMessage&)>;

  [[nodiscard]] static DebugLayer& instance() noexcept {
    static DebugLayer inst;
    return inst;
  }

  // Register a validation callback.  Pass nullptr to clear.
  void set_callback(Callback cb) {
    std::lock_guard lk{mutex_};
    callback_ = std::move(cb);
  }

  void clear_callback() {
    std::lock_guard lk{mutex_};
    callback_ = nullptr;
  }

  // Set a spdlog logger for structured output.  Pass nullptr to detach.
  void set_logger(std::shared_ptr<spdlog::logger> logger) {
    std::lock_guard lk{mutex_};
    logger_ = std::move(logger);
  }

  // Only deliver messages at or above this level (default: Debug).
  void set_min_level(DebugLevel level) noexcept {
    min_level_ = level;
  }

  [[nodiscard]] DebugLevel min_level() const noexcept {
    return min_level_;
  }

  // Dispatch a diagnostic.  Called by DS_* macros and library internals.
  void log(DebugLevel level, ErrorKind kind, std::string message, std::string_view file, int line) {
    if(static_cast<int>(level) < static_cast<int>(min_level_)) {
      return;
    }

    DebugMessage msg{level, kind, std::move(message), std::string{file}, line};

    std::lock_guard lk{mutex_};

    if(callback_) {
      callback_(msg);
    }

    if(logger_) {
      const auto& text = msg.message;
      switch(level) {
      case DebugLevel::Debug:
        logger_->debug("{}", text);
        break;
      case DebugLevel::Info:
        logger_->info("{}", text);
        break;
      case DebugLevel::Warn:
        logger_->warn("{}", text);
        break;
      case DebugLevel::Error:
        logger_->error("{}", text);
        break;
      case DebugLevel::Off:
        break;
      }
    }
  }

private:
  DebugLayer() = default;

  mutable std::mutex mutex_;
  Callback callback_;
  std::shared_ptr<spdlog::logger> logger_;
  DebugLevel min_level_{DebugLevel::Debug};
};

}    // namespace ds

// ============================================================================
// Public logging macros
// ============================================================================
// These go through DebugLayer so the user callback and spdlog both receive them.
// fmt-style format strings are supported: DS_DEBUG("frame {}", n);

// NOLINTBEGIN(cppcoreguidelines-macro-usage)
#define DS_DEBUG(...)                                                                                                            \
  ::ds::DebugLayer::instance().log(::ds::DebugLevel::Debug, ::ds::ErrorKind::Unknown, fmt::format(__VA_ARGS__), __FILE__, __LINE__)

#define DS_INFO(...)                                                                                                             \
  ::ds::DebugLayer::instance().log(::ds::DebugLevel::Info, ::ds::ErrorKind::Unknown, fmt::format(__VA_ARGS__), __FILE__, __LINE__)

#define DS_WARN(...)                                                                                                             \
  ::ds::DebugLayer::instance().log(::ds::DebugLevel::Warn, ::ds::ErrorKind::Unknown, fmt::format(__VA_ARGS__), __FILE__, __LINE__)

#define DS_ERROR(...)                                                                                                            \
  ::ds::DebugLayer::instance().log(::ds::DebugLevel::Error, ::ds::ErrorKind::Unknown, fmt::format(__VA_ARGS__), __FILE__, __LINE__)
// NOLINTEND(cppcoreguidelines-macro-usage)
