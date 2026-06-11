// Copyright (c) 2023, The MaPra Authors.

#ifndef VECTOR_H_
#define VECTOR_H_

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <vector>

namespace mapra {

template <typename T = double>
class Vector {
 public:
  explicit Vector(std::size_t len = 1) : elems_(len, 0) {}

  T& operator()(std::size_t i) {
#ifndef NDEBUG
    if (i >= elems_.size()) {
      mapra::Vector<T>::VecError("Ungueltiger Index!");
    }
#endif
    return elems_[i];
  }
  T operator()(std::size_t i) const {
#ifndef NDEBUG
    if (i >= elems_.size()) {
      mapra::Vector<T>::VecError("Ungueltiger Index!");
    }
#endif
    return elems_[i];
  }

  Vector<T>& operator+=(const Vector<T>& x) {
#ifndef NDEBUG
    if (elems_.size() != x.elems_.size()) {
      mapra::Vector<T>::VecError(
          "Inkompatible Dimensionen fuer 'Vektor += Vektor'!");
    }
#endif
    for (size_t i = 0; i < elems_.size(); i++) {
      (*this)(i) += x(i);
    }
    return *this;
  }
  Vector<T>& operator-=(const Vector<T>& x) {
#ifndef NDEBUG
    if (elems_.size() != x.elems_.size()) {
      mapra::Vector<T>::VecError(
          "Inkompatible Dimensionen fuer 'Vektor -= Vektor'!");
    }
#endif
    for (size_t i = 0; i < elems_.size(); i++) {
      (*this)(i) -= x(i);
    }
    return *this;
  }
  Vector<T>& operator*=(T c) {
    for (size_t i = 0; i < elems_.size(); i++) {
      (*this)(i) *= c;
    }
    return *this;
  }
  Vector<T>& operator/=(T c) {
    for (size_t i = 0; i < elems_.size(); i++) {
      (*this)(i) /= c;
    }
    return *this;
  }

  Vector<T>& Redim(std::size_t l) {
#ifndef NDEBUG
    if (l == 0) {
      mapra::Vector<T>::VecError("Nur Vektoren mit positiver Laenge!");
    }
#endif
    elems_ = std::vector<T>(l, 0);
    return *this;
  }
  std::size_t GetLength() const { return elems_.size(); }
  T Norm2() const { return std::sqrt((*this) * (*this)); }
  T NormMax() const {
    return std::abs(
        *std::max_element(elems_.begin(), elems_.end(),
                          [](T a, T b) { return std::abs(a) < std::abs(b); }));
  }

  static void VecError(const char str[]) {
    std::cerr << "\nVektorfehler: " << str << '\n' << std::endl;
    exit(1);
  }

  friend Vector<T> operator+(const Vector<T>& x, const Vector<T>& y) {
#ifndef NDEBUG
    if (x.elems_.size() != y.elems_.size()) {
      mapra::Vector<T>::VecError(
          "Inkompatible Dimensionen fuer 'Vektor + Vektor'!");
    }
#endif
    mapra::Vector<T> z = x;
    return z += y;
  }
  friend Vector<T> operator-(const Vector<T>& x, const Vector<T>& y) {
#ifndef NDEBUG
    if (x.elems_.size() != y.elems_.size()) {
      mapra::Vector<T>::VecError(
          "Inkompatible Dimensionen fuer 'Vektor - Vektor'!");
    }
#endif
    mapra::Vector<T> z = x;
    return z -= y;
  }
  friend Vector<T> operator-(const Vector<T>& x) {
    mapra::Vector<T> z(x.elems_.size());
    return z -= x;
  }

  friend T operator*(const Vector<T>& x, const Vector<T>& y) {
    T c = T{0};
#ifndef NDEBUG
    if (x.elems_.size() != y.elems_.size()) {
      mapra::Vector<T>::VecError(
          "Inkompatible Dimensionen fuer 'Vektor * Vektor'!");
    }
#endif
    for (size_t i = 0; i < x.elems_.size(); i++) {
      c += x(i) * y(i);
    }
    return c;
  }
  friend Vector<T> operator*(T c, const Vector<T>& x) {
    mapra::Vector<T> z = x;
    return z *= c;
  }
  friend Vector<T> operator*(const Vector<T>& x, T c) {
    mapra::Vector<T> z = x;
    return z *= c;
  }
  friend Vector<T> operator/(const Vector<T>& x, T c) {
    mapra::Vector<T> z = x;
    return z /= c;
  }

  // Division Vector/Vector "/"  <-->  D^(-1)*x
  friend Vector<T> operator/(const Vector<T>& x, const Vector<T>& d) {
    Vector<T> z = x;
    for (size_t i = 0; i < z.GetLength(); ++i) z(i) /= d(i);
    return z;
  }

  // Vector*.Vector "%"  <--> komponentenweise Multiplikation

  friend Vector<T> operator%(const Vector<T>& x, const Vector<T>& d) {
    Vector<T> z = x;
    for (size_t i = 0; i < z.GetLength(); ++i) z(i) *= d(i);
    return z;
  }

  friend bool operator==(const Vector<T>& x, const Vector<T>& y) {
    if (x.elems_.size() != y.elems_.size()) {
      return false;
    }
    for (std::size_t i = 0; i < x.elems_.size(); i++) {
      if (x(i) != y(i)) {
        return false;
      }
    }
    return true;
  }
  friend bool operator!=(const Vector<T>& x, const Vector<T>& y) {
    return !(x == y);
  }

  friend std::istream& operator>>(std::istream& s, Vector<T>& x) {
    std::cout << std::setiosflags(std::ios::right);
    for (size_t i = 0; i < x.elems_.size(); i++) {
      std::cout << "\n(" << std::setw(4) << i << ") " << std::flush;
      s >> x(i);
    }
    return s;
  }
  friend std::ostream& operator<<(std::ostream& s, const Vector<T>& x) {
    s << std::setiosflags(std::ios::right);
    for (std::size_t i = 0; i < x.elems_.size(); i++) {
      s << "\n(" << std::setw(4) << i << ") " << x(i);
    }
    return s << std::endl;
  }

 private:
  std::vector<T> elems_;
};

}  // namespace mapra

#endif  // VECTOR_H_
