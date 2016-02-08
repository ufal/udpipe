// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <list>
#include <set>
#include <string>
#include <vector>

using namespace std;

vector<string> namespaces_opening;
vector<string> namespaces_closing;

set<string> system_includes;
set<string> local_includes;

struct bundle_file {
  string file_name;
  vector<string> lines;

  bundle_file(const string& file_name) : file_name(file_name) {}
};
list<bundle_file> bundles;

void add_file(list<bundle_file>::iterator it, const string& top_directory, const string& file) {
  if (local_includes.count(file)) return;

  auto bundle = bundles.insert(it, file);
  local_includes.insert(file);

  ifstream is(top_directory + file);
  if (!is) cerr << "Cannot open file " << top_directory + file << "!" << endl, exit(1);

  // Before opening the namespaces, there should be only
  // - comments
  // - system includes
  // - local includes
  // - #pragma once
  string line;
  while (getline(is, line)) {
    if (line.empty() || line.find("//") == 0) {
      if (!line.empty() || bundle->lines.empty() || !bundle->lines.back().empty())
        bundle->lines.push_back(line);
    } else if (line == "#pragma once") {
    } else if (line.find("#include <") == 0 && line.substr(line.size() - 1) == ">") {
      system_includes.insert(string(line, 10, line.size() - 11));
    } else if (line.find("#include \"") == 0) {
      string header_path;
      for (int location = 0; header_path.empty() && location <= 1; location++) {
        header_path = file.substr(0, location == 0 ? file.find_last_of("/\\") + 1 : 0) + line.substr(10, line.size() - 11);
        ifstream hs(top_directory + header_path);
        if (!hs) header_path.clear();
      }
      if (header_path.empty()) cerr << "Cannot find include " << line << " from file " << file << "!" << endl, exit(1);
      add_file(bundle, top_directory, header_path);
    } else {
      break;
    }
  }

  // Stop if the file ended.
  if (is.eof()) return;

  // Check that correct namespaces are being opened
  for (auto&& namespace_opening : namespaces_opening) {
    if (line != namespace_opening) cerr << "Expected namespace declaration '" << namespace_opening << "' in file " << file << ", but got " << line << endl, exit(1);
    getline(is, line);
  }

  // Add all lines
  do
    if (!line.empty() || bundle->lines.empty() || !bundle->lines.back().empty())
      bundle->lines.push_back(line);
  while (getline(is, line));

  // Check that the last lines were the closing namespaces declarations
  for (auto index = namespaces_closing.size(); index; index--) {
    if (bundle->lines.empty() || bundle->lines.back() != namespaces_closing[index - 1])
      cerr << "Expected closing namespace declaration '" << namespaces_closing[index - 1] << "' in file " << file << "!" << endl, exit(1);
    bundle->lines.pop_back();
  }

  // Remove all empty lines at the end of the file
  while (!bundle->lines.empty() && bundle->lines.back().empty())
    bundle->lines.pop_back();
}

int main(int argc, char* argv[]) {
  if (argc < 3) return cerr << "Usage: " << argv[0] << " namespace top_directory [source_files]\n", 1;

  string namespaces = argv[1];
  string top_directory = argv[2];

  // Fill namespaces_opening and namespaces_closing.
  for (size_t index = 0, double_colon = namespaces.find("::"); ; index = double_colon + 2, double_colon = namespaces.find("::", index)) {
    namespaces_opening.push_back("namespace " + namespaces.substr(index, double_colon - index) + " {");
    namespaces_closing.insert(namespaces_closing.begin(), "} // namespace " + namespaces.substr(index, double_colon - index));
    if (double_colon == string::npos) break;
  }

  // Sort all given files
  vector<string> files;
  for (int argi = 3; argi < argc; argi++)
    files.push_back(argv[argi]);
  sort(files.begin(), files.end());

  // Parse all sources
  for (auto&& file : files)
    add_file(bundles.end(), top_directory, file);

  // Generate bundle
  cout << endl;
  for (auto&& system_include : system_includes)
    cout << "#include <" << system_include << ">" << endl;

  cout << endl;
  for (auto&& namespace_opening : namespaces_opening)
    cout << namespace_opening << endl;

  for (auto&& bundle : bundles) {
    cerr << "Adding file " << bundle.file_name << endl;

    cout << endl;
    cout << "/////////" << endl;
    cout << "// File: " << bundle.file_name << endl;
    cout << "/////////" << endl;
    cout << endl;
    for (auto&& line : bundle.lines)
      cout << line << endl;
  }

  cout << endl;
  for (auto&& namespace_closing : namespaces_closing)
    cout << namespace_closing << endl;

  return 0;
}
