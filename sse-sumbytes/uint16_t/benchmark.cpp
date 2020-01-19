#include <cstdio>
#include <vector>

#include "benchmark.h"
#include "all.h"

class Benchmark {

    std::vector<uint16_t> input;
    size_t result;

public:
    Benchmark(size_t size) : input(size) {}

public:
    void run() {
        test("scalar",                 scalar_sumwords);
        test("scalar (C++)",           scalar_cpp_sumwords);
        test("SSE",                    sse_sumwords);
        test("SSE (v2)",               sse_sumwords_variant2);
        test("SSE (sadbw)",            sse_sadbw_sumwords);
        test("SSE (sadbw, v2)",        sse_sadbw_sumwords_variant2);
        test("SSE (sadbw, unrolled)",  sse_sadbw_unrolled4_sumwords);
#ifdef HAVE_AVX2
        test("AVX2",                   avx2_sumwords);
        test("AVX2 (v2)",              avx2_sumwords_variant2);
        test("AVX2 (sadbw)",           avx2_sadbw_sumwords);
        test("AVX2 (sadbw-v2)",        avx2_sadbw_sumwords_variant2);
        test("AVX2 (sadbw, unrolled)", avx2_sadbw_unrolled4_sumwords);
        test("AVX2 (madd)",            avx2_madd_sumwords);
#endif
    }

private:
    template <typename FUN>
    void test(const char* name, FUN function) {

        const size_t repeat = 10000;
        const size_t size = input.size();

        auto wrapper = [this, function]() {
            result = function(&input[0], input.size());
        };

        BEST_TIME(/**/, wrapper(), name, repeat, size);
    }

};

int main() {

    std::vector<size_t> sizes = {1024*4, 1024*16, 1024*32};

    for (size_t size: sizes) {
        printf("element count %lu\n", size);
        Benchmark bench(size);
        bench.run();
    }

    return 0;
}
