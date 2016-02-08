// This file is part of MicroRestD <http://github.com/ufal/microrestd/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#define _SET_INIT_AND_DEINIT_FUNCS(FuncInitName, FuncDeInitName)                                           \
 class InitializerClass##FuncInitName##FuncDeInitName {                                                    \
  private:                                                                                                 \
   InitializerClass##FuncInitName##FuncDeInitName() { FuncInitName(); }                                    \
   ~InitializerClass##FuncInitName##FuncDeInitName() { FuncDeInitName(); }                                 \
   static InitializerClass##FuncInitName##FuncDeInitName singleton;                                        \
 };                                                                                                        \
 InitializerClass##FuncInitName##FuncDeInitName InitializerClass##FuncInitName##FuncDeInitName::singleton;
