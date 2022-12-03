/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief AVX2 support for LDPC.

#pragma once

#include "srsgnb/support/srsgnb_assert.h"
#include <array>
#include <immintrin.h>

namespace srsgnb {
/// Number of bytes in an AVX2 register.
constexpr unsigned AVX2_SIZE_BYTE = 32;

namespace mm256 {
/// \brief Mimics an array of AVX2 registers.
/// \tparam nof_elements The number of AVX2 registers in the array.
template <size_t nof_elements>
class avx2_array
{
public:
  /// Returns a pointer to the \c pos AVX2 register inside the array.
  __m256i* data_at(unsigned pos)
  {
    srsgnb_assert(pos < nof_elements, "Index {} out of bound.", pos);
    unsigned index = pos * AVX2_SIZE_BYTE;
    return reinterpret_cast<__m256i*>(inner_array.data() + index);
  }

  /// Returns a read-only pointer to the \c pos AVX2 register inside the array.
  const __m256i* data_at(unsigned pos) const
  {
    srsgnb_assert(pos < nof_elements, "Index {} out of bound.", pos);
    unsigned index = pos * AVX2_SIZE_BYTE;
    return reinterpret_cast<const __m256i*>(inner_array.data() + index);
  }

  /// Returns a pointer to the byte at position <tt>pos * AVX2_SIZE_BYTE + byte</tt> inside the array.
  int8_t* data_at(unsigned pos, unsigned byte)
  {
    unsigned index = pos * AVX2_SIZE_BYTE + byte;
    srsgnb_assert(index < nof_elements * AVX2_SIZE_BYTE, "Index ({}, {}) out of bound.", pos, byte);
    return (inner_array.data() + index);
  }

  /// Returns a read-only pointer to the \c pos AVX2 register inside the array.
  const int8_t* data_at(unsigned pos, unsigned byte) const
  {
    unsigned index = pos * AVX2_SIZE_BYTE + byte;
    srsgnb_assert(index < nof_elements * AVX2_SIZE_BYTE, "Index ({}, {}) out of bound.", pos, byte);
    return (inner_array.data() + index);
  }

  // Unfortunately, we can't work with the array subscript operator [] since there seems to be no easy way to access a
  // __m256i object by reference.

  /// Sets the \c pos AVX2 register to \c val.
  void set_at(unsigned pos, __m256i val)
  {
    srsgnb_assert(pos < nof_elements, "Index {} out of bound.", pos);
    unsigned index = pos * AVX2_SIZE_BYTE;
    _mm256_storeu_si256(reinterpret_cast<__m256i*>(inner_array.data() + index), val);
  }

  /// Gets the value stored in the \c pos AVX2 register.
  __m256i get_at(unsigned pos) const
  {
    srsgnb_assert(pos < nof_elements, "Index {} out of bound.", pos);
    unsigned index = pos * AVX2_SIZE_BYTE;
    return _mm256_loadu_si256(reinterpret_cast<const __m256i*>(inner_array.data() + index));
  }

private:
  /// Actual array where the AVX2 registers are stored.
  alignas(32) std::array<int8_t, nof_elements * AVX2_SIZE_BYTE> inner_array;
};

/// \brief Mimics a span of AVX2 registers.
class avx2_span
{
public:
  /// \brief Constructs a span from an \ref avx2_array.
  ///
  /// \tparam N     Array length.
  /// \param arr    Array the span is a view of.
  /// \param offset First element of the array (an AVX2 register) viewed by the span.
  /// \param length Length of the span.
  template <size_t N>
  avx2_span(avx2_array<N>& arr, unsigned offset, unsigned length) :
    array_ptr(arr.data_at(offset, 0)), view_length(length)
  {
    srsgnb_assert(offset + view_length <= N, "Cannot take a span longer than the array.");
  }

  /// \brief Constructs a span that is a view over an entire \ref avx2_array.
  template <size_t N>
  explicit avx2_span(avx2_array<N>& arr) : avx2_span(arr, 0, N)
  {
  }

  /// Returns a pointer to the \c pos AVX2 register inside the array.
  __m256i* data_at(unsigned pos)
  {
    srsgnb_assert(pos < view_length, "Index {} out of bound.", pos);
    unsigned index = pos * AVX2_SIZE_BYTE;
    return reinterpret_cast<__m256i*>(array_ptr + index);
  }

