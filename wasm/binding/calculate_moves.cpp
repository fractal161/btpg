#include "calculate_moves.h"

#include <map>
#include "../tetris/move_search_no_tmpl.h"

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
