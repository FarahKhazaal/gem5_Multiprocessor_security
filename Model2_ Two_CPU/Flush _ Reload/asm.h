/**
 * \brief  ARM Assembly.
 * \author Pierre AYOUB -- IRISA, CNRS
 * \date   2020
 *
 * \details Contain all ARM assembly related stuff. It can be constants or
 *          (macro-)function. The ARM ISA targeted here is only the ARMv8-A
 *          one.
 */

#ifndef _ASM_H_
#define _ASM_H_

#include <stdint.h>
#include <time.h>

/* * Documentation: */

/** DSB -- Data Synchronization Barrier for Reads/Writes
 * 
 * A DSB instruction is a memory barrier that ensures that memory accesses that
 * occur before the DSB instruction have completed before the completion of the
 * DSB instruction. In doing this, it acts as a stronger barrier than a DMB and
 * all ordering that is created by a DMB with specific options is also
 * generated by a DSB with the same options.
 *
 * A DSB instruction executed completes when all the following apply: 1) All
 * explicit memory accesses of the required access types appearing in program
 * order before the DSB are complete for the set of observers in the required
 * shareability domain. 2) If the required access types of the DSB is reads and
 * writes, then all cache maintenance instructions, all TLB maintenance
 * instructions, and all PSB CYNC instructions issued before the DSB are
 * complete for the required shareability domain.
 * 
 * In addition, no instruction that appears in program order after the DSB
 * instruction can alter any state of the system or perform any part of its
 * functionality until the DSB completes other than: 1) Being fetched from
 * memory and decoded. 2) Reading the general-purpose, SIMD and floating-point,
 * Special-purpose, or System registers that are directly or indirectly read
 * without causing side-effects.
 * 
 * Options:
 * - ISH: Inner Shareable
 * - SY:  Full System
 */

/** ISB -- Instruction Synchronization Barrier
 * 
 * An ISB instruction ensures that all instructions that come after the ISB
 * instruction in program order are fetched from the cache or memory after the
 * ISB instruction has completed. Using an ISB ensures that the effects of
 * context-changing operations executed before the ISB are visible to the
 * instructions fetched after the ISB instruction. Examples of context-changing
 * operations that require the insertion of an ISB instruction to ensure the
 * effects of the operation are visible to instructions fetched after the ISB
 * instruction are: 1) Completed cache and TLB maintenance instructions. 2)
 * Changes to System registers. Any context-changing operations appearing in
 * program order after the ISB instruction only take effect after the ISB has
 * been executed.
 */

/** DC CIVAC -- Data Cache Maintenance 
 *
 * Clean and Invalidate data cache by address to Point of Coherency (The point
 * at which all agents that can access memory are guaranteed to see the same
 * copy of a memory location for accesses of any memory type or cacheability
 * attribute. In many cases this is effectively the main system memory,
 * although the architecture does not prohibit the implementation of caches
 * beyond the PoC that have no effect on the coherency between memory system
 * agents.).
 *
 * Arguments:
 * - Virtual address to use. No alignment restrictions apply to this VA.
 */

/* * Implementation: */

/* ** Macro-functions: */

/**
 * \brief   Memory barrier for the inner shareable domain.
 * \details Memory barrier that ensures that memory accesses that occur before
 *          the function have completed before the completion of the function.
 */
#define mfence()                                \
    do {                                        \
        asm volatile("DSB ISH");                \
    } while (0)

/**
 * \brief   Memory barrier for the system domain.
 * \details Memory barrier that ensures that memory accesses that occur before
 *          the function have completed before the completion of the function.
 */

#define mfence_sys()                            \
    do {                                        \
        asm volatile("DSB SY");                 \
    } while (0)

/**
 * \brief   Instruction barrier.
 * \details Ensures that all instructions that come after the function in
 *          program order are fetched from the cache or memory after the
 *          function has completed.
 */
#define ifence()                                \
    do {                                        \
        asm volatile("ISB");                    \
    } while (0)

