#ifndef _AJ_CRYPTO_H
#define _AJ_CRYPTO_H

/**
 * @file aj_crypto.h
 * @defgroup aj_crypto Cryptographic Support
 * @{
 */
/******************************************************************************
 * Copyright AllSeen Alliance. All rights reserved.
 *
 *    Permission to use, copy, modify, and/or distribute this software for any
 *    purpose with or without fee is hereby granted, provided that the above
 *    copyright notice and this permission notice appear in all copies.
 *
 *    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *    WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *    MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *    ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *    WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *    ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 ******************************************************************************/

#include <ajtcl/aj_target.h>
#include <ajtcl/aj_status.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Implements AES-CCM (Counter with CBC-MAC) encryption as described in RFC 3610. The message in
 * encrypted in place.
 *
 * @param key     The AES-128 encryption key
 * @param msg     The buffer containing the entire message that is to be encrypted, The buffer must
 *                have room at the end to append an authentication tag of length tagLen.
 * @param msgLen  The length of the entire message
 * @param hdrLen  The length of the header portion that will be authenticated but not encrypted
 * @param tagLen  The length of the authentication tag to be appended to the message
 * @param nonce   The nonce
 * @param nLen    The length of the nonce
 *
 * @return
 *         - AJ_OK if the CCM context is initialized
 *         - AJ_ERR_RESOURCES if the resources required are not available.
 */
AJ_Status AJ_Encrypt_CCM(const uint8_t* key,
                         uint8_t* msg,
                         uint32_t msgLen,
                         uint32_t hdrLen,
                         uint8_t tagLen,
                         const uint8_t* nonce,
                         uint32_t nLen);

/**
 * Implements AES-CCM (Counter with CBC-MAC) decryption as described in RFC 3610. The message in
 * decrypted in place.
 *
 * @param key     The AES-128 encryption key
 * @param msg     The buffer containing the entire message to be decrypted.
 * @param msgLen  The length of the entire message, excluding the tag.
 * @param hdrLen  The length of the header portion that will be authenticated but not encrypted
 * @param tagLen  The length of the authentication tag to be appended to the message
 * @param nonce   The nonce
 * @param nLen    The length of the nonce
 *
 * @return
 *         - AJ_OK if the CCM context is initialized
 *         - AJ_ERR_RESOURCES if the resources required are not available.
 */
AJ_Status AJ_Decrypt_CCM(const uint8_t* key,
                         uint8_t* msg,
                         uint32_t msgLen,
                         uint32_t hdrLen,
                         uint8_t tagLen,
                         const uint8_t* nonce,
                         uint32_t nLen);

/**
 * Return a string of randomly generated bytes.
 *
 * @param rand  Pointer to a buffer to return the random data
 * @param size  The number of random bytes to return.
 */
void AJ_RandBytes(uint8_t* rand, uint32_t size);

/**
 * Return a random hexadecimal string of the requested length
 *
 * @param rand   Pointer to a buffer to return the random data
 * @param bufLen The length of the buffer. The buffer must be at
 *               least (len * 2) + 1 bytes.
 * @param len    The length of the hexadecimal string
 *
 * @return        Return AJ_Status
 *               - AJ_OK if the string was converted
 *               - AJ_ERR_RESOURCES if the hexLen is too small to fit the converted string.
 */
AJ_Status AJ_RandHex(char* rand, uint32_t bufLen, uint32_t len);

/**
 * Enable AES allocating any resources required
 *
 * @param key  The key in case this is required
 */
void AJ_AES_Enable(const uint8_t* key);

/**
 * Disable AES freeing any resources that were allocated
 */
void AJ_AES_Disable(void);

/**
 * Compare two buffers in constant time. For any two inputs buf1 and buf2, and
 * fixed count, the function will use the same number of cycles.
 *
 * @param buf1  The first buffer to compare.
 * @param buf2  The second buffer to compare.
 * @param count The number of bytes to compare.
 *
 * @return 0  if the first count bytes of buf1 and buf2 are equal, nonzero otherwise.
 *
 */
int AJ_Crypto_Compare(const void* buf1, const void* buf2, size_t count);

#ifdef __cplusplus
}
#endif
/**
 * @}
 */
#endif
