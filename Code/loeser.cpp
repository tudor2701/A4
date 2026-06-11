#include <cmath>
#include <iostream>

#include "sparse_matrix.h"
#include "unit.h"
#include "vector.h"

namespace mapra {


// Gauß-Seidel-Verfahren: Löse Ax = b iterativ, Startwert x0, max. Iterationen k_max, Toleranz eps
int GSV(SparseMatrix& A, const Vector<double>& b, Vector<double>& x0,
        const int k_max, double& eps) {
  const std::size_t n = A.GetRows(); // Dimension der Matrix A
  if (A.GetCols() != n || b.GetLength() != n || x0.GetLength() != n) { // Dimensionscheck
    return -1;
  }

  Vector<double> d(n); // Vektor für die Diagonalelemente von A
  for (std::size_t i = 0; i < n; ++i) { // Extrahiere die Diagonalelemente von A und speichere sie in d, setze die Diagonalelemente von A auf 0
    d(i) = A(i, i); // Speichere Diagonalelement in d
    A.Put(i, i, 0.0); // Setze Diagonalelement in A auf 0, da es im GSV-Verfahren separat behandelt wird
  }

  // Überprüfe, ob die Matrix streng diagonaldominant ist, was die Konvergenz des GSV-Verfahrens garantiert
  bool diag_dominant = true; 

  for (std::size_t i = 0; i < n && diag_dominant; ++i) { // Iteriere über alle Zeilen der Matrix
    double off_sum = 0.0; // Berechne die Summe der Beträge der Nicht-Diagonalelemente in der aktuellen Zeile
    for (std::size_t j = 0; j < n; ++j) { // Iteriere über alle Spalten der aktuellen Zeile
      off_sum += std::abs(A(i, j)); // Addiere den Betrag des aktuellen Nicht-Diagonalelements zur Summe hinzu
    }
    if (std::abs(d(i)) <= off_sum) { // Überprüfe, ob das Diagonalelement nicht größer als die Summe der Nicht-Diagonalelemente ist
      diag_dominant = false;
    }
  }
  if (!diag_dominant) {
    std::cerr << "Warnung (GSV): Matrix ist nicht streng diagonaldominant - "
                 "Konvergenz nicht garantiert!\n";
  }

  const double tol = eps; // Toleranz für Konvergenz
  Vector<double> x = x0; // Initialisiere den Lösungsvektor mit dem Startwert
  int k = 0; // Iterationszähler
  double res = 0.0; // Variable für die Residuennorm
  while (true) {
    const Vector<double> b_minus = b - A * x; // Berechne den Vektor b - A * x, der im GSV-Verfahren verwendet wird
    res = (b_minus - (d % x)).Norm2(); // Berechne die Residuennorm
    if (res < tol || k >= k_max) { // Überpüfe: residuennorm ist kleiner als Toleranz oder max Anzahl an Iterationen schon überschritten
      break;
    }
    x = b_minus / d; // Berechne den neuen Lösungsvektor x, indem du b_minus durch die Diagonalelemente d teilst (Elementweise Division)
    ++k; // Inkrementiere den Iterationszähler
  }

  for (std::size_t i = 0; i < n; ++i) { 
    A.Put(i, i, d(i)); // Setze die Diagonalelemente von A wieder zurück, da sie im GSV-Verfahren auf 0 gesetzt wurden
  }

  x0 = x; // Aktualisiere den Startvektor x0 mit der berechneten Lösung x
  eps = res; // Aktualisiere eps mit der berechneten Residuennorm, um die Genauigkeit der Lösung anzugeben
  return (res < tol) ? k : 0; // Rückgabe der Anzahl der Iterationen, falls die Lösung innerhalb der Toleranz liegt, ansonsten Rückgabe 0
}

int CG(const SparseMatrix& A, const Vector<double>& b, Vector<double>& x0,
       const int k_max, double& eps) {
  const std::size_t n = A.GetRows();
  if (A.GetCols() != n || b.GetLength() != n || x0.GetLength() != n) { // Dimensionscheck
    return -1;
  }

  // Überprüfe, ob die Matrix symmetrisch ist, was die Anwendbarkeit des CG-Verfahrens garantiert
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
    std::cerr << "Warnung (CG): Matrix ist nicht symmetrisch: Verfahren "
                 "möglicherweise nicht anwendbar!\n";
  }

  const double tol = eps;
  Vector<double> x = x0; // Initialisiere den Lösungsvektor mit dem Startwert
  Vector<double> r = b - A * x; // Berechne den Anfangsresiduum r = b - A * x
  Vector<double> dvec = r; // Initialisiere den Suchvektor d mit dem Anfangsresiduum r
  double rr = r * r; // Berechne das Skalarprodukt r * r, das im CG-Verfahren verwendet wird, um die Schrittweite zu berechnen
  double res = std::sqrt(rr); // Berechne die Residuennorm, um die Konvergenz zu überwachen
  int k = 0;
  while (res >= tol && k < k_max) { // während residuennorm großer als toleranz und max anzahl an iterationen nicht überschritten
    const Vector<double> Ad = A * dvec; // Berechne das Produkt A * d, das im CG-Verfahren verwendet wird, um die Schrittweite zu berechnen
    const double alpha = rr / (Ad * dvec); // Berechne die Schrittweite alpha, die bestimmt, wie weit in Richtung des Suchvektors d gegangen wird
    x += alpha * dvec; // Lösungsvektor aktualisieren
    r -= alpha * Ad; // Residuum aktualisieren
    const double rr_new = r * r; //Schrittweite für nächste Iteration berechnen
    ++k; // Iterationszähler inkrementieren
    res = std::sqrt(rr_new); // Residuennorm aktualisieren, um die Konvergenz zu überwachen
    if (res < tol) {
      break;
    }
    const double beta = rr_new / rr; // Berechne den Parameter beta, der bestimmt, wie stark der neue Suchvektor von dem alten abweicht
    dvec = r + beta * dvec; // Aktualisiere den Suchvektor d, indem du das neue Residuum r mit dem alten Suchvektor d gewichtet durch beta kombinierst
    rr = rr_new; // Aktualisiere rr für die nächste Iteration
  }

  x0 = x;
  eps = res;
  return (res < tol) ? k : 0; // Rückgabe der Anzahl der Iterationen, falls die Lösung innerhalb der Toleranz liegt, ansonsten Rückgabe 0
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