/**
 * \brief   Flush an address from the cache.
 * \details Clean and Invalidate data cache by address to Point of Coherency.
 * \warning After flushing, IMPERATIVELY use in this order:
 *          - \sa {mfence}
 *          - \sa {ifence}
 *
 * \param  pointer Virtual address (VA)) to flush from the cache.
 */
#define flush(pointer)                                  \
    do {                                                \
        asm volatile("DC CIVAC, %0" : : "r" (pointer)); \
    } while (0)

/**
 * \brief   Prohibit speculation.
 * \details By using a \sa {mfence_sys} and a \sa {ifence}, prohibit the CPU to
 *          speculate for the instructions that come after the function.
 */
#define nospec()                                \
    do {                                        \
        mfence_sys();                           \
        ifence();                               \
    } while (0)

/**
 * \brief Access to a byte.
 *
 * \param ptr Pointer to the byte to access.
 */
#define mem_access(ptr)                                             \
    do {                                                            \
        volatile uint32_t val;                                      \
        asm volatile("LDR %0, [%1]" : "=r"(val) : "r"(ptr));        \
        mfence();                                                   \
        ifence();                                                   \
    } while (0)

/* ** Static functions: */

/**
 * \brief   Read Time-Stamp Counter.
 * \details Reads the current value of the processor's time-stamp counter. The
 *          processor monotonically increments the time-stamp counter every
 *          clock cycle. This function is serializing. It does necessarily wait
 *          until all previous instructions have been executed before reading
 *          the counter. Similarly, subsequent instructions can't begin
 *          execution before the read operation is performed.
 * \note To simulate the x86 rdtsc instruction, we can either use the
 *       PMCCNTR_EL0 or an approximation. This counter require to enable PMU
 *       access to the user-land from a kernel module. Two good approximations
 *       are:
 *       - clock_gettime().
 *       - Counter thread (a thread on a separate core, which increment a
 *         counter indefinitely).
 *       For this implementation, we use the clock_gettime() which is enough.
 *
 * \warning DON'T PUT this function into a corresponding ".c" file. The reason
 *          is that if you implement the rdtsc() into an "asm.c" file and you
 *          link the "asm.o" object file with the "spectre.o" object file,
 *          Spectre will badly work (no guessed byte). It doesn't seems that
 *          the cause is the supplementary compilation/linkage step, but other
 *          thing (the distance between the function implementation and the
 *          code that use it ?). Thus, the function is declared static to be
 *          compliant with the "one definition" rule.

 * \return uint64_t 64-bit value of the counter.
 */
static uint64_t rdtsc() {
    /* Serialization. */
    mfence_sys();
    ifence();
    /* Get the current time. */
    struct timespec t1;
    /* CLOCK_MONOTONIC represents the absolute elapsed wall-clock time since
     * some arbitrary, fixed point in the past. It represents monotonic time
     * since—as described by POSIX—"some un specified point in the past". This
     * clock advances at one tick per tick. The important aspect of a monotonic
     * time source is NOT the current value, but the guarantee that the time
     * source is strictly linearly increasing, and thus useful for calculating
     * the difference in time between two samplings. */
    clock_gettime(CLOCK_MONOTONIC, &t1);
    /* "res" count the number of nanoseconds. */
    uint64_t res = t1.tv_sec * 1000 * 1000 * 1000ULL + t1.tv_nsec;
    /* Serialization. */
    ifence();
    mfence_sys();
    return res;
}

/* * Prototypes: */

/**
 * \brief Measure the time taken to load a byte.
 *
 * \param ptr Pointer to the byte to load.
 * \return int Number of cycle taken to load the byte.
 */
int reload_t(void *ptr);

/**
 * \brief Measure the time taken to load a flushed byte.
 *
 * \param ptr Pointer to the byte to load.
 * \return int Number of cycle taken to load the flushed byte.
 */
int flush_reload_t(void *ptr);

#endif
