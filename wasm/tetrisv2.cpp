// use c++20; extracted from https://github.com/adrien1018/betatetris-tablebase/blob/main/src/move_search_no_tmpl.h

/// constexpr_helpers.h

#include <cstdint>
#include <memory>
#include <type_traits>
#include <immintrin.h>

#ifdef _MSC_VER
#define NOINLINE __declspec(noinline)
#else
#define NOINLINE __attribute__((noinline))
#endif

template <class T>
constexpr T pext(T a, T mask) {
  static_assert(
      std::is_same<T, uint32_t>::value ||
      std::is_same<T, uint64_t>::value,
      "not implemented");
#ifdef __BMI2__
  if (std::is_constant_evaluated()) {
#endif
    T res = 0;
    for (T bb = 1; mask != 0; bb <<= 1) {
      if (a & mask & -mask) res |= bb;
      mask &= (mask - 1);
    }
    return res;
#ifdef __BMI2__
  } else if constexpr(std::is_same<T, uint64_t>::value) {
    return _pext_u64(a, mask);
  } else {
    return _pext_u32(a, mask);
  }
#endif
}

template <class T>
constexpr T pdep(T a, T mask) {
  static_assert(
      std::is_same<T, uint32_t>::value ||
      std::is_same<T, uint64_t>::value,
      "not implemented");
#ifdef __BMI2__
  if (std::is_constant_evaluated()) {
#endif
    T res = 0;
    for (T bb = 1; mask; bb <<= 1) {
      if (a & bb) res |= mask & -mask;
      mask &= mask - 1;
    }
    return res;
#ifdef __BMI2__
  } else if constexpr(std::is_same<T, uint64_t>::value) {
    return _pdep_u64(a, mask);
  } else {
    return _pdep_u32(a, mask);
  }
#endif
}

template <class T>
constexpr int popcount(T a) {
  static_assert(
      std::is_same<T, uint32_t>::value ||
      std::is_same<T, uint64_t>::value,
      "not implemented");
#ifdef _MSC_VER
  return std::popcount(a);
#else
  return __builtin_popcountll(a);
#endif
}

template <class T>
constexpr int ctz(T a) {
  static_assert(
      std::is_same<T, uint32_t>::value ||
      std::is_same<T, uint64_t>::value,
      "not implemented");
#ifdef _MSC_VER
  return std::countr_zero(a);
#else
  if constexpr(std::is_same<T, uint64_t>::value) {
    return __builtin_ctzll(a);
  } else {
    return __builtin_ctz(a);
  }
#endif
}

template <class T>
constexpr int clz(T a) {
  static_assert(
      std::is_same<T, uint32_t>::value ||
      std::is_same<T, uint64_t>::value,
      "not implemented");
#ifdef _MSC_VER
  return std::countl_zero(a);
#else
  if constexpr(std::is_same<T, uint64_t>::value) {
    return __builtin_clzll(a);
  } else {
    return __builtin_clz(a);
  }
#endif
}

[[noreturn]] inline void unreachable() {
#ifdef __GNUC__ // GCC, Clang, ICC
  __builtin_unreachable();
#else
#ifdef _MSC_VER // MSVC
  __assume(false);
#endif
#endif
}

#define DO_PIECE_CASE(piece) \
  switch (piece) { \
    ONE_CASE(0) ONE_CASE(1) ONE_CASE(2) ONE_CASE(3) ONE_CASE(4) ONE_CASE(5) ONE_CASE(6) \
  } \
  unreachable();

/// game.h

enum Level {
  kLevel18,
  kLevel19,
  kLevel29,
  kLevel39
};

constexpr int kLineCap = 390;
constexpr int kLevelSpeedLines[] = {0, 130, 230, 330, kLineCap};

constexpr int GetLevelByLines(int lines) {
  if (lines < 130) return 18;
  return lines / 10 + 6;
}

constexpr Level GetLevelSpeed(int level) {
  if (level == 18) return kLevel18;
  if (level < 29) return kLevel19;
  if (level < 39) return kLevel29;
  return kLevel39;
}

#ifdef DOUBLE_TUCK
constexpr bool kDoubleTuckAllowed = true;
#else
constexpr bool kDoubleTuckAllowed = false;
#endif

/// board.h

#include <array>
#include <vector>
#include <stdexcept>
#include <algorithm>

constexpr size_t kPieces = 7;

class alignas(32) Board;
constexpr Board operator|(const Board& x, const Board& y);
constexpr Board operator&(const Board& x, const Board& y);

using ByteBoard = std::array<std::array<uint8_t, 10>, 20>;

// A 20x10 board is represented by 4 64-bit integers.
// Each integer represents 3 columns except b4. b1 is the leftmost 3 columns.
//   column 0 (leftmost): bit 0(topmost)-19(bottommost)
//   column 1: bit 22-41
//   column 2: bit 44-63
// A bit of 1 means an empty cell; 0 otherwise.
class alignas(32) Board {
 private:
  // 1 wide, offset = (2, 0)
  static constexpr uint64_t kIPiece1_ = 0xf;
  // 2 wide, offset = (0, 1)
  static constexpr uint64_t kOPiece_ = 0xc00003;
  // 2 wide, offset = (1, 0)
  static constexpr uint64_t kTPiece3_ = 0x800007;
  static constexpr uint64_t kJPiece3_ = 0x400007;
  static constexpr uint64_t kZPiece1_ = 0xc00006;
  static constexpr uint64_t kSPiece1_ = 0x1800003;
  static constexpr uint64_t kLPiece3_ = 0x1000007;
  // 2 wide, offset = (1, 1)
  static constexpr uint64_t kTPiece1_ = 0x1c00002;
  static constexpr uint64_t kJPiece1_ = 0x1c00004;
  static constexpr uint64_t kLPiece1_ = 0x1c00001;
  // 3 wide, offset = (0, 1)
  static constexpr uint64_t kTPiece0_ = 0x100000c00001;
  static constexpr uint64_t kJPiece0_ = 0x300000400001;
  static constexpr uint64_t kZPiece0_ = 0x200000c00001;
  static constexpr uint64_t kSPiece0_ = 0x100000c00002;
  static constexpr uint64_t kLPiece0_ = 0x100000400003;
  // 3 wide, offset = (1, 1)
  static constexpr uint64_t kTPiece2_ = 0x200000c00002;
  static constexpr uint64_t kJPiece2_ = 0x200000800003;
  static constexpr uint64_t kLPiece2_ = 0x300000800002;
  // 4 wide, offset = (0, 2)
  static constexpr uint64_t kIPiece0a_ = 0x100000400001;
  static constexpr uint64_t kIPiece0b_ = 0x400001;
  static constexpr uint64_t kIPiece0c_ = 0x1;

