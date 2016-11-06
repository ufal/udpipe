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
    java.io.File libraryFile = new java.io.File(udpipe_java.libraryPath);

    if (libraryFile.exists())
      System.load(libraryFile.getAbsolutePath());
    else
      System.loadLibrary("udpipe_java");
  }
%}

%pragma(java) modulecode=%{
  static String libraryPath;

  static {
    libraryPath = System.mapLibraryName("udpipe_java");
  }

  public static void setLibraryPath(String path) {
    libraryPath = path;
  }
%}
