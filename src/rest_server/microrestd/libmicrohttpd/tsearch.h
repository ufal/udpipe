// This file is part of MicroRestD <http://github.com/ufal/microrestd/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <set>

namespace ufal {
namespace microrestd {

struct tsearch_comparer {
  tsearch_comparer(int (*compar)(const void *, const void *)) : compar(compar) {}

  int (*compar)(const void *, const void *);

  bool operator() (const void* x, const void* y) const { return compar(x, y) < 0; }
};
typedef std::set<const void*, tsearch_comparer> tsearch_set;

inline void *tsearch(const void *key, void **rootp, int (*compar)(const void *, const void *)) {
  if (rootp == nullptr) return nullptr;

  if (!*rootp)
    *rootp = (void*) new tsearch_set(tsearch_comparer(compar));

  return (void*) &*((tsearch_set*) *rootp)->insert(key).first;
}

inline void *tfind(const void *key, void *const *rootp, int (* /*compar*/)(const void *, const void *)) {
  if (rootp == nullptr || !*rootp) return nullptr;

  auto found = ((tsearch_set*) *rootp)->find(key);
  return found == ((tsearch_set*) *rootp)->end() ? nullptr : (void*) &*found;
}

inline void tdelete(const void *key, void **rootp, int (* /*compar*/)(const void *, const void *)) {
  if (rootp == nullptr || !*rootp) return;

  ((tsearch_set*) *rootp)->erase(key);
}

} // namespace microrestd
} // namespace ufal
