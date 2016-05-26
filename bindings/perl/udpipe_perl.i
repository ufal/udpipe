%module "Ufal::UDPipe"

%runtime %{
#ifdef seed
  #undef seed
#endif
%}

%include "../common/udpipe.i"
