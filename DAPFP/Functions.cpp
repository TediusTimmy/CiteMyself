/*
   This file contains code that has been removed from SlowCalc,
   also by Thomas DiModica.

   Copyright (C) 2010, 2011 Thomas DiModica <ricinwich@yahoo.com>

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "Float.hpp"

namespace BigInt
 {

	unsigned long SERIESEXTRA = 8;
	unsigned long DERIVEDEXTRA = 4;

   const Float M_Inf ("Inf");

      //ONE!
   const Float M_1 ("1");

/*
==============================================================================
   Function: square root
------------------------------------------------------------------------------
   INPUT: number to take the square root of
   OUTPUT: the square root of the input
   NOTES:
      Caved to IEEE: sqrt(-0) = -0 now.

      Newton-Rhapson is much faster than x ^ .5,
      which becomes e ^ (.5 * ln(x)).
==============================================================================
*/
   Float sqrt (const Float & opp)
    {
      Float copyOpp (opp);

      if (opp.isZero()) return opp;
      if (opp.isSigned())
       {
         copyOpp = Float();
         copyOpp.NaN = 16;
         return copyOpp;
       }
      if (opp.isNaN() || opp.isInfinity()) return opp;

      copyOpp.setPrecision(opp.getPrecision() + SERIESEXTRA);

      long diff, term = opp.getPrecision() + DERIVEDEXTRA;

      Float temp (copyOpp);
      Float curApprox (copyOpp);
      Float lastApprox (copyOpp);

         //This helps to produce a good first estimate, and is why
         //this function has to be a friend.
      curApprox.Exponent /= 2;
      do
       {
         lastApprox = curApprox;

         curApprox = (curApprox * curApprox + copyOpp) / (curApprox + curApprox);

         temp = curApprox - lastApprox;

         diff = curApprox.Exponent.toInt() - temp.Exponent.toInt();
         if (diff < 0) diff = -diff;
       }
      while ((diff <= term) && !temp.isZero());

      curApprox.setPrecision(opp.getPrecision());
      return curApprox;
    }

