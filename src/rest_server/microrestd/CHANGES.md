Version 1.2.2 [15 Nov 23]
-------------------------
- Update also the forgotten `rest_server/version.cpp`.


Version 1.2.1 [15 Nov 23]
-------------------------
- Fix warnings on Clang 15 by qualifying `std::forward`.


Version 1.2.0 [31 Aug 22]
-------------------------
- Allow specifying additional headers when responding.
- Remove the `bool make_copy = true` default argument and copy
  every time to avoid silent tricky problems.


Version 1.1.3 [24 Jun 20]
-------------------------
- Get rid of G++ warnings and VS 2017 warnings.


Version 1.1.2 [19 Jul 18]
-------------------------
- Fix compilation on TDM-GCC.


Version 1.1.1 [11 Oct 17]
-------------------------
- Fix undefined behaviour resulting from incorrect usage
  of `va_start` with enum as the last non-variadic argument.


Version 1.1.0 [30 Jun 16]
-------------------------
- Add support for integers, booleans and nulls in JSON.


Version 1.0.0 [06 Nov 14]
-------------------------
- Initial public release.
