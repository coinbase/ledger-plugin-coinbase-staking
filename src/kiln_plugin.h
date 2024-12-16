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

#pragma once

#include <string.h>

#include "eth_plugin_interface.h"
#include "cx.h"
#include <ctype.h>

#define PLUGIN_NAME "Coinbase"

// ****************************************************************************
// * SUPPORTED SELECTORS
// ****************************************************************************

// Available selectors:
//
// V2 selectors
// --- 1. stake()
// --- 2. requestExit(shares_amount)
// --- 3. multiClaim(exit_queues, ticket_ids, cask_ids)
// --- 4. claim(uint256[],uint32[],uint16)
//
#define NUM_SELECTORS 4
extern const uint32_t KILN_SELECTORS[NUM_SELECTORS];

// Selectors available (see mapping above).
typedef enum {
    KILN_V2_STAKE = 0,
    KILN_V2_REQUEST_EXIT,
    KILN_V2_MULTICLAIM,
    KILN_V2_CLAIM,
} selector_t;

// ****************************************************************************
// * UTILS
// ****************************************************************************

typedef struct {
    uint8_t prev_checksum[CX_KECCAK_256_SIZE];
    uint32_t new_offset;
} checksum_offset_params_t;

// ****************************************************************************
// * GLOBALS
// ****************************************************************************

#define ADDRESS_STR_LEN      42
#define OCV2_MAX_EXIT_QUEUES 2
extern const char ocv2_exit_queues[OCV2_MAX_EXIT_QUEUES][ADDRESS_STR_LEN];

// ****************************************************************************
// * PARSERS STATE MACHINES
// ****************************************************************************

typedef enum {
    V2_REQUEST_EXIT_UNEXPECTED_PARAMETER = 0,
    V2_REQUEST_EXIT_AMOUNT,
} v2_request_exit_parameters;

typedef enum {
    V2_CLAIM_UNEXPECTED_PARAMETER = 0,
    V2_CLAIM_TICKET_IDS_OFFSET,
    V2_CLAIM_CASK_IDS_OFFSET,
    V2_CLAIM_MAX_CLAIM_DEPTH,
    V2_CLAIM_TICKET_IDS_LENGTH,
    V2_CLAIM_TICKET_IDS__ITEMS,
    V2_CLAIM_CASK_IDS_LENGTH,
    V2_CLAIM_CASK_IDS__ITEMS,
} v2_claim;

typedef enum {
    V2_MULTICLAIM_UNEXPECTED_PARAMETER = 0,
    V2_MULTICLAIM_EXIT_QUEUES_OFFSET,
    V2_MULTICLAIM_TICKET_IDS_OFFSET,
    V2_MULTICLAIM_CASK_IDS_OFFSET,

    V2_MULTICLAIM_EXIT_QUEUES_LENGTH,
    V2_MULTICLAIM_EXIT_QUEUES__ITEMS,

    V2_MULTICLAIM_TICKETIDS_LENGTH,
    V2_MULTICLAIM_TICKETIDS__OFFSET_ITEMS,
    V2_MULTICLAIM_TICKETIDS__ITEM_LENGTH,
    V2_MULTICLAIM_TICKETIDS__ITEM__ITEMS,

    V2_MULTICLAIM_CASKIDS_LENGTH,
    V2_MULTICLAIM_CASKIDS__OFFSET_ITEMS,
    V2_MULTICLAIM_CASKIDS__ITEM_LENGTH,
    V2_MULTICLAIM_CASKIDS__ITEM__ITEMS,
} v2_multiclaim_parameters;

// ****************************************************************************
// * PARSERS DATA STRUCTURES
// ****************************************************************************

typedef struct {
    uint8_t amount[INT256_LENGTH];
} v2_request_exit_t;

typedef struct {
    // -- utils
    uint16_t cask_ids_offset;

    uint16_t current_item_count;
} v2_claim_t;

typedef struct {
    // -- utils
    uint16_t ticket_ids_offset;
    uint16_t cask_ids_offset;
    uint8_t checksum_preview[CX_KECCAK_256_SIZE];
    uint8_t checksum_value[CX_KECCAK_256_SIZE];
    uint32_t cached_offset;

    uint16_t parent_item_count;
    uint16_t current_item_count;
} v2_multiclaim_t;

// ****************************************************************************
// * SHARED PLUGIN CONTEXT MEMORY
// ****************************************************************************

typedef struct context_t {
    uint8_t next_param;

    union {
        v2_request_exit_t v2_request_exit;
        v2_claim_t v2_claim;
        v2_multiclaim_t v2_multiclaim;
    } param_data;

    selector_t selectorIndex;
} context_t;

_Static_assert(sizeof(context_t) <= 5 * 32, "Structure of parameters too big.");
