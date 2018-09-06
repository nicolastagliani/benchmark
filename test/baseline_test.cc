#include "benchmark/benchmark.h"

#include <memory>
#include <list>
#include <set>
#include <vector>

static void BM_InsertVector(benchmark::State& state) {
  int v = 42;
  for (auto _ : state) {
    std::vector<int> c;
    for (auto i = state.range(0); --i;) c.push_back(v);
  }
  state.SetItemsProcessed(state.iterations() * state.range(0));
  state.SetBytesProcessed(state.iterations() * state.range(0) * sizeof(v));
}

static void BM_InsertList(benchmark::State& state) {
  int v = 42;
  for (auto _ : state) {
    std::list<int> c;
    for (auto i = state.range(0); --i;) c.push_back(v);
  }
  state.SetItemsProcessed(state.iterations() * state.range(0));
  state.SetBytesProcessed(state.iterations() * state.range(0) * sizeof(v));
}

static void BM_InsertSet(benchmark::State& state) {
    int v = 42;
    for (auto _ : state) {
        std::set<int> c;
        for (auto i = state.range(0); --i;) c.insert(v);
    }
    state.SetItemsProcessed(state.iterations() * state.range(0));
    state.SetBytesProcessed(state.iterations() * state.range(0) * sizeof(v));
}

BENCHMARK(BM_InsertVector)->Range(1 << 0, 1 << 10);
BENCHMARK(BM_InsertList)->Range(1 << 0, 1 << 10)->Baseline(BM_InsertVector);
BENCHMARK(BM_InsertSet)->Range(1 << 0, 1 << 10)->Baseline(BM_InsertList);


class MyFixture : public ::benchmark::Fixture {
public:
    void SetUp(const ::benchmark::State& state) {
        if (state.thread_index == 0) {
            assert(data.get() == nullptr);
            data.reset(new int(42));
        }
    }
    
    void TearDown(const ::benchmark::State& state) {
        if (state.thread_index == 0) {
            assert(data.get() != nullptr);
            data.reset();
        }
    }
    
    ~MyFixture() { assert(data == nullptr); }
    
    std::unique_ptr<int> data;
};

BENCHMARK_F(MyFixture, Foo)(benchmark::State &st) {
    assert(data.get() != nullptr);
    assert(*data == 42);
    for (auto _ : st) {
    }
}

BENCHMARK_DEFINE_F(MyFixture, Bar)(benchmark::State& st) {
    if (st.thread_index == 0) {
        assert(data.get() != nullptr);
        assert(*data == 42);
    }
    for (auto _ : st) {
        assert(data.get() != nullptr);
        assert(*data == 42);
    }
    st.SetItemsProcessed(st.range(0));
}

BENCHMARK_DEFINE_F(MyFixture, Test)(benchmark::State& st) {
    if (st.thread_index == 0) {
        assert(data.get() != nullptr);
        assert(*data == 42);
    }
    for (auto _ : st) {
        assert(data.get() != nullptr);
        int test = 0;
        test = *data;
        assert(test == 42);
        
    }
    st.SetItemsProcessed(st.range(0));
}

BENCHMARK_REGISTER_F(MyFixture, Bar)->Arg(42);
BENCHMARK_REGISTER_F(MyFixture, Bar)->Arg(42)->ThreadPerCpu();
BENCHMARK_REGISTER_F(MyFixture, Test)->Arg(42)->Baseline_F(MyFixture, Bar);
BENCHMARK_REGISTER_F(MyFixture, Test)->Arg(42)->ThreadPerCpu()->Baseline_F(MyFixture, Bar);

template <typename T>
class TemplatedFixture : public ::benchmark::Fixture {
public:
    TemplatedFixture() : data(0) {}
    
    T data;
};


BENCHMARK_TEMPLATE_DEFINE_F(TemplatedFixture, PushVector, std::vector<int>)(benchmark::State& st) {
    for (auto _ : st) {
        data.push_back(1.0);
    }
}

BENCHMARK_TEMPLATE_DEFINE_F(TemplatedFixture, PushList, std::list<int>)(benchmark::State& st) {
    for (auto _ : st) {
        data.push_back(1.0);
    }
}

BENCHMARK_REGISTER_F(TemplatedFixture, PushVector);
BENCHMARK_REGISTER_F(TemplatedFixture, PushList)->Baseline_T(TemplatedFixture, PushVector, std::vector<int>);
BENCHMARK_REGISTER_F(TemplatedFixture, PushVector)->Range(1 << 0, 1 << 4);
BENCHMARK_REGISTER_F(TemplatedFixture, PushList)->Range(1 << 0, 1 << 4)->Baseline_T(TemplatedFixture, PushVector, std::vector<int>);

BENCHMARK_MAIN();
