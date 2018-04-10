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

#if __cplusplus < 201103L
//#error This file requires compiler and library support for the ISO C++ 2011 standard.
#else

#ifndef __GNUC__
#error This file requires builtin gnu compiler functions for math error checking.
#endif

#include <cinttypes>
#include <stdexcept>
#include <limits>
#include <ostream>
#include <cfenv>

//#pragma STDC FENV_ACCESS on

#ifndef _NUMERIC_H
#define _NUMERIC_H

namespace numeric {

  template <typename Ty>
  class Integer : public std::numeric_limits<Ty> {
  public:
    typedef Ty int_type;

    Integer() : value(0) {}

    template<typename int_ty>
    Integer(int_ty value) {
      this->value = __range_check(value);
    }

    Integer(const Integer &other) {
      this->value = __range_check(other.value);
    }

    template<typename int_ty>
    Integer<Ty> &operator =(int_ty value) {
      this->value = __range_check(value);
      return *this;
    }

    Integer<Ty> &operator =(const Integer &other) {
      if (this != &other)
        this->value = __range_check(other.value);
      return *this;
    }

    template<typename int_ty>
    Integer<Ty> operator +(int_ty value) const {
      Ty res;
      if (__builtin_add_overflow(this->value, value, &res))
        throw std::overflow_error("Integer::addition overflow");
      return res;
    }

    template<typename int_ty>
    Integer<Ty> operator +(const Integer<int_ty> &value) const {
      Ty res;
      if (__builtin_add_overflow(this->value, (int_ty)value, &res))
        throw std::overflow_error("Integer::addition overflow");
      return res;
    }

    template<typename int_ty>
    Integer<Ty> operator -(int_ty value) const {
      Ty res;
      if (__builtin_sub_overflow(this->value, value, &res))
        throw std::overflow_error("Integer::subtraction underflow");
      return res;
    }

    template<typename int_ty>
    Integer<Ty> operator -(const Integer<int_ty> &value) const {
      Ty res;
      if (__builtin_sub_overflow(this->value, (int_ty)value, &res))
        throw std::underflow_error("Integer::subtraction underflow");
      return res;
    }

    template<typename int_ty>
    Integer<Ty> operator *(int_ty value) const {
      Ty res;
      if (__builtin_mul_overflow(this->value, value, &res))
        throw std::underflow_error("Integer::multiplication overflow");
      return res;
    }

    template<typename int_ty>
    Integer<Ty> operator *(const Integer<int_ty> &value) const {
      Ty res;
      if (__builtin_mul_overflow(this->value, (int_ty)value, &res))
        throw std::overflow_error("Integer::multiplication overflow");
      return res;
    }

    template<typename int_ty>
    Integer<Ty> operator /(int_ty value) const {
      if (value == 0)
        throw std::range_error("Integer::divide by zero");
      return __range_check(this->value / value);
    }

    template<typename int_ty>
    Integer<Ty> operator /(const Integer<int_ty> &value) const {
      if (value.value == 0)
        throw std::range_error("Integer::divide by zero");
      return __range_check(this->value / (int_ty)value);
    }

    template<typename int_ty>
    Integer<Ty> operator %(int_ty value) const {
      return __range_check(this->value % value);
    }

    template<typename int_ty>
    Integer<Ty> operator %(const Integer<int_ty> &value) const {
      return __range_check(this->value % (int_ty)value);
    }

    Integer<Ty> operator -() const {
      return __range_check(-this->value);
    }

    Integer<Ty> operator +() const {
      return __range_check(+this->value);
    }

    template<typename int_ty>
    Integer<Ty> operator <<(int_ty value) const {
      Ty res;
      if (value < 0 or (unsigned int)value >= sizeof(Ty) * 8 or
          __builtin_mul_overflow(this->value, 1 << value, &res)) {
        throw std::overflow_error("Integer::left shift overflow");
      }
      return res;
    }

    template<typename int_ty>
    Integer<Ty> operator <<(const Integer<int_ty> &value) const {
      Ty res;
      if (value < 0 or (unsigned int)value >= sizeof(Ty) * 8 or
          __builtin_mul_overflow(this->value, 1 << (unsigned int)value, &res)) {
        throw std::overflow_error("Integer::left shift overflow");
      }
      return res;
    }

    operator Ty() const { return value; }

  private:

    template<typename int_ty>
    Ty __range_check(int_ty value) const {
      if (value < Integer<Ty>::min() or value > Integer<Ty>::max())
        throw std::range_error("Integer::__range_check");
      return value;
    }