  constexpr Board Place1Wide_(uint64_t piece, int x, int y, int ox) const {
    Board r = *this;
    x -= ox;
    if (x < 0) {
      piece >>= -x;
      x = 0;
    }
    switch (y) {
      case 0: case 1: case 2: r.b1 &= ~(piece << (x + y * 22)); break;
      case 3: case 4: case 5: r.b2 &= ~(piece << (x + (y - 3) * 22)); break;
      case 6: case 7: case 8: r.b3 &= ~(piece << (x + (y - 6) * 22)); break;
      case 9: r.b4 &= ~(piece << x); break;
      default: unreachable();
    }
    return r;
  }
  constexpr Board Place2Wide_(uint64_t piece, int x, int y, int ox, int oy) const {
    Board r = *this;
    x -= ox;
    y -= oy;
    if (x < 0) {
      piece >>= -x;
      x = 0;
    }
    switch (y) {
      case 2: r.b2 &= ~(piece >> (22 - x)); // fallthrough
      case 0: case 1: r.b1 &= ~(piece << (x + y * 22)); break;
      case 5: r.b3 &= ~(piece >> (22 - x)); // fallthrough
      case 3: case 4: r.b2 &= ~(piece << (x + (y - 3) * 22)); break;
      case 8: r.b4 &= ~(piece >> (22 - x)); // fallthrough
      case 6: case 7: r.b3 &= ~(piece << (x + (y - 6) * 22)); break;
      default: unreachable();
    }
    return r;
  }
  constexpr Board Place3Wide_(uint64_t piece, int x, int y, int ox, int oy) const {
    Board r = *this;
    x -= ox;
    y -= oy;
    if (x < 0) {
      piece >>= -x;
      x = 0;
    }
    switch (y) {
      case 1: case 2: r.b2 &= ~(piece >> (66 - x - y * 22)); // fallthrough
      case 0: r.b1 &= ~(piece << (x + y * 22)); break;
      case 4: case 5: r.b3 &= ~(piece >> (66 - x - (y - 3) * 22)); // fallthrough
      case 3: r.b2 &= ~(piece << (x + (y - 3) * 22)); break;
      case 7: r.b4 &= ~(piece >> (44 - x)); // fallthrough
      case 6: r.b3 &= ~(piece << (x + (y - 6) * 22)); break;
      default: unreachable();
    }
    return r;
  }
  constexpr Board PlaceI0_(int x, int y) const {
    Board r = *this;
    y -= 2;
    switch (y) {
      case 0: r.b1 &= ~(kIPiece0a_ << x); r.b2 &= ~(kIPiece0c_ << x); break;
      case 1: r.b1 &= ~(kIPiece0b_ << (x + 22)); r.b2 &= ~(kIPiece0b_ << x); break;
      case 2: r.b1 &= ~(kIPiece0c_ << (x + 44)); r.b2 &= ~(kIPiece0a_ << x); break;
      case 3: r.b2 &= ~(kIPiece0a_ << x); r.b3 &= ~(kIPiece0c_ << x); break;
      case 4: r.b2 &= ~(kIPiece0b_ << (x + 22)); r.b3 &= ~(kIPiece0b_ << x); break;
      case 5: r.b2 &= ~(kIPiece0c_ << (x + 44)); r.b3 &= ~(kIPiece0a_ << x); break;
      case 6: r.b3 &= ~(kIPiece0a_ << x); r.b4 &= ~(kIPiece0c_ << x); break;
      default: unreachable();
    }
    return r;
  }
 public:
  static constexpr uint64_t kBoardMask = 0xfffff3ffffcfffffL;
  static constexpr uint32_t kColumnMask = 0xfffff;

  uint64_t b1, b2, b3, b4;

  Board() = default;
  constexpr Board(uint64_t b1, uint64_t b2, uint64_t b3, uint64_t b4) :
      b1(b1), b2(b2), b3(b3), b4(b4) {}

  constexpr Board(std::initializer_list<std::pair<int, int>> positions) :
      b1(kBoardMask), b2(kBoardMask), b3(kBoardMask), b4(kColumnMask) {
    for (auto& i : positions) SetCellFilled(i.first, i.second);
  }

  constexpr Board(const ByteBoard& board) : b1(), b2(), b3(), b4() {
    for (int i = 0; i < 20; i++) {
      for (int j = 0; j < 3; j++) b1 |= (uint64_t)board[i][j] << (j * 22 + i);
      for (int j = 3; j < 6; j++) b2 |= (uint64_t)board[i][j] << ((j-3) * 22 + i);
      for (int j = 6; j < 9; j++) b3 |= (uint64_t)board[i][j] << ((j-6) * 22 + i);
      b4 |= (uint64_t)board[i][9] << i;
    }
  }

  constexpr Board(std::string_view sv) :
      b1(kBoardMask), b2(kBoardMask), b3(kBoardMask), b4(kColumnMask) {
    int rows = (sv.size() + 1) / 11;
    for (int i = 0; i < rows; i++) {
      int x = 20 - rows + i;
      for (int y = 0; y < 10; y++) {
        char chr = sv[i * 11 + y];
        if (chr == '1' || chr == 'X' || chr == 'O') SetCellFilled(x, y);
      }
    }
  }

  constexpr int Count() const {
    return 200 - (popcount(b1) + popcount(b2) + popcount(b3) + popcount(b4));
  }
  constexpr int Group() const {
    return (Count() >> 1) % 5;
  }

  constexpr void Normalize() {
    b1 &= kBoardMask;
    b2 &= kBoardMask;
    b3 &= kBoardMask;
    b4 &= kColumnMask;
  }

  constexpr uint32_t Column(int c) const {
    switch (c) {
      case 0: case 1: case 2: return b1 >> (c * 22) & kColumnMask;
      case 3: case 4: case 5: return b2 >> ((c - 3) * 22) & kColumnMask;
      case 6: case 7: case 8: return b3 >> ((c - 6) * 22) & kColumnMask;
      case 9: return b4;
    }
    unreachable();
  }

  constexpr bool Cell(int x, int y) const {
    return Column(y) >> x & 1;
  }

  constexpr ByteBoard ToByteBoard() const {
    ByteBoard b{};
    for (int i = 0; i < 10; i++) {
      uint32_t col = Column(i);
      for (int j = 0; j < 20; j++) b[j][i] = col >> j & 1;
    }
    return b;
  }

  constexpr void SetCellFilled(int row, int col) {
    switch (col) {
      case 0: case 1: case 2: b1 &= ~(1ll << (col * 22 + row)); break;
      case 3: case 4: case 5: b2 &= ~(1ll << ((col - 3) * 22 + row)); break;
      case 6: case 7: case 8: b3 &= ~(1ll << ((col - 6) * 22 + row)); break;
      case 9: b4 &= ~(1ll << row); break;
      default: unreachable();
    }
  }

  constexpr void SetCellEmpty(int row, int col) {
    switch (col) {
      case 0: case 1: case 2: b1 |= 1ll << (col * 22 + row); break;
      case 3: case 4: case 5: b2 |= 1ll << ((col - 3) * 22 + row); break;
      case 6: case 7: case 8: b3 |= 1ll << ((col - 6) * 22 + row); break;
      case 9: b4 |= 1ll << row; break;
      default: unreachable();
    }
  }

  constexpr void Set(int row, int col) { SetCellEmpty(row, col); }
  constexpr void Unset(int row, int col) { SetCellFilled(row, col); }

  constexpr bool IsCellSet(int row, int col) const {
    switch (col) {
      case 0: case 1: case 2: return b1 >> (col * 22 + row) & 1;
      case 3: case 4: case 5: return b2 >> ((col - 3) * 22 + row) & 1;
      case 6: case 7: case 8: return b3 >> ((col - 6) * 22 + row) & 1;
      case 9: return b4 >> row & 1;
      default: unreachable();
    }
  }

  constexpr bool IsColumnRangeSet(int row_start, int row_end, int col) const {
    uint32_t mask = (1 << row_end) - (1 << row_start);
    switch (col) {
      case 0: case 1: case 2: return (b1 >> (col * 22) & mask) == mask;
      case 3: case 4: case 5: return (b2 >> ((col - 3) * 22) & mask) == mask;
      case 6: case 7: case 8: return (b3 >> ((col - 6) * 22) & mask) == mask;
      case 9: return (b4 & mask) == mask;
      default: unreachable();
    }
  }

