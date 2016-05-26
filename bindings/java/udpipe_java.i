%module udpipe_java

%include "../common/udpipe.i"

%pragma(java) jniclasscode=%{
  static {
    java.io.File localUDPipe = new java.io.File(System.mapLibraryName("udpipe_java"));

    if (localUDPipe.exists())
      System.load(localUDPipe.getAbsolutePath());
    else
      System.loadLibrary("udpipe_java");
  }
%}
