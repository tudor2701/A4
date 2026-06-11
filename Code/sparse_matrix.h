#ifndef SPARSE_MATRIX_H_
#define SPARSE_MATRIX_H_

#include <iostream>
#include <unordered_map>
#include <utility>

#include "sparse_matrix.h"
#include "vector.h"

namespace mapra {

using coord_t = std::pair<size_t, size_t>;

struct coord_hash {
  std::size_t operator()(coord_t key) const {
    std::hash<size_t> size_t_hash;
    return size_t_hash(key.first) + size_t_hash(key.second);
  }
};

using HashMap = std::unordered_map<coord_t, double, coord_hash>;

class SparseMatrix {
 private:
  std::size_t rows_;
  std::size_t cols_;
  HashMap mat_;

  static void MatError(const char str[]);

 public:
  explicit SparseMatrix(size_t r = 1, size_t c = 1);

  void Put(size_t i, size_t j, double x);
  double operator()(size_t, size_t) const;
  double Get(size_t i, size_t j) const;

  SparseMatrix& operator+=(const SparseMatrix&);
  SparseMatrix& operator-=(const SparseMatrix&);
  SparseMatrix& operator*=(const SparseMatrix&);
  SparseMatrix& operator*=(double);
  SparseMatrix& operator/=(double);

  SparseMatrix& Redim(size_t, size_t);
  size_t GetRows() const { return rows_; }
  size_t GetCols() const { return cols_; }

  friend SparseMatrix operator+(const SparseMatrix&, const SparseMatrix&);
  friend SparseMatrix operator-(const SparseMatrix&, const SparseMatrix&);
  friend SparseMatrix operator-(const SparseMatrix&);

  friend SparseMatrix operator*(double, const SparseMatrix&);
  friend SparseMatrix operator*(const SparseMatrix&, double);
  friend SparseMatrix operator/(const SparseMatrix&, double);

  friend bool operator==(const SparseMatrix&,
                         const SparseMatrix&);
  friend bool operator!=(const SparseMatrix&, const SparseMatrix&);

  friend std::istream& operator>>(std::istream&,
                                  SparseMatrix&);
  friend std::ostream& operator<<(std::ostream&,
                                  const SparseMatrix&);

  friend Vector<double> operator*(const SparseMatrix&, const Vector<double>&);
  friend Vector<double> operator*(const Vector<double>&, const SparseMatrix&);
};

SparseMatrix operator+(const SparseMatrix&, const SparseMatrix&);
SparseMatrix operator-(const SparseMatrix&, const SparseMatrix&);
SparseMatrix operator-(const SparseMatrix&);

SparseMatrix operator*(double, const SparseMatrix&);
SparseMatrix operator*(const SparseMatrix&, double);
SparseMatrix operator/(const SparseMatrix&, double);

bool operator==(const SparseMatrix&, const SparseMatrix&);
bool operator!=(const SparseMatrix&, const SparseMatrix&);

std::istream& operator>>(std::istream&, SparseMatrix&);
std::ostream& operator<<(std::ostream&, const SparseMatrix&);

Vector<double> operator*(const SparseMatrix&, const Vector<double>&);
Vector<double> operator*(const Vector<double>&, const SparseMatrix&);

}

#endif