  constexpr std::pair<int, Board> ClearLines() const {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnarrowing"
    // use an order in favor of vectorization
    // pext will clear unnecessary bits
    uint32_t cols[] = {
        b1, b2, b3, b4,
        b1 >> 22, b2 >> 22, b3 >> 22, 0,
        b1 >> 44, b2 >> 44, b3 >> 44};
#pragma GCC diagnostic pop
    uint32_t linemask = (cols[0] | cols[1] | cols[2] | cols[3] | cols[4] |
                         cols[5] | cols[6] | cols[8] | cols[9] | cols[10]) & kColumnMask;
    if (linemask == kColumnMask) return {0, *this};
    int lines = 20 - popcount(linemask);
    for (int i = 0; i < 11; i++) {
      cols[i] = pext(cols[i], linemask) << lines | ((1 << lines) - 1);
    }
    return {lines, {
        cols[0] | (uint64_t)cols[4] << 22 | (uint64_t)cols[8] << 44,
        cols[1] | (uint64_t)cols[5] << 22 | (uint64_t)cols[9] << 44,
        cols[2] | (uint64_t)cols[6] << 22 | (uint64_t)cols[10] << 44,
        cols[3]}};
  }

  // x = 1 or 2 for these 4 methods
  constexpr Board ShiftLeft(int x) const {
    return {b1 >> (x * 22) | b2 << (66 - x * 22),
            b2 >> (x * 22) | b3 << (66 - x * 22),
            b3 >> (x * 22) | b4 << (66 - x * 22),
            0};
  }
  constexpr Board ShiftRight(int x) const {
    return {b1 << (x * 22),
            b2 << (x * 22) | b1 >> (66 - x * 22),
            b3 << (x * 22) | b2 >> (66 - x * 22),
            b3 >> (66 - x * 22) & kColumnMask};
  }
  constexpr Board ShiftUpNoFilter(int x) const {
    return {b1 >> x, b2 >> x, b3 >> x, b4 >> x};
  }
  constexpr Board ShiftDownNoFilter(int x) const {
    constexpr uint64_t kDownPadding = 0x100000400001;
    uint64_t padding = kDownPadding;
    if (x == 2) padding |= padding << 1;
    return {b1 << x | padding, b2 << x | padding, b3 << x | padding, b4 << x | padding};
  }

  constexpr std::array<Board, 4> TMap() const {
    Board u = ShiftUpNoFilter(1);
    Board d = ShiftDownNoFilter(1);
    Board l = ShiftLeft(1);
    Board r = ShiftRight(1);
    return {{
      u & l & r & *this,
      u & d & r & *this,
      d & l & r & *this,
      d & l & u & *this,
    }};
  }
  constexpr std::array<Board, 4> JMap() const {
    Board u = ShiftUpNoFilter(1);
    Board d = ShiftDownNoFilter(1);
    Board l = ShiftLeft(1);
    Board r = ShiftRight(1);
    Board ul = u.ShiftLeft(1);
    Board ur = u.ShiftRight(1);
    Board dl = d.ShiftLeft(1);
    Board dr = d.ShiftRight(1);
    return {{
      ul & l & r & *this,
      ur & u & d & *this,
      dr & l & r & *this,
      dl & u & d & *this,
    }};
  }
  constexpr std::array<Board, 2> ZMap() const {
    Board u = ShiftUpNoFilter(1);
    Board l = ShiftLeft(1);
    Board r = ShiftRight(1);
    Board ul = u.ShiftLeft(1);
    Board dl = l.ShiftDownNoFilter(1);
    return {{
      u & r & ul & *this,
      u & l & dl & *this,
    }};
  }
  constexpr std::array<Board, 1> OMap() const {
    Board u = ShiftUpNoFilter(1);
    Board r = ShiftRight(1);
    Board ur = u.ShiftRight(1);
    return {{u & r & ur & *this}};
  }
  constexpr std::array<Board, 2> SMap() const {
    Board u = ShiftUpNoFilter(1);
    Board d = ShiftDownNoFilter(1);
    Board l = ShiftLeft(1);
    //Board r = ShiftRight(1);
    Board ur = u.ShiftRight(1);
    Board ul = u.ShiftLeft(1);
    return {{
      u & l & ur & *this,
      d & l & ul & *this,
    }};
  }
  constexpr std::array<Board, 4> LMap() const {
    Board u = ShiftUpNoFilter(1);
    Board d = ShiftDownNoFilter(1);
    Board l = ShiftLeft(1);
    Board r = ShiftRight(1);
    Board ul = u.ShiftLeft(1);
    Board ur = u.ShiftRight(1);
    Board dl = d.ShiftLeft(1);
    Board dr = d.ShiftRight(1);
    return {{
      ur & l & r & *this,
      dr & u & d & *this,
      dl & l & r & *this,
      ul & u & d & *this,
    }};
  }
  constexpr std::array<Board, 2> IMap() const {
    Board u = ShiftUpNoFilter(1);
    Board d = ShiftDownNoFilter(1);
    Board d2 = ShiftDownNoFilter(2);
    Board l = ShiftLeft(1);
    Board r = ShiftRight(1);
    Board r2 = ShiftRight(2);
    return {{
      l & r & r2 & *this,
      u & d & d2 & *this,
    }};
  }

  // T J Z O S L I
  static constexpr int NumRotations(int piece) {
    switch (piece) {
      case 0: return 4;
      case 1: return 4;
      case 2: return 2;
      case 3: return 1;
      case 4: return 2;
      case 5: return 4;
      case 6: return 2;
    }
    unreachable();
  }

  template <int piece> constexpr std::array<Board, NumRotations(piece)> PieceMap() const {
    if constexpr (piece == 0) return TMap();
    if constexpr (piece == 1) return JMap();
    if constexpr (piece == 2) return ZMap();
    if constexpr (piece == 3) return OMap();
    if constexpr (piece == 4) return SMap();
    if constexpr (piece == 5) return LMap();
    if constexpr (piece == 6) return IMap();
    unreachable();
  }

  std::vector<Board> PieceMap(int piece) const {
    switch (piece) {
#define ONECASE(x) case x: { auto b = PieceMap<x>(); return std::vector<Board>(b.begin(), b.end()); }
      ONECASE(0)
      ONECASE(1)
      ONECASE(2)
      ONECASE(3)
      ONECASE(4)
      ONECASE(5)
      ONECASE(6)
#undef ONECASE
    }
    unreachable();
  }

