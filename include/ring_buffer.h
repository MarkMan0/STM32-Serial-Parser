#ifndef RING_BUFFER_H_
#define RING_BUFFER_H_

/** @file ring_buffer.h
 * templated inline ring buffer class
 */

#include <array>
#include <cstdint>

/**
 * @brief Very simple inline ring buffer, not fool-proof.
 * Will not accept incoming data when full
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
  data_t* getNextFree();
  /**
   * @brief Get the Next Occupied object
   * 
   * @return T* pointer to the next data, or nullptr if empty
   */
  data_t* getNextOccupied();

  // states
  /**
   * @brief Check if buffer is empty
   * 
   * @return true if empty
   */
  bool isEmpty() const;
  /**
   * @brief Check if buffer is full
   * 
   * @return true if full
   */
  bool isFull() const;
  /**
   * @brief Get number of data in buffer
   * 
   * @return uint8_t number of data
   */
  uint8_t numOccupied() const;

  // implementation
private:
  std::array<data_t, buffer_size_> buffer;
  uint8_t head_{ 0 };
  uint8_t tail_{ 0 };
  bool is_full_{ false };

};



template <class T, uint8_t L>
inline void RingBuffer<T, L>::reset() {
  head_ = tail_;
  is_full_ = false;
}

template <class T, uint8_t L>
inline void RingBuffer<T, L>::push() {
  const uint8_t head2 = (head_ + 1) % buffer_size_;
  if(is_full_) return;
  if (head2 == tail_) {
    is_full_ = true;
  }
  head_ = head2;
}

template <class T, uint8_t L>
inline void RingBuffer<T, L>::pop() {
  if (isEmpty())
    return;
  tail_ = (tail_ + 1) % buffer_size_;
  is_full_ = false;
}

template <class T, uint8_t L>
inline typename RingBuffer<T, L>::data_t* RingBuffer<T, L>::getNextFree() {
  if (isFull()) {
    return nullptr;
  }
  return &(buffer[head_]);
}

template <class T, uint8_t L>
inline typename RingBuffer<T, L>::data_t* RingBuffer<T, L>::getNextOccupied() {
  if (isEmpty())
    return nullptr;
  return &(buffer[tail_]);
}

template <class T, uint8_t L>
inline bool RingBuffer<T, L>::isEmpty() const {
  return (!is_full_ && (head_ == tail_));
}

template <class T, uint8_t L>
inline bool RingBuffer<T, L>::isFull() const {
  return is_full_;
}

template <class T, uint8_t L>
inline uint8_t RingBuffer<T, L>::numOccupied() const {
  if (isFull()) {
    return buffer_size_;
  } else if (head_ >= tail_) {
    return head_ - tail_;
  } else {
    return buffer_size_ + head_ - tail_;
  }
}

#endif  // MESSAGE_BUFFER_H_