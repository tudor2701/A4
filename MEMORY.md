# MaPra Aufgabe 3 (A4) — Projektgedächtnis

Dokumentation der Bearbeitung: was die Aufgabe verlangt, welche Entscheidungen
wir getroffen haben, der Dialog dahinter, und wie gebaut/getestet wird.

Stand: 2026-06-06. Abgabe: Fr 12.06.2026.

---

## 1. Aufgabenstellung (Kurzfassung von `ma3.pdf`)

C++-Bibliothek für dünnbesetzte Matrizen plus zwei iterative Löser für `Ax=b`:

- **Gesamtschrittverfahren** (Jacobi, `GSV`) — Algorithmus 1.
- **CG-Verfahren** (konjugierte Gradienten, `CG`) für symmetrisch positiv
  definite Matrizen — Algorithmus 2.

Dünnbesetzte Matrix wird über eine **Hashtabelle** (`std::unordered_map`) mit
`coord_t = pair<size_t,size_t>` als Schlüssel gespeichert. Vorgegeben:
`vector.h` (fertig), `sparse_matrix.h` (Schnittstelle, **darf nicht geändert
werden**), `mapra_test.h`, `unit.h`, sowie vorkompilierte Object-Files
`test_mv.o` / `unit.o`.

Zu erstellen: `sparse_matrix.cpp`, die Löser, eigene Unit-Tests, Makefile.

---

## 2. Was wir gebaut haben

| Datei | Inhalt |
|-------|--------|
| `Code/sparse_matrix.cpp` | komplette `SparseMatrix`-Klasse |
| `Code/loeser.cpp` | `mapra::GSV` + `mapra::CG` + Treiber-`main()` |
| `Code/test_sparse.cpp` | 22 eigene `MapraTest`-Unit-Tests |
| `Code/Makefile` | leere Targets `loesung`/`loeser.o` gefüllt + Plattform-Override |

### sparse_matrix.cpp — Designpunkte
- **Kernregel: nie einen Eintrag mit Wert 0 in der Hashmap speichern.**
  `Put(i,j,0.0)` → `mat_.erase({i,j})`; sonst `mat_[{i,j}] = x`.
- Lesen via `mat_.find` (kein `operator[]`, das würde Nullen einfügen und ist
  nicht const). Fehlt der Schlüssel → `0.0`.
- `+=`/`-=` setzen Ergebnis über `Put`, sodass entstehende Nullen automatisch
  rausfallen. `*=(double)` mit `c==0` → `mat_.clear()`.
- `==` vergleicht direkt die Hashmaps (zulässig, da garantiert keine Nullen).
- Matrix-Vektor-Produkte als Friend-`operator*`, iterieren nur über die
  Nicht-Null-Einträge: `A*x` → `y(i) += a_ij·x(j)`; `x*A` (= `Aᵀx`) →
  `y(j) += a_ij·x(i)`.

### loeser.cpp — Designpunkte
- Signaturen exakt wie in der Aufgabe (S. 7). Rückgabe: `>0` = Iterationen bei
  Konvergenz, `0` = `k_max` erreicht, `-1` = Dimensionsfehler. `eps` rein =
  gewünschtes Residuum, raus = erreichtes (`‖r‖₂`).
- **GSV**: Diagonale `d` sichern, `A` auf `A_L+A_R` reduzieren
  (`A.Put(i,i,0)`), strenge Diagonaldominanz (Satz 1) **vor** der Iteration
  prüfen → Warnung. Iteration `(b - A·x)/d`, Residuum `(b - A·x) - d%x`
  (nutzt die in `vector.h` vorhandenen Operatoren `/` als `D⁻¹` und `%` als
  komponentenweise Multiplikation). **`A` vor Rücksprung wiederherstellen.**
- **CG**: `A` bleibt const, Symmetrie prüfen → Warnung, nur die Vektoren
  `x, r, d, Ad` (keine zusätzlichen Felder, wie gefordert).