  constexpr Board PlaceT(int r, int x, int y) const {
    switch (r) {
      case 0: return Place3Wide_(kTPiece0_, x, y, 0, 1);
      case 1: return Place2Wide_(kTPiece1_, x, y, 1, 1);
      case 2: return Place3Wide_(kTPiece2_, x, y, 1, 1);
      case 3: return Place2Wide_(kTPiece3_, x, y, 1, 0);
    }
    unreachable();
  }
  constexpr Board PlaceJ(int r, int x, int y) const {
    switch (r) {
      case 0: return Place3Wide_(kJPiece0_, x, y, 0, 1);
      case 1: return Place2Wide_(kJPiece1_, x, y, 1, 1);
      case 2: return Place3Wide_(kJPiece2_, x, y, 1, 1);
      case 3: return Place2Wide_(kJPiece3_, x, y, 1, 0);
    }
    unreachable();
  }
  constexpr Board PlaceZ(int r, int x, int y) const {
    switch (r) {
      case 0: return Place3Wide_(kZPiece0_, x, y, 0, 1);
      case 1: return Place2Wide_(kZPiece1_, x, y, 1, 0);
    }
    unreachable();
  }
  constexpr Board PlaceO(int r, int x, int y) const {
    return Place2Wide_(kOPiece_, x, y, 0, 1);
  }
  constexpr Board PlaceS(int r, int x, int y) const {
    switch (r) {
      case 0: return Place3Wide_(kSPiece0_, x, y, 0, 1);
      case 1: return Place2Wide_(kSPiece1_, x, y, 1, 0);
    }
    unreachable();
  }
  constexpr Board PlaceL(int r, int x, int y) const {
    switch (r) {
      case 0: return Place3Wide_(kLPiece0_, x, y, 0, 1);
      case 1: return Place2Wide_(kLPiece1_, x, y, 1, 1);
      case 2: return Place3Wide_(kLPiece2_, x, y, 1, 1);
      case 3: return Place2Wide_(kLPiece3_, x, y, 1, 0);
    }
    unreachable();
  }
  constexpr Board PlaceI(int r, int x, int y) const {
    switch (r) {
      case 0: return PlaceI0_(x, y);
      case 1: return Place1Wide_(kIPiece1_, x, y, 2);
    }
    unreachable();
  }

  constexpr Board Place(int piece, int r, int x, int y) const {
    switch (piece) {
      case 0: return PlaceT(r, x, y);
      case 1: return PlaceJ(r, x, y);
      case 2: return PlaceZ(r, x, y);
      case 3: return PlaceO(r, x, y);
      case 4: return PlaceS(r, x, y);
      case 5: return PlaceL(r, x, y);
      case 6: return PlaceI(r, x, y);
    }
    unreachable();
  }

  constexpr bool operator==(const Board& x) const = default;
  constexpr bool operator!=(const Board& x) const = default;
  constexpr Board& operator|=(const Board& x) {
    b1 |= x.b1; b2 |= x.b2; b3 |= x.b3; b4 |= x.b4;
    return *this;
  }
  constexpr Board& operator&=(const Board& x) {
    b1 &= x.b1; b2 &= x.b2; b3 &= x.b3; b4 &= x.b4;
    return *this;
  }

  constexpr Board operator~() const {
    Board r = {~b1, ~b2, ~b3, ~b4};
    r.Normalize();
    return r;
  }

  static const Board Zeros;
  static const Board Ones;
};

inline constexpr Board Board::Zeros = Board(0, 0, 0, 0);
inline constexpr Board Board::Ones = ~Board(0, 0, 0, 0);

constexpr Board operator|(const Board& x, const Board& y) {
  return {x.b1 | y.b1, x.b2 | y.b2, x.b3 | y.b3, x.b4 | y.b4};
}
constexpr Board operator&(const Board& x, const Board& y) {
  return {x.b1 & y.b1, x.b2 & y.b2, x.b3 & y.b3, x.b4 & y.b4};
}

/// position.h

#include <tuple>

struct Position {
  static constexpr bool kIsConstSize = true;

  int r, x, y;
  constexpr Position L() const { return {r, x, y - 1}; }
  constexpr Position R() const { return {r, x, y + 1}; }
  constexpr Position D() const { return {r, x + 1, y}; }
  template <int R> constexpr Position A() const { return {(r + 1) % R, x, y}; }
  template <int R> constexpr Position B() const { return {(r + R - 1) % R, x, y}; }

  constexpr auto operator<=>(const Position&) const = default;

  static constexpr size_t NumBytes() { return 2; }
  void GetBytes(uint8_t ret[]) const {
    ret[0] = r << 5 | x;
    ret[1] = y;
  }
  constexpr Position() = default;
  constexpr Position(int r, int x, int y) : r(r), x(x), y(y) {}
  constexpr Position(const uint8_t data[], size_t) : r(data[0] >> 5), x(data[0] & 31), y(data[1]) {}

  static const Position Start;
  static const Position Invalid;
};

inline constexpr Position Position::Start = Position(0, 0, 5);
// all pieces has cell to the left when r=0
inline constexpr Position Position::Invalid = Position(0, 0, 0);

/// move_search_no_tmpl.h

class PossibleMoves {
  static void UniqueVector_(std::vector<Position>& p, bool unique) {
    std::sort(p.begin(), p.end());
    p.resize(std::unique(p.begin(), p.end()) - p.begin());
  }
 public:
  std::vector<Position> non_adj;
  std::vector<std::pair<Position, std::vector<Position>>> adj;
  void Normalize(bool unique = false) {
    UniqueVector_(non_adj, unique);
    for (auto& i : adj) UniqueVector_(i.second, unique);
    std::sort(adj.begin(), adj.end());
  }
};

