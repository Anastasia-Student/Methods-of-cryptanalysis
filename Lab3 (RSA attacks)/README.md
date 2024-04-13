## Cryptanalysis of asymmetric cryptosystems using RSA as an example

**Objective:** The purpose of this work is to explore approaches to constructing attacks on asymmetric cryptosystems. Specifically, attacks based on the Chinese Remainder Theorem, which are successful when using the same small value of the public exponent for many users, and the "Meet-in-the-Middle" attack, which is feasible if the ciphertext is a small number.

### Conclusions

In this work, I implemented an attack based on the Chinese Remainder Theorem (CRT) and the "Meet-in-the-Middle" attack on RSA. I found that in the CRT-based attack with a 256-bit key length $n$ and exponent $e=9$, the time spent on cracking is 2208 ms, while with a 1024-bit key and $e=5$, it is 3498 ms. For the "Meet-in-the-Middle" attack with a 2048-bit key and $l=20$, it took 5293 ms. Increasing the value of $l$ requires more computations and memory allocation. The Newton's method is implemented for finding the root in the attack using the CRT.
