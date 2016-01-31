#include <benchmark/benchmark.h>
#include "Parser.h"
#include <string>
#include <vector>
#include <fstream>

std::string readFile(const std::string &fileName)
{
    std::ifstream ifs(fileName.c_str(), std::ios::in | std::ios::binary | std::ios::ate);

    std::ifstream::pos_type fileSize = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    std::vector<char> bytes(fileSize);
    ifs.read(&bytes[0], fileSize);

    return std::string(&bytes[0], fileSize);
}

static void BM_StringCreation(benchmark::State& state) {
  while(state.KeepRunning()) {
    state.PauseTiming();
    std::string bigString = readFile("../frontend/liverpool.fuz");
    state.ResumeTiming();
    FSparqlParser::Tokenize(bigString);
  }  
}
// Register the function as a benchmark
BENCHMARK(BM_StringCreation)->UseRealTime();

BENCHMARK_MAIN();