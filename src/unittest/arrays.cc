/*
 * liboz - OpenZone Core Library.
 *
 * Copyright © 2002-2014 Davorin Učakar
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software in
 *    a product, an acknowledgement in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

/**
 * @file unittest/arrays.cc
 */

#include "unittest.hh"

using namespace oz;

void test_arrays()
{
  Log() << "+ arrays";

  CIterator<Foo> ici;
  Iterator<Foo>  ii;

  Foo a[4] = { 1, 2, 3, 2 };
  Foo b[4];

  aMove(a, 4, b);
  aMove(b, 4, b);
  aMove(a, 0, b);
  OZ_CHECK_CONTENTS(a, -1, -1, -1, -1);
  OZ_CHECK_CONTENTS(b, 1, 2, 3, 2);
  OZ_CHECK(!aEquals(a, 4, b));
  OZ_CHECK(aEquals(a, 0, b));

  aMoveBackward(b, 4, a);
  aMoveBackward(a, 4, a);
  aMoveBackward(b, 0, a);
  aMove(a, 4, b);
  OZ_CHECK_CONTENTS(a, -1, -1, -1, -1);
  OZ_CHECK_CONTENTS(b, 1, 2, 3, 2);
  OZ_CHECK(!aEquals(a, 4, b));
  OZ_CHECK(aEquals(a, 0, b));

  aFill(a, 4, 0);
  aFill(a, 0, -1);
  OZ_CHECK_CONTENTS(a, 0, 0, 0, 0);

  aCopy(b, 4, a);
  aCopy(b, 0, a);
  OZ_CHECK(aEquals(a, 4, b));
  OZ_CHECK(iEquals(citer(a), citer(b)));

  aCopyBackward(a, 4, b);
  aCopyBackward(a, 0, b);
  OZ_CHECK(aEquals(a, 4, b));

  aCopy(a, 4, a);
  OZ_CHECK(aEquals(a, 4, b));

  aCopyBackward(a, 4, a);
  OZ_CHECK(aEquals(a, 4, b));

  OZ_CHECK(!aContains(a, 4, 0));
  OZ_CHECK(!aContains(b, 4, 0));
  OZ_CHECK(aContains(a, 4, 1));
  OZ_CHECK(aContains(b, 4, 1));
  OZ_CHECK(aContains(a, 4, 2));
  OZ_CHECK(aContains(b, 4, 2));
  OZ_CHECK(aContains(a, 4, 3));
  OZ_CHECK(aContains(b, 4, 3));
  OZ_CHECK(!aContains(b, 0, 3));

  OZ_CHECK(aIndex(a, 4, 0) == -1);
  OZ_CHECK(aIndex(a, 4, 1) == 0);
  OZ_CHECK(aIndex(a, 4, 2) == 1);
  OZ_CHECK(aIndex(a, 4, 3) == 2);
  OZ_CHECK(aIndex(a, 0, 3) == -1);

  OZ_CHECK(aLastIndex(a, 4, 0) == -1);
  OZ_CHECK(aLastIndex(a, 4, 1) == 0);
  OZ_CHECK(aLastIndex(a, 4, 2) == 3);
  OZ_CHECK(aLastIndex(a, 4, 3) == 2);
  OZ_CHECK(aLastIndex(a, 0, 3) == -1);

  aReverse(a, 4);
  OZ_CHECK_CONTENTS(a, 2, 3, 2, 1);

  Foo** c = new Foo*[5]();
  for (Foo*& i : iter(c, 5)) {
    i = new Foo();
  }
  aFree(c, 5);
  delete[] c;

  OZ_CHECK(aLength(a) == 4);

  Foo* d = new Foo[4]();
  aCopy(b, 4, d);
  d = aReallocate(d, 4, 10);
  OZ_CHECK(aEquals(b, 4, d));
  delete[] d;

  for (int j = 0; j < 100; ++j) {
    int r[1000];
    for (int i = 0; i < 1000; ++i) {
      r[i] = Math::rand(1000);
    }
    aSort(r, 1000);

    for (int i = -1; i <= 1000; ++i) {
      int index = aBisection(r, 1000, i);

      if (1 <= i && i < 1000) {
        OZ_CHECK(r[i - 1] <= r[i]);
      }

      OZ_CHECK((index == -1 && r[0] > i) ||
               (index == 999 && r[999] <= i) ||
               (r[index] <= i && r[index + 1] > i));
    }
  }
}
