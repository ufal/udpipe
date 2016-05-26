%module udpipe_csharp

%include "../common/udpipe_stl.i"

namespace std {

%naturalvar string;

class string;

%typemap(imtype, inattributes="[global::System.Runtime.InteropServices.MarshalAs(global::System.Runtime.InteropServices.UnmanagedType.CustomMarshaler, MarshalTypeRef = typeof(UTF8Marshaler))]", outattributes="[return: global::System.Runtime.InteropServices.MarshalAs(global::System.Runtime.InteropServices.UnmanagedType.CustomMarshaler, MarshalTypeRef = typeof(UTF8Marshaler))]") char *, char *&, char[ANY], char[]   "string"
%typemap(imtype, inattributes="[global::System.Runtime.InteropServices.MarshalAs(global::System.Runtime.InteropServices.UnmanagedType.CustomMarshaler, MarshalTypeRef = typeof(UTF8Marshaler))]", outattributes="[return: global::System.Runtime.InteropServices.MarshalAs(global::System.Runtime.InteropServices.UnmanagedType.CustomMarshaler, MarshalTypeRef = typeof(UTF8Marshaler))]") string "string"
%typemap(imtype, inattributes="[global::System.Runtime.InteropServices.MarshalAs(global::System.Runtime.InteropServices.UnmanagedType.CustomMarshaler, MarshalTypeRef = typeof(UTF8Marshaler))]", outattributes="[return: global::System.Runtime.InteropServices.MarshalAs(global::System.Runtime.InteropServices.UnmanagedType.CustomMarshaler, MarshalTypeRef = typeof(UTF8Marshaler))]") const string & "string"

}

%include "../common/udpipe.i"

%pragma(csharp) imclasscode=%{
public class UTF8Marshaler : System.Runtime.InteropServices.ICustomMarshaler {
    static UTF8Marshaler static_instance;

    public System.IntPtr MarshalManagedToNative(object managedObj) {
        if (managedObj == null)
            return System.IntPtr.Zero;
        if (!(managedObj is string))
            throw new System.Runtime.InteropServices.MarshalDirectiveException("UTF8Marshaler must be used on a string.");

        // not null terminated
        byte[] strbuf = System.Text.Encoding.UTF8.GetBytes((string)managedObj);
        System.IntPtr buffer = System.Runtime.InteropServices.Marshal.AllocHGlobal(strbuf.Length + 1);
        System.Runtime.InteropServices.Marshal.Copy(strbuf, 0, buffer, strbuf.Length);

        // write the terminating null
        System.Runtime.InteropServices.Marshal.WriteByte(buffer + strbuf.Length, 0);
        return buffer;
    }

    public object MarshalNativeToManaged(System.IntPtr pNativeData) {
        int length = 0;
        while (System.Runtime.InteropServices.Marshal.ReadByte(pNativeData, length) != 0)
            length++;

        // should not be null terminated
        byte[] strbuf = new byte[length];
        // skip the trailing null
        System.Runtime.InteropServices.Marshal.Copy((System.IntPtr)pNativeData, strbuf, 0, length);
        string data = System.Text.Encoding.UTF8.GetString(strbuf);
        return data;
    }

    public void CleanUpNativeData(System.IntPtr pNativeData) {
        System.Runtime.InteropServices.Marshal.FreeHGlobal(pNativeData);
    }

    public void CleanUpManagedData(object managedObj) {
    }

    public int GetNativeDataSize() {
        return -1;
    }

    public static System.Runtime.InteropServices.ICustomMarshaler GetInstance(string cookie) {
        if (static_instance == null)
            return static_instance = new UTF8Marshaler();
        return static_instance;
    }
}
%}