    Ty value;
  };

  typedef Integer<std::int8_t> Int8;
  typedef Integer<std::int16_t> Int16;
  typedef Integer<std::int32_t> Int32;
  typedef Integer<std::int64_t> Int64;
  typedef Integer<std::uint8_t> UInt8;
  typedef Integer<std::uint16_t> UInt16;
  typedef Integer<std::uint32_t> UInt32;
  typedef Integer<std::uint64_t> UInt64;

  template <typename Ty>
  class FloatType : public std::numeric_limits<Ty> {
  public:
    FloatType() : value(0.0) {}

    template<typename float_ty>
    FloatType(float_ty value) {
      feclearexcept(FE_ALL_EXCEPT);
      this->value = value;
      __error_check();
    }

    FloatType(const FloatType &other) {
      feclearexcept(FE_ALL_EXCEPT);
      this->value = other.value;
      __error_check();
    }

    template<typename float_ty>
    FloatType<Ty> &operator =(float_ty value) {
      feclearexcept(FE_ALL_EXCEPT);
      this->value = value;
      __error_check();
      return *this;
    }

    FloatType<Ty> &operator =(const FloatType &other) {
      if (this != &other) {
        feclearexcept(FE_ALL_EXCEPT);
        this->value = other.value;
        __error_check();
      }
      return *this;
    }

    template<typename float_ty>
    FloatType<Ty> operator +(float_ty other) const {
      feclearexcept(FE_ALL_EXCEPT);
      Ty res = value + other;
      __error_check();
      return res;
    }

    template<typename int_ty>
    FloatType<Ty> operator +(const FloatType<int_ty> &other) const {
      feclearexcept(FE_ALL_EXCEPT);
      FloatType<Ty> res = value + other.value;
      __error_check();
      return res;
    }

    template<typename float_ty>
    FloatType<Ty> operator -(float_ty other) const {
      feclearexcept(FE_ALL_EXCEPT);
      Ty res = value - other;
      __error_check();
      return res;
    }

    template<typename int_ty>
    FloatType<Ty> operator -(const FloatType<int_ty> &other) const {
      feclearexcept(FE_ALL_EXCEPT);
      FloatType<Ty> res = value - other.value;
      __error_check();
      return res;
    }

    template<typename float_ty>
    FloatType<Ty> operator *(float_ty other) const {
      feclearexcept(FE_ALL_EXCEPT);
      Ty res = value * other;
      __error_check();
      return res;
    }

    template<typename int_ty>
    FloatType<Ty> operator *(const FloatType<int_ty> &other) const {
      feclearexcept(FE_ALL_EXCEPT);
      FloatType<Ty> res = value * other.value;
      __error_check();
      return res;
    }

    template<typename float_ty>
    FloatType<Ty> operator /(float_ty other) const {
      feclearexcept(FE_ALL_EXCEPT);
      Ty res = value / other;
      __error_check();
      return res;
    }

    template<typename int_ty>
    FloatType<Ty> operator /(const FloatType<int_ty> &other) const {
      feclearexcept(FE_ALL_EXCEPT);
      FloatType<Ty> res = value / other.value;
      __error_check();
      return res;
    }

    operator Ty() const { return value; }

  private:

    void __error_check() const {
      int fpe = fetestexcept(FE_ALL_EXCEPT);
      if (fpe) {
        if (fpe & FE_INEXACT) return;
          //throw std::range_error("FloatType::inexact");
        if (fpe & FE_INVALID)
          throw std::range_error("FloatType::invalid");
        if (fpe & FE_OVERFLOW)
          throw std::overflow_error("FloatType::overflow");
        if (fpe & FE_UNDERFLOW)
          throw std::underflow_error("FloatType::underflow");
        if (fpe & FE_DIVBYZERO)
          throw std::runtime_error("FloatType::divide by zero");
        throw std::runtime_error("FloatType::unknown error");
      }
    }

    Ty value;
  };

  typedef FloatType<float> Float;
  typedef FloatType<double> Double;
  typedef FloatType<long double> LongDouble;
}

template <typename Ty>
std::ostream &operator <<(std::ostream &out,
                          const numeric::Integer<Ty> &value) {
  out << (int)((Ty)value);
  return out;
}

template <typename Ty>
std::ostream &operator <<(std::ostream &out,
                          const numeric::FloatType<Ty> &value) {
  out << (Ty)((Ty)value);
  return out;
}

#endif /* _NUMERIC_H */

#endif /* __cplusplus < 201103L */