  /// Returns a read-only pointer to the \c pos AVX2 register inside the array.
  const __m256i* data_at(unsigned pos) const
  {
    srsgnb_assert(pos < view_length, "Index {} out of bound.", pos);
    unsigned index = pos * AVX2_SIZE_BYTE;
    return reinterpret_cast<const __m256i*>(array_ptr + index);
  }

  /// Returns a pointer to the byte at position <tt>pos * AVX2_SIZE_BYTE + byte</tt> inside the array.
  int8_t* data_at(unsigned pos, unsigned byte)
  {
    unsigned index = pos * AVX2_SIZE_BYTE + byte;
    srsgnb_assert(index < view_length * AVX2_SIZE_BYTE, "Index ({}, {}) out of bound.", pos, byte);
    return (array_ptr + index);
  }

  /// Returns a read-only pointer to the \c pos AVX2 register inside the array.
  const int8_t* data_at(unsigned pos, unsigned byte) const
  {
    unsigned index = pos * AVX2_SIZE_BYTE + byte;
    srsgnb_assert(index < view_length * AVX2_SIZE_BYTE, "Index ({}, {}) out of bound.", pos, byte);
    return (array_ptr + index);
  }

  // Unfortunately, we can't work with the array subscript operator [] since there seems to be no easy way to access a
  // __m256i object by reference.

  /// Sets the \c pos AVX2 register to \c val.
  void set_at(unsigned pos, __m256i val)
  {
    srsgnb_assert(pos < view_length, "Index {} out of bound.", pos);
    unsigned index = pos * AVX2_SIZE_BYTE;
    _mm256_storeu_si256(reinterpret_cast<__m256i*>(array_ptr + index), val);
  }

  /// Gets the value stored in the \c pos AVX2 register.
  __m256i get_at(unsigned pos) const
  {
    srsgnb_assert(pos < view_length, "Index {} out of bound.", pos);
    unsigned index = pos * AVX2_SIZE_BYTE;
    return _mm256_loadu_si256(reinterpret_cast<const __m256i*>(array_ptr + index));
  }

  /// Returns the number of AVX2 registers viewed by the span.
  size_t size() const { return view_length; }

private:
  /// Pointer to the first element viewed by the span.
  int8_t* array_ptr;
  /// Number of elements viewed by the span.
  size_t view_length;
};

/// \brief Mimics a span of constant AVX2 registers.
class avx2_const_span
{
public:
  /// \brief Constructs a const span from an \ref avx2_array.
  ///
  /// \tparam N     Array length.
  /// \param arr    Array the span is a view of.
  /// \param offset First element of the array (an AVX2 register) viewed by the span.
  /// \param length Length of the span.
  template <size_t N>
  avx2_const_span(const avx2_array<N>& arr, unsigned offset, unsigned length) :
    array_ptr(arr.data_at(offset, 0)), view_length(length)
  {
    srsgnb_assert(offset + view_length <= N, "Cannot take a span longer than the array.");
  }

  /// \brief Constructs a span that is a view over an entire \ref avx2_array.
  template <size_t N>
  explicit avx2_const_span(const avx2_array<N>& arr) : avx2_const_span(arr, 0, N)
  {
  }

  /// Returns a read-only pointer to the \c pos AVX2 register inside the array.
  const __m256i* data_at(unsigned pos) const
  {
    srsgnb_assert(pos < view_length, "Index {} out of bound.", pos);
    unsigned index = pos * AVX2_SIZE_BYTE;
    return reinterpret_cast<const __m256i*>(array_ptr + index);
  }

  /// Returns a read-only pointer to the \c pos AVX2 register inside the array.
  const int8_t* data_at(unsigned pos, unsigned byte) const
  {
    unsigned index = pos * AVX2_SIZE_BYTE + byte;
    srsgnb_assert(index < view_length * AVX2_SIZE_BYTE, "Index ({}, {}) out of bound.", pos, byte);
    return (array_ptr + index);
  }

  // Unfortunately, we can't work with the array subscript operator [] since there seems to be no easy way to access a
  // __m256i object by reference.