namespace move_search {

constexpr int GetRow(int frame, Level level) {
  switch (level) {
    case kLevel18: return frame / 3;
    case kLevel19: return frame / 2;
    case kLevel29: return frame;
    case kLevel39: return frame * 2;
  }
  unreachable();
}

constexpr bool IsDropFrame(int frame, Level level) {
  switch (level) {
    case kLevel18: return frame % 3 == 2;
    case kLevel19: return frame % 2 == 1;
    default: return true;
  }
}

constexpr int NumDrops(int frame, Level level) {
  if (!IsDropFrame(frame, level)) return 0;
  switch (level) {
    case kLevel39: return 2;
    default: return 1;
  }
}

constexpr int GetFirstFrameOnRow(int row, Level level) {
  switch (level) {
    case kLevel18: return row * 3;
    case kLevel19: return row * 2;
    case kLevel29: return row;
    case kLevel39: return (row + 1) / 2;
  }
  unreachable();
}

constexpr int GetLastFrameOnRow(int row, Level level) {
  switch (level) {
    case kLevel18: return row * 3 + 2;
    case kLevel19: return row * 2 + 1;
    case kLevel29: return row;
    case kLevel39: return row / 2;
  }
  unreachable();
}

constexpr int abs(int x) { return x < 0 ? -x : x; }
constexpr int sgn(int x) {
  return x == 0 ? 0 : x > 0 ? 1 : -1;
}

// Check each bit in mask is set in board
template <int R>
constexpr bool Contains4(const std::array<Board, R>& board, const std::array<Board, 4>& mask) {
  bool ret = true;
  for (int i = 0; i < R; i++) ret &= (board[i] & mask[i]) == mask[i];
  return ret;
}

struct TableEntryNoTmpl {
  uint8_t rot, col, num_taps;
  std::array<Board, 4> masks_nodrop;
};

template <int R, class Entry>
constexpr int Phase1TableGen(
    Level level, const int taps[], int initial_frame, int initial_rot, int initial_col,
    Entry entries[]) {
  int sz = 0;
  static_assert(R == 1 || R == 2 || R == 4, "unexpected rotations");
  constexpr uint8_t kA = 0x1;
  constexpr uint8_t kB = 0x2;
  constexpr uint8_t kL = 0x4;
  constexpr uint8_t kR = 0x8;
  std::array<Board, R> masks[R][10] = {};
  std::array<Board, R> masks_nodrop[R][10] = {};
  uint8_t last_tap[R][10] = {};
  bool cannot_reach[R][10] = {};
  bool cannot_finish[R][10] = {};
  for (int i = 0; i < R; i++) {
    for (int j = 0; j < 10; j++) {
      for (int k = 0; k < R; k++) masks[i][j][k] = Board(0, 0, 0, 0);
    }
  }
  for (int col = 0; col < 10; col++) {
    for (int delta_rot = 0; delta_rot < 4; delta_rot++) {
      // piece end up at column col and rotation (initial_rot + delta_rot)
      if (R == 1 && delta_rot != 0) continue;
      if (R == 2 && delta_rot >= 2) continue;
      int rot = (initial_rot + delta_rot) % R;
      int num_lr_tap = abs(col - initial_col);
      int num_ab_tap = delta_rot == 3 ? 1 : delta_rot; // [0,1,2,1]
      int num_tap = std::max(num_ab_tap, num_lr_tap);
      // the frame that this tap occurred; initial_frame if no input
      int start_frame = (num_tap == 0 ? 0 : taps[num_tap - 1]) + initial_frame;
      // the frame that next input is allowed
      int end_frame = taps[num_tap] + initial_frame;
      if (num_tap) {
        if (num_tap == num_lr_tap) last_tap[rot][col] |= col > initial_col ? kR : kL;
        if (num_tap == num_ab_tap) last_tap[rot][col] |= delta_rot == 3 ? kB : kA;
      }
      // the position before this tap
      int start_row = GetRow(start_frame, level);
      if (start_row >= 20) {
        cannot_reach[rot][col] = true;
        continue;
      }
      int start_col = num_tap == num_lr_tap ? col - sgn(col - initial_col) : col;
      int start_rot = num_tap == num_ab_tap ? ((delta_rot == 2 ? 1 : 0) + initial_rot) % R : rot;
      auto& cur = masks[rot][col];
      cur[start_rot].Set(start_row, start_col);
      cur[start_rot].Set(start_row, col); // first shift
      cur[rot].Set(start_row, col); // then rotate
      masks_nodrop[rot][col] = cur;
      if (GetRow(end_frame, level) >= 20) {
        cannot_finish[rot][col] = true;
        continue;
      }
      for (int frame = start_frame; frame < end_frame; frame++) {
        int row = GetRow(frame, level);
        cur[rot].Set(row, col);
        if (IsDropFrame(frame, level)) {
          cur[rot].Set(row + 1, col);
          if (level == kLevel39) cur[rot].Set(row + 2, col);
        }
      }
    }
  }
  // start from (initial_col, initial_row) and build the entries according to
  //   ascending tap count
  auto Push = [&](uint8_t rot, uint8_t col, uint8_t orot, uint8_t ocol, uint8_t prev, uint8_t num_taps) {
    if (!cannot_reach[rot][col]) {
      if constexpr (std::is_same<Entry, TableEntryNoTmpl>::value) {
        entries[sz] = {rot, col, num_taps, {}};
      } else {
        entries[sz] = {rot, col, prev, num_taps, cannot_finish[rot][col], {}, {}};
      }
      for (int i = 0; i < R; i++) {
        if constexpr (std::is_same<Entry, TableEntryNoTmpl>::value) {
          entries[sz].masks_nodrop[i] = masks_nodrop[rot][col][i];
          if (num_taps) {
            entries[sz].masks_nodrop[i] |= masks[orot][ocol][i];
            masks[rot][col][i] |= masks[orot][ocol][i];
          }
        } else {
          entries[sz].masks[i] = masks[rot][col][i];
          entries[sz].masks_nodrop[i] = masks_nodrop[rot][col][i];
        }
      }
      sz++;
    }
  };
  Push(initial_rot, initial_col, 0, 0, 0, 0);
  for (int cur = 0; cur < sz; cur++) {
    int rot = entries[cur].rot, col = entries[cur].col, taps = entries[cur].num_taps;
    int last = last_tap[rot][col];
    bool should_l = col > 0 && (taps == 0 || (last & kL));
    bool should_r = col < 9 && (taps == 0 || (last & kR));
    bool should_a = (R > 1 && taps == 0) || (R == 4 && taps == 1 && (last & kA));
    bool should_b = R == 4 && taps == 0;
    if (should_l) Push(rot, col - 1, rot, col, cur, taps + 1); // L
    if (should_r) Push(rot, col + 1, rot, col, cur, taps + 1); // R
    if (should_a) {
      int nrot = (rot + 1) % R;
      Push(nrot, col, rot, col, cur, taps + 1); // A
      if (should_l) Push(nrot, col - 1, rot, col, cur, taps + 1); // L
      if (should_r) Push(nrot, col + 1, rot, col, cur, taps + 1); // R
    }
    if (should_b) {
      int nrot = (rot + 3) % R;
      Push(nrot, col, rot, col, cur, taps + 1); // B
      if (should_l) Push(nrot, col - 1, rot, col, cur, taps + 1); // L
      if (should_r) Push(nrot, col + 1, rot, col, cur, taps + 1); // R
    }
  }
  return sz;
}

template <class Entry>
constexpr int Phase1TableGen(
    Level level, int R, const int taps[], int initial_frame, int initial_rot, int initial_col,
    Entry entries[]) {
  if (R == 1) {
    return Phase1TableGen<1>(level, taps, initial_frame, initial_rot, initial_col, entries);
  } else if (R == 2) {
    return Phase1TableGen<2>(level, taps, initial_frame, initial_rot, initial_col, entries);
  } else {
    return Phase1TableGen<4>(level, taps, initial_frame, initial_rot, initial_col, entries);
  }
}

struct Phase1TableNoTmpl {
  std::vector<TableEntryNoTmpl> initial;
  std::vector<std::vector<TableEntryNoTmpl>> adj;
  constexpr Phase1TableNoTmpl(Level level, int R, int adj_frame, const int taps[]) : initial(40) {
    initial.resize(10 * R);
    initial.resize(Phase1TableGen(level, R, taps, 0, 0, Position::Start.y, initial.data()));
    for (auto& i : initial) {
      int frame_start = std::max(adj_frame, taps[i.num_taps]);
      adj.emplace_back(10 * R);
      adj.back().resize(Phase1TableGen(level, R, taps, frame_start, i.rot, i.col, adj.back().data()));
    }
  }
};

// Column is simply a column; each bit is a cell, LSB is topmost
// Frames is a processed form of a column; each bit is a frame, LSB is the first frame
// Frames comes in two flavors: normal and drop mask
//   normal mask just corresponds to the row the piece is in on each frame
//   drop mask is AND of all rows that the piece will pass when dropping
// For level 18,19,29, drop mask is just (normal_mask & normal_mask >> 1);
//   drop mask exists only to make it easy to deal with level 39
using Column = uint32_t;
using Frames = uint64_t;

template <int R>
struct FrameMasks {
  Frames frame[R][10], drop[R][10];
};

constexpr Frames ColumnToNormalFrameMask(Level level, Column col) {
  switch (level) {
    case kLevel18: {
      constexpr uint64_t kMask = 0x249249249249249;
      uint64_t expanded = pdep<uint64_t>(col, kMask);
      return expanded | expanded << 1 | expanded << 2;
    }
    case kLevel19: {
      constexpr uint64_t kMask = 0x5555555555;
      uint64_t expanded = pdep<uint64_t>(col, kMask);
      return expanded | expanded << 1;
    }
    case kLevel29: return col;
    case kLevel39: {
      constexpr uint32_t kMask = 0x55555;
      return pext(col, kMask);
    }
  }
  unreachable();
}

constexpr Frames ColumnToDropFrameMask(Level level, Column col) {
  switch (level) {
    case kLevel18: [[fallthrough]];
    case kLevel19: [[fallthrough]];
    case kLevel29: {
      uint64_t mask = ColumnToNormalFrameMask(level, col);
      return mask & mask >> 1;
    }
    case kLevel39: {
      constexpr uint32_t kMask = 0x55555;
      return pext(col & col >> 1 & col >> 2, kMask);
    }
  }
  unreachable();
}

constexpr Column FramesToColumn(Level level, Frames frames) {
  switch (level) {
    case kLevel18: {
      constexpr uint64_t kMask = 0x249249249249249;
      return pext(frames | frames >> 1 | frames >> 2, kMask);
    }
    case kLevel19: {
      constexpr uint64_t kMask = 0x5555555555;
      return pext(frames | frames >> 1, kMask);
    }
    case kLevel29: return frames;
    case kLevel39: {
      constexpr uint32_t kMask = 0x55555;
      return pdep<uint32_t>(frames, kMask);
    }
  }
  unreachable();
}

constexpr int FindLockRow(uint32_t col, int start_row) {
  // given (col & 1<<row) != 0
  // col               = 00111100011101
  // 1<<row            = 00000000001000
  // col+(1<<row)      = 00111100100101
  // col^(col+(1<<row))= 00000000111000
  //              highbit=31-clz ^
  return 31 - clz<uint32_t>(col ^ (col + (1 << start_row))) - 1;
}

// note: "tuck" here means tucks, spins or spintucks
template <int R>
using TuckMask = std::array<std::array<Frames, 10>, R>;

constexpr int TuckTypes(int R) {
  return (R == 1 ? 2 : R == 2 ? 7 : 12) + (kDoubleTuckAllowed ? 2 : 0);
  // R = 1: L R (LL RR)
  // R = 2: A LA RA AL AR
  // R = 4: B LB RB BL BR
  // should also change frame_sequence.h if changed
  // it is possible to add other tuck types suck as buco-like spins
  // but we just keep it simple here
}

template <int R>
using TuckMasks = std::array<TuckMask<R>, TuckTypes(R)>;

struct TuckType {
  int delta_rot, delta_col, delta_frame;
};

template <int R>
struct TuckTypeTable {
  std::array<TuckType, TuckTypes(R)> table;
  constexpr TuckTypeTable() : table() {
    table[0] = {0, -1, 0}; // L
    table[1] = {0, 1, 0}; // R
#ifdef DOUBLE_TUCK
    table[2] = {0, -2, 2}; // L-/-L
    table[3] = {0, 2, 2}; // R-/-R
#endif
    constexpr int x = kDoubleTuckAllowed ? 2 : 0;
    if constexpr (R == 1) return;
    table[x+2] = {1, 0, 0}; // A
    table[x+3] = {1, -1, 0}; // LA
    table[x+4] = {1, 1, 0}; // RA
    table[x+5] = {1, -1, 1}; // A-L L-A
    table[x+6] = {1, 1, 1}; // A-R R-A
    if constexpr (R == 2) return;
    table[x+7] = {3, 0, 0}; // B
    table[x+8] = {3, -1, 0}; // LB
    table[x+9] = {3, 1, 0}; // RB
    table[x+10] = {3, -1, 1}; // B-L L-B
    table[x+11] = {3, 1, 1}; // B-R R-B
  }
};

template <int R>
constexpr TuckMasks<R> GetTuckMasks(const FrameMasks<R> m) {
  TuckMasks<R> ret{};
  constexpr int x = kDoubleTuckAllowed ? 2 : 0;
#pragma GCC unroll 4
  for (int rot = 0; rot < R; rot++) {
    for (int col = 0; col < 10; col++) {
      if (col > 0) ret[0][rot][col] = m.frame[rot][col] & m.frame[rot][col-1];
      if (col < 9) ret[1][rot][col] = m.frame[rot][col] & m.frame[rot][col+1];
#ifdef DOUBLE_TUCK
      if (col > 1) ret[2][rot][col] = m.frame[rot][col] & m.drop[rot][col-1] & m.drop[rot][col-1] >> 1 & m.frame[rot][col-2] >> 2;
      if (col < 8) ret[3][rot][col] = m.frame[rot][col] & m.drop[rot][col+1] & m.drop[rot][col+1] >> 1 & m.frame[rot][col+2] >> 2;
#endif
    }
  }
  if (R == 1) return ret;
#pragma GCC unroll 4
  for (int rot = 0; rot < R; rot++) {
    int nrot = (rot + 1) % R;
    for (int col = 0; col < 10; col++) {
      ret[x+2][rot][col] = m.frame[rot][col] & m.frame[nrot][col];
      if (col > 0) ret[x+3][rot][col] = ret[0][rot][col] & m.frame[nrot][col-1];
      if (col < 9) ret[x+4][rot][col] = ret[1][rot][col] & m.frame[nrot][col+1];
      if (col > 0) ret[x+5][rot][col] = m.frame[rot][col] & (m.drop[nrot][col] | m.drop[rot][col-1]) & m.frame[nrot][col-1] >> 1;
      if (col < 9) ret[x+6][rot][col] = m.frame[rot][col] & (m.drop[nrot][col] | m.drop[rot][col+1]) & m.frame[nrot][col+1] >> 1;
    }
  }
  if (R == 2) return ret;
#pragma GCC unroll 4
  for (int rot = 0; rot < R; rot++) {
    int nrot = (rot + 3) % R;
    for (int col = 0; col < 10; col++) {
      ret[x+7][rot][col] = m.frame[rot][col] & m.frame[nrot][col];
      if (col > 0) ret[x+8][rot][col] = ret[0][rot][col] & m.frame[nrot][col-1];
      if (col < 9) ret[x+9][rot][col] = ret[1][rot][col] & m.frame[nrot][col+1];
      if (col > 0) ret[x+10][rot][col] = m.frame[rot][col] & (m.drop[nrot][col] | m.drop[rot][col-1]) & m.frame[nrot][col-1] >> 1;
      if (col < 9) ret[x+11][rot][col] = m.frame[rot][col] & (m.drop[nrot][col] | m.drop[rot][col+1]) & m.frame[nrot][col+1] >> 1;
    }
  }
  return ret;
}

template <int R>
NOINLINE constexpr void SearchTucks(
    Level level,
    const Column cols[R][10],
    const TuckMasks<R> tuck_masks,
    const Column lock_positions_without_tuck[R][10],
    const Frames can_tuck_frame_masks[R][10],
    int& sz, Position* positions) {
  constexpr TuckTypeTable<R> tucks;
  Frames tuck_result[R][10] = {};
  for (int i = 0; i < TuckTypes(R); i++) {
    const auto& tuck = tucks.table[i];
    int start_col = std::max(0, -tuck.delta_col);
    int end_col = std::min(10, 10 - tuck.delta_col);
    for (int rot = 0; rot < R; rot++) {
      int nrot = (rot + tuck.delta_rot) % R;
      for (int col = start_col; col < end_col; col++) {
        tuck_result[nrot][col + tuck.delta_col] |=
            (tuck_masks[i][rot][col] & can_tuck_frame_masks[rot][col]) << tuck.delta_frame;
      }
    }
  }
  for (int rot = 0; rot < R; rot++) {
    for (int col = 0; col < 10; col++) {
      Column after_tuck_positions = FramesToColumn(level, tuck_result[rot][col]);
      Column cur = cols[rot][col];
      Column tuck_lock_positions = (after_tuck_positions + cur) >> 1 & (cur & ~cur >> 1) & ~lock_positions_without_tuck[rot][col];
      while (tuck_lock_positions) {
        int row = ctz(tuck_lock_positions);
        positions[sz++] = {rot, row, col};
        tuck_lock_positions ^= 1 << row;
      }
    }
  }
}

template <int R, class Tap, class Entry>
constexpr void CheckOneInitial(
    Level level, int adj_frame, const Tap& taps, bool is_adj,
    int total_frames, int initial_frame, const Entry& entry, const Column cols[R][10],
    Column lock_positions_without_tuck[R][10],
    Frames can_tuck_frame_masks[R][10],
    int& sz, Position* positions,
    bool& can_adj, bool& phase_2_possible) {
  int start_frame = (entry.num_taps == 0 ? 0 : taps[entry.num_taps - 1]) + initial_frame;
  int start_row = GetRow(start_frame, level);
  int end_frame = is_adj ? total_frames : std::max(adj_frame, taps[entry.num_taps]);
  // Since we verified masks_nodrop, start_row should be in col
  //if ((cols[entry.rot][entry.col] & 1 << start_row) == 0) throw std::runtime_error("unexpected");
  int lock_row = FindLockRow(cols[entry.rot][entry.col], start_row);
  int lock_frame = GetLastFrameOnRow(lock_row, level) + 1;
  if (!is_adj && lock_frame > end_frame) {
    can_adj = true;
  } else {
    positions[sz++] = {entry.rot, lock_row, entry.col};
  }
  int first_tuck_frame = initial_frame + taps[entry.num_taps];
  int last_tuck_frame = std::min(lock_frame, end_frame);
  lock_positions_without_tuck[entry.rot][entry.col] |= 1 << lock_row;
  if (last_tuck_frame > first_tuck_frame) {
    can_tuck_frame_masks[entry.rot][entry.col] = (1ll << last_tuck_frame) - (1ll << first_tuck_frame);
    phase_2_possible = true;
  }
}

template <int R>
constexpr FrameMasks<R> GetColsAndFrameMasks(Level level, const std::array<Board, R>& board, Column cols[R][10]) {
  FrameMasks<R> frame_masks = {};
  for (int rot = 0; rot < R; rot++) {
    for (int col = 0; col < 10; col++) {
      cols[rot][col] = board[rot].Column(col);
      // ColumnToNormalFrameMask<level>(col), ColumnToDropFrameMask<level>(col)
      frame_masks.frame[rot][col] = ColumnToNormalFrameMask(level, cols[rot][col]);
      frame_masks.drop[rot][col] = ColumnToDropFrameMask(level, cols[rot][col]);
    }
  }
  return frame_masks;
}

template <int R>
int DoOneSearch(
    bool is_adj, int initial_taps, Level level, int adj_frame, const int taps[],
    const std::vector<TableEntryNoTmpl>& table,
    const std::array<Board, R>& board, const Column cols[R][10],
    const TuckMasks<R> tuck_masks,
    bool can_adj[],
    Position* positions) {
  int total_frames = GetLastFrameOnRow(19, level) + 1;
  int N = table.size();
  int initial_frame = is_adj ? std::max(adj_frame, taps[initial_taps]) : 0;
  if (initial_frame >= total_frames) return 0;

  int sz = 0;
  // phase 1
  Frames can_tuck_frame_masks[R][10] = {}; // frames that can start a tuck
  Column lock_positions_without_tuck[R][10] = {};

  bool phase_2_possible = false;
  bool can_reach[R * 10] = {};
  for (int i = 0; i < N; i++) {
    can_reach[i] = Contains4<R>(board, table[i].masks_nodrop);
  }
  for (int i = 0; i < N; i++) {
    if (!can_reach[i]) continue;
    CheckOneInitial<R>(
        level, adj_frame, taps, is_adj, total_frames, initial_frame, table[i], cols,
        lock_positions_without_tuck, can_tuck_frame_masks,
        sz, positions, can_adj[i], phase_2_possible);
  }
  if (phase_2_possible) {
    SearchTucks<R>(level, cols, tuck_masks, lock_positions_without_tuck, can_tuck_frame_masks, sz, positions);
  }
  return sz;
}

template <int R>
inline PossibleMoves MoveSearchInternal(
    Level level, int adj_frame, const int taps[], const Phase1TableNoTmpl& table,
    const std::array<Board, R>& board) {
  Column cols[R][10] = {};
  auto tuck_masks = GetTuckMasks<R>(GetColsAndFrameMasks<R>(level, board, cols));
  bool can_adj[R * 10] = {}; // whether adjustment starting from this (rot, col) is possible

  PossibleMoves ret;
  Position buf[256];
  ret.non_adj.assign(buf, buf + DoOneSearch<R>(
      false, 0, level, adj_frame, taps, table.initial, board, cols, tuck_masks, can_adj, buf));

  for (size_t i = 0; i < table.initial.size(); i++) {
    auto& entry = table.initial[i];
    if (!can_adj[i]) continue;
    int x = DoOneSearch<R>(
        true, entry.num_taps, level, adj_frame, taps, table.adj[i], board, cols, tuck_masks, can_adj, buf);
    if (x) {
      int row = GetRow(std::max(adj_frame, taps[entry.num_taps]), level);
      ret.adj.emplace_back(Position{entry.rot, row, entry.col}, std::vector<Position>(buf, buf + x));
    }
  }
  return ret;
}

} // namespace move_search

