using System.Collections;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;

namespace Uzstd.Test
{
    public class ZstdLoadTest : MonoBehaviour
    {
        // Use this for initialization
        void Start()
        {
            var before   = Resources.Load<TextAsset>("sample").bytes; //圧縮前データ
            var deflated = Resources.Load<TextAsset>("sample_deflated").bytes; //圧縮後データ

            Debug.Log(string.Format("deflate before {0}byte after {1}byte", before.Length, deflated.Length));


            //simple decompress
            {
                var buffer = new byte[before.Length];
                var result = API.decompress(buffer, before.Length, deflated, deflated.Length);
                Debug.Log(string.Format("decompress result {0}", result));
                CompareBuffer(before, buffer, "simple decompress buffer check");
            }

            //stream decompress
            using (var decompressContext = DecompressContext.Create())
            {
                var buffer = new byte[before.Length];
                var result = API.decompressStream(decompressContext, buffer, 0, buffer.Length, deflated, deflated.Length);
                Debug.Log(string.Format("streaming decompress result {0}", result));
                CompareBuffer(before, buffer, "streaming decompress buffer check");
            }

            //stream decompress using work buffer
            using (var decompressContext = DecompressContext.Create())
            {
                var buffer = new byte[before.Length];
                int totalDecompressed = 0;
                var work = new byte[buffer.Length / 4];
                do
                {
                    var result = API.decompressStream(decompressContext, work, 0, work.Length, deflated, deflated.Length);
                    if (result == 0) { break; } // success decompress all.
                    if (result < 0) { throw new System.Exception("decompress failed"); }

                    System.Array.Copy(work, 0, buffer, totalDecompressed, result);
                    totalDecompressed += result;

                } while (totalDecompressed < buffer.Length);
                CompareBuffer(before, buffer, "streaming decompress2 buffer check");
            }
        }

        private void CompareBuffer(byte[] first, byte[] second, string msg)
        {
            var compare = Enumerable.SequenceEqual(first, second);
            Debug.Log(string.Format("{0}. {1}", msg, compare));
        }
    }
}
