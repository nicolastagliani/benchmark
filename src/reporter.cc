// Copyright 2015 Google Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "benchmark/benchmark.h"
#include "timers.h"

#include <cstdlib>

#include <iostream>
#include <tuple>
#include <vector>

#include "check.h"

namespace benchmark {

BenchmarkReporter::BenchmarkReporter()
    : output_stream_(&std::cout), error_stream_(&std::cerr) {}

BenchmarkReporter::~BenchmarkReporter() {}

void BenchmarkReporter::PrintBasicContext(std::ostream *out,
                                          Context const &context) {
  CHECK(out) << "cannot be null";
  auto &Out = *out;

  Out << LocalDateTimeString() << "\n";

  if (context.executable_name)
    Out << "Running " << context.executable_name << "\n";

  const CPUInfo &info = context.cpu_info;
  Out << "Run on (" << info.num_cpus << " X "
      << (info.cycles_per_second / 1000000.0) << " MHz CPU "
      << ((info.num_cpus > 1) ? "s" : "") << ")\n";
  if (info.caches.size() != 0) {
    Out << "CPU Caches:\n";
    for (auto &CInfo : info.caches) {
      Out << "  L" << CInfo.level << " " << CInfo.type << " "
          << (CInfo.size / 1000) << "K";
      if (CInfo.num_sharing != 0)
        Out << " (x" << (info.num_cpus / CInfo.num_sharing) << ")";
      Out << "\n";
    }
  }

  if (info.scaling_enabled) {
    Out << "***WARNING*** CPU scaling is enabled, the benchmark "
           "real time measurements may be noisy and will incur extra "
           "overhead.\n";
  }

#ifndef NDEBUG
  Out << "***WARNING*** Library was built as DEBUG. Timings may be "
         "affected.\n";
#endif
}
    
    const BenchmarkReporter::Run * BenchmarkReporter::GetRunWithIndex(const size_t run_index)
    {
        const auto it = std::find_if(completed_runs_.begin(), completed_runs_.end(), [&run_index](const Run& r){
            return r.index == run_index;
        });
        if (it != completed_runs_.end()){
            return &(*it);
        } else {
            return nullptr;
        }
    }

    void BenchmarkReporter::AppendCompletedRuns(const std::vector<Run>& r){
        completed_runs_.reserve(completed_runs_.size() + r.size());
        completed_runs_.insert(completed_runs_.end(), r.begin(), r.end());
    }
    
// No initializer because it's already initialized to NULL.
const char *BenchmarkReporter::Context::executable_name;

BenchmarkReporter::Context::Context() : cpu_info(CPUInfo::Get()), report_baseline(false) {}

double BenchmarkReporter::Run::GetAdjustedRealTime() const {
  double new_time = real_accumulated_time * GetTimeUnitMultiplier(time_unit);
  if (iterations != 0) new_time /= static_cast<double>(iterations);
  return new_time;
}

double BenchmarkReporter::Run::GetAdjustedCPUTime() const {
  double new_time = cpu_accumulated_time * GetTimeUnitMultiplier(time_unit);
  if (iterations != 0) new_time /= static_cast<double>(iterations);
  return new_time;
}

}  // end namespace benchmark