using PrecomputedTable = move_search::Phase1TableNoTmpl;

template <int R>
NOINLINE PossibleMoves MoveSearch(
    Level level, int adj_frame, const int taps[], const PrecomputedTable& table,
    const std::array<Board, R>& board) {
  return move_search::MoveSearchInternal<R>(level, adj_frame, taps, table, board);
}

class PrecomputedTableTuple {
  const PrecomputedTable tables[3];
 public:
  constexpr PrecomputedTableTuple(Level level, int adj_frame, const int taps[]) :
      tables{{level, 1, adj_frame, taps}, {level, 2, adj_frame, taps}, {level, 4, adj_frame, taps}} {}
  const PrecomputedTable& operator[](int R) const {
    switch (R) {
      case 1: return tables[0];
      case 2: return tables[1];
      default: return tables[2];
    }
  }
};

inline PossibleMoves MoveSearch(
    Level level, int adj_frame, const int taps[], const PrecomputedTableTuple& table,
    const Board& b, int piece) {
#define ONE_CASE(x) \
    case x: return MoveSearch<Board::NumRotations(x)>(level, adj_frame, taps, table[Board::NumRotations(x)], b.PieceMap<x>());
  DO_PIECE_CASE(piece);
#undef ONE_CASE
}

/// other things

#include <map>
#include <cstring>

