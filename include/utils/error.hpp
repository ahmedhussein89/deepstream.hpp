#pragma once
#include <cstddef>
#include <string>
#include <string_view>

namespace ds {

// Identifies the category of a structured error.
enum class ErrorKind {
  Unknown,
  // Element-level
  ElementCreation,    // gst_element_factory_make returned nullptr
  ElementLink,        // gst_element_link failed
  ElementState,       // gst_element_set_state returned FAILURE
  // Pipeline-level
  NoElements,         // build() called with no elements added
  DuplicateName,      // two elements share the same instance name
  IncompatibleCaps,   // static pad-template caps cannot intersect
  PipelineCreation,   // gst_pipeline_new returned nullptr
  BinAdd,             // gst_bin_add rejected an element
  // Parse
  ParseLaunch,        // gst_parse_launch returned an error
};

[[nodiscard]] inline std::string_view error_kind_str(ErrorKind k) noexcept {
  switch(k) {
    case ErrorKind::Unknown: return "Unknown";
    case ErrorKind::ElementCreation: return "ElementCreation";
    case ErrorKind::ElementLink: return "ElementLink";
    case ErrorKind::ElementState: return "ElementState";
    case ErrorKind::NoElements: return "NoElements";
    case ErrorKind::DuplicateName: return "DuplicateName";
    case ErrorKind::IncompatibleCaps: return "IncompatibleCaps";
    case ErrorKind::PipelineCreation: return "PipelineCreation";
    case ErrorKind::BinAdd: return "BinAdd";
    case ErrorKind::ParseLaunch: return "ParseLaunch";
  }
  return "Unknown";
}

// Structured error type used throughout the ds:: API.
//
// Provides a string-compatible interface (empty(), find(), operator==) so that
// code already written against nonstd::expected<T, std::string> continues to
// compile unchanged after migration to nonstd::expected<T, ds::Error>.
struct Error {
  ErrorKind   kind{ErrorKind::Unknown};
  std::string message;

  Error() = default;
  Error(ErrorKind k, std::string msg) : kind(k), message(std::move(msg)) {}

  // String-compatible interface for backward compatibility.
  [[nodiscard]] bool empty() const noexcept { return message.empty(); }
  [[nodiscard]] std::size_t find(std::string_view s, std::size_t pos = 0) const noexcept { return message.find(s, pos); }
  [[nodiscard]] std::string_view what() const noexcept { return message; }

  bool operator==(std::string_view s) const noexcept { return message == s; }
  bool operator!=(std::string_view s) const noexcept { return message != s; }

  friend bool operator==(std::string_view s, const Error& e) noexcept { return e.message == s; }
  friend bool operator!=(std::string_view s, const Error& e) noexcept { return e.message != s; }
};

// Returned by ds:: element create() methods (gst_element_factory_make failures).
struct ElementError : Error {
  using Error::Error;
};

// Returned by ds::Builder::build() and gst::build() (pipeline construction failures).
struct PipelineError : Error {
  using Error::Error;
};

}    // namespace ds
