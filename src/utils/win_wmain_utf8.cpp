// This file is part of UFAL C++ Utils <http://github.com/ufal/cpp_utils/>.
//
// Copyright 2022 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifdef _WIN32

#include <cstdlib>

#include <windows.h>

int main(int argc, char* argv[]);

static UINT console_cp;
static UINT console_output_cp;
static void restore_console() {
  SetConsoleCP(console_cp);
  SetConsoleOutputCP(console_output_cp);
}

int wmain(int argc, wchar_t* argv[])
{
  console_cp = GetConsoleCP();
  console_output_cp = GetConsoleOutputCP();
  atexit(restore_console);
  SetConsoleCP(CP_UTF8);
  SetConsoleOutputCP(CP_UTF8);

  int nbytes = sizeof(char*) * (argc + 1);
  for (int i = 0; i < argc; ++i)
    nbytes += WideCharToMultiByte(CP_UTF8, 0, argv[i], -1, NULL, 0, NULL, NULL);

  HANDLE heap = GetProcessHeap();
  char** args = (char**) HeapAlloc(heap, 0, nbytes);

  args[0] = (char*)(args + argc + 1);
  for (int i = 0; i < argc; ++i)
    args[i+1] = args[i] + WideCharToMultiByte(CP_UTF8, 0, argv[i], -1, args[i], nbytes, NULL, NULL);
  args[argc] = NULL;

  int ret = main(argc, args);
  HeapFree(heap, 0, args);

  return ret;
}

#endif
