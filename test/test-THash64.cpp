#include <gtest/gtest.h>
#include "Snap.h"
#include "iostream"

int64 Prime64(const int64& n);
bool IsPrime64(const int64& d);
// Test the default constructor
TEST(TIntInt64H, DefaultConstructor) {
  TIntInt64H TableInt;

  EXPECT_EQ(1,TableInt.Empty());
  EXPECT_EQ(0,TableInt.Len());
  EXPECT_EQ(0,TableInt.GetMxKeyIds());
}

// Table manipulations and inserting keys and data >32bits
TEST(TIntInt64H, ManipulateTableWithInt64KeysAndValues) {
  const int64 OFFSET = 100000000000000;
  const int64 NElems = 1000000;
  int64 DDist = 10;
  const char *FName = "test.hashint64.dat";
  TIntInt64H TableInt;
  TIntInt64H TableInt1;
  TIntInt64H TableInt2;
  int64 i;
  int64 d;
  int64 n;
  int64 Id;
  int64 Key;
  int64 KeySumVal;
  int64 DatSumVal;
  int64 KeySum;
  int64 DatSum;
  int64 KeySumDel;
  int64 DatSumDel;
  int64 DelCount;
  int64 Count;

  // add table elements
  d = Prime64(NElems);
  n = d + OFFSET;
  KeySumVal = 0;
  DatSumVal = 0;
  for (i = 0; i < NElems; i++) {
    TableInt.AddDat(n,n+1);
    KeySumVal += n;
    DatSumVal += (n+1);
    //printf("add %d %d\n", n, n+1);
    //n = (n + d) % NElems;
    n = n+ d;
  }
  EXPECT_EQ(0,TableInt.Empty());
  EXPECT_EQ(NElems,TableInt.Len());

  // verify elements by successive keys
  KeySum = 0;
  DatSum = 0;
  n = d + OFFSET;
  //std::cout<< OFFSET<< " "<<n<<std::endl;
  for (i = 0; i < NElems; i++) {
    Id = TableInt.GetKeyId(n);
    EXPECT_EQ(1,Id >= 0);
    Key = TableInt.GetKey(Id);
    EXPECT_EQ(0,TableInt.GetDat(Key)-Key-1);
    KeySum += Key;
    DatSum += TableInt.GetDat(Key);
    //std::cout <<n <<" "<<Key<<" "<< TableInt.GetDat(Key)<<std::endl;
    n = n + d;
  }

  EXPECT_EQ(0,KeySumVal - KeySum);
  EXPECT_EQ(0,DatSumVal - DatSum);

//  // verify elements by distant keys
//    KeySum = 0;
//    DatSum = 0;
//    n = d + OFFSET;
//    for (i = 0; i < NElems; i++) {
//      Id = TableInt.GetKeyId(n);
//      EXPECT_EQ(1,Id >= 0);
//      Key = TableInt.GetKey(Id);
//      EXPECT_EQ(0,TableInt.GetDat(Key)-Key-1);
//      KeySum += Key;
//      DatSum += TableInt.GetDat(Key);
//      n = (n + d) % NElems;
//    }
//
//    EXPECT_EQ(0,KeySumVal - KeySum);
//    EXPECT_EQ(0,DatSumVal - DatSum);

    // verify elements by iterator
    KeySum = 0;
    DatSum = 0;
    for (TIntInt64H::TIter It = TableInt.BegI(); It < TableInt.EndI(); It++) {
      EXPECT_EQ(0,It.GetDat()-It.GetKey()-1);
      KeySum += It.GetKey();
      DatSum += It.GetDat();
    }

    EXPECT_EQ(0,KeySumVal - KeySum);
    EXPECT_EQ(0,DatSumVal - DatSum);

    // verify elements by key index
    KeySum = 0;
    DatSum = 0;
    Id = TableInt.FFirstKeyId();
    while (TableInt.FNextKeyId(Id)) {
      EXPECT_EQ(1,Id >= 0);
      Key = TableInt.GetKey(Id);
      EXPECT_EQ(0,TableInt.GetDat(Key)-Key-1);
      KeySum += Key;
      DatSum += TableInt.GetDat(Key);
    }

    EXPECT_EQ(0,KeySumVal - KeySum);
    EXPECT_EQ(0,DatSumVal - DatSum);

    // delete elements
    DelCount = 0;
    KeySumDel = 0;
    DatSumDel = 0;
    n = d + OFFSET;
    for (n = d + OFFSET; n < d + OFFSET + NElems*d; n += DDist*d) {
      Id = TableInt.GetKeyId(n);
      //printf("del %d %d %d\n", n, Id, (int) TableInt[Id]);
      KeySumDel += n;
      DatSumDel += TableInt[Id];
      TableInt.DelKeyId(Id);
      DelCount++;
    }
    EXPECT_EQ(0,TableInt.Empty());
    EXPECT_EQ(NElems-DelCount,TableInt.Len());

    // verify elements by iterator
    KeySum = 0;
    DatSum = 0;
    Count = 0;
    for (TIntInt64H::TIter It = TableInt.BegI(); It < TableInt.EndI(); It++) {
      EXPECT_EQ(0,It.GetDat()-It.GetKey()-1);
      //printf("get %d %d\n", (int) It.GetKey(), (int) It.GetDat());
      KeySum += It.GetKey();
      DatSum += It.GetDat();
      Count++;
    }

    EXPECT_EQ(NElems-DelCount,Count);
    EXPECT_EQ(0,KeySumVal - KeySumDel - KeySum);
    EXPECT_EQ(0,DatSumVal - DatSumDel - DatSum);

    // assignment
    TableInt1 = TableInt;
    EXPECT_EQ(0,TableInt1.Empty());
    EXPECT_EQ(NElems-DelCount,TableInt1.Len());

    // verify elements by iterator
    KeySum = 0;
    DatSum = 0;
    Count = 0;
    for (TIntInt64H::TIter It = TableInt1.BegI(); It < TableInt1.EndI(); It++) {
      EXPECT_EQ(0,It.GetDat()-It.GetKey()-1);
      //printf("get %d %d\n", (int) It.GetKey(), (int) It.GetDat());
      KeySum += It.GetKey();
      DatSum += It.GetDat();
      Count++;
    }

    EXPECT_EQ(NElems-DelCount,Count);
    EXPECT_EQ(0,KeySumVal - KeySumDel - KeySum);
    EXPECT_EQ(0,DatSumVal - DatSumDel - DatSum);

    // saving and loading
    {
      TFOut FOut(FName);
      TableInt.Save(FOut);
      FOut.Flush();
    }

    {
      TFIn FIn(FName);
      TableInt2.Load(FIn);
    }

    EXPECT_EQ(NElems-DelCount,TableInt2.Len());

    // verify elements by iterator
    KeySum = 0;
    DatSum = 0;
    Count = 0;
    for (TIntInt64H::TIter It = TableInt2.BegI(); It < TableInt2.EndI(); It++) {
      EXPECT_EQ(0,It.GetDat()-It.GetKey()-1);
      //printf("get %d %d\n", (int) It.GetKey(), (int) It.GetDat());
      KeySum += It.GetKey();
      DatSum += It.GetDat();
      Count++;
    }

    EXPECT_EQ(NElems-DelCount,Count);
    EXPECT_EQ(0,KeySumVal - KeySumDel - KeySum);
    EXPECT_EQ(0,DatSumVal - DatSumDel - DatSum);

    // remove all elements
    for (i = 0; i < Count; i++) {
      Id = TableInt.GetRndKeyId(TInt::Rnd, 0.5);
      TableInt.DelKeyId(Id);
    }
    EXPECT_EQ(0,TableInt.Len());
    EXPECT_EQ(1,TableInt.Empty());

    // verify elements by iterator
    KeySum = 0;
    DatSum = 0;
    Count = 0;
    for (TIntInt64H::TIter It = TableInt.BegI(); It < TableInt.EndI(); It++) {
      EXPECT_EQ(0,It.GetDat()-It.GetKey()-1);
      //printf("get %d %d\n", (int) It.GetKey(), (int) It.GetDat());
      KeySum += It.GetKey();
      DatSum += It.GetDat();
      Count++;
    }

    EXPECT_EQ(0,Count);
    EXPECT_EQ(0,KeySum);
    EXPECT_EQ(0,DatSum);

    // clear the table
    TableInt1.Clr();
    EXPECT_EQ(0,TableInt1.Len());
    EXPECT_EQ(1,TableInt1.Empty());

    // verify elements by iterator
    KeySum = 0;
    DatSum = 0;
    Count = 0;
    for (TIntInt64H::TIter It = TableInt1.BegI(); It < TableInt1.EndI(); It++) {
      EXPECT_EQ(0,It.GetDat()-It.GetKey()-1);
      //printf("get %d %d\n", (int) It.GetKey(), (int) It.GetDat());
      KeySum += It.GetKey();
      DatSum += It.GetDat();
      Count++;
    }

    EXPECT_EQ(0,Count);
    EXPECT_EQ(0,KeySum);
    EXPECT_EQ(0,DatSum);

}

