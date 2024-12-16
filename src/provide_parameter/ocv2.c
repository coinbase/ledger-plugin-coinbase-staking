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

#include "provide_parameter.h"

void handle_v2_request_exit(ethPluginProvideParameter_t *msg, context_t *context) {
    // **************************************************************************
    // FUNCTION TO PARSE
    // **************************************************************************
    //
    // function requestExit(
    //    uint256 amount
    // ) external
    //
    // **************************************************************************
    // example
    // [  0] selector
    // [  4] amount

    v2_request_exit_t *params = &context->param_data.v2_request_exit;

    switch (context->next_param) {
        case V2_REQUEST_EXIT_AMOUNT:
            copy_parameter(params->amount, msg->parameter, sizeof(params->amount));
            context->next_param = V2_REQUEST_EXIT_UNEXPECTED_PARAMETER;
            break;
        default:
            PRINTF("Param not supported: %d\n", context->next_param);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            return;
    }
    msg->result = ETH_PLUGIN_RESULT_OK;
}

void handle_v2_claim(ethPluginProvideParameter_t *msg, context_t *context) {
    // **************************************************************************
    // FUNCTION TO PARSE
    // **************************************************************************
    //
    // function claim(
    //   uint256[] ticketIds,
    //   uint32[] caskIds,
    //   uint16 maxClaimDepth
    // )
    //
    // **************************************************************************
    // example for 2 tickets and 3 cask ids
    // [  0] selector
    // [  4] ticketIds_offset
    // [ 36] caskIds_offset
    // [ 68] maxClaimDepth
    // [100] ticketIds_length
    // [132] ticketIds_0
    // [164] ticketIds_1
    // [196] caskIds_length
    // [228] caskIds_0
    // [260] caskIds_1
    // [292] caskIds_2

    v2_claim_t *params = &context->param_data.v2_claim;

    switch (context->next_param) {
        case V2_CLAIM_TICKET_IDS_OFFSET: {
            uint16_t offset;
            U2BE_from_parameter(msg->parameter, &offset);
            if (offset != PARAMETER_LENGTH * 3) {
                PRINTF("Malformed calldata, unexpected parameter offset %d != %d\n",
                       offset,
                       PARAMETER_LENGTH);
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }
            context->next_param = V2_CLAIM_CASK_IDS_OFFSET;
            break;
        }
        case V2_CLAIM_CASK_IDS_OFFSET:
            U2BE_from_parameter(msg->parameter, &params->cask_ids_offset);
            params->cask_ids_offset += SELECTOR_SIZE;
            context->next_param = V2_CLAIM_MAX_CLAIM_DEPTH;
            break;
        case V2_CLAIM_MAX_CLAIM_DEPTH:
            context->next_param = V2_CLAIM_TICKET_IDS_LENGTH;
            break;
        case V2_CLAIM_TICKET_IDS_LENGTH:
            U2BE_from_parameter(msg->parameter, &params->current_item_count);
            if (params->current_item_count == 0) {
                context->next_param = V2_CLAIM_CASK_IDS_LENGTH;
            } else {
                context->next_param = V2_CLAIM_TICKET_IDS__ITEMS;
            }
            break;
        case V2_CLAIM_TICKET_IDS__ITEMS:
            if (params->current_item_count > 0) {
                params->current_item_count -= 1;
            }
            if (params->current_item_count == 0) {
                context->next_param = V2_CLAIM_CASK_IDS_LENGTH;
            }
            break;
        case V2_CLAIM_CASK_IDS_LENGTH:
            if (msg->parameterOffset != params->cask_ids_offset) {
                PRINTF("Malformed calldata, unexpected parameter offset %d != %d\n",
                       msg->parameterOffset,
                       params->cask_ids_offset);
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }
            U2BE_from_parameter(msg->parameter, &params->current_item_count);
            if (params->current_item_count == 0) {
                context->next_param = V2_CLAIM_UNEXPECTED_PARAMETER;
            } else {
                context->next_param = V2_CLAIM_CASK_IDS__ITEMS;
            }
            break;
        case V2_CLAIM_CASK_IDS__ITEMS:
            if (params->current_item_count > 0) {
                params->current_item_count -= 1;
            }
            if (params->current_item_count == 0) {
                context->next_param = V2_CLAIM_UNEXPECTED_PARAMETER;
            }
            break;
        default:
            PRINTF("Param not supported: %d\n", context->next_param);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            return;
    }
}

