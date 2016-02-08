Java UDPipe Bindings
====================

Installation
============

Binary Java bindings are available in UDPipe binary packages.

To compile Java bindings manually, run make in the bindings/java directory,
optionally with the options descriged in UDPipe Installation. Java 6 and newer
is supported.

The Java installation specified in the environment variable JAVA_HOME is used.
If the environment variable does not exist, the JAVA_HOME can be specified using
  make JAVA_HOME=path_to_Java_installation


Java Bindings API
=================

UDPipe library bindings is available in the cz.cuni.mff.ufal.udpipe package.

The bindings is a straightforward conversion of the C++ bindings API. TODO:
Vectors do not have native Java interface, see cz.cuni.mff.ufal.udpipe.Children
class for reference. Also, class members are accessible and modifiable using
using getField and setField wrappers.

The bindings require native C++ library libudpipe_java (called udpipe_java on
Windows). If the library is found in the current directory, it is used,
otherwise standard library search process is used.


C++ Bindings API
================


Helper Structures
-----------------


Main Classes
------------
