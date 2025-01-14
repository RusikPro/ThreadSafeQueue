#ifndef __SHAREDQUEUE_SRC_IQUEUE_H__
#define __SHAREDQUEUE_SRC_IQUEUE_H__

/*----------------------------------------------------------------------------*/

/**
 * @class IQueue
 *
 * @brief An interface for a thread-safe queue of pointers to T.
 *
 * Please note: The queue DOES NOT own the pointers to T. It is the caller's
 * responsibility to ensure memory is allocated before enqueueing and freed
 * after dequeueing (or when no longer needed).
 */
template < typename _T >
class IQueue
{

public:

    virtual ~IQueue() = 0;

    virtual int count () const = 0;

    virtual void enqueue ( _T * _pNewValue ) = 0;

    virtual bool enqueue ( _T * _pNewValue, int _millisecondsTimeout ) = 0;

    virtual _T * dequeue () = 0;

    virtual _T * dequeue ( int _millisecondsTimeout ) = 0;
};

/*----------------------------------------------------------------------------*/

template < typename _T >
IQueue< _T >::~IQueue() {}

/*----------------------------------------------------------------------------*/

#endif // __SHAREDQUEUE_SRC_IQUEUE_H__
