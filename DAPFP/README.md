Doubly-Arbitrary Precision Floating Point
=========================================

This started life as a calculator that I wrote for a college assignment. It's bad, but it was easy to modify to suit my needs. It's doubly-arbitrary because both the significand and the exponent have an arbitrary number of digits. However, that's not totally true, as the significand can only be 134217728 digits. Believe me: you don't want more. We could increase the limit to 2^64-1, but no one has enough RAM to handle that.

The `make.sh` script assumes a Cygwin build with the mingw-w64 cross-compiler. You will also need the GMP library installed.

To do the thing:
```
2 length 20000 copy * copy * copy * copy * copy * copy * copy * copy * copy * copy * copy * copy * copy * copy * copy * copy * 2 swap pow print
```

Now, there is the Looper.c program. Use it like so:
```
./Looper | ./DAPFP
```
What really confuses me, right now, is the precision needs of the program in order to get a good result. Looper is set for 19800 digits of precision. Just going down to 19000 makes all of the digits of the significand garbage. I need to think about this more. At some squaring of the number, we will start losing a digit of accuracy per squaring (or, per some number x squarings). That makes sense. It also makes sense that over 65536 squarings, we will need extra precision. But, why is the precision `ln(2^^5) / ln(10)`?

It's not. We start to converge on the answer at 19724 digits of precision. We lose a digit of accuracy for about every 3.32 multiplications. I'm sure there is a meaningful relationship here, but I'll have to look at it again later.

Update: We lose a digit of accuracy for every `1 / log(2)` multiplications. When we take into account that the first fifteen operations are exact, then we need `65521 * log(2)` digits, which is 19724.
