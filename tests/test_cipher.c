/**
 * @file test_cipher.c
 * @brief Unit tests for the embedded cipher library.
 *
 * Tests are intentionally written without any external test framework so the
 * file compiles and runs on bare-metal targets as well as a host PC.
 *
 * Build and run (from repo root):
 *   make test
 *
 * Exit code 0 = all tests passed.
 * Exit code 1 = one or more tests failed.
 *
 * @author Rushikesh Kaduskar
 */

#include "cipher.h"

#include <stdio.h>
#include <string.h>

/* -------------------------------------------------------------------------
 * Minimal test framework (no external dependencies)
 * ---------------------------------------------------------------------- */

static int tests_run    = 0;
static int tests_failed = 0;

#define TEST_ASSERT(condition, message)          \
    do {                                         \
        tests_run++;                             \
        if (!(condition)) {                      \
            tests_failed++;                      \
            printf("[FAIL] %s\n"                 \
                   "       %s:%d — %s\n",        \
                   message, __FILE__, __LINE__,  \
                   #condition);                  \
        } else {                                 \
            printf("[PASS] %s\n", message);      \
        }                                        \
    } while (0)

/* -------------------------------------------------------------------------
 * Test cases
 * ---------------------------------------------------------------------- */

/** Encrypt then decrypt must recover the original plaintext. */
static void test_roundtrip_basic(void)
{
    char buf[64];
    const char *plain = "ABCDEF";
    int key = 3;

    strncpy(buf, plain, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';

    cipher_status_t enc = cipher_encrypt(buf, key);
    TEST_ASSERT(enc == CIPHER_SUCCESS, "roundtrip_basic: encrypt returns OK");

    cipher_status_t dec = cipher_decrypt(buf, key);
    TEST_ASSERT(dec == CIPHER_SUCCESS, "roundtrip_basic: decrypt returns OK");

    TEST_ASSERT(strcmp(buf, plain) == 0,
                "roundtrip_basic: decrypt recovers original plaintext");
}

/** Roundtrip with key = 1. */
static void test_roundtrip_key1(void)
{
    char buf[64];
    const char *plain = "1234ABCD";

    strncpy(buf, plain, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';

    cipher_status_t enc = cipher_encrypt(buf, 1);
    TEST_ASSERT(enc == CIPHER_SUCCESS, "roundtrip_key1: encrypt returns OK");

    cipher_status_t dec = cipher_decrypt(buf, 1);
    TEST_ASSERT(dec == CIPHER_SUCCESS, "roundtrip_key1: decrypt returns OK");

    TEST_ASSERT(strcmp(buf, plain) == 0,
                "roundtrip_key1: recovers original with key=1");
}

/** Roundtrip with a longer string. */
static void test_roundtrip_long(void)
{
    char buf[64];
    const char *plain = "0123456789ABCDEF";

    strncpy(buf, plain, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';

    cipher_status_t enc = cipher_encrypt(buf, 5);
    TEST_ASSERT(enc == CIPHER_SUCCESS, "roundtrip_long: encrypt returns OK");

    cipher_status_t dec = cipher_decrypt(buf, 5);
    TEST_ASSERT(dec == CIPHER_SUCCESS, "roundtrip_long: decrypt returns OK");

    TEST_ASSERT(strcmp(buf, plain) == 0,
                "roundtrip_long: recovers original 16-char string");
}

/** Encrypted output must differ from plaintext. */
static void test_encrypt_changes_input(void)
{
    char buf[64];
    const char *plain = "ABCDEF";

    strncpy(buf, plain, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';

    cipher_status_t enc = cipher_encrypt(buf, 2);
    TEST_ASSERT(enc == CIPHER_SUCCESS, "encrypt_changes_input: encrypt returns OK");

    TEST_ASSERT(strcmp(buf, plain) != 0,
                "encrypt_changes_input: ciphertext differs from plaintext");
}

/** cipher_to_upper must convert lowercase to uppercase. */
static void test_to_upper(void)
{
    char buf[] = "abcdef123";
    cipher_to_uppercase(buf);
    TEST_ASSERT(strcmp(buf, "ABCDEF123") == 0,
                "to_upper: converts lowercase to uppercase");
}

/** NULL pointer argument must return error, not crash. */
static void test_null_ptr_guard(void)
{
    cipher_status_t s = cipher_encrypt(NULL, 1);
    TEST_ASSERT(s == CIPHER_ERROR_NULL_POINTER,
                "null_ptr_guard: encrypt(NULL) returns CIPHER_ERROR_NULL_POINTER");

    s = cipher_decrypt(NULL, 1);
    TEST_ASSERT(s == CIPHER_ERROR_NULL_POINTER,
                "null_ptr_guard: decrypt(NULL) returns CIPHER_ERROR_NULL_POINTER");
}

/** Invalid key (<= 0) must return error. */
static void test_invalid_key_guard(void)
{
    char buf[] = "ABCD";
    cipher_status_t s = cipher_encrypt(buf, 0);
    TEST_ASSERT(s == CIPHER_ERROR_INVALID_KEY,
                "invalid_key_guard: key=0 returns CIPHER_ERROR_INVALID_KEY");

    s = cipher_encrypt(buf, -3);
    TEST_ASSERT(s == CIPHER_ERROR_INVALID_KEY,
                "invalid_key_guard: key=-3 returns CIPHER_ERROR_INVALID_KEY");
}

/* -------------------------------------------------------------------------
 * Main
 * ---------------------------------------------------------------------- */

int main(void)
{
    printf("\n=== Embedded Cipher Library — Unit Tests ===\n\n");

    test_roundtrip_basic();
    test_roundtrip_key1();
    test_roundtrip_long();
    test_encrypt_changes_input();
    test_to_upper();
    test_null_ptr_guard();
    test_invalid_key_guard();

    printf("\n--- Results: %d/%d passed ---\n\n",
           tests_run - tests_failed, tests_run);

    return (tests_failed > 0) ? 1 : 0;
}