void handle_v2_multiclaim(ethPluginProvideParameter_t *msg, context_t *context) {
    // **************************************************************************
    // FUNCTION TO PARSE
    // **************************************************************************
    //
    // function multiClaim(
    //   address[]  exitQueues,
    //   uint256[][]  ticketIds,
    //   uint32[][]  casksIds
    // )
    //
    // **************************************************************************
    // example for 2 exit queues, 4 tickets and 4 cask ids
    // [  0] selector
    // [  4] exitQueues_offset
    // [ 36] ticketIds_offset
    // [ 68] caskIds_offset
    // [100] exitQueues_length
    // [132] exitQueues_0
    // [164] exitQueues_1
    // [196] ticketIds_length
    // [228] ticketIds_0_offset
    // [260] ticketIds_1_offset
    // [292] ticketIds_0_length
    // [324] ticketIds_0_0
    // [356] ticketIds_0_1
    // [388] ticketIds_1_length
    // [420] ticketIds_1_0
    // [452] ticketIds_1_1
    // [484] caskIds_length
    // [516] caskIds_0_offset
    // [548] caskIds_1_offset
    // [580] caskIds_0_length
    // [612] caskIds_0_0
    // [644] caskIds_0_1
    // [676] caskIds_1_length
    // [708] caskIds_1_0
    // [740] caskIds_1_1

    v2_multiclaim_t *params = &context->param_data.v2_multiclaim;

    switch (context->next_param) {
        case V2_MULTICLAIM_EXIT_QUEUES_OFFSET: {
            uint16_t offset;
            U2BE_from_parameter(msg->parameter, &offset);
            if (offset != PARAMETER_LENGTH * 3) {
                PRINTF("Malformed calldata, unexpected exitqueues parameter offset %d != %d\n",
                       offset,
                       PARAMETER_LENGTH);
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }
            context->next_param = V2_MULTICLAIM_TICKET_IDS_OFFSET;
            break;
        }
        case V2_MULTICLAIM_TICKET_IDS_OFFSET:
            U2BE_from_parameter(msg->parameter, &params->ticket_ids_offset);
            params->ticket_ids_offset += SELECTOR_SIZE;
            context->next_param = V2_MULTICLAIM_CASK_IDS_OFFSET;
            break;
        case V2_MULTICLAIM_CASK_IDS_OFFSET:
            U2BE_from_parameter(msg->parameter, &params->cask_ids_offset);
            params->cask_ids_offset += SELECTOR_SIZE;
            context->next_param = V2_MULTICLAIM_EXIT_QUEUES_LENGTH;
            break;
        case V2_MULTICLAIM_EXIT_QUEUES_LENGTH:
            U2BE_from_parameter(msg->parameter, &params->current_item_count);
            if (params->current_item_count == 0) {
                context->next_param = V2_MULTICLAIM_TICKETIDS_LENGTH;
            } else {
                context->next_param = V2_MULTICLAIM_EXIT_QUEUES__ITEMS;
            }
            break;
        case V2_MULTICLAIM_EXIT_QUEUES__ITEMS: {
            uint8_t buffer[ADDRESS_LENGTH];
            copy_address(buffer, msg->parameter, sizeof(buffer));
            char address_buffer[ADDRESS_STR_LEN + 1];
            getEthDisplayableAddress(buffer, address_buffer, sizeof(address_buffer), 0);
            // we add a check to make sure we know the exit queue addresses
            // that will be called in the multiclaim tx
            bool is_valid = false;
            for (int i = 0; i < OCV2_MAX_EXIT_QUEUES; i += 1) {
                if (memcmp(ocv2_exit_queues[i], address_buffer, ADDRESS_STR_LEN) == 0) {
                    is_valid = true;
                    break;
                }
            }
            if (!is_valid) {
                PRINTF("Unexpected exit queue address: %s\n", address_buffer);
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }

            if (params->current_item_count > 0) {
                params->current_item_count -= 1;
            }
            if (params->current_item_count == 0) {
                context->next_param = V2_MULTICLAIM_TICKETIDS_LENGTH;
            }
            break;
        }
        case V2_MULTICLAIM_TICKETIDS_LENGTH:
            if (msg->parameterOffset != params->ticket_ids_offset) {
                PRINTF("Malformed calldata, unexpected ticketids[X][] parameter offset %d != %d\n",
                       msg->parameterOffset,
                       params->ticket_ids_offset);
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }

            memset(params->checksum_preview, 0, sizeof(params->checksum_preview));
            memset(params->checksum_value, 0, sizeof(params->checksum_value));
            params->cached_offset = 0;

            U2BE_from_parameter(msg->parameter, &params->parent_item_count);
            params->current_item_count = params->parent_item_count;
            if (params->current_item_count == 0) {
                context->next_param = V2_MULTICLAIM_CASKIDS_LENGTH;
            } else {
                context->next_param = V2_MULTICLAIM_TICKETIDS__OFFSET_ITEMS;
            }
            break;
        // ********************************************************************
        // TICKETIDS[][]
        // ********************************************************************
        case V2_MULTICLAIM_TICKETIDS__OFFSET_ITEMS: {
            uint16_t offset;
            U2BE_from_parameter(msg->parameter, &offset);
            // We have limited size on the context and can't store all the offset values
            // of the ticketids subarrays. So we compute their checksum and expect to
            // be able to recompute it using the offset of the parsed structures later.
            // _preview will be equal to _value at the end of the parsing if everything is fine
            checksum_offset_params_t h_params;
            memset(&h_params, 0, sizeof(h_params));
            memcpy(&h_params.prev_checksum,
                   &(params->checksum_preview),
                   sizeof(h_params.prev_checksum));

            // if we are on the first element of the array, we save the offset, which all
            // received offset values will be based on for checksum computation
            if (params->cached_offset == 0) {
                params->cached_offset = msg->parameterOffset;
            }

            // we hash the previous checksum with the offset of the beginning of the structure.
            h_params.new_offset = offset + params->cached_offset;
            PRINTF("V2_MULTICLAIM_TICKETIDS__OFFSET_ITEMS_PREVIEW: %d\n", h_params.new_offset);

            if (cx_keccak_256_hash((void *) &h_params,
                                   sizeof(h_params),
                                   params->checksum_preview) != CX_OK) {
                PRINTF("unable to compute keccak hash\n");
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }

            if (params->current_item_count > 0) {
                params->current_item_count -= 1;
            }
            if (params->current_item_count == 0) {
                context->next_param = V2_MULTICLAIM_TICKETIDS__ITEM_LENGTH;
            }
            break;
        }
        // ********************************************************************
        // TICKETIDS[][]
        // .          ^
        // ********************************************************************
        case V2_MULTICLAIM_TICKETIDS__ITEM_LENGTH: {
            // here we compute the offset of the ticketIds subarray checksum
            checksum_offset_params_t h_params;
            memset(&h_params, 0, sizeof(h_params));
            memcpy(&h_params.prev_checksum,
                   &(params->checksum_value),
                   sizeof(h_params.prev_checksum));

            // we hash the previous checksum with the offset of the beginning of the structure.
            h_params.new_offset = msg->parameterOffset;
            PRINTF("V2_MULTICLAIM_TICKETIDS__OFFSET_ITEMS_VALUE: %d\n", h_params.new_offset);

            if (cx_keccak_256_hash((void *) &h_params, sizeof(h_params), params->checksum_value) !=
                CX_OK) {
                PRINTF("unable to compute keccak hash\n");
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }

            U2BE_from_parameter(msg->parameter, &params->current_item_count);
            if (params->current_item_count == 0) {
                if (params->parent_item_count > 0) {
                    params->parent_item_count -= 1;
                }
                if (params->parent_item_count == 0) {
                    // we check the checksums
                    if (memcmp(params->checksum_preview,
                               params->checksum_value,
                               sizeof(params->checksum_preview)) != 0) {
                        PRINTF("Tokenids[][] checksums do not match\n");
                        msg->result = ETH_PLUGIN_RESULT_ERROR;
                        return;
                    }

                    context->next_param = V2_MULTICLAIM_CASKIDS_LENGTH;
                }
            } else {
                context->next_param = V2_MULTICLAIM_TICKETIDS__ITEM__ITEMS;
            }
            break;
        }
        case V2_MULTICLAIM_TICKETIDS__ITEM__ITEMS:
            if (params->current_item_count > 0) {
                params->current_item_count -= 1;
            }
            if (params->current_item_count == 0) {
                if (params->parent_item_count > 0) {
                    params->parent_item_count -= 1;
                }
                if (params->parent_item_count == 0) {
                    // we check the checksums
                    if (memcmp(params->checksum_preview,
                               params->checksum_value,
                               sizeof(params->checksum_preview)) != 0) {
                        PRINTF("Tokenids[][] checksums do not match\n");
                        msg->result = ETH_PLUGIN_RESULT_ERROR;
                        return;
                    }

                    context->next_param = V2_MULTICLAIM_CASKIDS_LENGTH;

                } else {
                    context->next_param = V2_MULTICLAIM_TICKETIDS__ITEM_LENGTH;
                }
            }
            break;
        // ********************************************************************
        // CASKIDS[][]
        // ********************************************************************
        case V2_MULTICLAIM_CASKIDS_LENGTH:
            if (msg->parameterOffset != params->cask_ids_offset) {
                PRINTF("Malformed calldata, unexpected caskids parameter offset %d != %d\n",
                       msg->parameterOffset,
                       params->cask_ids_offset);
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }

            memset(params->checksum_preview, 0, sizeof(params->checksum_preview));
            memset(params->checksum_value, 0, sizeof(params->checksum_value));
            params->cached_offset = 0;

            U2BE_from_parameter(msg->parameter, &params->parent_item_count);
            params->current_item_count = params->parent_item_count;
            if (params->current_item_count == 0) {
                context->next_param = V2_MULTICLAIM_UNEXPECTED_PARAMETER;
            } else {
                context->next_param = V2_MULTICLAIM_CASKIDS__OFFSET_ITEMS;
            }
            break;
        case V2_MULTICLAIM_CASKIDS__OFFSET_ITEMS: {
            uint16_t offset;
            U2BE_from_parameter(msg->parameter, &offset);
            // We have limited size on the context and can't store all the offset values
            // of the caskids subarrays. So we compute their checksum and expect to
            // be able to recompute it using the offset of the parsed structures later.
            // _preview will be equal to _value at the end of the parsing if everything is fine
            checksum_offset_params_t h_params;
            memset(&h_params, 0, sizeof(h_params));
            memcpy(&h_params.prev_checksum,
                   &(params->checksum_preview),
                   sizeof(h_params.prev_checksum));

            // if we are on the first element of the array, we save the offset, which all
            // received offset values will be based on for checksum computation
            if (params->cached_offset == 0) {
                params->cached_offset = msg->parameterOffset;
            }

            // we hash the previous checksum with the offset of the beginning of the structure.
            h_params.new_offset = offset + params->cached_offset;
            PRINTF("V2_MULTICLAIM_CASKIDS__OFFSET_ITEMS_PREVIEW: %d\n", h_params.new_offset);

            if (cx_keccak_256_hash((void *) &h_params,
                                   sizeof(h_params),
                                   params->checksum_preview) != CX_OK) {
                PRINTF("unable to compute keccak hash\n");
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }

            if (params->current_item_count > 0) {
                params->current_item_count -= 1;
            }
            if (params->current_item_count == 0) {
                context->next_param = V2_MULTICLAIM_CASKIDS__ITEM_LENGTH;
            }
            break;
        }
        // ********************************************************************
        // CASKIDS[][]
        // .         ^
        // ********************************************************************
        case V2_MULTICLAIM_CASKIDS__ITEM_LENGTH: {
            // here we compute the offset of the caskIds subarray checksum
            checksum_offset_params_t h_params;

            memset(&h_params, 0, sizeof(h_params));
            memcpy(&h_params.prev_checksum,
                   &(params->checksum_value),
                   sizeof(h_params.prev_checksum));

            // we hash the previous checksum with the offset of the beginning of the structure.
            h_params.new_offset = msg->parameterOffset;
            PRINTF("V2_MULTICLAIM_CASKIDS__OFFSET_ITEMS_VALUE: %d\n", h_params.new_offset);

            if (cx_keccak_256_hash((void *) &h_params, sizeof(h_params), params->checksum_value) !=
                CX_OK) {
                PRINTF("unable to compute keccak hash\n");
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                return;
            }

            U2BE_from_parameter(msg->parameter, &params->current_item_count);
            if (params->current_item_count == 0) {
                if (params->parent_item_count > 0) {
                    params->parent_item_count -= 1;
                }

                if (params->parent_item_count == 0) {
                    // we check the checksums
                    if (memcmp(params->checksum_preview,
                               params->checksum_value,
                               sizeof(params->checksum_preview)) != 0) {
                        PRINTF("Caskids[][] checksums do not match\n");
                        msg->result = ETH_PLUGIN_RESULT_ERROR;
                        return;
                    }

                    context->next_param = V2_MULTICLAIM_UNEXPECTED_PARAMETER;
                }
            } else {
                context->next_param = V2_MULTICLAIM_CASKIDS__ITEM__ITEMS;
            }
            break;
        }
        case V2_MULTICLAIM_CASKIDS__ITEM__ITEMS:
            if (params->current_item_count > 0) {
                params->current_item_count -= 1;
            }
            if (params->current_item_count == 0) {
                if (params->parent_item_count > 0) {
                    params->parent_item_count -= 1;
                }

                if (params->parent_item_count == 0) {
                    // we check the checksums
                    if (memcmp(params->checksum_preview,
                               params->checksum_value,
                               sizeof(params->checksum_preview)) != 0) {
                        PRINTF("Caskids[][] checksums do not match\n");
                        msg->result = ETH_PLUGIN_RESULT_ERROR;
                        return;
                    }

                    context->next_param = V2_MULTICLAIM_UNEXPECTED_PARAMETER;
                } else {
                    context->next_param = V2_MULTICLAIM_CASKIDS__ITEM_LENGTH;
                }
            }
            break;
        default:
            PRINTF("Param not supported: %d\n", context->next_param);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            return;
    }
    msg->result = ETH_PLUGIN_RESULT_OK;
}