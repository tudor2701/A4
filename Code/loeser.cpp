#include <cmath>
#include <iostream>

#include "sparse_matrix.h"
#include "unit.h"
#include "vector.h"

namespace mapra {

int GSV(SparseMatrix& A, const Vector<double>& b, Vector<double>& x0,
        const int k_max, double& eps) {
  const std::size_t n = A.GetRows();
  if (A.GetCols() != n || b.GetLength() != n || x0.GetLength() != n) {
    return -1;
  }

  Vector<double> d(n);
  for (std::size_t i = 0; i < n; ++i) {
    d(i) = A(i, i);
    A.Put(i, i, 0.0);
  }

  bool diag_dominant = true;
  for (std::size_t i = 0; i < n && diag_dominant; ++i) {
    double off_sum = 0.0;
    for (std::size_t j = 0; j < n; ++j) {
      off_sum += std::abs(A(i, j));
    }
    if (std::abs(d(i)) <= off_sum) {
      diag_dominant = false;
    }
  }
  if (!diag_dominant) {
    std::cerr << "Warnung (GSV): Matrix ist nicht streng diagonaldominant - "
                 "Konvergenz nicht garantiert!\n";
  }

  const double tol = eps;
  Vector<double> x = x0;
  int k = 0;
  double res = 0.0;
  while (true) {
    const Vector<double> b_minus = b - A * x;
    res = (b_minus - (d % x)).Norm2();
    if (res < tol || k >= k_max) {
      break;
    }
    x = b_minus / d;
    ++k;
  }

  for (std::size_t i = 0; i < n; ++i) {
    A.Put(i, i, d(i));
  }

  x0 = x;
  eps = res;
  return (res < tol) ? k : 0;
}

int CG(const SparseMatrix& A, const Vector<double>& b, Vector<double>& x0,
       const int k_max, double& eps) {
  const std::size_t n = A.GetRows();
  if (A.GetCols() != n || b.GetLength() != n || x0.GetLength() != n) {
    return -1;
  }

  bool symmetric = true;
  for (std::size_t i = 0; i < n && symmetric; ++i) {
    for (std::size_t j = i + 1; j < n; ++j) {
      if (A(i, j) != A(j, i)) {
        symmetric = false;
        break;
      }
    }
  }
  if (!symmetric) {
    std::cerr << "Warnung (CG): Matrix ist nicht symmetrisch - Verfahren "
                 "moeglicherweise nicht anwendbar!\n";
  }

  const double tol = eps;
  Vector<double> x = x0;
  Vector<double> r = b - A * x;
  Vector<double> dvec = r;
  double rr = r * r;
  double res = std::sqrt(rr);
  int k = 0;
  while (res >= tol && k < k_max) {
    const Vector<double> Ad = A * dvec;
    const double alpha = rr / (Ad * dvec);
    x += alpha * dvec;
    r -= alpha * Ad;
    const double rr_new = r * r;
    ++k;
    res = std::sqrt(rr_new);
    if (res < tol) {
      break;
    }
    const double beta = rr_new / rr;
    dvec = r + beta * dvec;
    rr = rr_new;
  }

  x0 = x;
  eps = res;
  return (res < tol) ? k : 0;
}

}

int main() {
  using namespace mapra;

  for (int ex = 0; ex < kNumberOfExamples; ++ex) {
    SparseMatrix A;
    Vector<double> x0, b;
    double tol = 0.0;
    int max_iter = 0;

    GetExample(ex, A, x0, b, tol, max_iter);
    {
      Vector<double> x = x0;
      double eps = tol;
      const int iters = GSV(A, b, x, max_iter, eps);
      CheckSolution(x, iters, Method::GSV);
    }

    GetExample(ex, A, x0, b, tol, max_iter);
    {
      Vector<double> x = x0;
      double eps = tol;
      const int iters = CG(A, b, x, max_iter, eps);
      CheckSolution(x, iters, Method::CG);
    }
  }

  return 0;
}