/*
==============================================================================
   Function: exponential function, Euler's constant to a real power
------------------------------------------------------------------------------
   NOTES:
      Basically: the Maclaurin series approximation for e ^ x

      I made one slight change: I noticed poor convergence and
      premature loop termination with negative exponents, so I
      force the exponent to be positive, then divide one by
      the result if it was originally negative.
      This has fixed my problem.

      I had a similar problem with sine and cosine with all large
      numbers (numbers with great magnitude), and the problem
      is/was that as the series goes on, the terms become the very
      small difference between very large numbers, and so there is
      a very big error due to round-off.

      I had an epiphany today, thanks to R. Brent in "Fast Multiple-Precision
      Evaluation of Elementary Functions":
         exp(x * 10 ^ y) = (exp(x * 10 ^ (y - 1))) ^ 10 = (exp(x)) ^ (10 ^ y)
      This works SO MUCH BETTER for base 2. It reduces the range and would be
      easy to implement as loops. My concern is the accuracy after repeated
      multiplications (probably negligable) and if it really speeds things up.
==============================================================================
*/
   Float exp (const Float & opp)
    {
      Float one (opp);

      if (opp.isZero()) return (one |= M_1);
      else if (opp.isNaN()) return opp;
      else if (opp.isInfinity())
       {
         if (!opp.isSigned()) return opp;
         return Float();
       }

       /*
         Three was chosen arbitrarily as the trade-off point between speed
         and the extra digits needed to maintain accuracy.
       */
      bool special = opp.Exponent > -3;
      Integer loops = opp.Exponent + Integer(3L);

       /*
         This is a bit arbitrary and is in need of explanation:
         As written SERIESEXTRA is 8, and thus we add 16 digits
         to the computation. In the "special" loop, we loose one
         digit of accuracy for each iteration. This way, for very
         large numbers, we can loose up to 10 digits, which leaves
         us 6 for rounding.
       */
      if (special)
         one.setPrecision(opp.getPrecision() + SERIESEXTRA * 2);
      else
         one.setPrecision(opp.getPrecision() + SERIESEXTRA);
      one |= M_1;

      Float currentFactorial (one);
      Float currentDivisor (one);
      Float currentPower (one);
      Float copyOpp (one);
      Float res (one);
      Float temp (one);

      currentFactorial |= Float("2");
      currentDivisor |= Float("2");
      currentPower |= opp;
      copyOpp |= opp;

      currentPower.abs();
      copyOpp.abs();
      if (special)
       {
         currentPower.Exponent = -3;
         copyOpp.Exponent = -3;
       }


      res += currentPower;
      currentPower *= copyOpp;

      temp = currentPower / currentDivisor;
      while (change(temp, res))
       {
         res += temp;
         currentFactorial += one;
         currentDivisor *= currentFactorial;
         currentPower *= copyOpp;
         temp = currentPower / currentDivisor;
       }

      if (special)
       {
         /*
            This loop can only execute ten times before the exponent exceeds
            999999999. It requires the extra precision given.
         */
         while (!loops.isZero() && !res.Infinity)
          {
            temp = res;
            temp *= temp; //2
            temp *= temp; //4
            res *= temp;  //5
            res *= res;   //10
            --loops;
          }
       }

      if (opp.isSigned()) res = one / res;

      res.setPrecision(opp.getPrecision());
      return res;
    }

   Float makeLN10 (const Float & opp)
    {
      Float temp1 (opp);
      Float temp2 (opp);
      Float temp3 (opp);

      temp1 |= Float("1.25");
      temp1 = log(temp1);

      temp2 |= Float("2");
      temp2 = sqrt(temp2);
      temp2 = log(temp2);

      temp3 |= Float("6");
      return temp1 + temp3 * temp2;
    }
