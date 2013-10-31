#pragma once

#include <ossxsd.h>
struct ossxEnv;
struct ossxMemoryHandle;
//struct OssxBufExtended;

class COssxWrapper
{
public:
    COssxWrapper();
    virtual ~COssxWrapper();

    bool    Init();

protected:
    ossxEnv*            m_pEnv;
    ossxMemoryHandle*	m_hOssMem;
    OssxBufExtended*    m_pBufExtend;
};
