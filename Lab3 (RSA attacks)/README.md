# Cryptanalysis of asymmetric cryptosystems using RSA as an example

**Objective:** The purpose of this work is to explore approaches to constructing attacks on asymmetric cryptosystems. Specifically, attacks based on the Chinese Remainder Theorem, which are successful when using the same small value of the public exponent for many users, and the "Meet-in-the-Middle" attack, which is feasible if the ciphertext is a small number.

## Conclusions

This work explores attacks that based on the Chinese Remainder Theorem (CRT) and the "Meet-in-the-Middle" technique to break RSA encryption. I implemented the attacks and measured their execution time for different key lengths and parameters. 

- For the CRT-based attack, I found that cracking a 256-bit key $n$ with an exponent $e$ of 9 takes 2208 ms. This time increases to 3498 ms for a 1024-bit key with an exponent of 5.

- For the "Meet-in-the-Middle" attack with a 2048-bit key and $l=20$, it took 5293 ms. Increasing the value of $l$ requires more computations and memory allocation.

- The Newton's method is implemented for finding the root in the attack using the CRT.