// Table manipulations and inserting with >int32 elements
// Table manipulations
TEST(TIntInt64H, ManipulateTablewithInt64) {
  const int64 NElems = 5000000000;
  int64 DDist = 10000;
  const char *FName = "test.hashint.dat";
  TIntInt64H TableInt;
  TIntInt64H TableInt1;
  TIntInt64H TableInt2;
  int64 i;
  int64 d;
  int64 n;
  int64 Id;
  int64 Key;
  int64 DelCount;
  int64 Count;
  std::cerr<<"Add table\n";
  // add table elements
  d = Prime64(NElems);
  n = d;
  std::cerr<<d<<std::endl;
  for (i = 0; i < NElems; i++) {
    TableInt.AddDat(n,n+1);
    n = (n + d) % NElems;
    if (i%100000000 == 0)
      std::cerr<<i/100000000<<std::endl;
  }
  EXPECT_EQ(0,TableInt.Empty());
  EXPECT_EQ(NElems,TableInt.Len());

  std::cerr<<"Verify elements by successive keys\n";

  // verify elements by successive keys
  for (i = 0; i < NElems; i++) {
    Id = TableInt.GetKeyId(i);
    EXPECT_EQ(1,Id >= 0);
    Key = TableInt.GetKey(Id);
    EXPECT_EQ(0,TableInt.GetDat(Key)-Key-1);
  }

  std::cerr<<"verify elements by distant keys\n";

  // verify elements by distant keys
  n = Prime64(d);
  for (i = 0; i < NElems; i++) {
    Id = TableInt.GetKeyId(n);
    EXPECT_EQ(1,Id >= 0);
    Key = TableInt.GetKey(Id);
    EXPECT_EQ(0,TableInt.GetDat(Key)-Key-1);
    n = (n + d) % NElems;
  }

  std::cerr<<"Verify elements by iterator\n";

  // verify elements by iterator
  for (TIntInt64H::TIter It = TableInt.BegI(); It < TableInt.EndI(); It++) {
    EXPECT_EQ(0,It.GetDat()-It.GetKey()-1);
  }

  std::cerr<<"Verify elements by key index\n";
  // verify elements by key index
  Id = TableInt.FFirstKeyId();
  while (TableInt.FNextKeyId(Id)) {
    EXPECT_EQ(1,Id >= 0);
    Key = TableInt.GetKey(Id);
    EXPECT_EQ(0,TableInt.GetDat(Key)-Key-1);
  }

  std::cerr<<"Delete elements\n";
  // delete elements
  int64 DelKeySum = 0;
  int64 DelDatSum = 0;
  DelCount = 0;
  for (n = 0; n < NElems; n += DDist) {
    Id = TableInt.GetKeyId(n);
    //printf("del %d %d %d\n", n, Id, (int) TableInt[Id]);
    DelKeySum += n;
    DelDatSum += TableInt[Id];
    TableInt.DelKeyId(Id);
    DelCount++;

  }
  EXPECT_EQ(0,TableInt.Empty());
  EXPECT_EQ(NElems-DelCount,TableInt.Len());
  // verify elements by iterator
  Count = 0;
  for (TIntInt64H::TIter It = TableInt.BegI(); It < TableInt.EndI(); It++) {
    EXPECT_EQ(0,It.GetDat()-It.GetKey()-1);
    //printf("get %d %d\n", (int) It.GetKey(), (int) It.GetDat());
    Count++;
  }

  EXPECT_EQ(NElems-DelCount,Count);

  std::cerr<<"Assignment\n";
  // assignment
  TableInt1 = TableInt;
  EXPECT_EQ(0,TableInt1.Empty());
  EXPECT_EQ(NElems-DelCount,TableInt1.Len());

  // verify elements by iterator
  Count = 0;
  for (TIntInt64H::TIter It = TableInt1.BegI(); It < TableInt1.EndI(); It++) {
    EXPECT_EQ(0,It.GetDat()-It.GetKey()-1);
    //printf("get %d %d\n", (int) It.GetKey(), (int) It.GetDat());
    Count++;
  }

  EXPECT_EQ(NElems-DelCount,Count);

  std::cerr<<"Saving and loading\n";
  // saving and loading
  {
    TFOut FOut(FName);
    TableInt.Save(FOut);
    FOut.Flush();
  }

  {
    TFIn FIn(FName);
    TableInt2.Load(FIn);
  }

  EXPECT_EQ(NElems-DelCount,TableInt2.Len());

  // verify elements by iterator
  Count = 0;
  for (TIntInt64H::TIter It = TableInt2.BegI(); It < TableInt2.EndI(); It++) {
    EXPECT_EQ(0,It.GetDat()-It.GetKey()-1);
    //printf("get %d %d\n", (int) It.GetKey(), (int) It.GetDat());
    Count++;
  }

  EXPECT_EQ(NElems-DelCount,Count);
  int64 KeySum = 0;
  int64 DatSum = 0;
//  std::cerr<<"Remove all elements\n";
//  // remove all elements
//  for (i = 0; i < Count; i++) {
//    Id = TableInt.GetRndKeyId(TInt::Rnd, 0.5);
//    TableInt.DelKeyId(Id);
//  }
//  EXPECT_EQ(0,TableInt.Len());
//  EXPECT_EQ(1,TableInt.Empty());
//  std::cerr<<"Verifying elements by iterator after removal\n";
//  // verify elements by iterator
//  Count = 0;
//  for (TIntInt64H::TIter It = TableInt.BegI(); It < TableInt.EndI(); It++) {
//    EXPECT_EQ(0,It.GetDat()-It.GetKey()-1);
//    //printf("get %d %d\n", (int) It.GetKey(), (int) It.GetDat());
//    KeySum += It.GetKey();
//    DatSum += It.GetDat();
//    Count++;
//  }
//
//  EXPECT_EQ(0,Count);
//  EXPECT_EQ(0,KeySum);
//  EXPECT_EQ(0,DatSum);
  std::cerr<<"Clear Table\n";
  // clear the table
  TableInt1.Clr();
  EXPECT_EQ(0,TableInt1.Len());
  EXPECT_EQ(1,TableInt1.Empty());

  // verify elements by iterator
  KeySum = 0;
  DatSum = 0;
  Count = 0;
  for (TIntInt64H::TIter It = TableInt1.BegI(); It < TableInt1.EndI(); It++) {
    EXPECT_EQ(0,It.GetDat()-It.GetKey()-1);
    //printf("get %d %d\n", (int) It.GetKey(), (int) It.GetDat());
    KeySum += It.GetKey();
    DatSum += It.GetDat();
    Count++;
  }

  EXPECT_EQ(0,Count);
  EXPECT_EQ(0,KeySum);
  EXPECT_EQ(0,DatSum);
}


