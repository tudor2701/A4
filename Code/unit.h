// Copyright (c) 2023, The MaPra Authors.

#ifndef UNIT_H_
#define UNIT_H_

#include "sparse_matrix.h"
#include "vector.h"

namespace mapra {

inline constexpr int kNumberOfExamples = 3;

enum class Method { GSV = 0, CG = 1 };

void GetExample(int ex_id, SparseMatrix& A, Vector<double>& x0,
                Vector<double>& b, double& tol, int& max_iter);

void CheckSolution(const Vector<double>& x, int num_iterations,
                   mapra::Method method);

}  // namespace mapra

#endif  // UNIT_H_
