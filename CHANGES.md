Version 2.1.0 [16 Nov 23]
-------------------------
- Add support for using a morphological dictionary via `ufal.morphodita`
  during prediction – if the dictionary returns some analyses for a given
  form, we return the one most probable according to the predicted logits.
- Add support for `--no_single_root` in the evaluation script.


Version 2.0.0 [05 Aug 22]
-------------------------
- Initial release.
