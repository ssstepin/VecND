#pragma once

#include <array>
#include <cstddef>
#include <stdexcept>
#include <tuple>
#include <type_traits>

namespace {
static auto check_bounds = [](const auto &idx, const auto &dimensions_arr,
                              size_t &cnt) -> bool {
  return idx < dimensions_arr[cnt++];
};

static auto count_pos = [](const auto &idx, const auto &dimensions_arr,
                           size_t &cnt, size_t &mul) -> size_t {
  mul /= dimensions_arr[cnt++];
  return mul * idx;
};

template <size_t N> class MultiIndexIterator {
public:
  using IndexArray = std::array<size_t, N>;

  MultiIndexIterator(const IndexArray &sizes, bool isEnd = false)
      : sizes_(sizes), indices_() {
    if (isEnd) {
      for (size_t i = 0; i < N; ++i) {
        indices_[i] = sizes[i];
      }
    } else {
      indices_.fill(0);
    }
  }

  const IndexArray &operator*() const { return indices_; }

  MultiIndexIterator &operator++() {
    for (size_t i = 0; i < N; ++i) {
      ++indices_[i];
      if (indices_[i] < sizes_[i]) {
        return *this;
      }
      indices_[i] = 0;
    }

    indices_ = sizes_;
    return *this;
  }

  MultiIndexIterator operator++(int) {
    MultiIndexIterator temp = *this;
    ++(*this);
    return temp;
  }

  bool operator==(const MultiIndexIterator &other) const {
    return indices_ == other.indices_;
  }

  bool operator!=(const MultiIndexIterator &other) const {
    return !(*this == other);
  }

private:
  IndexArray sizes_;
  IndexArray indices_;
};

template <size_t N> class MultiIndexRange {
public:
  using Iterator = MultiIndexIterator<N>;
  using IndexArray = typename Iterator::IndexArray;

  explicit MultiIndexRange(const IndexArray &sizes) : sizes_(sizes) {}

  Iterator begin() const { return Iterator(sizes_, false); }

  Iterator end() const { return Iterator(sizes_, true); }

  Iterator cbegin() const { return begin(); }

  Iterator cend() const { return end(); }

private:
  IndexArray sizes_;
};

} // namespace

namespace Personal {
template <typename T, size_t... dimensions> class VecND {
private:
  static constexpr size_t N_dim = sizeof...(dimensions);
  static constexpr size_t N_cells = (dimensions * ...);

  using IndexRange = MultiIndexRange<N_dim>;
  using DimensionsArr = std::array<size_t, sizeof...(dimensions)>;

public:
  VecND() {
    for (size_t i = 0; i < N_cells; ++i) {
      _data[i] = T();
    }
  };

  VecND(const T &element) {
    for (size_t i = 0; i < N_cells; ++i) {
      _data[i] = element;
    }
  }

  VecND(T &&element) {
    for (size_t i = 0; i < N_cells; ++i) {
      _data[i] = element;
    }
  }

  template <typename F> VecND(F &&f) {
    size_t i = 0;
    const IndexRange index_range = create_new_range();
    for (const auto &idx : index_range) {
      _data[i] = std::apply(std::forward<F>(f), idx);
      ++i;
    }
  }

  template <typename... Indices> const T &at(Indices... indices) const {
    return _at(indices...);
  }

  template <typename... Indices> T &get(Indices... indices) {
    return _at(indices...);
  }

  constexpr size_t cells_number() const { return N_cells; }
  constexpr const auto &sizes() const { return _dimensions; }

private:
  T _data[N_cells];
  const DimensionsArr _dimensions = {dimensions...};

  template <typename... Indices> bool in_bounds(Indices... indices) const {
    static_assert(sizeof...(Indices) == N_dim,
                  "Number of indices must match number of dimensions");
    static_assert((std::is_convertible_v<Indices, size_t> && ...),
                  "All indices must be convertible to size_t");
    size_t i = 0;
    return (check_bounds(indices, _dimensions, i) && ...);
  }

  template <typename... Indices>
  size_t count_position(Indices... indices) const {
    if (!in_bounds(indices...)) {
      throw std::invalid_argument("Out of bounds");
    }

    size_t i = 0;
    size_t mul = N_cells;
    return (count_pos(indices, _dimensions, i, mul) + ...);
  }

  constexpr IndexRange create_new_range() const {
    return IndexRange(_dimensions);
  }

  template <typename... Indices> const T &_at(Indices... indices) const {
    return _data[count_position(indices...)];
  }

  template <typename... Indices> T &_at(Indices... indices) {
    return _data[count_position(indices...)];
  }
};
} // namespace Personal