  /// Gets the value stored in the \c pos AVX2 register.
  __m256i get_at(unsigned pos) const
  {
    srsgnb_assert(pos < view_length, "Index {} out of bound.", pos);
    unsigned index = pos * AVX2_SIZE_BYTE;
    return _mm256_loadu_si256(reinterpret_cast<const __m256i*>(array_ptr + index));
  }

  /// Returns the number of AVX2 registers viewed by the span.
  size_t size() const { return view_length; }

private:
  /// Pointer to the first element viewed by the span.
  const int8_t* array_ptr;
  /// Number of elements viewed by the span.
  size_t view_length;
};

/// \brief Scales packed 8-bit integers in \c a by the scaling factor \c sf.
/// \param[in] a   Vector of packed 8-bit integers.
/// \param[in] sf  Scaling factor (0, 1].
/// \return    Vector of packed 8-bit integers with the scaling result.
inline __m256i scale_epi8(__m256i a, float sf)
{
  srsgnb_assert((sf > 0) && (sf <= 1), "Scaling factor out of range.");

  if (sf >= .9999) {
    return a;
  }

  static const __m256i mask_even_epi8 = _mm256_set1_epi16(0x00FF);
  // FLOAT2INT = 2^16 = 65536
  static constexpr unsigned FLOAT2INT = 1U << 16U;

  // Create a register filled with copies of the scaling factor times FLOAT2INT. Note that the upper bound on sf implies
  // that (sf * FLOAT2INT) can be represented over 16 bits.
  __m256i sf_epi16 = _mm256_set1_epi16(static_cast<uint16_t>(sf * FLOAT2INT));
  // Even bytes in a, alternated with a zero-valued byte.
  __m256i even_epi16 = _mm256_and_si256(a, mask_even_epi8);
  // Odd bytes in a, moved to the even positions and alternated with a zero-valued byte.
  __m256i odd_epi16 = _mm256_srli_epi16(a, 8);

  // This is equivalent to multiply each byte by sf: indeed, we multiply each byte by (sf * FLOAT2INT), store the result
  // in over 32 bit and pick only the 16 most significant bits (i.e., we divide by FLOAT2INT).
  __m256i p_even_epi16 = _mm256_mulhi_epu16(even_epi16, sf_epi16);
  __m256i p_odd_epi16  = _mm256_mulhi_epu16(odd_epi16, sf_epi16);

  // Move the odd bits back to their original positions.
  p_odd_epi16 = _mm256_slli_epi16(p_odd_epi16, 8);

  // Combine even and odd bits. Note that the "odd" bytes of p_even_epi16 are equal to 0 (the result of multiplying a
  // byte by a 16-bit value will occupy at most 24 of the 32 bits).
  return _mm256_xor_si256(p_even_epi16, p_odd_epi16);
}
} // namespace mm256

/// \brief Rotates the contents of a node towards the left by \c steps chars, that is the \c steps * 8 least significant
/// bits become the most significant ones - for long lifting sizes.
/// \param[out] out       Pointer to the first AVX2 block of the output rotated node.
/// \param[in]  in        Pointer to the first AVX2 block of the input node to rotate.
/// \param[in]  steps     The order of the rotation as a number of chars.
/// \param[in]  ls        The size of the node (lifting size).
/// \remark Cannot be used to override memory.
inline void rotate_node_left(int8_t* out, const int8_t* in, unsigned steps, unsigned ls)
{
  srsgnb_assert(std::abs(in - out) >= ls, "Input and output memory overlap.");
  std::memcpy(out, in + ls - steps, steps);
  std::memcpy(out + steps, in, ls - steps);
}

/// \brief Rotates the contents of a node towards the right by \c steps chars, that is the \c steps * 8 most significant
/// bits become the least significant ones - for long lifting sizes.
/// \param[out] out       Pointer to the first AVX2 block of the output rotated node.
/// \param[in]  in        Pointer to the first AVX2 block of the input node to rotate.
/// \param[in]  steps     The order of the rotation as a number of chars.
/// \param[in]  ls        The size of the node (lifting size).
/// \remark Cannot be used to override memory.
inline void rotate_node_right(int8_t* out, const int8_t* in, unsigned steps, unsigned ls)
{
  srsgnb_assert(std::abs(in - out) >= ls, "Input and output memory overlap.");
  std::memcpy(out, in + steps, ls - steps);
  std::memcpy(out + ls - steps, in, steps);
}

} // namespace srsgnb
