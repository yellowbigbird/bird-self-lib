/* 7zFile.h -- File IO
2009-11-24 : Igor Pavlov : Public domain */

#ifndef __7Z_FILE_H
#define __7Z_FILE_H

#ifdef _WIN32
#define USE_WINDOWS_FILE
#endif

#ifdef USE_WINDOWS_FILE
#include <windows.h>
#else
#include <stdio.h>
#endif

//bird
#include "lzma_dll.h"

#include "Types.h"

EXTERN_C_BEGIN

/* ---------- File ---------- */

typedef struct
{
  #ifdef USE_WINDOWS_FILE
  HANDLE handle;
  #else
  FILE *file;
  #endif
} CSzFile;

LZMA_DLL void File_Construct(CSzFile *p);

#if !defined(UNDER_CE) || !defined(USE_WINDOWS_FILE)
LZMA_DLL WRes InFile_Open(CSzFile *p, const char *name);
LZMA_DLL WRes OutFile_Open(CSzFile *p, const char *name);
#endif

#ifdef USE_WINDOWS_FILE
LZMA_DLL WRes InFile_OpenW(CSzFile *p, const WCHAR *name);
LZMA_DLL WRes OutFile_OpenW(CSzFile *p, const WCHAR *name);
#endif

LZMA_DLL WRes File_Close(CSzFile *p);

/* reads max(*size, remain file's size) bytes */
LZMA_DLL WRes File_Read(CSzFile *p, void *data, size_t *size);

/* writes *size bytes */
LZMA_DLL WRes File_Write(CSzFile *p, const void *data, size_t *size);

LZMA_DLL WRes File_Seek(CSzFile *p, Int64 *pos, ESzSeek origin);
LZMA_DLL WRes File_GetLength(CSzFile *p, UInt64 *length);


/* ---------- FileInStream ---------- */

typedef struct
{
  ISeqInStream s;
  CSzFile file;
} CFileSeqInStream;

LZMA_DLL void FileSeqInStream_CreateVTable(CFileSeqInStream *p);


typedef struct
{
  ISeekInStream s;
  CSzFile file;
} CFileInStream;

void FileInStream_CreateVTable(CFileInStream *p);


typedef struct
{
  ISeqOutStream s;
  CSzFile file;
} CFileOutStream;

LZMA_DLL void FileOutStream_CreateVTable(CFileOutStream *p);

EXTERN_C_END

#endif
