
#include <stdint.h>
#include "zstd.h"
#include "decompress/zstd_decompress_internal.h"

#if defined(_WIN32)
#define ZSTD_EXPORT __declspec(dllexport)
#else
#define ZSTD_EXPORT extern 
#endif

/////////////////////////////////////
// forward declaration
/////////////////////////////////////

typedef struct
{
  ZSTD_DCtx *dctx;
  ZSTD_DDict *ddict;
  ZSTD_DStream *dstream;
  size_t totalReadSize;
} DecompressContext;

typedef size_t (*decompressStreamFunc_t)(DecompressContext *const context, ZSTD_outBuffer *const output, ZSTD_inBuffer *const input);

size_t decompressStreamWithoutDictionary(DecompressContext *const context, ZSTD_outBuffer *const output, ZSTD_inBuffer *const input);
size_t decompressStreamWithDictionary(DecompressContext *const context, ZSTD_outBuffer *const output, ZSTD_inBuffer *const input);
int32_t decompressStreamImpl( decompressStreamFunc_t func, DecompressContext *const context,uint8_t* dst, int32_t dstSize, const uint8_t *const src, int32_t srcSize);

/////////////////////////////////////
// Simple Decompress
/////////////////////////////////////

// decompress zstd data
// return 1; decompress failed.
ZSTD_EXPORT int32_t decompress(uint8_t *dst, int32_t dstSize, const uint8_t *src, int32_t srcSize)
{
  size_t result = ZSTD_decompress(dst, dstSize, src, srcSize);
  return ZSTD_isError(result);
}

/////////////////////////////////////
// Streaming Decompress
/////////////////////////////////////
// initialize DecompressContext common.
// return 1; initialize error.
ZSTD_EXPORT int32_t initializeDecompressContext(DecompressContext *const context)
{
  context->dctx = ZSTD_createDCtx();
  context->ddict = NULL;
  context->dstream = NULL;
  context->totalReadSize = 0;

  return (context->dctx != NULL) ? 0 : 1;
}

// initialize DecompressContext with dictionary.
// return 1; initialize error.
ZSTD_EXPORT int32_t initializeDecompressContextDictionary(DecompressContext *const context, const uint8_t *const dict, int32_t dictSize)
{
  if(initializeDecompressContext(context))
  {
    return 1;
  }
  context->ddict = ZSTD_createDDict(dict, dictSize);
  return (context->ddict != NULL) ? 0 : 1;
}

// initialize DecompressContext with stream and dictionary..
// return 1; initialize error.
ZSTD_EXPORT int32_t initializeDecompressContextStreamDictionary(DecompressContext *const context, const uint8_t *const dict, int32_t dictSize)
{
  if(initializeDecompressContextDictionary(context, dict, dictSize))
  {
    return 1;
  }
  context->dstream = ZSTD_createDStream();
  if(context->dstream == NULL)
  {
    return 1;
  }
  size_t const initError = ZSTD_initDStream_usingDDict(context->dstream, context->ddict);
  return ZSTD_isError(initError);
}

// finalize decompress stream context
ZSTD_EXPORT void finalizeDecompressContext(DecompressContext *const context)
{
  if(context == NULL) return;
  if(context->dctx != NULL)
  {
    ZSTD_freeDCtx(context->dctx);
    context->dctx = NULL;
  }
  if(context->ddict != NULL)
  {
    ZSTD_freeDDict(context->ddict);
    context->ddict = NULL;
  }
  if(context->dstream != NULL)
  {
    ZSTD_freeDStream(context->dstream);
    context->dstream = NULL;
  }
}

// streaming decompress
// return -1, cause error
// return >=0, decompressSize
ZSTD_EXPORT int32_t decompressStream(
  DecompressContext *const context,
  uint8_t* dst,
  int32_t dstSize,
  const uint8_t *const src,
  int32_t srcSize)
{
  return decompressStreamImpl( decompressStreamWithoutDictionary, context, dst, dstSize, src, srcSize);
}

// streaming decompress (with dictionary)
// return -1, cause error
// return >=0, decompressSize
ZSTD_EXPORT int32_t decompressStreamDictionary(
    DecompressContext *const context,
    uint8_t *dst,
    int32_t dstSize,
    const uint8_t *const src,
    int32_t srcSize)
{
  return decompressStreamImpl(decompressStreamWithDictionary, context, dst, dstSize, src, srcSize);
}


size_t decompressStreamWithoutDictionary(DecompressContext *const context, ZSTD_outBuffer *const output, ZSTD_inBuffer *const input)
{
  return ZSTD_decompressStream(context->dctx, output, input);
}

size_t decompressStreamWithDictionary(DecompressContext *const context, ZSTD_outBuffer *const output, ZSTD_inBuffer *const input)
{
  return ZSTD_decompressStream(context->dstream, output, input);
}

int32_t decompressStreamImpl(
  decompressStreamFunc_t func,
  DecompressContext *const context,
  uint8_t* dst,
  int32_t dstSize,
  const uint8_t *const src,
  int32_t srcSize)
{
  const size_t maxReadSize = ZSTD_DStreamInSize();

  const uint8_t* const readPtr = src + context->totalReadSize;
  size_t readSize = srcSize - context->totalReadSize;
  if( readSize > maxReadSize ) { readSize = maxReadSize; }

  ZSTD_inBuffer input = {readPtr, readSize, 0};

  size_t lastRet;
  int32_t decompressSize = 0;
  while(input.pos < input.size)
  {
    uint8_t* const writePtr = dst + decompressSize;
    ZSTD_outBuffer output = {writePtr, dstSize, 0};

    // when return code is zero, the frame is complete
    size_t const ret = func(context, &output, &input);
    if(ZSTD_isError(ret)) { return -1; }
    
    lastRet = ret;
    decompressSize += (int32_t)output.pos;
    dstSize -= (int32_t)output.pos;
    if(dstSize < 0) { return -1; }
    if(dstSize == 0) { lastRet = 0; break; } //dst buffer is full. 
  }

  if(lastRet != 0) { return -1; }

  context->totalReadSize += input.pos;

  return decompressSize;
}

/////////////////////////////////////
// Decompress with Dictionary
/////////////////////////////////////

// decompress with dictionary.
// return 1; decompress failed.
ZSTD_EXPORT int32_t decompressDictionary(
  DecompressContext *const context,
  uint8_t *const dst,
  int32_t dstSize,
  const uint8_t *const src,
  int32_t srcSize)
{
  size_t result = ZSTD_decompress_usingDDict( context->dctx, dst, dstSize, src, srcSize, context->ddict);
  return ZSTD_isError(result);
}

