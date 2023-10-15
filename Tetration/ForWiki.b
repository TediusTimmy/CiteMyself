/*
Copyright (c) 2023 Thomas DiModica.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. Neither the name of Thomas DiModica nor the names of other contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THOMAS DIMODICA AND CONTRIBUTORS ``AS IS'' AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED.  IN NO EVENT SHALL THOMAS DIMODICA OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.
*/

define si (x) {
   auto s, r
   s = scale
   scale = 0
   r = x / 1
   scale = s
   return r
}

define pow (x,y) {
   auto a,i,s,z
   if (0 == y) return 1 / 1
   if (0 == x) {
      if (y > 0) return 0
      return 1 / 0
   }
   a = si(y)
   if (y == a) return (x ^ a) / 1
   z = 0
   if (x < 1) {
      y = -y
      a = -a
      z = x
      x = 1 / x
   }
   if (y < 0) {
      return e(y * l(x))
   }
   i = x ^ a
   s = scale
   scale += length(i)
   if (z) {
      x = 1 / z
      i = x ^ a
   }
   i *= e((y - a) * l(x))
   scale = s
   return i/1
}

define tet (x, y) {
   auto p
   p = 1
   while (y) {
      p = x ^ p
      y = y - 1
   }
   return p
}

define lt (x, y) {
   auto l, p, s
   p = tet(x, y - 1)
   s = scale
   scale += length(p)
   l = l(x) / l(10)
   p = p * l
   scale = s
   return p / 1
}

define tetra (x, y) {
   auto n, m, w, z
   z = lt(x, y)
   w = si(z)
   if (length(w) > 15) {
      n = l(w) / l(10)
      m = si(n)
      print pow(10, z - w), " * 10 ^ (", pow(10, n - m), " * 10 ^ ", m, ")"
   } else {
      print pow(10, z - w), " * 10 ^ ", w
   }
}

define order66 () {
   auto n
"Welcome to the demonstration of this tetration program!
This program requires either GNU bc, which I strongly advise
against, or the FreeBSD/Android bc, which is better.
If you want an answer the fastest, the author recommends
their own version of GNU bc, which uses GMP to implement
numbers.

First, here is 2^^6 :
 "
   n = tetra(2, 6)
"
Now, the other lacking column,
3^^4 :
 "
   n = tetra(3, 4)
"
4^^4 :
 "
   n = tetra(4, 4)
"
5^^4 :
 "
   n = tetra(5, 4)
"
6^^4 :
 "
   n = tetra(6, 4)
"
We stop here, because the scale needed for the next
computation is "
   scale + length(tet(7, 3))
"Thank you, and don't mind the extra zero. "
}
