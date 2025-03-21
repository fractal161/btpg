#include "calculate_moves.h"

#include <map>
#include <bitset>
#include <unordered_map>
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

  if (moves.adj.size() > 64) throw std::runtime_error("unexpected many initial placements");
  uint64_t initial_mask = (1ll << moves.adj.size()) - 1;
  if (moves.adj.size() > 1) {
    using AdjItem = std::pair<Position, std::vector<Position>>;
    std::sort(moves.adj.begin(), moves.adj.end(), [](const AdjItem& x, const AdjItem& y) {
      if (x.second.size() != y.second.size()) return x.second.size() > y.second.size();
      return abs(x.first.y - 5) < abs(y.first.y - 5);
    });
    std::unordered_map<Position, uint8_t> pos_mp;
    for (auto& [_, i] : moves.adj) {
      for (auto& j : i) pos_mp.emplace(j, pos_mp.size());
    }
    std::vector<std::bitset<256>> adj_bitset(moves.adj.size());
    for (size_t i = 0; i < moves.adj.size(); i++) {
      for (auto& j : moves.adj[i].second) adj_bitset[i][pos_mp[j]] = true;
    }
    for (size_t i = 0; i < moves.adj.size(); i++) {
      if (!(initial_mask >> i & 1)) continue;
      for (size_t j = 0; j < moves.adj.size(); j++) {
        if (i == j || !(initial_mask >> j & 1) || moves.adj[i].second.size() < moves.adj[j].second.size()) continue;
        if ((adj_bitset[i] & adj_bitset[j]) == adj_bitset[j]) initial_mask &= ~(1ll << j);
      }
    }
  }

  MoveMap move_map;
  memset(move_map.data(), 0, sizeof(move_map));
  if (premove == Position::Invalid) {
    for (auto& i : moves.non_adj) move_map[i.r][i.x][i.y] = kNoAdj;
    for (size_t idx = 0; idx < moves.adj.size(); idx++) {
      auto& i = moves.adj[idx].first;
      move_map[i.r][i.x][i.y] = (initial_mask >> idx & 1) ? kHasAdjNonReduced : kHasAdjReduced;
    }
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
