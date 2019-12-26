# ZstdUnity
simple zstandard decompress binding for unity

# QuickStart

* make plugins
    * `$ sh make_osx.sh`
* copy plugins
    * `$ cp -ap plugin/Plugins your/unity/project/Assets/`
* copy C# bindings
    * `$ cp -ap Assets/Uzstd your/unity/project/Assets/`

```csharp

//simple pattern
byte[] compressed;
byte[] dst = new byte[decompressedSize];
Uzstd.API.decompress(dst, dst.Length, compressed, compressed.Length);

//stream
using(var context = DecompressContext.Create())
{
  byte[] work;
  int totalDecompressed = 0;
  do
  {
    var result = Uzstd.API.decompressStream(context, work, work.Length, compressed, compressed.Length);
    if( result == 0 ) {break;} //decompress done.
    if( result <  0 ) {throw new System.Exception("decompress error"); }
    System.Array.Copy(work, 0, dst, totalDecompressed, result);
    totalDecompressed += result;
  }while(totalDecompressed < dst.Length);
}

//with dictionary
using(var context = DecompressContext.CreateWithDictionary(dict))
{
  Uzstd.API.decompressDictionary(context, dst, dst.Length, compressed, compressed.Length); 
}

//stream with dictionary
using(var context = DecompressContext.CreateWithStreamDictionary(dit))
{
  byte[] work;
  int totalDecompressed = 0;
  do
  {
    var result = Uzstd.API.decompressStream(context, work, work.Length, compressed, compressed.Length);
    if( result == 0 ) {break;} //decompress done.
    if( result <  0 ) {throw new System.Exception("decompress error"); }
    System.Array.Copy(work, 0, dst, totalDecompressed, result);
    totalDecompressed += result;
  }while(totalDecompressed < dst.Length);
}
```

# TODO

* test plugins android/ios/linux.

