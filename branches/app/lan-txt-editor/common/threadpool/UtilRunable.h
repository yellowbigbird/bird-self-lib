#ifndef _RUNABLE_THREAD_POOL_
#define _RUNABLE_THREAD_POOL_

#pragma once

namespace UtilThreadPool
{
class CThreadPool;
class IRunable
{
    friend class CThreadPool;
public:
    virtual void run()  = 0;

    bool autoDelete() { return bAutoDelete; }
    void setAutoDelete(bool b = true) { bAutoDelete = b; }

protected:
    IRunable() : bAutoDelete(true) {}

protected:
    CThreadPool * threadPool;

private:
    bool bAutoDelete;
    
};

} // end namespace UtilThreadPool


#endif // end of _RUNABLE_THREAD_POOL_
