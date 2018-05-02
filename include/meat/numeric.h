/*                                                                  -*- c++ -*-
 * Copyright (c) 2017 Ron R Wills <ron.rwsoft@gmail.com>
 *
 * This file is part of Meat.
 *
 * Meat is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Meat is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Meat.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <cinttypes>
#include <stdexcept>
#include <limits>
#include <ostream>
#include <cfenv>

//#pragma STDC FENV_ACCESS on

#ifndef _NUMERIC_H
#define _NUMERIC_H

namespace numeric {

  /** Template class for integers with range, over and under flow checks.
   *
   * @todo Comparison operators.
   */
  template <typename Ty>
  class integer : public std::numeric_limits<Ty> {
  public:
    /** Primitive type of the integer.
     */
    typedef Ty int_type;

    /** Default constructor.
     */
    integer() : _value(0) {}

    /** Constructor from a primitive value.
     */
    template<typename int_ty>
    integer(int_ty value) {
      _value = __range_check(value);
    }

    /** Copy constructor.
     */
    integer(const integer &other) {
      _value = __range_check(other._value);
    }

    /** Assignment from a primitive value.
     */
    template<typename int_ty>
    integer<Ty> &operator =(int_ty value) {
      _value = __range_check(value);
      return *this;
    }

    /** Assignment from another integer object.
     */

    integer<Ty> &operator =(const integer &other) {
      if (this != &other)
        _value = __range_check(other._value);
      return *this;
    }

    template<typename int_ty>
    integer<Ty> operator +(int_ty value) const {
      Ty res;
      if (__builtin_add_overflow(_value, value, &res))
        throw std::overflow_error("integer::addition overflow");
      return res;
    }

    template<typename int_ty>
    integer<Ty> operator +(const integer<int_ty> &value) const {
      Ty res;
      if (__builtin_add_overflow(_value, (int_ty)value, &res))
        throw std::overflow_error("integer::addition overflow");
      return res;
    }

    template<typename int_ty>
    integer<Ty> operator -(int_ty value) const {
      Ty res;
      if (__builtin_sub_overflow(_value, value, &res))
        throw std::overflow_error("integer::subtraction underflow");
      return res;
    }

    template<typename int_ty>
    integer<Ty> operator -(const integer<int_ty> &value) const {
      Ty res;
      if (__builtin_sub_overflow(_value, (int_ty)value, &res))
        throw std::underflow_error("integer::subtraction underflow");
      return res;
    }

    template<typename int_ty>
    integer<Ty> operator *(int_ty value) const {
      Ty res;
      if (__builtin_mul_overflow(_value, value, &res))
        throw std::underflow_error("integer::multiplication overflow");
      return res;
    }

    template<typename int_ty>
    integer<Ty> operator *(const integer<int_ty> &value) const {
      Ty res;
      if (__builtin_mul_overflow(_value, (int_ty)value, &res))
        throw std::overflow_error("integer::multiplication overflow");
      return res;
    }

    template<typename int_ty>
    integer<Ty> operator /(int_ty value) const {
      if (value == 0)
        throw std::range_error("integer::divide by zero");
      return (_value / value);
    }

    template<typename int_ty>
    integer<Ty> operator /(const integer<int_ty> &value) const {
      if (value.value == 0)
        throw std::range_error("integer::divide by zero");
      return (_value / (int_ty)value);
    }

    template<typename int_ty>
    integer<Ty> operator %(int_ty value) const {
      return __range_check(_value % value);
    }

    template<typename int_ty>
    integer<Ty> operator %(const integer<int_ty> &value) const {
      return __range_check(_value % (int_ty)value);
    }

    integer<Ty> operator -() const {
      Ty res;
      if (__builtin_mul_overflow(_value, -1, &res))
        throw std::range_error("integer negation range error");
      return res;
    }

    /** Unary +.
     * This is the equivalent of multiplying the value by one, so we don't
     * really need to do any checks here.
     */
    integer<Ty> operator +() const {
      return +_value;
    }

    template<typename int_ty>
    integer<Ty> operator <<(int_ty value) const {
      Ty res;
      if (value < 0 or (unsigned int)value >= sizeof(Ty) * 8 or
          __builtin_mul_overflow(_value, 1 << value, &res)) {
        throw std::overflow_error("integer::left shift overflow");
      }
      return res;
    }

    template<typename int_ty>
    integer<Ty> operator <<(const integer<int_ty> &value) const {
      Ty res;
      if (value < 0 or (unsigned int)value >= sizeof(Ty) * 8 or
          __builtin_mul_overflow(_value, 1 << (unsigned int)value, &res)) {
        throw std::overflow_error("integer::left shift overflow");
      }
      return res;
    }

    operator Ty() const { return _value; }

  private:

    template<typename int_ty>
    Ty __range_check(int_ty value) const {
      if (value < integer<Ty>::min() or value > integer<Ty>::max())
        throw std::range_error("integer value out of range");
      return value;
    }

    Ty _value;
  };

  typedef integer<std::int8_t> integer_8;
  typedef integer<std::int16_t> integer_16;
  typedef integer<std::int32_t> integer_32;
  typedef integer<std::int64_t> integer_64;
  typedef integer<std::uint8_t> unsigned_integer_8;
  typedef integer<std::uint16_t> unsigned_integer_16;
  typedef integer<std::uint32_t> unsigned_integer_32;
  typedef integer<std::uint64_t> unsigned_integer_64;

  template <typename Ty>
  class Float : public std::numeric_limits<Ty> {
  public:
    Float() : _value(0.0) {}

    template<typename float_ty>
    Float(float_ty value) {
      feclearexcept(FE_ALL_EXCEPT);
      _value = value;
      __error_check();
    }

    Float(const Float &other) {
      feclearexcept(FE_ALL_EXCEPT);
      _value = other.value;
      __error_check();
    }

    template<typename float_ty>
    Float<Ty> &operator =(float_ty value) {
      feclearexcept(FE_ALL_EXCEPT);
      _value = value;
      __error_check();
      return *this;
    }

    Float<Ty> &operator =(const Float &other) {
      if (this != &other) {
        feclearexcept(FE_ALL_EXCEPT);
        _value = other.value;
        __error_check();
      }
      return *this;
    }

    template<typename float_ty>
    Float<Ty> operator +(float_ty other) const {
      feclearexcept(FE_ALL_EXCEPT);
      Ty res = _value + other;
      __error_check();
      return res;
    }

    template<typename int_ty>
    Float<Ty> operator +(const Float<int_ty> &other) const {
      feclearexcept(FE_ALL_EXCEPT);
      Float<Ty> res = _value + other._value;
      __error_check();
      return res;
    }

    operator Ty() const { return _value; }

  private:

    void __error_check() const {
      int fpe = fetestexcept(FE_ALL_EXCEPT);
      if (fpe) {
        if (fpe & FE_INEXACT) return;
          //throw std::range_error("Float::inexact");
        if (fpe & FE_INVALID)
          throw std::range_error("Float::invalid");
        if (fpe & FE_OVERFLOW)
          throw std::overflow_error("Float::overflow");
        if (fpe & FE_UNDERFLOW)
          throw std::underflow_error("Float::underflow");
        if (fpe & FE_DIVBYZERO)
          throw std::runtime_error("Float::divide by zero");
        throw std::runtime_error("Float::unknown error");
      }
    }

    Ty _value;
  };
}

template <typename Ty>
std::ostream &operator <<(std::ostream &out,
                          const numeric::integer<Ty> &value) {
  out << (int)((Ty)value);
  return out;
}

template <typename Ty>
std::istream &operator >>(std::istream &in,
                          numeric::integer<Ty> &value) {
  Ty in_value;
  in >> in_value;
  value = in_value;
  return in;
}

template <typename Ty>
std::ostream &operator <<(std::ostream &out,
                          const numeric::Float<Ty> &value) {
  out << (double)((Ty)value);
  return out;
}

#endif /* _NUMERIC_H */

#endif /* __cplusplus < 201103L */
