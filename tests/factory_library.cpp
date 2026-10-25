#include "../BBIREvan/IREvanFactoryLibrary.h"
#include <cassert>
#include <iostream>
int main(int argc, char** argv) {
  assert(argc == 2);
  const std::filesystem::path root(argv[1]);
  for (const auto& e : irevan::factory::amps) {
    assert(irevan::factory::Available(root, e));
    assert(irevan::factory::Resolve(root, e).extension() == ".nam");
  }
  for (const auto& e : irevan::factory::cabinets) {
    assert(irevan::factory::Available(root, e));
    assert(irevan::factory::Resolve(root, e).extension() == ".wav");
  }
  assert(!irevan::factory::Available(root / "missing-library", irevan::factory::amps[0]));
  assert(irevan::factory::Resolve(root, irevan::factory::amps[irevan::factory::defaultAmp]).filename() == "AMP-SLO1-3.nam");
  assert(irevan::factory::Resolve(root, irevan::factory::cabinets[irevan::factory::defaultCabinet]).filename() == "jester-v30-sm57.wav");
  std::cout << "Factory paths, defaults and missing-library detection passed\n";
}
