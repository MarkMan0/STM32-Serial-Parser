#ifndef RING_BUFFER_H_
#define RING_BUFFER_H_

/** @file ring_buffer.h
 * templated inline ring buffer class
 */

#include <array>
#include <cstdint>

/**
 * @brief Very simple inline ring buffer, not fool-proof.
 *
 * Will not accept incoming data when full
 * IMPORTANT: push() shall not be called, when get_next_free returned nullptr
 * IMPORTANT: non-const functions shall be called from within critical sections
 * @tparam T type which the buffer holds
 * @tparam L The size of the buffer
 */
template <class T, uint8_t L>
class RingBuffer {
  // types
public:
  static constexpr uint8_t buffer_size_ = L;
  using data_t = T;
  // interface
public:
  /**
   * @brief Resets head and tail
   *
   */
  void reset();

  // data manipulation
  /**
   * @brief Moves head forward, if full, overwrites
   * User should write the data to the pointer returned by getNextFree()
   */
  void push();
  /**
   * @brief Moves tail forward
   *
   */
  void pop();
  /**
   * @brief Get the Next Free object
   *
   * @return T* Pointer to the next free, or nullptr if full
   */
  data_t* get_next_free();
  /**
   * @brief Get the Next Occupied object
   *
   * @return T* pointer to the next data, or nullptr if empty
   */
  data_t* get_next_occupied();

  /**
   * @brief Pointer to constant next occupied object
   * @see getNextOccupied()
   * @return const data_t*
   */
  const data_t* get_next_occupied() const;
  // states
  /**
   * @brief Check if buffer is empty
   *
   * @return true if empty
   */
  bool is_empty() const;
  /**
   * @brief Check if buffer is full
   *
   * @return true if full
   */
  bool is_full() const;
  /**
   * @brief Get number of data in buffer
   *
   * @return uint8_t number of data
   */
  uint8_t num_occupied() const;

  // implementation
private:
  /**
   * @brief The buffer
   *
   */
  std::array<data_t, buffer_size_> buffer_;
  /**
   * @brief The real head of the buffer
   *
   * This variable marks, where usable data is in the space it points to
   * This is the real head, which is moved on every push()
   */
  volatile uint8_t head_{ 0 };

  /**
   * @brief This head is used to get the next free space
   *
   * Whenever get_next_free() is called this is moved forward.
   * This is done so multiple tasks can write to the buffer concurrently
   * Without this, get_next_free() would give the same space in the buffer to multiple tasks
   */
  volatile uint8_t virtual_head_{ 0 };
  /**
   * @brief next next occupied space
   *
   */
  volatile uint8_t tail_{ 0 };
  /**
   * @brief To track the edge case
   *
   */
  volatile bool is_full_{ false };
};



template <class T, uint8_t L>
inline void RingBuffer<T, L>::reset() {
  head_ = tail_;
  is_full_ = false;
}

template <class T, uint8_t L>
inline void RingBuffer<T, L>::push() {
  head_ = (head_ + 1) % buffer_size_;
}

template <class T, uint8_t L>
inline void RingBuffer<T, L>::pop() {
  if (is_empty()) return;
  tail_ = (tail_ + 1) % buffer_size_;
  is_full_ = false;
}

template <class T, uint8_t L>
inline typename RingBuffer<T, L>::data_t* RingBuffer<T, L>::get_next_free() {
  if (is_full()) {
    return nullptr;
  }
  // Get pointer to space in buffer
  const auto ret = &(buffer_[virtual_head_]);
  // calculate the next virtual_head_
  const uint8_t next_v_head = (virtual_head_ + 1) % buffer_size_;
  // track if buffer is full
  if (next_v_head == tail_) {
    is_full_ = true;
  }
  virtual_head_ = next_v_head;
  return ret;
}

template <class T, uint8_t L>
inline typename RingBuffer<T, L>::data_t* RingBuffer<T, L>::get_next_occupied() {
  if (is_empty()) return nullptr;
  return &(buffer_[tail_]);
}

template <class T, uint8_t L>
inline const typename RingBuffer<T, L>::data_t* RingBuffer<T, L>::get_next_occupied() const {
  if (is_empty()) return nullptr;
  return &(buffer_[tail_]);
}

template <class T, uint8_t L>
inline bool RingBuffer<T, L>::is_empty() const {
  return (!is_full_ && (head_ == tail_));
}

template <class T, uint8_t L>
inline bool RingBuffer<T, L>::is_full() const {
  return is_full_;
}

template <class T, uint8_t L>
inline uint8_t RingBuffer<T, L>::num_occupied() const {
  if (is_full()) {
    return buffer_size_;
  } else if (head_ >= tail_) {
    return head_ - tail_;
  } else {
    return buffer_size_ + head_ - tail_;
  }
}

#endif  // MESSAGE_BUFFER_H_
