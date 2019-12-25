
#include "zstd.h"
#include "decompress/zstd_decompress_internal.h"

/////////////////////////////////////
// Simple Decompress
/////////////////////////////////////

// decompress zstd data
// return 1 if error decompress src.
extern int32_t decompress(uint8_t* dst, int32_t dstSize, const uint8_t* src, int32_t srcSize)
{
  size_t result = ZSTD_decompress(dst, dstSize, src, srcSize);
  return ZSTD_isError(result);
}

/////////////////////////////////////
// Streaming Decompress
/////////////////////////////////////

typedef struct {
    ZSTD_DCtx* dctx;
    size_t totalReadSize;
} DecompressContext;

// initialize decompress stream context which has created in C#.
extern void initializeDecompressContext(DecompressContext *const context)
{
  context->dctx = ZSTD_createDCtx();
  if(context->dctx == NULL) { return; }
  context->totalReadSize = 0;
}

// finalize decompress stream context
extern void finalizeDecompressContext(DecompressContext *const context)
{
  if(context == NULL) return;
  if(context->dctx != NULL)
  {
    ZSTD_freeDCtx(context->dctx);
    context->dctx = NULL;
  }
}

// streaming decompress
// return -1, cause error
// return >=0, decompressSize
extern int32_t decompressStream(
  DecompressContext *const context,
  uint8_t* dst,
  int32_t dstOffset,
  int32_t dstSize,
  const uint8_t *const src,
  int32_t srcSize)
{
  const size_t maxReadSize = ZSTD_DStreamInSize();

  const uint8_t* const readPtr = src + context->totalReadSize;
  size_t readSize = srcSize - context->totalReadSize;
  if( readSize > maxReadSize ) { readSize = maxReadSize; }

  ZSTD_inBuffer input = {readPtr, readSize, 0};

  int32_t lastRet;
  int32_t decompressSize = 0;
  while(input.pos < input.size)
  {
    uint8_t* const writePtr = dst + dstOffset + decompressSize;
    ZSTD_outBuffer output = {writePtr, dstSize, 0};

    // when return code is zero, the frame is complete
    size_t const ret = ZSTD_decompressStream(context->dctx, &output, &input);
    if(ZSTD_isError(ret)) { return -1; }
    
    lastRet = ret;
    decompressSize += output.pos;
    dstSize -= output.pos;
    if(dstSize < 0) { return -1; }
    if(dstSize == 0) { lastRet = 0; break; } //dst buffer is full. 
  }

  if(lastRet != 0) { return -1; }

  context->totalReadSize += input.pos;

  return decompressSize;
}