/*
==============================================================================
   Function: natural logrithm
------------------------------------------------------------------------------
   NOTES:
      log(x) = 2 * sum (i = 1 to Inf)  of
         (((x - 1) / (x + 1)) ^ (2 * i - 1)) / (2 * i - 1)

      This started general-purpose, but the series for ln has poor
      convergence outside of (.5,2). Remove all areas labeled new code
      to get the log function that can compute ln(10) itself.
      You should probably use the equation in the header, though,
      it converges faster and can be used here,
      without changing the function.

      So, by basic laws of logrithms:
         ln(x * 10 ^ y) = ln(x) + y * ln(10)
      Predefining ln(10) may limit the precision to that definition,
      but it speeds things up.

      As convergence is best at one and decays around one such that
      the convergence of x is as bad as 1 / x, we try to force
      numbers into an area of better convergence. As it is,
      x > sqrt(10) converges slower than x / 10. To speed things up,
      if (x > 3) we use x / 10 instead of x. This puts pi in the
      region of worst convergence, but everywhere else will be faster.

      Update: we will use one of three modes:
         [1, 1.78)      ln(x) + y * ln(10)
         [1.78, 5.62)   ln(x / sqrt(10)) + (y + .5) * ln(10)
         [5.62, 10)     ln(x / 10) + (y + 1) * ln(10)

      We can continue to use reductions to the argument to keep the actual
      calculation within some range of fast convergence, however it has
      diminishing returns.
==============================================================================
*/
   Float log (const Float & opp)
    {
      Float one (opp);

      if (opp.isZero()) return -M_Inf;
      if (opp.isSigned())
       {
         one = Float();
         one.NaN = 16;
         return one;
       }
      if (opp.isNaN() || opp.isInfinity()) return opp;

      one.setPrecision(opp.getPrecision() + SERIESEXTRA);
      one |= M_1;

      Float currentDivisor (one);
      currentDivisor |= Float("3");
      Float copyOpp (one);
      copyOpp |= opp;

      Float currentPower (one);
      Float res (one);
      Float temp (one);
      long mode;

      Fixed msDigits;

      msDigits = opp.Data;
      msDigits.changePrecision(msDigits.getPrecision() + 8);
      msDigits.setPrecision(msDigits.getPrecision() - 8);

      mode = msDigits.roundToInteger().toInt();

      if (mode < 177827941) /* 100000000 .. 177827940 */
       {
         copyOpp.Exponent = 0L;
         mode = 1;
       }
      else if (mode < 562341325) /* 177827941 .. 562341324 */
       {
         copyOpp.Exponent = 0L;
         temp |= Float("10");
         temp = sqrt(temp);
         copyOpp /= temp;
         mode = 2;
       }
      else /* 562341325 .. 999999999 */
       {
         copyOpp.Exponent = -1;
         mode = 3;
       }

      res = copyOpp - one;
      temp = copyOpp + one;

      currentPower = res / temp;
      copyOpp = currentPower;
      res = currentPower;
      one |= Float("2");

      copyOpp *= copyOpp;
      currentPower *= copyOpp;

      temp = currentPower / currentDivisor;
      while (change(temp, res))
       {
         res += temp;
         currentDivisor += one; //which is now 2
         currentPower *= copyOpp;
         temp = currentPower / currentDivisor;
       }
      res *= one;

      switch (mode)
       {
         case 1:
            if (opp.Exponent.isZero()) break;
            temp |= Float(Integer(opp.Exponent).toString());
            one |= makeLN10(one);

            temp *= one; //which is now ln(10)
            res += temp;
            break;
         case 2:
            temp |= Float(Integer(opp.Exponent).toString() + ".5");
            one |= makeLN10(one);

            temp *= one;
            res += temp;
            break;
         case 3:
            if (opp.Exponent == -1) break;
            temp |= Float(Integer(opp.Exponent + 1).toString());
            one |= makeLN10(one);

            temp *= one;
            res += temp;
            break;
       }

      res.setPrecision(opp.getPrecision());
      return res;
    }

/*
==============================================================================
   Function: exponentiation
------------------------------------------------------------------------------
   NOTES:
      This is simply x ^ y = e ^ (y * ln(x))
      It doesn't work for -x, so (-1) ^ 2 = NaN.

      1^Inf becomes an NaN naturally by this definition.

      We define 0^0 = 1 because:
         1. It follows that this must be the case from the empty product
            definition for x ^ 0.
         2. The functions exp and cos use this identity in their Maclaurin
            series definitions implicitly.

      This now works rudimentarily for negative numbers. To be better, it
      would have to rationalize the fractional part and decide if that
      exponent is real, however this implies that the rational part is
      precise, which is incorrect for most purposes.
==============================================================================
*/
   Float pow (const Float & left, const Float & right)
    {
      Float base (left), exponent (right), res;
      match(base, exponent);

         //exceptions to the rules
         //Yes, the forcing of a proper NaN is ugly.
      if (left.isInfinity() && right.isZero()) return sqrt(-M_1);
      if (left.isZero() && right.isInfinity() && right.isSigned())
         return M_Inf;

      if (right.isZero()) return (base |= M_1);
      if (left.isZero()) return Float();

      if (left.isSigned() && !right.isInteger()) return sqrt(-M_1);

      res.setPrecision(base.getPrecision());

      base.setPrecision(base.getPrecision() + DERIVEDEXTRA);
      exponent.setPrecision(exponent.getPrecision() + DERIVEDEXTRA);

      if (left.isSigned())
       {
         base.abs();
         if (right.isOdd()) res = -exp(log(base) * exponent);
         else res = exp(log(base) * exponent);
       }
      else
         res = exp(log(base) * exponent);

      res.setPrecision(base.getPrecision() - DERIVEDEXTRA);
      return res;
    }

 } /* namespace BigInt */
