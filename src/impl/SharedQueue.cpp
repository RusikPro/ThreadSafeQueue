#include "impl/SharedQueue.h"

#include <chrono>
#include <iostream>

/*----------------------------------------------------------------------------*/

template < typename _T >
struct SharedQueue< _T >::Node
{
    _T * data;
    std::unique_ptr< Node > next;
};

/*----------------------------------------------------------------------------*/

template < typename _T >
SharedQueue< _T >::SharedQueue ( std::size_t _size )
    :   m_pHead( std::make_unique< Node >() )
    ,   m_pTail( m_pHead.get() )
    ,   m_currentQueueSizeLockable( 0 )
    ,   m_queueSize( _size )
{
}

/*----------------------------------------------------------------------------*/

template < typename _T >
SharedQueue< _T >::~SharedQueue ()
{
}

/*----------------------------------------------------------------------------*/

template < typename _T >
int SharedQueue< _T >::count () const noexcept
{
    return m_currentQueueSizeLockable.load();
}

/*----------------------------------------------------------------------------*/

template < typename _T >
void SharedQueue< _T >::enqueue ( _T * _pNewValue )
{
    std::unique_ptr< Node > pNewNode( std::make_unique< Node >() );

    {
        std::unique_lock< std::mutex > tailLock( m_tailMutex );

        // Wait while the queue is full
        m_notFullCond.wait( tailLock, [ this ] () {
            return m_currentQueueSizeLockable.load() < m_queueSize;
        } );

        ++m_currentQueueSizeLockable;

        // Link new node at tail
        m_pTail->data = _pNewValue;
        Node* const pNewTail = pNewNode.get();
        m_pTail->next = std::move(pNewNode);
        m_pTail = pNewTail;
    }

    // Notify a waiting consumer that queue is not empty
    {
        std::lock_guard< std::mutex > headLock( m_headMutex );
        m_notEmptyCond.notify_one();
    }
}

/*----------------------------------------------------------------------------*/

template < typename _T >
bool SharedQueue< _T >::enqueue ( _T * _pNewValue, int _millisecondsTimeout )
{
    using namespace std::chrono;

    std::unique_ptr< Node > pNewNode = std::make_unique< Node >();

    std::unique_lock< std::mutex > tailLock( m_tailMutex );

    bool notFull = m_notFullCond.wait_for(
        tailLock, milliseconds( _millisecondsTimeout ),
        [ this ] ()
        {
            return m_currentQueueSizeLockable.load() < m_queueSize;
        }
    );

    if ( !notFull )
    {
        return false; // timed out
    }

    ++m_currentQueueSizeLockable; // increment size

    // Link new node at tail
    m_pTail->data = _pNewValue;
    Node * const pNewTail = pNewNode.get();
    m_pTail->next = std::move( pNewNode );
    m_pTail = pNewTail;

    // Notify a waiting consumer that queue is not empty
    {
        std::lock_guard< std::mutex > headLock( m_headMutex );
        m_notEmptyCond.notify_one();
    }

    return true;
}

/*----------------------------------------------------------------------------*/

template < typename _T >
_T * SharedQueue< _T >::dequeue ()
{
    std::unique_ptr< Node > pOldHead;
    _T * pReturnVal = nullptr;

    // Lock m_headMutex for removing from the head
    {
        std::unique_lock< std::mutex > headLock( m_headMutex );

        m_notEmptyCond.wait( headLock, [ this ] () {
            return m_pHead.get() != getTail(); // queue not empty
        } );

        // Remove the head node
        --m_currentQueueSizeLockable;

        pOldHead = std::move(m_pHead);
        m_pHead = std::move(pOldHead->next);
        pReturnVal = pOldHead->data;
    }

    // Signal "not full"
    {
        std::lock_guard< std::mutex > tailLock( m_tailMutex );
        m_notFullCond.notify_one();
    }

    return pReturnVal;
}

/*----------------------------------------------------------------------------*/

template < typename _T >
_T * SharedQueue< _T >::dequeue ( int _millisecondsTimeout )
{
    using namespace std::chrono;

    std::unique_ptr< Node > pOldHead;
    _T* returnVal = nullptr;

    std::unique_lock< std::mutex > headLock( m_headMutex );

    bool notEmpty = m_notEmptyCond.wait_for(
        headLock, milliseconds( _millisecondsTimeout ),
        [ this ] ()
        {
            return m_pHead.get() != getTail(); // queue not empty
        }
    );

    if ( !notEmpty )
    {
        return nullptr; // timed out
    }

    --m_currentQueueSizeLockable;

    pOldHead = std::move( m_pHead );
    m_pHead = std::move( pOldHead->next );
    returnVal = pOldHead->data;

    // Notify "not full" after unlocking the head
    {
        std::lock_guard< std::mutex > tailLock( m_tailMutex );
        m_notFullCond.notify_one();
    }

    return returnVal;
}

/*----------------------------------------------------------------------------*/

template < typename _T >
typename SharedQueue< _T >::Node * SharedQueue< _T >::getTail ()
{
    std::lock_guard< std::mutex > tailLock( m_tailMutex );
    return m_pTail;
}

/*----------------------------------------------------------------------------*/
