#ifndef __SHAREDQUEUE_SRC_IMPL_QUEUEIMPL_H__
#define __SHAREDQUEUE_SRC_IMPL_QUEUEIMPL_H__

#include <memory>

/*----------------------------------------------------------------------------*/

struct IData
{
    virtual void * get () = 0;

    virtual ~IData () = default;
};

struct SharedQueueImpl
{
    explicit SharedQueueImpl ( std::size_t _size );

    ~SharedQueueImpl ();

    int count () const noexcept;

    void enqueue ( IData * _pNewValue );

    bool enqueue ( IData * _pNewValue, int _millisecondsTimeout );

    IData * dequeue ();

    IData * dequeue ( int _millisecondsTimeout );

private:

    struct Node;

    Node * get_tail ();

private:

    struct ImplData;
    std::unique_ptr< ImplData > pImplData;
};

/*----------------------------------------------------------------------------*/

#endif // __SHAREDQUEUE_SRC_IMPL_QUEUEIMPL_H__
