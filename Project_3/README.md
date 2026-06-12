# Kółko i Krzyżyk – C++ / SFML / Minimax α-β

## Wymagania

| Narzędzie | Wersja |
|-----------|--------|
| CMake     | ≥ 3.16 |
| Kompilator | GCC 9 / Clang 10 / MSVC 2019 (C++17) |
| SFML      | 2.6.x  |

### Instalacja SFML

**Ubuntu / Debian**
```bash
sudo apt install libsfml-dev
```

**macOS (Homebrew)**
```bash
brew install sfml
```

**Windows (vcpkg)**
```powershell
vcpkg install sfml:x64-windows
```

---

## Budowanie

```bash
mkdir build && cd build
cmake ..
cmake --build . --config Release
```

Executable pojawi się w katalogu `build/` (lub `build/Release/` na Windows).

---

## Uruchamianie

```bash
./TicTacToe
```

Na starcie program zapyta o rozmiar planszy (3–10).  
Po zakończeniu gry naciśnij **R** aby zrestartować i wybrać nowy rozmiar.

---

## Sterowanie

| Akcja | Sterowanie |
|-------|-----------|
| Ruch gracza | kliknięcie lewym przyciskiem myszy |
| Nowa gra / zmiana rozmiaru | klawisz `R` |
| Zamknięcie okna | `×` lub `Alt+F4` |

---

## Architektura

```
Board      – stan planszy, legalne ruchy, wykrywanie wygranej
AI         – minimax z cięciami α-β + heurystyka dla dużych plansz
Game       – zarządzanie turami, interfejs dla logiki
Renderer   – renderowanie SFML (siatka, X, O, linia wygranej, pasek statusu)
main       – pętla zdarzeń, wątek AI, prompt rozmiaru planszy
```

### Limity głębokości minimax

| Rozmiar | maxDepth |
|---------|----------|
| 3       | pełne przeszukanie (9) |
| 4       | 5 |
| 5       | 4 |
| 6+      | 3 |

### Heurystyka (dla węzłów na głębokości granicznej)

Każda linia (rząd, kolumna, przekątna) oceniana jest jako ±10^k:
- `k` = liczba symboli AI/gracza w linii
- linia mieszana (oba symbole) = 0 (zablokowana)

---

## Czcionka

Umieść plik `font.ttf` w katalogu `assets/`.  
Program automatycznie próbuje też systemowych czcionek (DejaVu, Helvetica, Arial).
