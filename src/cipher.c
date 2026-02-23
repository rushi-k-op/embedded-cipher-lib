/**
 * @file cipher.c
 * @brief Implementation of the embedded cipher library.
 *
 * See cipher.h for full algorithm description and usage notes.
 *
 * @author Rushikesh Kaduskar
 */
#include "cipher.h"

#include <string.h>
#include <ctype.h>
#include <stddef.h>

/**
* Substitution Table:
* Each row is a pair.
* Covers uppercase hex digits and four punctuation symbols.
*/

static const char SUBSTITUTION_TABLE[CIPHER_TABLE_SIZE][2] = {
    {'0', 'B'}, {'1', ';'}, {'2', 'C'}, {'3', 'D'},
    {'4', ':'}, {'5', 'F'}, {'6', 'E'}, {'7', '9'},
    {'8', '3'}, {'9', '8'}, {'A', '2'}, {'B', '4'},
    {'C', ','}, {'D', '0'}, {'E', '='}, {'F', '1'},
    {':', 'A'}, {',', '7'}, {'=', '5'}, {';', '6'}
};

/*-------------------------------------------------------------
Validate input parameters 
*-------------------------------------------------------------*/
static cipher_status_t validate_arg(const char *str, int key) {
    if (str == NULL) {
        return CIPHER_ERROR_NULL_POINTER;
    }
    if (key <= 0) {
        return CIPHER_ERROR_INVALID_KEY;
    }
    if (strlen(str) > CIPHER_MAX_INPUT_LEN) {
        return CIPHER_ERROR_INVALID_LENGTH;
    }
    return CIPHER_SUCCESS;
}

/*-------------------------------------------------------------
 * split-shift left
 *
 * Performs one iteration of the split-shift used during encryption:
 *   - Left-rotates characters in the lower half  [0 .. mid-1]
 *   - Left-rotates characters in the upper half  [mid+1 .. len-1]
 * The midpoint character at index `mid` is left in place.
*-------------------------------------------------------------*/
static void shift_left_once(char *str, int len) {
    int mid = (len - 1) / 2;
    int j;
    char temp;

    /* Rotate lower half left by one position */
    for (j = 0; j < mid; j++) {
        temp      = str[j + 1];
        str[j + 1] = str[j];
        str[j]    = temp;
    }

    /* Rotate upper half left by one position */
    for (j = mid + 1; j < len - 1; j++) {
        temp       = str[j + 1];
        str[j + 1] = str[j];
        str[j]     = temp;
    }
}

/*-------------------------------------------------------------
 * split-shift right (inverse of left shift)
*-------------------------------------------------------------*/
static void shift_right_once(char *str, int len) {
    int mid = (len - 1) / 2;
    int j;
    char temp;

    /* Rotate upper half right by one position */
    for (j = len - 1; j > mid; j--) {
        temp       = str[j - 1];
        str[j - 1] = str[j];
        str[j]     = temp;
    }

    /* Rotate lower half right by one position */
    for (j = mid + 1; j > 0; j--) {
        temp       = str[j - 1];
        str[j - 1] = str[j];
        str[j]     = temp;
    }
}

/* -------------------------------------------------------------------------
 * forward substitution  (plaintext -> ciphertext)
 * ---------------------------------------------------------------------- */
static void substitute_forward(char *str, int len)
{
    int i, a;
    for (i = 0; i < len && str[i] != '\0'; i++) {
        for (a = 0; a < (int)CIPHER_TABLE_SIZE; a++) {
            if (SUBSTITUTION_TABLE[a][0] == str[i]) {
                str[i] = SUBSTITUTION_TABLE[a][1];
                break;
            }
        }
    }
}

/* -------------------------------------------------------------------------
 * reverse substitution  (ciphertext -> plaintext)
 * ---------------------------------------------------------------------- */
static void substitute_reverse(char *str, int len)
{
    int i, a;
    for (i = 0; i < len && str[i] != '\0'; i++) {
        for (a = 0; a < (int)CIPHER_TABLE_SIZE; a++) {
            if (SUBSTITUTION_TABLE[a][1] == str[i]) {
                str[i] = SUBSTITUTION_TABLE[a][0];
                break;
            }
        }
    }
}

/* -------------------------------------------------------------------------
 * Public
 * ---------------------------------------------------------------------- */

 cipher_status_t cipher_encrypt(char *str, int key)
{
    cipher_status_t status = validate_arg(str, key);
    if (status != CIPHER_SUCCESS) {
        return status;
    }

    int len = (int)strlen(str);
    int i;

    for (i = 0; i < key; i++) {         //Stage 1: Shift
        shift_left_once(str, len);
    }

    substitute_forward(str, len);       //Stage 2: Substitution
    return CIPHER_SUCCESS;
}

cipher_status_t cipher_decrypt(char *str, int key)
{
    cipher_status_t status = validate_arg(str, key);
    if (status != CIPHER_SUCCESS) {
        return status;
    }

    int len = (int)strlen(str);
    int i;

    for (i = 0; i < key; i++) {         //Stage 1: Inverse Shift
        shift_right_once(str, len);
    }

    substitute_reverse(str, len);       //Stage 2: Reverse Substitution
    return CIPHER_SUCCESS;
}

void cipher_to_uppercase(char *str)
{
    if (str == NULL) {
        return;
    }
    while (*str != '\0') {
        *str = (char)toupper((unsigned char)*str);
        str++;
    }
}