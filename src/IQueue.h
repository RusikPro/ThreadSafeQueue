#ifndef __SHAREDQUEUE_SRC_IQUEUE_H__
#define __SHAREDQUEUE_SRC_IQUEUE_H__

/*----------------------------------------------------------------------------*/

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