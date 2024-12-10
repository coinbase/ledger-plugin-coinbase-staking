/*******************************************************************************
 *
 * ██╗  ██╗██╗██╗     ███╗   ██╗
 * ██║ ██╔╝██║██║     ████╗  ██║
 * █████╔╝ ██║██║     ██╔██╗ ██║
 * ██╔═██╗ ██║██║     ██║╚██╗██║
 * ██║  ██╗██║███████╗██║ ╚████║
 * ╚═╝  ╚═╝╚═╝╚══════╝╚═╝  ╚═══╝
 *
 * Kiln Ethereum Ledger App
 * (c) 2022-2024 Kiln
 *
 * contact@kiln.fi
 ********************************************************************************/

#include "kiln_plugin.h"

// ONCHAIN V2 - Pooled Staking
// --- cast sig "stake()"
static const uint32_t KILN_V2_STAKE_SELECTOR = 0x3a4b66f1;
// --- cast sig "requestExit(uint256)"
static const uint32_t KILN_V2_REQUEST_EXIT_SELECTOR = 0x721c6513;
// --- cast sig "multiClaim(address[],uint256[][],uint32[][])"
static const uint32_t KILN_V2_MULTICLAIM_SELECTOR = 0xb7ba18c7;
// --- cast sig "claim(uint256[],uint32[],uint16)"
static const uint32_t KILN_V2_CLAIM_SELECTOR = 0xadcf1163;

// Array of all supported OCV2 exit queues.
const char ocv2_exit_queues[OCV2_MAX_EXIT_QUEUES][ADDRESS_STR_LEN] = {
    "0x8d6Fd650500f82c7D978a440348e5a9b886943bF",  // Kiln
    "0x86358F7B33b599c484e0335B8Ee4f7f7f92d8b60"   // Coinbase
};

// Array of all supported selectors.
const uint32_t KILN_SELECTORS[NUM_SELECTORS] = {
    // V2
    KILN_V2_STAKE_SELECTOR,
    KILN_V2_REQUEST_EXIT_SELECTOR,
    KILN_V2_MULTICLAIM_SELECTOR,
    KILN_V2_CLAIM_SELECTOR,
};
