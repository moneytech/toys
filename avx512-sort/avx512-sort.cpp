#include <immintrin.h>


#define FORCE_INLINE inline __attribute__((always_inline))


__m512i avx512_sort_epi32(const __m512i v) {

    __m512i result = _mm512_setzero_si512();
    __m512i index = _mm512_setzero_si512();
    __m512i incr  = _mm512_set1_epi32(1);

#define STEP(unused) { \
    const __m512i  b    = _mm512_permutexvar_epi32(index, v); \
    const uint16_t lt   = _mm_popcnt_u32(_mm512_cmplt_epi32_mask(v, b)); \
    const uint16_t eq   = _mm_popcnt_u32(_mm512_cmpeq_epi32_mask(v, b)); \
    const uint16_t mask = (uint32_t(1) << (lt + eq)) - (uint32_t(1) << lt); \
    result = _mm512_mask_mov_epi32(result, mask, b); \
    index  = _mm512_add_epi32(index, incr); \
}

    STEP(0);
    STEP(1);
    STEP(2);
    STEP(3);
    STEP(4);
    STEP(5);
    STEP(6);
    STEP(7);
    STEP(8);
    STEP(9);
    STEP(10);
    STEP(11);
    STEP(12);
    STEP(13);
    STEP(14);
    STEP(15);

#undef STEP

    return result;
}


__m512i avx512_sort_epi32_unrolled(const __m512i v) {

    __m512i result = _mm512_setzero_si512();
    __m512i index0 = _mm512_set1_epi32(0);
    __m512i index1 = _mm512_set1_epi32(1);
    __m512i index2 = _mm512_set1_epi32(2);
    __m512i index3 = _mm512_set1_epi32(3);
    __m512i incr   = _mm512_set1_epi32(4);

#define STEP(__number__) { \
    const __m512i  b0    = _mm512_permutexvar_epi32(index0, v); \
    const __m512i  b1    = _mm512_permutexvar_epi32(index1, v); \
    const __m512i  b2    = _mm512_permutexvar_epi32(index2, v); \
    const __m512i  b3    = _mm512_permutexvar_epi32(index3, v); \
    index0  = _mm512_add_epi32(index0, incr); \
    index1  = _mm512_add_epi32(index1, incr); \
    index2  = _mm512_add_epi32(index2, incr); \
    index3  = _mm512_add_epi32(index3, incr); \
    const uint16_t lt0   = _mm_popcnt_u32(_mm512_cmplt_epi32_mask(v, b0)); \
    const uint16_t eq0   = _mm_popcnt_u32(_mm512_cmpeq_epi32_mask(v, b0)); \
    const uint16_t lt1   = _mm_popcnt_u32(_mm512_cmplt_epi32_mask(v, b1)); \
    const uint16_t eq1   = _mm_popcnt_u32(_mm512_cmpeq_epi32_mask(v, b1)); \
    const uint16_t lt2   = _mm_popcnt_u32(_mm512_cmplt_epi32_mask(v, b2)); \
    const uint16_t eq2   = _mm_popcnt_u32(_mm512_cmpeq_epi32_mask(v, b2)); \
    const uint16_t lt3   = _mm_popcnt_u32(_mm512_cmplt_epi32_mask(v, b3)); \
    const uint16_t eq3   = _mm_popcnt_u32(_mm512_cmpeq_epi32_mask(v, b3)); \
    const uint16_t mask0 = (uint32_t(1) << (lt0 + eq0)) - (uint32_t(1) << lt0); \
    const uint16_t mask1 = (uint32_t(1) << (lt1 + eq1)) - (uint32_t(1) << lt1); \
    const uint16_t mask2 = (uint32_t(1) << (lt2 + eq2)) - (uint32_t(1) << lt2); \
    const uint16_t mask3 = (uint32_t(1) << (lt3 + eq3)) - (uint32_t(1) << lt3); \
    result = _mm512_mask_mov_epi32(result, mask0, b0); \
    result = _mm512_mask_mov_epi32(result, mask1, b1); \
    result = _mm512_mask_mov_epi32(result, mask2, b2); \
    result = _mm512_mask_mov_epi32(result, mask3, b3); \
}

    STEP(0);
    STEP(1);
    STEP(2);
    STEP(3);

