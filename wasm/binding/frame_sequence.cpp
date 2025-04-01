#include "frame_sequence.h"

#include <unordered_map>

std::vector<AdjItem> GetBestAdjModes(
    const Board& board, int now_piece,
    int lines, TapSpeed tap_speed, int adj_delay,
    const PossibleMoves& moves, const std::vector<Position>& adjs) {
  if (adjs.size() != kPieces) {
    throw std::runtime_error("adjs size must be 7");
  }
  Level level = GetLevelSpeed(GetLevelByLines(lines));
  auto adj_infor = GetAdjTaps(
      level, kTapTables[static_cast<int>(tap_speed)].data(), board, now_piece,
      moves, adj_delay, adjs.data());
  std::unordered_map<Position, AdjItem> ret_map;
  for (auto [mode_str, mode] : std::vector<std::pair<std::string, BestAdjMode>>{
      {"LWT", BestAdjMode::kWeightedTaps},
      {"LMT", BestAdjMode::kWorstTaps},
      {"LAP", BestAdjMode::kAdjProb}}) {
    auto [index, seq] = GetBestAdj(adj_infor, mode);
    const Position& pos = moves.adj[index].first;
    auto& item = ret_map[pos];
    if (item.modes.size()) {
      item.modes += ',';
    } else {
      item.position = pos;
      for (const auto& f : seq) {
        item.frame_seq.push_back(f.ToString());
      }
    }
    item.modes += mode_str;
  }
  std::vector<AdjItem> ret;
  ret.reserve(ret_map.size());
  for (auto& [_, item] : ret_map) ret.push_back(std::move(item));
  auto ModeInt = [](const std::string& str) {
    if (str.find("LWT") != std::string::npos) return 0;
    if (str.find("LMT") != std::string::npos) return 1;
    if (str.find("LAP") != std::string::npos) return 2;
    return -1;
  };
  std::sort(ret.begin(), ret.end(), [ModeInt](const AdjItem& a, const AdjItem& b) {
    return ModeInt(a.modes) < ModeInt(b.modes);
  });
  return ret;
}
