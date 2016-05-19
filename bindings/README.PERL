Perl UDPipe Bindings
====================

Installation
============

The Perl bindings are available as Ufal-UDPipe package on CPAN.

To compile Perl bindings manually, run make in the bindings/perl directory,
optionally with the options descriged in UDPipe Installation. Perl 5.10 and
later is supported.

Path to the include headers of the required Perl version must be specified in
the PERL_INCLUDE variable using
  make PERL_INCLUDE=path_to_Perl_includes


Perl Bindings API
=================

UDPipe library bindings is available in the Ufal::UDPipe
(http://search.cpan.org/~straka/Ufal-UDPipe/) package. The classes can be
imported into the current namespace using the :all export tag.

The bindings is a straightforward conversion of the C++ bindings API. Vectors do
not have native Perl interface, see Ufal::UDPipe::Words for reference. Static
methods and enumerations are available only through the module, not through
object instance.


C++ Bindings API
================


Helper Structures
-----------------


Main Classes
------------