struct PrecomputedTableCache {
  struct PrecomputedTableKey {
    Level level;
    int adj_frame;
    std::array<int, 10> taps;

    auto operator<=>(const PrecomputedTableKey&) const = default;
  };
  std::map<PrecomputedTableKey, PrecomputedTableTuple> mp;

  const PrecomputedTableTuple& operator()(Level level, int adj_frame, const std::array<int, 10>& taps) {
    PrecomputedTableKey key{level, adj_frame, taps};
    if (auto it = mp.find(key); it != mp.end()) {
      return it->second;
    }
    PrecomputedTableTuple table(level, adj_frame, taps.data());
    auto it = mp.insert({key, std::move(table)});
    return it.first->second;
  }
} precomputed_table_cache;

PossibleMoves MoveSearch(
    Level level, int adj_frame, const std::array<int, 10>& taps,
    const Board& b, int piece) {
  auto& table = precomputed_table_cache(level, adj_frame, taps);
  return MoveSearch(level, adj_frame, taps.data(), table, b, piece);
}

using MoveMap = std::array<ByteBoard, 4>;
static constexpr uint8_t kNoAdj = 1;
static constexpr uint8_t kHasAdj = 2;

MoveMap CalculateMoves(
    const Board& b, int now_piece, Level level, int adj_frame, const std::array<int, 10>& taps, const Position& premove) {
  auto moves = MoveSearch(level, adj_frame, taps, b, now_piece);
  if (moves.non_adj.empty() && moves.adj.empty()) {
    throw std::runtime_error("Game over");
  }
  MoveMap move_map;
  memset(move_map.data(), 0, sizeof(move_map));
  if (premove == Position::Invalid) {
    for (auto& i : moves.non_adj) move_map[i.r][i.x][i.y] = kNoAdj;
    for (auto& [i, _] : moves.adj) move_map[i.r][i.x][i.y] = kHasAdj;
  } else {
    size_t initial_move = std::find_if(
        moves.adj.begin(), moves.adj.end(), [&premove](auto& i){ return i.first == premove; }
    ) - moves.adj.begin();
    if (initial_move == moves.adj.size()) {
      throw std::runtime_error("No premove exists");
    }
    for (auto& i : moves.adj[initial_move].second) move_map[i.r][i.x][i.y] = kNoAdj;
  }
  return move_map;
}


