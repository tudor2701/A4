#include "sparse_matrix.h"

#include <cstdlib>

namespace mapra {


// Konstruktor: Sparsematrix besteht aus rows, cols und der Hashmap.
// Konstuktor setzt Dimensionen und initialisiert die Hashmap als leer.
// also es wird kein speicherplatz verbraucht, da die Hashmap leer ist.
SparseMatrix::SparseMatrix(size_t r, size_t c) : rows_(r), cols_(c), mat_() {} 

// MatError: Hilfsfunktion zur Fehlerbehandlung. Gibt Fehlermeldung aus und beendet das Programm.
void SparseMatrix::MatError(const char str[]) {
  std::cerr << "\nMatrixfehler: " << str << '\n' << std::endl;
  exit(1);
}

// Put: setze Element (i, j) auf Wert x
void SparseMatrix::Put(size_t i, size_t j, double x) {
  if (i >= rows_ || j >= cols_) {
    MatError("Ungueltiger Index bei Put!");
  }
  if (x == 0.0) {
    mat_.erase({i, j}); // wenn x = 0, dann wird Element aus HashMap entfernt, da es nicht gespeichert werden muss
  } else {
    mat_[{i, j}] = x; // sonst schrieben wir es rein, da es ein nicht-null Element ist.
  }
}

// liest Element (i, j) aus der Matrix. Wenn es nicht in der Hashmap ist, dann ist es 0.
double SparseMatrix::operator()(size_t i, size_t j) const {
  if (i >= rows_ || j >= cols_) {
    MatError("Ungueltiger Index!");
  }
  HashMap::const_iterator it = mat_.find({i, j}); // Suche nach Element (i, j) in der HashMap
  return it == mat_.end() ? 0.0 : it->second; // Wenn Element nicht gefunden wird, gibt es 0 zurück, sonst den gespeicherten Wert.
}


// rufe Klammer-Operator auf dem eigenen Objekt auf
double SparseMatrix::Get(size_t i, size_t j) const { return (*this)(i, j); }


// Matrixaddition: A += B
SparseMatrix& SparseMatrix::operator+=(const SparseMatrix& B) {
  if (rows_ != B.rows_ || cols_ != B.cols_) {
    MatError("Inkompatible Dimensionen fuer 'Matrix += Matrix'!");
  }
  for (const auto& entry : B.mat_) { // Iteriere über alle nicht-null Elemente von B
    const size_t i = entry.first.first; // Zeilenindex
    const size_t j = entry.first.second; // Spaltenindex
    Put(i, j, (*this)(i, j) + entry.second); // Addiere Wert von B zum passenden Wert von A und speichere es zurück in A
  }
  return *this;
}

// Matrixsubtraktion: A -= B
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

// Matrixmultiplikation: A *= B
SparseMatrix& SparseMatrix::operator*=(const SparseMatrix& B) {
  if (cols_ != B.rows_) {
    MatError("Inkompatible Dimensionen fuer 'Matrix *= Matrix'!");
  }
  SparseMatrix C(rows_, B.cols_); // Ergebnis-Matrix C mit der passenden Dimension für das Produkt A * B erstellen
  for (const auto& a : mat_) { // Iteriere über alle nicht-null Elemente von A
    for (const auto& b : B.mat_) { // Iteriere über alle nicht-null Elemente von B
      if (a.first.second == b.first.first) { // Überprüfe, ob die Spalte von A mit der Zeile von B übereinstimmt (für Matrixmultiplikation)
        const size_t i = a.first.first; // Zeilenindex von A
        const size_t j = b.first.second; // Spaltenindex von B
        C.Put(i, j, C(i, j) + a.second * b.second); // Berechne das Produkt der beiden Elemente und addiere es zum entsprechenden Element in C
      }
    }
  }
  *this = C;
  return *this;
}

// Skalar-Matrix-Multiplikation: A *= c
SparseMatrix& SparseMatrix::operator*=(double c) {
  if (c == 0.0) {
    mat_.clear();
  } else {
    for (auto& entry : mat_) { // Iteriere über alle nicht-null Elemente der Matrix
      entry.second *= c; // Multipliziere jedes Element mit dem Skalar c
    }
  }
  return *this;
}

// Skalar-Matrix-Division: A /= c
SparseMatrix& SparseMatrix::operator/=(double c) {
  if (c == 0.0) {
    MatError("Division durch Null!");
  }
  for (auto& entry : mat_) {
    entry.second /= c;
  }
  return *this;
}


// Redimensionierung der Matrix: Setze neue Anzahl von Zeilen und Spalten, lösche alle Elemente in der Hashmap, da die Matrix jetzt leer ist.
SparseMatrix& SparseMatrix::Redim(size_t r, size_t c) {
  rows_ = r;
  cols_ = c;
  mat_.clear(); // Alle Elemente entfernen, da die Matrix jetzt leer ist.
  return *this;
}


// Addition von zwei Matrizen: C = A + B
SparseMatrix operator+(const SparseMatrix& A, const SparseMatrix& B) {
  SparseMatrix C = A;
  return C += B;
}

// Subtraktion von zwei Matrizen: C = A - B
SparseMatrix operator-(const SparseMatrix& A, const SparseMatrix& B) {
  SparseMatrix C = A;
  return C -= B;
}


// Negation einer Matrix: C = -A
SparseMatrix operator-(const SparseMatrix& A) {
  SparseMatrix C = A;
  return C *= -1.0;
}

// Skalar-Matrix-Multiplikation: C = c * A
SparseMatrix operator*(double c, const SparseMatrix& A) {
  SparseMatrix C = A;
  return C *= c;
}


// Skalar-Matrix-Multiplikation: C = A * c
SparseMatrix operator*(const SparseMatrix& A, double c) {
  SparseMatrix C = A;
  return C *= c;
}


// Skalar-Matrix-Division: C = A / c
SparseMatrix operator/(const SparseMatrix& A, double c) {
  SparseMatrix C = A;
  return C /= c;
}

// Vergleich von zwei Matrizen: A == B
bool operator==(const SparseMatrix& A, const SparseMatrix& B) {

  return A.rows_ == B.rows_ && A.cols_ == B.cols_ && A.mat_ == B.mat_;
}


// Vergleich von zwei Matrizen: A != B
bool operator!=(const SparseMatrix& A, const SparseMatrix& B) {
  return !(A == B);
}


// Einlesen einer Matrix aus einem Stream: Format: Zeilen, Spalten, Anzahl der nicht-null Elemente, gefolgt von den nicht-null Elementen (i, j, Wert)
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


// Ausgabe einer Matrix in einem Stream: Format: Zeilen, Spalten, Anzahl der nicht-null Elemente, gefolgt von den nicht-null Elementen (i, j, Wert)
std::ostream& operator<<(std::ostream& s, const SparseMatrix& A) {
  s << A.rows_ << ' ' << A.cols_ << ' ' << A.mat_.size() << '\n';
  for (const auto& entry : A.mat_) {
    s << entry.first.first << ' ' << entry.first.second << ' ' << entry.second
      << '\n';
  }
  return s;
}


// Matrix-Vektor-Multiplikation: y = A * x
Vector<double> operator*(const SparseMatrix& A, const Vector<double>& x) {
  if (A.cols_ != x.GetLength()) {
    SparseMatrix::MatError("Inkompatible Dimensionen fuer 'Matrix * Vektor'!");
  }
  Vector<double> y(A.rows_);
  for (const auto& entry : A.mat_) {
    y(entry.first.first) += entry.second * x(entry.first.second);
  }
  return y;
}


// Vektor-Matrix-Multiplikation: y = x * A
Vector<double> operator*(const Vector<double>& x, const SparseMatrix& A) {

  if (x.GetLength() != A.rows_) {
    SparseMatrix::MatError("Inkompatible Dimensionen fuer 'Vektor * Matrix'!");
  }
  Vector<double> y(A.cols_);
  for (const auto& entry : A.mat_) {
    y(entry.first.second) += entry.second * x(entry.first.first);
  }
  return y;
}

}
