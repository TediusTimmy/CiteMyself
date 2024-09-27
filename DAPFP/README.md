Doubly-Arbitrary Precision Floating Point
=========================================

This started life as a calculator that I wrote for a college assignment. It's bad, but it was easy to modify to suit my needs. It's doubly-arbitrary because both the significand and the exponent have an arbitrary number of digits. However, that's not totally true, as the significand can only be 134217728 digits. Believe me: you don't want more. We could increase the limit to 2^64-1, but no one has enough RAM to handle that.

The `make.sh` script assumes a Cygwin build with the mingw-w64 cross-compiler. You will also need the GMP library installed.

To do the thing:
```
2 length 20000 copy * copy * copy * copy * copy * copy * copy * copy * copy * copy * copy * copy * copy * copy * copy * copy * 2 swap pow print
```
