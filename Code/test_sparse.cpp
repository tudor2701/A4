// Copyright (c) 2023, The MaPra Authors.
//
// Eigene Unit-Tests fuer die Klasse SparseMatrix mithilfe von MapraTest.

#include "mapra_test.h"
#include "sparse_matrix.h"
#include "vector.h"

using mapra::SparseMatrix;
using mapra::Vector;

// Baut die Beispielmatrix
//   A = [[2,0,1],
//        [0,3,0],
//        [4,0,5]]
static SparseMatrix Example() {
  SparseMatrix a(3, 3);
  a.Put(0, 0, 2.0);
  a.Put(0, 2, 1.0);
  a.Put(1, 1, 3.0);
  a.Put(2, 0, 4.0);
  a.Put(2, 2, 5.0);
  return a;
}

int main() {
  mapra::MapraTest test("SparseMatrix");

  // --- Konstruktion / Dimensionen ---
  SparseMatrix a(3, 4);
  test.AssertEq<std::size_t>("rows", 3, a.GetRows());
  test.AssertEq<std::size_t>("cols", 4, a.GetCols());
  test.AssertEq<double>("init_zero", 0.0, a(2, 3));

  // --- Put / Get / operator() ---
  a.Put(1, 2, 7.5);
  test.AssertEq<double>("get_op", 7.5, a(1, 2));
  test.AssertEq<double>("get_fn", 7.5, a.Get(1, 2));
  test.AssertEq<double>("get_unset", 0.0, a(0, 0));

  // --- Null wird nicht gespeichert ---
  {
    SparseMatrix z(2, 2);
    z.Put(0, 1, 9.0);
    z.Put(0, 1, 0.0);  // soll den Eintrag wieder entfernen
    test.Assert("zero_not_stored", z == SparseMatrix(2, 2));
  }

  // --- Redim setzt auf Nullmatrix und neue Dimension ---
  a.Redim(2, 2);
  test.AssertEq<std::size_t>("redim_rows", 2, a.GetRows());
  test.AssertEq<std::size_t>("redim_cols", 2, a.GetCols());
  test.Assert("redim_zero", a == SparseMatrix(2, 2));

  // --- Arithmetik ---
  {
    SparseMatrix m = Example();
    SparseMatrix sum = m + m;
    test.AssertEq<double>("add", 4.0, sum(0, 0));
    test.AssertEq<double>("add2", 10.0, sum(2, 2));

    SparseMatrix diff = m - m;
    test.Assert("sub_zero", diff == SparseMatrix(3, 3));  // keine Nullen drin

    SparseMatrix scaled = 2.0 * m;
    test.AssertEq<double>("scal_mul", 8.0, scaled(2, 0));
    test.AssertEq<double>("scal_mul_r", 2.0, (m * 2.0)(0, 2));

    SparseMatrix divd = m / 2.0;
    test.AssertEq<double>("scal_div", 2.5, divd(2, 2));

    SparseMatrix neg = -m;
    test.AssertEq<double>("unary_minus", -3.0, neg(1, 1));
  }

  // --- Vergleich ---
  {
    SparseMatrix m1 = Example();
    SparseMatrix m2 = Example();
    test.Assert("equal", m1 == m2);
    m2.Put(1, 1, 99.0);
    test.Assert("not_equal", m1 != m2);
  }

  // --- Matrix-Vektor-Produkt A*x ---
  {
    SparseMatrix m = Example();
    Vector<double> x(3);
    x(0) = 1.0;
    x(1) = 2.0;
    x(2) = 3.0;

    Vector<double> ax = m * x;  // erwartet (5, 6, 19)
    Vector<double> expected_ax(3);
    expected_ax(0) = 5.0;
    expected_ax(1) = 6.0;
    expected_ax(2) = 19.0;
    test.Assert("matvec_Ax", ax == expected_ax);

    Vector<double> atx = x * m;  // x*A = A^T x, erwartet (14, 6, 16)
    Vector<double> expected_atx(3);
    expected_atx(0) = 14.0;
    expected_atx(1) = 6.0;
    expected_atx(2) = 16.0;
    test.Assert("matvec_ATx", atx == expected_atx);
  }

  // --- Matrix-Matrix-Produkt ---
  {
    SparseMatrix id(2, 2);
    id.Put(0, 0, 1.0);
    id.Put(1, 1, 1.0);
    SparseMatrix b(2, 2);
    b.Put(0, 1, 4.0);
    b.Put(1, 0, 5.0);
    SparseMatrix prod = id;
    prod *= b;  // I * B == B
    test.Assert("matmat_identity", prod == b);
  }

  return 0;  // Destruktor von MapraTest gibt die Zusammenfassung aus
}
