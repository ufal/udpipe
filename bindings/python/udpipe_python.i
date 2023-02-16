%module(package="ufal") udpipe

%pythonbegin %{
# __version__ =
%}

#define HAVE_CUSTOM_BYTES
%typemap(out) std::vector<uint8_t>* {
  $result = PyBytes_FromStringAndSize((const char*) $1->data(), $1->size());
  delete $1;
}

%include "../common/udpipe.i"
