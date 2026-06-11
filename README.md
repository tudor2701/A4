# MaPra Aufgabe 3 — Dünnbesetzte Matrizen & iterative Löser

Lösung der dritten Aufgabe des Mathematischen Praktikums (MaPra, RWTH Aachen):
eine C++-Bibliothek für **dünnbesetzte Matrizen** (Sparse Matrix) auf Basis
einer Hashtabelle sowie zwei **iterative Verfahren** zur Lösung linearer
Gleichungssysteme `Ax = b`:

- **Gesamtschrittverfahren** (Jacobi-Verfahren, `GSV`)
- **CG-Verfahren** (konjugierte Gradienten, `CG`) für symmetrisch positiv
  definite Matrizen

Die vollständige Aufgabenstellung liegt als [`ma3.pdf`](ma3.pdf) bei.

---

## Inhaltsverzeichnis

- [Projektstruktur](#projektstruktur)
- [Was das Programm macht](#was-das-programm-macht)
- [Repository klonen](#repository-klonen)
- [Voraussetzungen](#voraussetzungen)
- [Bauen](#bauen)
- [Ausführen](#ausführen)
- [Designentscheidungen](#designentscheidungen)
- [Verifizierte Ergebnisse](#verifizierte-ergebnisse)

---

## Projektstruktur

```
A4/
├── README.md              # diese Datei
├── ma3.pdf                # Aufgabenstellung
├── MEMORY.md              # Projektgedächtnis (Entscheidungen, Dialog, Doku)
└── Code/
    ├── sparse_matrix.h    # Schnittstelle SparseMatrix (vorgegeben, unverändert)
    ├── sparse_matrix.cpp  # eigene Implementierung der SparseMatrix-Klasse
    ├── loeser.cpp         # GSV + CG + Treiber-main()
    ├── test_sparse.cpp    # 22 eigene Unit-Tests
    ├── vector.h           # Vektorklasse (vorgegeben)
    ├── unit.h             # GetExample / CheckSolution (vorgegeben)
    ├── mapra_test.h       # Test-Framework (vorgegeben)
    ├── Makefile           # Build-Regeln inkl. Plattform-Override
    ├── test_mv.o          # vorkompiliert: Test Matrix/Vektor (Linux x86-64)
    ├── unit.o             # vorkompiliert: Beispiele + Prüfer (Linux x86-64)
    ├── test_mv_macM1.o    # dito für Apple Silicon (arm64)
    └── unit_macM1.o       # dito für Apple Silicon (arm64)
```

Vorgegeben und **nicht verändert**: `sparse_matrix.h`, `vector.h`, `unit.h`,
`mapra_test.h` sowie die vorkompilierten Object-Files. Eigenleistung:
`sparse_matrix.cpp`, `loeser.cpp`, `test_sparse.cpp`, `Makefile`.

## Was das Programm macht

### 1. `SparseMatrix` — dünnbesetzte Matrix per Hashtabelle

Die Klasse `mapra::SparseMatrix` speichert nur die **Nicht-Null-Einträge**
einer Matrix in einer `std::unordered_map` mit dem Koordinatenpaar
`coord_t = std::pair<size_t, size_t>` als Schlüssel (eigene Hashfunktion
`coord_hash`). Das spart bei dünnbesetzten Matrizen (z. B. aus
Finite-Differenzen-Diskretisierungen) massiv Speicher und Rechenzeit.

Unterstützte Operationen:

| Operation | Bedeutung |
|---|---|
| `Put(i, j, x)` / `Get(i, j)` / `operator()(i, j)` | Element schreiben/lesen |
| `+=`, `-=`, `*=` (Matrix), `*=`, `/=` (Skalar) | Arithmetik in-place |
| `+`, `-` (binär/unär), `*`, `/` | Arithmetik als freie Operatoren |
| `A * x` und `x * A` (= `Aᵀx`) | Matrix-Vektor-Produkte, iterieren nur über Nicht-Null-Einträge |
| `==`, `!=` | Vergleich |
| `<<`, `>>` | Aus-/Eingabe |
| `Redim(r, c)`, `GetRows()`, `GetCols()` | Dimensionen |

**Kerninvariante:** Es wird nie ein Eintrag mit Wert `0` in der Hashmap
gespeichert. `Put(i, j, 0.0)` löscht den Schlüssel; Lesen eines fehlenden
Schlüssels liefert `0.0`. Dadurch ist u. a. der `==`-Vergleich direkt über
die Hashmaps möglich.

### 2. Iterative Löser (`loeser.cpp`)

Beide Löser geben zurück: Anzahl der Iterationen bei Konvergenz (`> 0`),
`0` falls `k_max` erreicht wurde, `-1` bei Dimensionsfehler. Der Parameter
`eps` ist beim Aufruf das gewünschte Residuum und enthält nach Rückkehr das
tatsächlich erreichte Residuum `‖r‖₂`.

- **GSV (Jacobi):** prüft vor der Iteration strenge Diagonaldominanz
  (hinreichendes Konvergenzkriterium, Satz 1 der Aufgabe) und warnt, falls
  nicht erfüllt. Iterationsvorschrift `x⁽ᵏ⁺¹⁾ = D⁻¹(b − (A_L + A_R)x⁽ᵏ⁾)`.
- **CG (konjugierte Gradienten):** prüft Symmetrie der Matrix und warnt,
  falls nicht symmetrisch. Verwendet wie gefordert nur die Vektoren
  `x, r, d, Ad`; die Matrix `A` bleibt `const`.

### 3. Treiber (`main` in `loeser.cpp`)

Das Programm `loesung` holt sich über `GetExample` nacheinander **drei
Beispiele** (IDs 0–2) und löst **jedes mit beiden Verfahren**. Die
vorgegebene Prüfroutine `CheckSolution` testet die Voraussetzungen selbst und
überspringt ein Verfahren, wenn die Matrix dafür ungeeignet ist (z. B. CG bei
unsymmetrischer Matrix).

### 4. Tests

- `test_mv` — vorgegebenes Testprogramm (aus `test_mv.o`), prüft die gesamte
  `SparseMatrix`-Funktionalität.
- `test_sparse` — 22 eigene Unit-Tests mit dem `MapraTest`-Framework
  (Konstruktor, Put/Get, Null-Invariante, Arithmetik, Vergleiche,
  Matrix-Vektor-Produkte, Redim, Fehlerfälle).

## Repository klonen

```bash
# per HTTPS
git clone https://github.com/tudor2701/A4.git

# oder per SSH (SSH-Key bei GitHub hinterlegt)
git clone git@github.com:tudor2701/A4.git

# oder mit der GitHub CLI
gh repo clone tudor2701/A4

# danach ins Code-Verzeichnis wechseln
cd A4/Code
```

## Voraussetzungen

- C++17-fähiger Compiler (`g++` oder `clang++`)
- `make`
- Plattform passend zu den vorkompilierten Object-Files:
  - **Linux x86-64** → Standard-Object-Files `test_mv.o` / `unit.o`
  - **macOS Apple Silicon (arm64)** → Varianten `test_mv_macM1.o` / `unit_macM1.o`

Kompiliert wird mit `-std=c++17 -Wall -Wextra -Wpedantic -O2` — der Code ist
warnungsfrei.

## Bauen

Alle Befehle im Verzeichnis `Code/` ausführen.

### Linux (z. B. Lernraum-/Testat-Umgebung)

```bash
make            # baut loesung und test_mv
```

### macOS Apple Silicon (M1/M2/M3/M4)

Die vorgegebenen Object-Files sind standardmäßig Linux x86-64. Lokal müssen
die arm64-Varianten übersteuert werden:

```bash
make TEST_MV_O=test_mv_macM1.o UNIT_O=unit_macM1.o
```

### Eigene Unit-Tests bauen

```bash
make test_sparse
```

### Aufräumen

```bash
make clean      # löscht nur eigene Objekte/Programme, nie die vorgegebenen .o
```

## Ausführen

```bash
./loesung       # löst die 3 Beispiele mit GSV und CG, prüft via CheckSolution
./test_mv       # vorgegebene Tests der SparseMatrix-Klasse
./test_sparse   # eigene Unit-Tests (22 Stück)
```

Typischer Komplett-Durchlauf lokal auf Apple Silicon:

```bash
cd Code
make clean
make TEST_MV_O=test_mv_macM1.o UNIT_O=unit_macM1.o
./test_mv
./loesung
make test_sparse && ./test_sparse
```

`loesung` benötigt keine Argumente und liest keine Eingaben — die Beispiele
kommen aus `unit.o`/`unit_macM1.o`. Pro Beispiel werden Iterationszahl und
erreichtes Residuum ausgegeben; `CheckSolution` meldet, ob die Lösung
akzeptiert wurde.

## Designentscheidungen

- **Null-Invariante:** kein `0.0`-Eintrag landet je in der Hashmap (siehe
  oben). `*= 0.0` leert die Map komplett.
- **Lesen ohne Einfügen:** Elementzugriff über `find`, nicht `operator[]` —
  letzteres würde Nullen einfügen und wäre nicht `const`-fähig.
- **GSV stellt `A` wieder her:** für die Iteration wird die Diagonale
  temporär aus `A` entfernt (`A = A_L + A_R`), vor dem Rücksprung aber
  garantiert wieder eingesetzt.
- **Beide Verfahren pro Beispiel:** `CheckSolution` enthält die Logik, nicht
  passende Verfahren zu überspringen („Matrix nicht symmetrisch … wird daher
  nicht getestet") — daher kein eigener Dispatch im Treiber.
- **Beispiel-IDs sind 0-basiert** (gültig 0–2, `kNumberOfExamples == 3`).

Ausführliche Begründungen und der Bearbeitungsverlauf stehen in
[`MEMORY.md`](MEMORY.md).

## Verifizierte Ergebnisse

Lokal (arm64) verifiziert:

| Beispiel | Eigenschaften | GSV | CG |
|---|---|---|---|
| 0 | diagonaldominant, symmetrisch | 33 Iterationen ✓ | 2 Iterationen ✓ |
| 1 | diagonaldominant, nicht symmetrisch | 228 Iterationen ✓ | Warnung, übersprungen |
| 2 | 2D-Poisson 6×6, spd, nicht streng diag.dom. | Warnung, übersprungen | 6 Iterationen, Residuum ≈ 1e-16 ✓ |

`test_mv`: alle Tests bestanden. `test_sparse`: 22/22 OK.
