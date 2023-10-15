Tetration Values
================

I wrote this up because someone at Wikipedia reverted my commits. I guess, however, that this forces my hand in sharing the code used (which is a good thing). And, I may as well explain it, too.

So, I'll go over the ForWiki.b code. This is code for the POSIX utility `bc`. It will run on GNU bc and Gavin Howard's bc (FreeBSD / Android). However, it isn't standards-conforming enough to run on OpenBSD bc. It uses GNU extensions. In addition, you must invoke bc with its standard library, using the `-l` flag, as it uses natural logarithms and exponentials.

I've run this against three different implementations of bc, and have gotten the same results. All that really says is that the math between the various versions of bc is implemented to produce the same results.

### si
This function scales a number to an integer. In Gavin Howard's bc, there is the `$` operator, but the isn't a GNU extension. All this function does is take in a number, rescale it to zero, and return it. Note that ` / 1 ` is idiomatic bc for rescaling a number to the value of the current scale global.

### pow
If you have seen the extended standard library in Gavin Howard's bc, this function may look suspiciously familiar. This is the `p(x,y)` function, mostly, because I donated it to him after writing it. I was doing something with very large exponentials, and I needed a highly *accurate* `pow(x,y)` function. The implementation `e(y * l(x))` doesn't work very well when you are making large results. Basically, if you follow the significant digits, computing `l(x)` to only `scale` digits means that you are only going to get `scale` digits of the result correct. The only improvement that could be made, that I know of, would be to rationalize the fractional part of the exponent.

So, pow first looks for special cases: the result is one, the result is zero, or the result requires rationalizing the fractional part of the exponent.  
Then, if the exponent is an integer, it passes the computation over to bc's `^` operator, which only handles integer exponents but is native code.  
Next, if the number is being driven to zero, just use `e(y * l(x))` as there is no accuracy to be gained.  
Finally, we know that the number is going to be very large. We break the exponent into its integral and fractional components. We use the evaluation of the integral part to get the number of significant digits, the scale, to compute the fractional part to. This is in order to get a final result that is *accurate* for all of the digits to the left of the decimal point and the scale digits to the right. And this can be a very large number: for what is done to get `2^^6` we need 20 thousand digits, and `6^^4` we need 37 thousand digits.

### tet
This is a simple loop to do real tetration. It only works for integers, as it uses bc's `^` operator.

### lt
This computes the common log of a tetration. The assumption here is that the actual tetration is too large to be evaluated, so we sneak up on it and compute it's logarithm. We use the common log so that the result has meaning to a human reader.

The fundamental observation here is that `l(tet(x, y)) == tet(x, y - 1) * l(x)`. This is a standard law of logarithms: `l(x ^ y) == y * l(x)`. Again, we use the magnitude of `tet(x, y - 1)` to determine how many digits of logarithm we need in order for the whole result to be *accurate*.

Accuracy here is very important: this is a logarithm. The integral component is the characteristic (the power of ten in scientific notation), and the fractional component is the mantissa which defines the digits in the resulting number. The value of `10 ^ mantissa` is the coefficient, or significand, in scientific notation. We need many of the scale digits of the given answer to be accurate in order to pull the initial digits of the desired answer from the result that this function gives.

### tetra
This is a strange function in that it prints out the result and doesn't return it. The tetra function makes sense of the result of `lt` and prints out the result in scientific notation (and the exponent is printed in scientific notation if it is longer than 15 digits). Notably, due to how bc works, if you just type `tetra(2,5)` you will get an extra `0` at the end, because the function doesn't print a newline and returns zero. You need to type something like `n = tetra(2, 5)` to not get the result of the function.

### order66
Only the Crown can introduce budgetary measures to Commons. This function basically calls `tetra(x,y)` for the various values that I wanted to add to Wikipedia. I'm not doing `7^^4`, as it took 5 hours and 20 minutes on the version of bc that takes 30 seconds to **execute order66()**. I'm not running that on another version of bc. Here is the expected output:

```
This program requires either GNU bc, which I strongly advise
against, or the FreeBSD/Android bc, which is better.
If you want an answer the fastest, the author recommends
their own version of GNU bc, which uses GMP to implement
numbers.

First, here is 2^^6 :
 2.12003872880821198486 * 10 ^ (6.03122606263029632220 * 10 ^ 19727)
Now, the other lacking column,
3^^4 :
 1.25801429062749131784 * 10 ^ 3638334640024
4^^4 :
 2.36102267145973132064 * 10 ^ (8.07230472602822538919 * 10 ^ 153)
5^^4 :
 1.11102880817999744527 * 10 ^ (1.33574048387213700657 * 10 ^ 2184)
6^^4 :
 4.44623519023693469749 * 10 ^ (2.06919737562090357907 * 10 ^ 36305)
We stop here, because the scale needed for the next
computation is 695995
Thank you, and don't mind the extra zero. 0
```

On my machine, this took 30 seconds for my version of GNU bc [https://github.com/TediusTimmy/GNU_bc_with_GMP], 8 minutes for Gavin Howard's bc [https://github.com/gavinhoward/bc], and 1 hour 40 minutes for the GNU bc shipped with Cygwin.
