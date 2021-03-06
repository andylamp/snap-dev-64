namespace TSnap {

#if 0
// OP RS 2016/09/12 commented out, fails to compile for Snap.py on Windows.
// This function is obsolete, since it is TNGraph specific.
// Use MergeNbrs() instead.
void GetMergeSortedV(TInt64V& NeighbourV, TNGraph::TNodeI NI) {
  int64 j = 0;
  int64 k = 0;
  int64 prev = -1;
  int64 indeg = NI.GetInDeg();
  int64 outdeg = NI.GetOutDeg();
  if (indeg > 0  &&  outdeg > 0) {
    int64 v1 = NI.GetInNId(j);
    int64 v2 = NI.GetOutNId(k);
    while (1) {
      if (v1 <= v2) {
        if (prev != v1) {
          NeighbourV.Add(v1);
          prev = v1;
        }
        j += 1;
        if (j >= indeg) {
          break;
        }
        v1 = NI.GetInNId(j);
      } else {
        if (prev != v2) {
          NeighbourV.Add(v2);
          prev = v2;
        }
        k += 1;
        if (k >= outdeg) {
          break;
        }
        v2 = NI.GetOutNId(k);
      }
    }
  }
  while (j < indeg) {
    int64 v = NI.GetInNId(j);
    if (prev != v) {
      NeighbourV.Add(v);
      prev = v;
    }
    j += 1;
  }
  while (k < outdeg) {
    int64 v = NI.GetOutNId(k);
    if (prev != v) {
      NeighbourV.Add(v);
      prev = v;
    }
    k += 1;
  }
}
#endif

int64 GetCommon(TInt64V& A, TInt64V& B) {
  int64 i, j;
  int64 ret = 0;
  int64 alen, blen;
  int64 d;
  TInt64 ai;

  alen = A.Len();
  blen = B.Len();
  i = 0;
  j = 0;
  if (i >= alen  ||  j >= blen) {
    return ret;
  }

  while (1) {
    d = A[i] - B[j];
    if (d < 0) {
      i++;
      if (i >= alen) {
        break;
      }
    } else if (d > 0) {
      j++;
      if (j >= blen) {
        break;
      }
    } else {
      ret++;
      i++;
      if (i >= alen) {
        break;
      }
      j++;
      if (j >= blen) {
        break;
      }
    }
  }
  return ret;
}

} // namespace TSnap
