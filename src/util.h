#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// A small library of useful functions - Saul Dugan 2026

                                                                                
// Typedefs and Utils
                                                                                

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef i8 b8;
typedef i32 b32;

typedef float f32;
typedef double f64;

#define KiB(n) ((u64)(n) << 10)
#define MiB(n) ((u64)(n) << 20)
#define GiB(n) ((u64)(n) << 30)

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define ALIGN_UP_POW2(n, p) (((u64)(n) + ((u64)(p) - 1)) & (~((u64)(p) - 1)))

                                                                                
// Length-based Strings
                                                                                

#define STR8_LIT(s) (string8){ (u8*)(s), sizeof((s)) - 1 }
#define STR8_FMT(s8) (int)(s8).size, (s8.str)

typedef struct {
    u8* str;
    u64 size;
} string8;

string8 str8_substr(string8 str, u64 start, u64 end);

                                                                                
// Arena Allocator
                                                                                

#define ARENA_BASE_POS (sizeof(mem_arena))
#define ARENA_ALIGN (sizeof(void*))

typedef struct {
    u64 reserve_size;
    u64 commit_size;

    u64 pos;
    u64 commit_pos;
} mem_arena;

typedef struct {
    mem_arena* arena;
    u64 start_pos;
} mem_arena_temp;

mem_arena* arena_create(u64 reserve_size, u64 commit_size);
void arena_destroy(mem_arena* arena);

void* arena_push(mem_arena* arena, u64 size, b32 non_zero);
void arena_pop(mem_arena* arena, u64 size);
void arena_pop_to(mem_arena* arena, u64 pos);
void arena_clear(mem_arena* arena);

mem_arena_temp arena_temp_begin(mem_arena* arena);
void arena_temp_end(mem_arena_temp temp);

mem_arena_temp arena_scratch_get(mem_arena** conflicts, u32 num_conflicts);
void arena_scratch_release(mem_arena_temp scratch);

#define PUSH_STRUCT(arena, T) (T*)arena_push((arena), sizeof(T), false)
#define PUSH_STRUCT_NZ(arena, T) (T*)arena_push((arena), sizeof(T), true);
#define PUSH_ARRAY(arena, T, n) (T*)arena_push((arena), sizeof(T) * (n), false)
#define PUSH_ARRAY_NZ(arena, T, n)                                             \
    (T*)arena_push((arena), sizeof(T) * (n), true)

u32 plat_get_pagesize(void);

void* plat_mem_reserve(u64 size);
b32 plat_mem_commit(void* ptr, u64 size);
b32 plat_mem_decommit(void* ptr, u64 size);
b32 plat_mem_release(void*, u64 size);

                                                                                
// PCG Random Number Generator
                                                                                

typedef struct {
    u64 state;
    u64 inc;
} prng_state;

void prng_seed_r(prng_state* rng, u64 initstate, u64 initseq);
void prng_seed(u64 initstate, u64 initseq);

u32 prng_rand_r(prng_state* rng);
u32 prng_rand(void);

f32 prng_randf_r(prng_state* rng);
f32 prng_randf(void);

static prng_state s_prng_state = {0x853c49e6748fea9bULL, 0xda3e39cb94b95bdbULL};
