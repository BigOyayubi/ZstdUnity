using System;
using System.Runtime.InteropServices;

namespace Uzstd
{
    public static class API
    {
#if (UNITY_IPHONE || UNITY_WEBGL || UNITY_SWITCH) && !UNITY_EDITOR
        const string ZSTDDLL = "__Internal";
#else
        const string ZSTDDLL = "zstd";
#endif

        /// <summary>
        /// decompress zstd
        /// return 1 is error. 0 is success.
        /// </summary>
        /// <example>
        /// byte[] src;
        /// byte[] dst;
        /// var result = Uzstd.decompress(dst, dst.Length, src, src.Length);
        /// </example>
        [DllImport(ZSTDDLL, CallingConvention = CallingConvention.Cdecl)]
        public static extern int decompress([Out] byte[] dst, int dstSize, [In] byte[] src, int srcSize);

        /// <summary>
        /// streaming decompress zstd
        /// return > 0. decompressed size
        /// return = 0. done decompress.
        /// return < 0. error.
        /// </summary>
        /// <example>
        /// using(var context = DecompressContext.Create())
        /// {
        ///   byte[] compressed;
        ///   byte[] decompressed;
        ///   byte[] work; //it can be smaller than decompressed.
        ///   int totalDecompressedSize = 0;
        ///   do
        ///   {
        ///      var decompressedSize = decompressStream(context, work, 0, work.Length, compressed, compressed.Length );
        ///      if( decompressedSize == 0 ) { break; } //success to read.
        ///      if( decompressedSize < 0 ) { throw new System.Exception("..."); }
        ///      System.Array.Copy( work, 0, decompressed, totalDecompressedSize, decompressedSize );
        ///      totalDecompressedSize += decompressedSize;
        ///   } while (totalDecompressedSize < decompressed.Length);
        /// }
        /// </example>
        [DllImport(ZSTDDLL, CallingConvention = CallingConvention.Cdecl)]
        public static extern int decompressStream([In][Out] DecompressContext context, [Out] byte[] dst, int dstOffset, int dstSize, [In] byte[] src, int srcSize);
    }

    /// <summary>
    /// streaming decompress informations.
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public class DecompressContext : IDisposable
    {
#if (UNITY_IPHONE || UNITY_WEBGL || UNITY_SWITCH) && !UNITY_EDITOR
        const string ZSTDDLL = "__Internal";
#else
        const string ZSTDDLL = "zstd";
#endif

        public static DecompressContext Create()
        {
            return new DecompressContext();
        }

        public IntPtr dctx;
        public int    totalReadSize;

        public void Dispose()
        {
            finalizeDecompressContext(this);
        }

        DecompressContext()
        {
            this.dctx = default(IntPtr);
            this.totalReadSize = 0;
            initializeDecompressContext(this);
        }

        [DllImport(ZSTDDLL, CallingConvention = CallingConvention.Cdecl)]
        public static extern void initializeDecompressContext([In][Out] DecompressContext context);

        [DllImport(ZSTDDLL, CallingConvention = CallingConvention.Cdecl)]
        public static extern void finalizeDecompressContext([In][Out] DecompressContext context);
    }

}
