// This file is part of UDPipe <http://github.com/ufal/udpipe/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "common.h"
#include "utils/iostreams.h"
#include "utils/options.h"
#include "utils/process_args.h"
#include "unilib/utf8.h"
#include "version/version.h"

using namespace ufal::udpipe;
using namespace std;

class normalize_form {
 public:
  virtual ~normalize_form() {}

  virtual string normalize(string_piece text) const = 0;
};

class normalize_form_morphodita_parsito : public normalize_form {
 public:
  virtual string normalize(string_piece text) const override {
    using unilib::utf8;

    // Keep in sync with model_morphodita_parsito::normalize_form!

    string output;
    for (auto chr : utf8::decoder(text.str, text.len)) {
      // Arabic normalization
      if (chr == 0x640 || (chr >= 0x64B && chr <= 0x657) || chr == 0x670) {}
      else if (chr == 0x622) utf8::append(output, 0x627);
      else if (chr == 0x623) utf8::append(output, 0x627);
      else if (chr == 0x624) utf8::append(output, 0x648);
      else if (chr == 0x625) utf8::append(output, 0x627);
      else if (chr == 0x626) utf8::append(output, 0x64A);
      else if (chr == 0x671) utf8::append(output, 0x627);
      else if (chr == 0x6A9) utf8::append(output, 0x643);
      else if (chr == 0x6AA) utf8::append(output, 0x643);
      else if (chr == 0x6CC) utf8::append(output, 0x64A);
      // Default
      else utf8::append(output, chr);
    }

    return output;
  }
};

int main(int argc, char* argv[]) {
  iostreams_init();

  options::map options;
  if (!options::parse({{"method", options::value{"morphodita_parsito"}},
                       {"version", options::value::none},
                       {"help", options::value::none}}, argc, argv, options) ||
      options.count("help"))
    runtime_failure("Usage: " << argv[0] << " [options] [input_files]\n"
                    "Options: --method=[morphodita_parsito] which method to use\n"
                    "         --version\n"
                    "         --help");
  if (options.count("version"))
    return cout << version::version_and_copyright() << endl, 0;

  // Create a normalizer
  unique_ptr<normalize_form> normalizer;

  string method = options.count("method") ? options["method"] : "morphodita_parsito";
  if (method == "morphodita_parsito")
    normalizer.reset(new normalize_form_morphodita_parsito);
  else
    runtime_failure("Unknown method '" << method << "'!");

  if (!normalizer)
    runtime_failure("Cannot create specified form normalizer");

  // Process input
  string line;
  while (getline(cin, line))
    cout << normalizer->normalize(line) << '\n';

  return 0;
}
