#pragma once
#include <array>
#include <filesystem>

namespace irevan::factory {
struct Entry { const char* name; const char* path; };
inline constexpr std::array<Entry, 6> amps{{
  {"Twin Bright", "community-starter/Tim R Fender TwinVerb Norm Bright.nam"},
  {"AC15 Top Boost", "community-starter/Phillipe P VOXAC15-TopBoost.nam"},
  {"JVM Crunch", "community-starter/Phillipe P JVM-CR-OR-NoDrive-ESR0,017.nam"},
  {"5150", "community-starter/Helga B 5150 BlockLetter - NoBoost.nam"},
  {"Mark IV", "community-starter/Roman A LT_MESA_MARKIV_1.nam"},
  {"SLO", "volum/AMP-SLO1-3.nam"}
}};
inline constexpr std::array<Entry, 3> cabinets{{
  {"V30 / SM57", "cabinets/jester-v30-sm57.wav"},
  {"DV77 / SM57", "cabinets/jester-dv77-sm57.wav"},
  {"Rockdriver / e606", "cabinets/jester-rockdriver-e606.wav"}
}};
inline constexpr size_t defaultAmp = 5;
inline constexpr size_t defaultCabinet = 0;
inline std::filesystem::path InstalledRoot() {
  return "/Library/Application Support/I.R.Evan/Beavis And Butthead/models";
}
inline std::filesystem::path Resolve(const std::filesystem::path& root, const Entry& entry) {
  return root / entry.path;
}
inline bool Available(const std::filesystem::path& root, const Entry& entry) {
  std::error_code error;
  return std::filesystem::is_regular_file(Resolve(root, entry), error);
}
}
