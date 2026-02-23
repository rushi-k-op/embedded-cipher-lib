/**
 * @file cipher.h
 * @brief Lightweight cipher library for embedded systems.
 *
 * Implements a two-stage cipher designed for resource-constrained targets:
 *   Stage 1 — Split-shift: the input string is split at its midpoint and
 *             each half is independently rotated by `key` positions.
 *   Stage 2 — Substitution: each character is mapped through a fixed
 *             16-symbol hexadecimal substitution table.
 *
 * Designed with embedded constraints in mind:
 *   - No heap allocation (operates in-place on caller's buffer)
 *   - No stdio dependency in the library itself
 *   - Operates only on uppercase hex characters and punctuation symbols
 *     defined in the substitution table
 *
 * Developed during internship at Intangles Lab Pvt. Ltd. (2022-2023)
 * for use in Quectel 4G modem/ STM32 microProcessors firmware telemetry pipelines.
 *
 * @author Rushikesh Kaduskar
 */
#ifndef CIPHER_H
#define CIPHER_H

#include <stddef.h>

#define CIPHER_MAX_INPUT_LEN 10000U
#define CIPHER_TABLE_SIZE 20U

typedef enum {
    CIPHER_SUCCESS = 0,
    CIPHER_ERROR_NULL_POINTER = -1,
    CIPHER_ERROR_INVALID_KEY = -2,
    CIPHER_ERROR_INVALID_LENGTH = -3
} cipher_status_t;

/**
 * @brief Encrypt a null-terminated string in-place.
 *
 * The string must contain only characters present in the substitution table
 * (uppercase hex digits 0-9, A-F and the symbols `:`, `,`, `=`, `;`).
 * Characters outside the table are passed through unchanged by the
 * substitution stage but are still affected by the shift stage.
 *
 * The caller is responsible for converting the string to uppercase before
 * calling this function (see cipher_to_upper()).
 *
 * @param[in,out] str   Null-terminated string to encrypt in-place.
 * @param[in]     key   Number of shift iterations (must be > 0).
 * @return CIPHER_OK on success, or a negative cipher_status_t error code.
 */
cipher_status_t cipher_encrypt(char *str, int key);

/**
 * @brief Decrypt a null-terminated string in-place.
 *
 * Reverses the operation performed by cipher_encrypt() when called with the
 * same key.
 *
 * @param[in,out] str   Null-terminated encrypted string to decrypt in-place.
 * @param[in]     key   Number of shift iterations used during encryption.
 * @return CIPHER_OK on success, or a negative cipher_status_t error code.
 */
cipher_status_t cipher_decrypt(char *str, int key);

/**
 * @brief Convert a string to uppercase in-place.
 *
 * Portable replacement for the non-standard strupr(). Only ASCII letters
 * are affected; all other characters are unchanged.
 *
 * @param[in,out] str  Null-terminated string to convert.
 */
void cipher_to_uppercase(char *str);

#endif