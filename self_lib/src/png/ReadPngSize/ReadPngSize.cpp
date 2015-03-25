#include <stdafx.h>
#include "png.h"
#include "ReadPngSize.h"


using namespace std;

//////////////////////////////////////////////////////////////////////////

static png_structp png_ptr = NULL;
static png_infop			info_ptr = NULL;
//////////////////////////////////////////////////////////////////////////
static void png_cexcept_error(png_structp png_ptr, png_const_charp msg);
static void pngtest_read_data(png_structp png_ptr, png_bytep data, png_size_t length);
//////////////////////////////////////////////////////////////////////////


static void png_read_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
  /* fread() returns 0 on error, so it is OK to store this in a png_size_t
  * instead of an int, which is what fread() actually returns.
  */
  //png_size_t check;
  //READFILE((png_FILE_p)png_ptr->io_ptr, data, length, check);
  CFileInMem* pFileMeme = (CFileInMem*)(png_ptr->io_ptr);
  assert(pFileMeme);
  if(pFileMeme){
    u32 readBytesOk = 0;
    const bool ifReadOk = pFileMeme->ReadBytes(data, (u32)length, readBytesOk);
    if(ifReadOk)
      return;
  }  
  
  //png_error(png_ptr, "Read Error!");
  assert(false);
  cout<<"png_read_data , Read Error!"<<endl;
}

//TCHAR* pstrPathName
BOOL LoadImageFile (CFileInMem& fileInMem,int *pxImgSize, int *pyImgSize)
{
  //FILE*               pfFile;
  png_byte            pbSig[8];
  int                 iBitDepth;
  int                 iColorType;

  // open the PNG input file

  //if (!(pfFile = _tfopen(pstrPathName, _T("rb"))))
  //{
  //  return FALSE;
  //}
  if(fileInMem.GetFileSize() <1){
    cout<<"fileInMem.GetFileSize() <1;"<<endl;
    return FALSE;
  }

  // first check the eight byte PNG signature

  //fread(pbSig, 1, 8, pfFile);
  u32 readedBytes = 0;
  const bool ifReadSigOk = fileInMem.ReadBytes(pbSig, 8, readedBytes);
  assert(ifReadSigOk && 8 == readedBytes);

  if (!ifReadSigOk
    || readedBytes != 8
    || !png_check_sig(pbSig, 8))
  {
    return FALSE;
  }

  // create the two png(-info) structures

  png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL,
    (png_error_ptr)png_cexcept_error, (png_error_ptr)NULL);
  if (!png_ptr)
  {
    return FALSE;
  }

  info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr)
  {
    png_destroy_read_struct(&png_ptr, NULL, NULL);
    return FALSE;
  }

  try
  {

    // initialize the png structure

    //#if !defined(PNG_NO_STDIO)
    //png_init_io(png_ptr, pfFile);
    //#else
    //(png_voidp)pfFile
    png_set_read_fn(png_ptr, (png_voidp)(&fileInMem), png_read_data);		 
    //#endif

    png_set_sig_bytes(png_ptr, 8);

    // read all PNG info up to image data
    png_read_info(png_ptr, info_ptr);

    // get width, height, bit-depth and color-type

    png_uint_32 width;
    png_uint_32 height;

    png_get_IHDR(png_ptr, info_ptr, &width, &height, &iBitDepth,
      &iColorType, NULL, NULL, NULL);

    *pxImgSize = width;
    *pyImgSize = height;
  }
  catch(...)
  {

  }

  //fclose(pfFile);


  return TRUE;
}

static void png_cexcept_error(png_structp png_ptr, png_const_charp msg)
{
  //if(png_ptr)
  //	;
#ifndef PNG_NO_CONSOLE_IO
  fprintf(stderr, "libpng error: %s\n", msg);
#endif
}
