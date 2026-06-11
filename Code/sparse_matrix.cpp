// Copyright (c) 2023, The MaPra Authors.

#include "sparse_matrix.h"

#include <cstdlib>

namespace mapra {

// ----------------------------------------------------------------------------
// Konstruktor und Fehlerbehandlung
// ----------------------------------------------------------------------------

SparseMatrix::SparseMatrix(size_t r, size_t c) : rows_(r), cols_(c), mat_() {}

void SparseMatrix::MatError(const char str[]) {
  std::cerr << "\nMatrixfehler: " << str << '\n' << std::endl;
  exit(1);
}

// ----------------------------------------------------------------------------
// Elementzugriff
// ----------------------------------------------------------------------------

void SparseMatrix::Put(size_t i, size_t j, double x) {
  if (i >= rows_ || j >= cols_) {
    MatError("Ungueltiger Index bei Put!");
  }
  if (x == 0.0) {
    mat_.erase({i, j});  // Niemals einen Eintrag mit Wert Null speichern.
  } else {
    mat_[{i, j}] = x;
  }
}

double SparseMatrix::operator()(size_t i, size_t j) const {
  if (i >= rows_ || j >= cols_) {
    MatError("Ungueltiger Index!");
  }
  HashMap::const_iterator it = mat_.find({i, j});  // kein operator[] -> const
  return it == mat_.end() ? 0.0 : it->second;
}

double SparseMatrix::Get(size_t i, size_t j) const { return (*this)(i, j); }

// ----------------------------------------------------------------------------
// Compound-Operatoren
// ----------------------------------------------------------------------------

SparseMatrix& SparseMatrix::operator+=(const SparseMatrix& B) {
  if (rows_ != B.rows_ || cols_ != B.cols_) {
    MatError("Inkompatible Dimensionen fuer 'Matrix += Matrix'!");
  }
  for (const auto& entry : B.mat_) {
    const size_t i = entry.first.first;
    const size_t j = entry.first.second;
    Put(i, j, (*this)(i, j) + entry.second);  // Put entfernt entstehende Nullen
  }
  return *this;
}

SparseMatrix& SparseMatrix::operator-=(const SparseMatrix& B) {
  if (rows_ != B.rows_ || cols_ != B.cols_) {
    MatError("Inkompatible Dimensionen fuer 'Matrix -= Matrix'!");
  }
  for (const auto& entry : B.mat_) {
    const size_t i = entry.first.first;
    const size_t j = entry.first.second;
    Put(i, j, (*this)(i, j) - entry.second);
  }
  return *this;
}

SparseMatrix& SparseMatrix::operator*=(const SparseMatrix& B) {
  if (cols_ != B.rows_) {
    MatError("Inkompatible Dimensionen fuer 'Matrix *= Matrix'!");
  }
  SparseMatrix C(rows_, B.cols_);
  for (const auto& a : mat_) {
    for (const auto& b : B.mat_) {
      if (a.first.second == b.first.first) {  // a_{ik} * b_{kj}
        const size_t i = a.first.first;
        const size_t j = b.first.second;
        C.Put(i, j, C(i, j) + a.second * b.second);
      }
    }
  }
  *this = C;
  return *this;
}

SparseMatrix& SparseMatrix::operator*=(double c) {
  if (c == 0.0) {
    mat_.clear();  // Ergebnis ist die Nullmatrix
  } else {
    for (auto& entry : mat_) {
      entry.second *= c;
    }
  }
  return *this;
}

SparseMatrix& SparseMatrix::operator/=(double c) {
  if (c == 0.0) {
    MatError("Division durch Null!");
  }
  for (auto& entry : mat_) {
    entry.second /= c;
  }
  return *this;
}

SparseMatrix& SparseMatrix::Redim(size_t r, size_t c) {
  rows_ = r;
  cols_ = c;
  mat_.clear();  // Neue Nullmatrix
  return *this;
}

// ----------------------------------------------------------------------------
// Binaere Operatoren (Friends), abgeleitet aus den Compound-Operatoren
// ----------------------------------------------------------------------------

SparseMatrix operator+(const SparseMatrix& A, const SparseMatrix& B) {
  SparseMatrix C = A;
  return C += B;
}

SparseMatrix operator-(const SparseMatrix& A, const SparseMatrix& B) {
  SparseMatrix C = A;
  return C -= B;
}

SparseMatrix operator-(const SparseMatrix& A) {
  SparseMatrix C = A;
  return C *= -1.0;
}

SparseMatrix operator*(double c, const SparseMatrix& A) {
  SparseMatrix C = A;
  return C *= c;
}

SparseMatrix operator*(const SparseMatrix& A, double c) {
  SparseMatrix C = A;
  return C *= c;
}

SparseMatrix operator/(const SparseMatrix& A, double c) {
  SparseMatrix C = A;
  return C /= c;
}

// ----------------------------------------------------------------------------
// Vergleich
// ----------------------------------------------------------------------------

bool operator==(const SparseMatrix& A, const SparseMatrix& B) {
  // Da niemals Nullen gespeichert werden, genuegt der Vergleich der Hashmaps.
  return A.rows_ == B.rows_ && A.cols_ == B.cols_ && A.mat_ == B.mat_;
}

bool operator!=(const SparseMatrix& A, const SparseMatrix& B) {
  return !(A == B);
}

// ----------------------------------------------------------------------------
// Ein-/Ausgabe (Format: Zeilen Spalten, danach je Nicht-Null "i j wert")
// ----------------------------------------------------------------------------

std::istream& operator>>(std::istream& s, SparseMatrix& A) {
  size_t rows = 0, cols = 0, nnz = 0;
  s >> rows >> cols >> nnz;
  A.Redim(rows, cols);
  for (size_t k = 0; k < nnz; ++k) {
    size_t i = 0, j = 0;
    double x = 0.0;
    s >> i >> j >> x;
    A.Put(i, j, x);
  }
  return s;
}

std::ostream& operator<<(std::ostream& s, const SparseMatrix& A) {
  s << A.rows_ << ' ' << A.cols_ << ' ' << A.mat_.size() << '\n';
  for (const auto& entry : A.mat_) {
    s << entry.first.first << ' ' << entry.first.second << ' ' << entry.second
      << '\n';
  }
  return s;
}

// ----------------------------------------------------------------------------
// Matrix-Vektor-Produkte
// ----------------------------------------------------------------------------

Vector<double> operator*(const SparseMatrix& A, const Vector<double>& x) {
  if (A.cols_ != x.GetLength()) {
    SparseMatrix::MatError("Inkompatible Dimensionen fuer 'Matrix * Vektor'!");
  }
  Vector<double> y(A.rows_);  // mit Nullen initialisiert
  for (const auto& entry : A.mat_) {
    y(entry.first.first) += entry.second * x(entry.first.second);
  }
  return y;
}

Vector<double> operator*(const Vector<double>& x, const SparseMatrix& A) {
  // x * A  entspricht  A^T * x
  if (x.GetLength() != A.rows_) {
    SparseMatrix::MatError("Inkompatible Dimensionen fuer 'Vektor * Matrix'!");
  }
  Vector<double> y(A.cols_);
  for (const auto& entry : A.mat_) {
    y(entry.first.second) += entry.second * x(entry.first.first);
  }
  return y;
}

}  // namespace mapra