- **Treiber**: löst jedes Beispiel mit *beiden* Verfahren — siehe Entscheidung
  unten.

---

## 3. Dialog & Entscheidungen

1. **Auftrag**: „Lies `ma3.pdf`, analysiere, sag mir was zu tun ist, mach einen
   Plan, halte dich exakt an die Aufgabenstellung." → Planmodus.

2. **Analyse der Object-Files** (`nm`, `otool`) lieferte Fakten, die nicht im
   Quelltext stehen:
   - `test_mv.o` hat `main` → linkt mit `sparse_matrix.o` zum Test-Programm.
   - `unit.o` liefert nur `GetExample` + `CheckSolution`, **kein `main`** und
     ruft die Löser **nicht** → Treiber und Löser sind unsere Sache.
   - `GetExample(ex_id, …)`: Beispiel-IDs **0-basiert, gültig 0..2**
     (`cmp w0,#3; b.hs <err>`), `kNumberOfExamples == 3`.

3. **Offene Designfrage** (per AskUserQuestion): Wie verteilt `main` die
   Beispiele auf GSV/CG? Antwort des Nutzers: *„Was empfiehlst du?"*
   → Empfehlung zunächst **Auto-Dispatch nach Matrixtyp**.

4. **Korrektur durch Binär-Analyse**: `CheckSolution` enthält die Strings
   *„Matrix nicht symmetrisch … wird daher nicht getestet"* und
   *„Matrix nicht diagonaldominant … wird daher nicht getestet"*. Heißt:
   **`CheckSolution` prüft die Voraussetzung selbst und überspringt nicht
   passende Verfahren.** → Sauberer und offensichtlich gewollt: **jedes
   Beispiel mit beiden Verfahren lösen**, CheckSolution gated intern. Treiber
   entsprechend umgesetzt (statt eigenem Auto-Dispatch).

5. **Plattform**: Maschine ist **arm64**, vorgegebene `.o` sind Linux x86-64
   (für das Lernraum-Testat). Lokal die `*_macM1.o`-Varianten nutzen.

---

## 4. Bauen & Testen

```bash
cd Code
make clean
# Lokal (Apple Silicon):
make TEST_MV_O=test_mv_macM1.o UNIT_O=unit_macM1.o   # baut test_mv + loesung
./test_mv                                            # vorgegebene Tests
./loesung                                            # GSV/CG auf 3 Beispielen
make test_sparse && ./test_sparse                    # eigene Unit-Tests
```

Im Lernraum (Linux): einfach `make` — die Defaults `test_mv.o`/`unit.o` greifen.
`make clean` löscht **nur** unsere eigenen Objekte/Programme, nicht die nicht
regenerierbaren vorkompilierten `.o`.

### Verifizierte Ergebnisse (lokal, arm64)
- `test_mv`: **alle Tests bestanden**, keine Compilerwarnung (`-Wall -Wextra
  -Wpedantic`).
- `loesung`:
  - Bsp 0 (diag.dom. + symm.): GSV 33 Iter, CG 2 Iter — beide OK.
  - Bsp 1 (diag.dom., nicht symm.): GSV 228 Iter OK; CG → Warnung, von
    CheckSolution übersprungen.
  - Bsp 2 (2D-Poisson 6×6, symm./spd, nicht streng diag.dom.): GSV → Warnung,
    übersprungen; CG 6 Iter, Residuum 1e-16 — OK.
- `test_sparse`: **22/22 OK** → `[CONGRATS]`.

---

## 5. Offene Punkte / Hinweise
- `operator>>`-Eingabeformat ist frei gewählt (Dim + Tripel), nicht getestet.
- Abgabe: Quellcode als Gruppe in den Lernraum hochladen. `git` wird in den
  Testaten geprüft — dieses Verzeichnis ist noch **kein** Git-Repo (`git init`
  bei Bedarf).
- `sparse_matrix.h` bleibt unverändert; Makefile-Defaults bleiben Linux.
