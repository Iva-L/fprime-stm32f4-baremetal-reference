/**
 * \file
 * \brief Project override for Svc::TlmChan hash-table sizing
 *
 * The framework default (TLMCHAN_HASH_BUCKETS = 500) is sized for large
 * multi-hundred-channel deployments and is the single largest static memory
 * consumer in this bare-metal image. The generated dictionary currently has
 * 99 telemetry channels across 19 producing components.
 *
 * Sized here to comfortably exceed the current channel count with headroom
 * for the planned USART1 DMA ground-link driver's telemetry, while freeing
 * the bulk of AXI SRAM back to the budget.
 */

#ifndef TLMCHANIMPLCFG_HPP_
#define TLMCHANIMPLCFG_HPP_

namespace {

enum {
    // Twice the number of components that currently produce telemetry (19),
    // per the framework's own sizing guidance in the default config header.
    TLMCHAN_NUM_TLM_HASH_SLOTS = 41,

    // Only used for 8-bit FwChanIdType; has no effect for this deployment's
    // (wider) channel ID type, kept at the framework default for parity.
    TLMCHAN_HASH_MOD_VALUE = 99,

    // Must be >= number of telemetry channels in the system (99 today); four
    // spare buckets preserve a small amount of growth without wasting SRAM.
    TLMCHAN_HASH_BUCKETS = 104,

    // At least the number of current channels, so no values are deferred.
    TLMCHAN_MAX_ENTRIES_PER_RUN = 104,
};

}

#endif /* TLMCHANIMPLCFG_HPP_ */
