#include "kiln_plugin.h"
#include "lcx_common.h"
#include "lcx_hash.h"
#include <stddef.h>

size_t strlcat(char *dst, const char *src, size_t size) {
    size_t srclen; /* Length of source string */
    size_t dstlen; /* Length of destination string */

    dstlen = strlen(dst);
    size -= dstlen + 1;

    if (!size) return (dstlen); /* No room, return immediately... */

    srclen = strlen(src);

    if (srclen > size) srclen = size;

    memcpy(dst + dstlen, src, srclen);
    dst[dstlen + srclen] = '\0';

    return (dstlen + srclen);
}

size_t strlcpy(char *dst, const char *src, size_t size) {
    size_t srclen; /* Length of source string */

    size--;

    srclen = strlen(src);

    if (srclen > size) srclen = size;

    memcpy(dst, src, srclen);
    dst[srclen] = '\0';

    return (srclen);
}

cx_err_t cx_keccak_256_hash_iovec(const cx_iovec_t *iovec,
                                  size_t iovec_len,
                                  uint8_t digest[static CX_KECCAK_256_SIZE]) {
    return CX_OK;
}

void os_sched_exit(bolos_task_status_t exit_code) {
    return;
}

void *pic(void *p) {
    return p;
}
