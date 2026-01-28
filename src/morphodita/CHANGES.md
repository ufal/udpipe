Version 1.11.4-dev
------------------
- Fix compilation and avoid warnings in C++20.


Version 1.11.3 [24 Nov 25]
--------------------------
- Update UFAL C++ Utils to 3.5.5 to silence g++ warning and fix UBSan errors.
- Update MicroRestD to 1.2.4 to avoid Clang warning.
- Fix UBSan errors.


Version 1.11.2 [15 Nov 23]
--------------------------
- Fix a bug in the tokenizers, which did not end a sentence on an
  empty line if the preceding line ended with whitespace. However,
  only the REST server and the API was affected; the `run_*` binaries
  read input a paragraph at a time, and so circumvent the problem.
- Fix warnings in Clang 15 by qualifying `std::move`.


Version 1.11.1 [18 Feb 23]
--------------------------
- Do not assume char is signed; fixes silent errors on aarch64
  when compiled from the source (e.g. in Docker on ARM macOS).
- No new binaries are provided, because none were affected.


Version 1.11.0 [01 Sep 22]
--------------------------
- When analyzing numerals, if the form ends with a dot, keep the dot
  in the lemma (previously the dot was discarded).
- Correctly handle lemma number which is not null-terminated in `generate`.
- On Windows, the file paths are now UTF-8 encoded, instead of ANSI.
  This change affects the API, binary arguments, and program outputs.
- Add ARM64 macOS build.
- The Windows binaries are now compiled with VS 2019, older systems
  than Windows 7 are no longer supported.
- Add billing information to the REST server.
- Add type information to the Python package.
- The Python wheels are provided for Pythons 3.6-3.11.


Version 1.10.1 [29 Jun 20]
--------------------------
- Allow overriding path to Java library in the bindings.
- Document statistical guesser training using train_guesser binary.
- Improve the GRU tokenizer.
- Do not generate empty lemmas in guesser.
- Add explicit `GUESSER_UNSPECIFIED` guesser mode already used by tagger.
- Fix unaligned pointer access and other UndefinedBehaviourSanitizer findings.
- Fix bug in tag_filter incorrectly matching negates sets of 2+ characters.
- Add several REST server options.
- Add methods running tagset_converter during derivation_formatter.
  This allows correct handling of their interactions.
- Add binary wheels of the Python package.


Version 1.9.2 [22 Sep 16]
-------------------------
- Fix regression in `morpho::generate` caused in 1.9.1.
- Add check in `encode_dictionary` that no lemma is repeated.


Version 1.9.1 [20 Sep 16]
-------------------------
- Allow loading larger dictionaries by increasing the internal
  limit of size of search structures created during loading.
- Export `string_piece` using correct namespace in `src_lib_only/morphodita.h`.


Version 1.9.0-prerelease [15 May 16]
------------------------------------
- Use t2t_docsys http://github.com/ufal/t2t_docsys as documentation system.
- Use C++ Builtem http://github.com/ufal/cpp_builtem as build system.
- Add C# bindings.
- Remove support for shared library build.
- Change license from LGPL to MPL 2.0.
- Use C++ iostreams instead of C stdio.
- Start using Semantic Versioning http://semver.org/.
- Add REST server using MicroRestD http://github.com/ufal/microrestd.
- Add `tagger::tag_analyzed` method.
- Add `derivator` and `derivation_formatter` classes.


Version 1.3.0 [29 Aug 14]
-------------------------
- Add support for OS X 10.7 and later.
- Use UniLib instead of MorphoDiTa specific utf8 class.
- Update Unicode in tokenizer from 6.3.0 to 7.0.0.
- Fix English tokenizer bug #1.
- Use `PyTypeObject` in Python bindings instead of proxy classes.


Version 1.2.0 [22 Apr 14]
-------------------------
- Add `strip_lemma_comment` and `strip_lemma_id` tag set converters.


Version 1.1.0 [09 Apr 14]
-------------------------
- Allow English negative prefix rules to overlap, allowing
  to use also "non-" as negative prefix in additon to "non".
- Allow passing tagger model instead of morphological model
  to `run_morpho_analyze`, `run_morpho_cli` and `run_morpho_generate`
  binaries.
- Add `--version` option printing current version of binaries.
- Clarify stable/development versions.


Version 1.0.0 [07 Apr 14]
-------------------------
- Initial public release.
