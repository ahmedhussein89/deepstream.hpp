#pragma once
#include <concepts>
#include <iterator>

#include <nvdsmeta.h>

namespace ds {

// MetaView<View, Native>: View must be constructible from Native*, matching
// the dereference contract of MetaListView's iterator.
template <typename View, typename Native>
concept MetaView = std::constructible_from<View, Native*>;

// Generic forward-iterator range over NvDsMetaList (typedef for GList).
// Constructs a View from each Native* node element on dereference.
template <typename View, typename Native>
  requires MetaView<View, Native>
class MetaListView {
public:
  struct Iterator {
    using difference_type = std::ptrdiff_t;
    using value_type = View;
    using pointer = void;
    using reference = View;
    using iterator_category = std::forward_iterator_tag;

    explicit Iterator(NvDsMetaList* node) : node_(node) {}

    View operator*() const { return View{static_cast<Native*>(node_->data)}; }

    Iterator& operator++() {
      node_ = node_->next;
      return *this;
    }

    Iterator operator++(int) {
      auto tmp = *this;
      ++*this;
      return tmp;
    }

    bool operator==(const Iterator& other) const { return node_ == other.node_; }
    bool operator!=(const Iterator& other) const { return node_ != other.node_; }

  private:
    NvDsMetaList* node_;
  };

  explicit MetaListView(NvDsMetaList* head) : head_(head) {}

  [[nodiscard]] Iterator begin() const { return Iterator{head_}; }
  [[nodiscard]] Iterator end() const { return Iterator{nullptr}; }
  [[nodiscard]] bool empty() const { return head_ == nullptr; }

private:
  NvDsMetaList* head_;
};

}    // namespace ds