TEST(TIntInt64H, ManipulateTablewithInt64GetRndId) {
  const int64 NElems = 5000000000;
  int64 DDist = 10000;
  const char *FName = "test.hashint.dat";
  TIntInt64H TableInt;
  TIntInt64H TableInt1;
  TIntInt64H TableInt2;
  int64 i;
  int64 d;
  int64 n;
  int64 Id;
  int64 Key;
  int64 DelCount;
  int64 Count;
  std::cerr<<"Add table\n";
  // add table elements
  d = Prime64(NElems);
  n = d;
  std::cerr<<d<<std::endl;
  for (i = 0; i < NElems; i++) {
    TableInt.AddDat(n,n+1);
    n = (n + d) % NElems;
    if (i%100000000 == 0)
      std::cerr<<i/100000000<<std::endl;
  }
  EXPECT_EQ(0,TableInt.Empty());
  EXPECT_EQ(NElems,TableInt.Len());

  std::cerr<<"Remove all elements\n";
  // remove all elements
  for (i = 0; i < NElems; i++) {
    Id = TableInt.GetRndKeyId(TInt::Rnd, 0.8);
    TableInt.DelKeyId(Id);
    if (i%10000000 == 0)
      std::cerr<<i/10000000<<std::endl;
  }
  EXPECT_EQ(0,TableInt.Len());
  EXPECT_EQ(1,TableInt.Empty());
  std::cerr<<"Verifying elements by iterator after removal\n";
  // verify elements by iterator
  int64 KeySum = 0;
  int64 DatSum = 0;
  Count = 0;
  for (TIntInt64H::TIter It = TableInt.BegI(); It < TableInt.EndI(); It++) {
    EXPECT_EQ(0,It.GetDat()-It.GetKey()-1);
    //printf("get %d %d\n", (int) It.GetKey(), (int) It.GetDat());
    KeySum += It.GetKey();
    DatSum += It.GetDat();
    Count++;
  }

  EXPECT_EQ(0,Count);
  EXPECT_EQ(0,KeySum);
  EXPECT_EQ(0,DatSum);

}


bool IsPrime64(const int64& d) {
  int64 i;
  int64 j;

  // 1, 2, 3 are primes
  if (d < 4  ||  (d & 0x01) == 0) {
    return true;
  }

  // even numbers are not primes
  if (d < 4  ||  (d & 0x01) == 0) {
    return true;
  }

  // test divisions with < sqrt(d)
  for (i = 2; i*i <= d; i++) {
    j = d/i;
    if (j*i == d) {
      return false;
    }
  }

  return true;
}

int64 Prime64(const int64& n) {
  int64 d;

  d = (int64) ((float) n * 0.61803398875);    // golden ratio
  d |= 0x01;                                // get an odd number
  while (!IsPrime64(d)) {
    d += 2;
  }

  return d;
}

