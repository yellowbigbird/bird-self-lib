#pragma once

class CRC32
{
public:
  CRC32();

  virtual	~CRC32();

  inline	void NEW_CRC32();
  inline	u32 GetCRC32( u8 const*const pbuf , u32 buflen );

protected:
  u32	m_CRC32_seed;
}; 

u32   getCrc32(u8 const*const pbuf , u32 buflen);
u32   getCrc32FromFile(const TCHAR* filePathName);
bool  compareFile(const TCHAR* filePathName0, const TCHAR* filePathName1);