#undef STEP

    return result;
}


__m512i avx512_sort_loop_epi32(const __m512i v) {

    __m512i result = _mm512_setzero_si512();
    __m512i index = _mm512_setzero_si512();
    __m512i incr  = _mm512_set1_epi32(1);

    for (int i=0; i < 16; i++) {
        const __m512i  b    = _mm512_permutexvar_epi32(index, v);
        const uint16_t lt   = _mm_popcnt_u32(_mm512_cmplt_epi32_mask(v, b));
        const uint16_t eq   = _mm_popcnt_u32(_mm512_cmpeq_epi32_mask(v, b));
        const uint16_t mask = (uint32_t(1) << (lt + eq)) - (uint32_t(1) << lt);
        result = _mm512_mask_mov_epi32(result, mask, b);
        index  = _mm512_add_epi32(index, incr);
    }

    return result;
}


__m512i avx512_sort_while_epi32(const __m512i v) {

    __m512i result = _mm512_setzero_si512();
    __m512i index = _mm512_setzero_si512();
    __m512i incr  = _mm512_set1_epi32(1);

    uint16_t updated = 0;
    while (updated != 0xffff) {
        const __m512i  b    = _mm512_permutexvar_epi32(index, v);
        const uint16_t lt   = _mm_popcnt_u32(_mm512_cmplt_epi32_mask(v, b));
        const uint16_t eq   = _mm_popcnt_u32(_mm512_cmpeq_epi32_mask(v, b));
        const uint16_t mask = (uint32_t(1) << (lt + eq)) - (uint32_t(1) << lt);
        result = _mm512_mask_mov_epi32(result, mask, b);
        index  = _mm512_add_epi32(index, incr);

        updated |= mask;
    }

    return result;
}


void FORCE_INLINE avx512_sort2xreg_update(const __m512i b, const __m512i v1, const __m512i v2, __m512i& r1, __m512i& r2) {

    const uint64_t lt1    = _mm512_cmplt_epi32_mask(v1, b);
    const uint64_t lt2    = _mm512_cmplt_epi32_mask(v2, b);
    const uint64_t lt_cnt = _mm_popcnt_u64(lt1 | (lt2 << 16));

    const uint64_t eq1    = _mm512_cmpeq_epi32_mask(v1, b);
    const uint64_t eq2    = _mm512_cmpeq_epi32_mask(v2, b);
    const uint64_t eq_cnt = _mm_popcnt_u64(eq1 | (eq2 << 16));

    const uint64_t mask   = (uint64_t(1) << (lt_cnt + eq_cnt)) - (uint64_t(1) << lt_cnt);

    r1 = _mm512_mask_mov_epi32(r1, mask & 0xffff, b);
    r2 = _mm512_mask_mov_epi32(r2, mask >> 16, b);
}


void avx512_sort2xreg_epi32(const __m512i v1, const __m512i v2, __m512i& r1, __m512i& r2) {

    r1 = _mm512_setzero_si512();
    r2 = _mm512_setzero_si512();
    __m512i index;
    __m512i incr  = _mm512_set1_epi32(1);

#define STEP(input) { \
    const __m512i  b = _mm512_permutexvar_epi32(index, input); \
    index  = _mm512_add_epi32(index, incr); \
    avx512_sort2xreg_update(b, v1, v2, r1, r2); \
}

#define STEPx4(input) \
    STEP(input); STEP(input); STEP(input); STEP(input);

#define STEPx16(input) \
    index = _mm512_setzero_si512(); \
    STEPx4(input); STEPx4(input); STEPx4(input); STEPx4(input);

    STEPx16(v1);
    STEPx16(v2);

#undef STEP
#undef STEPx4
#undef STEPx16
}