enum TapSpeed {
  kTap12Hz,
  kTap15Hz,
  kTap20Hz,
  kTap24Hz,
  kTap30Hz
};
constexpr std::array<int, 10> kTapTables[] = { // match TapSpeed
  {0, 5, 10, 15, 20, 25, 30, 35, 40, 45},
  {0, 4, 8, 12, 16, 20, 24, 28, 32, 36},
  {0, 3, 6, 9, 12, 15, 18, 21, 24, 27},
  {0, 3, 5, 8, 10, 13, 15, 18, 20, 23},
  {0, 2, 4, 6, 8, 10, 12, 14, 16, 18}
};

struct State {
  std::array<std::array<std::array<float, 10>, 20>, 6> board;
  std::array<float, 28> meta;
  std::array<std::array<std::array<float, 10>, 20>, 14> moves;
  std::array<float, 28> move_meta;
  std::array<int, 2> meta_int;
};

// piece == -1
State GetState(const ByteBoard& byte_board, int now_piece, int next_piece, const Position& premove,
               int lines, TapSpeed tap_speed, int adj_frame) {
  State state = {};
  memset(state.board.data(), 0, sizeof(state.board));

  Board board(byte_board);
  int level = GetLevelByLines(lines);
  int count = board.Count();
  int pieces = (lines * 10 + count) / 4;
  Level level_speed = GetLevelSpeed(level);
  bool is_adj = next_piece != -1;
  // board: shape (6, 20, 10) [board, one, initial_move(4)]
  // meta: shape (28,) [group(5), now_piece(7), next_piece(7), is_adj(1), hz(4), adj(4)]
  // meta_int: shape (2,) [entry, now_piece]
  // moves: shape (14, 20, 10) [board, one, moves(4), adj_moves(4), initial_move(4)]
  // move_meta: shape (28,) [speed(4), to_transition(21), (level-18)*0.1, lines*0.01, pieces*0.004]
  {
    for (int i = 0; i < 20; i++) {
      for (int j = 0; j < 10; j++) state.board[0][i][j] = byte_board[i][j];
      for (int j = 0; j < 10; j++) state.board[1][i][j] = 1;
      for (int j = 0; j < 10; j++) state.moves[0][i][j] = byte_board[i][j];
      for (int j = 0; j < 10; j++) state.moves[1][i][j] = 1;
    }
    auto move_map = CalculateMoves(
        board, now_piece, level_speed, adj_frame, kTapTables[static_cast<int>(tap_speed)],
        is_adj ? premove : Position::Invalid);
    for (int r = 0; r < 4; r++) {
      for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 10; j++) state.moves[2 + r][i][j] = move_map[r][i][j] ? 1 : 0;
        for (int j = 0; j < 10; j++) state.moves[6 + r][i][j] = move_map[r][i][j] == 2;
      }
    }
  }
  if (is_adj) {
    state.board[2 + premove.r][premove.x][premove.y] = 1;
    state.moves[10 + premove.r][premove.x][premove.y] = 1;
  }

  state.meta[0 + board.Count() / 2 % 5] = 1;
  state.meta[5 + now_piece] = 1;
  if (is_adj) {
    state.meta[12 + next_piece] = 1;
    state.meta[19] = 1;
  }
  state.meta[20] = 1; // hardcode now; modify if extended
  state.meta[24] = 1;

  state.meta_int[0] = lines / 2;
  state.meta_int[1] = now_piece;

  int to_transition = 0;
  state.move_meta[level_speed] = 1;
  to_transition = std::max(1, kLevelSpeedLines[level_speed + 1] - lines);
  if (to_transition <= 10) { // 4..13
    state.move_meta[4 + (to_transition - 1)] = 1;
  } else if (to_transition <= 22) { // 14..17
    state.move_meta[14 + (to_transition - 11) / 3] = 1;
  } else if (to_transition <= 40) { // 18..20
    state.move_meta[18 + (to_transition - 22) / 6] = 1;
  } else if (to_transition <= 60) { // 21,22
    state.move_meta[21 + (to_transition - 40) / 10] = 1;
  } else {
    state.move_meta[23] = 1;
  }
  state.move_meta[24] = to_transition * 0.01;
  state.move_meta[25] = (level - 18) * 0.1;
  state.move_meta[26] = lines * 0.01;
  state.move_meta[27] = pieces * 0.004;

  return state;
}

#include <emscripten/bind.h>
#include "emarray.h"

EMSCRIPTEN_BINDINGS(my_module) {
  DeclareArray<ByteBoard>("ByteBoard");
  DeclareArray<ByteBoard::value_type>("ByteBoard_value");
  
  DeclareArray<decltype(State::board)>("State_board");
  DeclareArray<decltype(State::board)::value_type>("State_board_value");
  DeclareArray<decltype(State::board)::value_type::value_type>("State_board_value_value");
  DeclareArray<decltype(State::moves)>("State_moves");
  DeclareArray<decltype(State::meta)>("State_meta");
  //DeclareArray<decltype(State::move_meta)>("State_move_meta");
  DeclareArray<decltype(State::meta_int)>("State_meta_int");

  emscripten::value_object<State>("State")
      .field("board", &State::board)
      .field("moves", &State::moves)
      .field("meta", &State::meta)
      .field("move_meta", &State::move_meta)
      .field("meta_int", &State::meta_int)
      ;

  emscripten::value_object<Position>("Position")
      .field("r", &Position::r)
      .field("x", &Position::x)
      .field("y", &Position::y)
      ;
  emscripten::enum_<TapSpeed>("TapSpeed")
      .value("kTap12Hz", kTap12Hz)
      .value("kTap15Hz", kTap15Hz)
      .value("kTap20Hz", kTap20Hz)
      .value("kTap24Hz", kTap24Hz)
      .value("kTap30Hz", kTap30Hz)
      ;

  emscripten::function("GetState", &GetState);
}
