# Embedded Cipher Library

A lightweight, portable cipher library written in C for resource-constrained embedded targets.

Developed during an internship at **Intangles Lab Pvt. Ltd.** (2022–2023) for securing telemetry payloads transmitted over Quectel 4G modem/ STM32 Micro Controller firmware pipelines.

---

## Algorithm

The cipher operates in two sequential stages:

**Stage 1 — Split-Shift**
The input string is split at its midpoint. Each half is independently rotated (left for encryption, right for decryption) by `key` positions. This provides positional diffusion without any dependency on external libraries.

```
Input:     A B C | D E F
           ↓ left-rotate each half ↓
Shifted:   B C A | E F D
```

**Stage 2 — Substitution**
Each character is mapped through a fixed 20-entry lookup table covering uppercase hex digits (`0–9`, `A–F`) and four punctuation symbols (`:`, `,`, `=`, `;`). Characters outside the table pass through unchanged.

```
Shifted:   B C A | E F D
           ↓ forward substitution ↓
Encrypted: 4 , 2 | = 1 0
```

Decryption applies these stages in reverse order.

---

## Design Constraints (Embedded-Friendly)

| Property | Detail |
|---|---|
| No heap allocation | Operates entirely in-place on caller's buffer |
| No stdio dependency | Library itself has zero `printf` / `scanf` calls |
| Portable C99 | Compiles cleanly with `gcc`, `clang`, `arm-none-eabi-gcc` |
| Fixed memory footprint | Substitution table is `static const`, placed in `.rodata` |
| Cross-compilable | `make CC=arm-none-eabi-gcc` for ARM bare-metal targets |

---

## Project Structure

```
Embedded-cipher-lib/
├── include/
│   └── cipher.h          ← Public API and type definitions
├── src/
│   ├── cipher.c          ← Library implementation
│   └── demo.c            ← Interactive demo (optional, not part of lib)
├── tests/
│   └── test_cipher.c     ← Unit test suite (no external framework)
├── Makefile
└── README.md
```

---

## Build

**Requirements:** GCC (or any C99-compatible compiler), Make

```bash
# Clone the repo
git clone https://github.com/rushi-k-op/Embedded-cipher-lib.git
cd Embedded-cipher-lib

# Build static library + demo
make

# Run unit tests
make test

# Cross-compile for ARM bare-metal
make CC=arm-none-eabi-gcc AR=arm-none-eabi-ar
```

---

## Usage

```c
#include "cipher.h"
#include <string.h>

int main(void) {
    char buf[] = "ABCDEF";
    int key = 3;

    // Optional: convert to uppercase first
    cipher_to_upper(buf);

    // Encrypt in-place
    cipher_encrypt(buf, key);   // buf is now ciphertext

    // Decrypt in-place
    cipher_decrypt(buf, key);   // buf is back to "ABCDEF"

    return 0;
}
```

### API Summary

```c
// Encrypt a null-terminated string in-place
cipher_status_t cipher_encrypt(char *str, int key);

// Decrypt a null-terminated string in-place
cipher_status_t cipher_decrypt(char *str, int key);

// Convert string to uppercase (portable, replaces non-standard strupr)
void cipher_to_upper(char *str);
```

### Return Codes

| Code | Value | Meaning |
|---|---|---|
| `CIPHER_OK` | 0 | Success |
| `CIPHER_ERR_NULL_PTR` | -1 | NULL pointer passed |
| `CIPHER_ERR_INVALID_KEY` | -2 | key ≤ 0 |
| `CIPHER_ERR_INPUT_LEN` | -3 | Input exceeds `CIPHER_MAX_INPUT_LEN` |

---

## Test Output

```
=== Embedded Cipher Library — Unit Tests ===

[PASS] roundtrip_basic: encrypt returns OK
[PASS] roundtrip_basic: decrypt returns OK
[PASS] roundtrip_basic: decrypt recovers original plaintext
[PASS] roundtrip_key1: recovers original with key=1
[PASS] roundtrip_long: recovers original 16-char string
[PASS] encrypt_changes_input: ciphertext differs from plaintext
[PASS] to_upper: converts lowercase to uppercase
[PASS] null_ptr_guard: encrypt(NULL) returns CIPHER_ERR_NULL_PTR
[PASS] null_ptr_guard: decrypt(NULL) returns CIPHER_ERR_NULL_PTR
[PASS] invalid_key_guard: key=0 returns CIPHER_ERR_INVALID_KEY
[PASS] invalid_key_guard: key=-3 returns CIPHER_ERR_INVALID_KEY

--- Results: 11/11 passed ---
```

---

## Context

This library was part of firmware work at Intangles Lab for telematics modules transmitting GPS and vehicle diagnostic data over cellular networks. The original implementation was refactored here into a standalone, portable library with proper error handling, no global state, and a clean C99 API suitable for direct integration into RTOS-based firmware projects.

---

## Author

**Rushikesh Kaduskar** — [GitHub](https://github.com/rushi-k-op) · [LinkedIn](https://www.linkedin.com/in/rushikesh-kaduskar/)