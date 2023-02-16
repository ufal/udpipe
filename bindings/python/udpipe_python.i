%module(package="ufal") udpipe

%begin %{
#ifdef _WIN32
// On Windows, pyconfig.h defines macro hypot (#define hypot _hypot).
// That breaks cmath, which declares using ::hypot.
// As a temporary hack, cmath can be included first (it is included anyway).
// Nevertheless, including other headers before Python.h is officially not
// supported. so we may change the behaviour in the future if needed.
// Python 2.7 and 3.4 both require the hack and work well with it.
#include <cmath>
#endif
%}

%pythonbegin %{
# __version__ =
%}

#define HAVE_CUSTOM_BYTES
%typemap(out) std::vector<uint8_t>* {
  $result = PyBytes_FromStringAndSize((const char*) $1->data(), $1->size());
  delete $1;
}

%include "../common/udpipe.i"
