%module udpipe_java

/* Implement DISOWN */
%typemap(javacode) input_format %{
  protected static long getCPtrAndDisown($javaclassname obj) {
    if (obj != null) obj.swigCMemOwn = false;
    return getCPtr(obj);
  }
%}
%typemap(javain) SWIGTYPE* DISOWN "$javaclassname.getCPtrAndDisown($javainput)"

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
