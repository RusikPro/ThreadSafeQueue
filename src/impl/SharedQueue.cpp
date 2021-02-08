#include "impl/SharedQueue.h"

#include <chrono>
#include <iostream>

/*----------------------------------------------------------------------------*/

template < typename _T >
struct SharedQueue< _T >::Node
{
    _T * data;
    std::unique_ptr<Node> next;
};

/*----------------------------------------------------------------------------*/

class EndScopeNotifier
{
public:
    EndScopeNotifier ( std::condition_variable & _rCond )
        :   m_rCond( _rCond )
    {
    }

    ~EndScopeNotifier ()
    {
        m_rCond.notify_one();
    }

private:
    std::condition_variable & m_rCond;
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
    std::lock_guard< std::mutex > lck( m_sizeMutex );
    return m_currentQueueSizeLockable;
}

/*----------------------------------------------------------------------------*/

template < typename _T >
void SharedQueue< _T >::enqueue ( _T * _pNewValue )
{
    {
        std::unique_lock< std::mutex > lck( m_sizeMutex );
        if ( m_currentQueueSizeLockable >= m_queueSize )
        {
            m_popDataCond.wait( lck, [ & ] {
                return m_currentQueueSizeLockable < m_queueSize;
            } );
        }
        ++m_currentQueueSizeLockable;

    }

    std::unique_ptr< Node > pNewNode( std::make_unique< Node >() );
    {
        std::lock_guard< decltype( m_tailMutex ) > tailLock( m_tailMutex );
        m_pTail->data = _pNewValue;
        Node * const pNewTail = pNewNode.get();
        m_pTail->next = std::move( pNewNode );
        m_pTail = pNewTail;
    }
    m_headMutex.lock();
    m_pushDataCond.notify_one();
    m_headMutex.unlock();
}

/*----------------------------------------------------------------------------*/

template < typename _T >
bool SharedQueue< _T >::enqueue ( _T * _pNewValue, int _millisecondsTimeout )
{
    using namespace std::chrono;

    {
        std::unique_lock< std::mutex > lck( m_sizeMutex );
        bool notFull = m_popDataCond.wait_for(
                lck
            ,   milliseconds( _millisecondsTimeout )
            ,   [ this ]
            {
                return m_currentQueueSizeLockable < m_queueSize;
            }
        );
        if ( !notFull )
            return false;

        ++m_currentQueueSizeLockable;
    }
    EndScopeNotifier notifier( m_pushDataCond );

    std::unique_ptr< Node > pNewNode( std::make_unique< Node >() );
    {
        std::lock_guard< decltype( m_tailMutex ) > tailLock( m_tailMutex );
        m_pTail->data = _pNewValue;
        Node * const pNewTail = pNewNode.get();
        m_pTail->next = std::move( pNewNode );
        m_pTail = pNewTail;
    }
    return true;
}

/*----------------------------------------------------------------------------*/

template < typename _T >
_T * SharedQueue< _T >::dequeue ()
{
    EndScopeNotifier notifier( m_popDataCond );

    std::unique_lock< std::mutex > headLock( m_headMutex );
    m_pushDataCond.wait( headLock, [ this ] {
        return m_pHead.get() != get_tail();
    } );

    {
        std::lock_guard< std::mutex > lck( m_sizeMutex );
        --m_currentQueueSizeLockable;
    }

    auto pOldHead = std::move( m_pHead );
    m_pHead = std::move( pOldHead->next );
    return pOldHead->data;
}

/*----------------------------------------------------------------------------*/

template < typename _T >
_T * SharedQueue< _T >::dequeue ( int _millisecondsTimeout )
{
    EndScopeNotifier notifier( m_popDataCond );

    using namespace std::chrono;

    bool popped;

    std::unique_lock< std::mutex > headLock( m_headMutex );
    popped = m_pushDataCond.wait_for(
            headLock
        ,   milliseconds( _millisecondsTimeout )
        ,   [ this ]
        {
            return m_pHead.get() != get_tail();
        }
    );

    if ( !popped )
        return nullptr;

    {
        std::lock_guard< std::mutex > lck( m_sizeMutex );
        --m_currentQueueSizeLockable;
    }

    auto pOldHead = std::move( m_pHead );
    m_pHead = std::move( pOldHead->next );
    return pOldHead->data;
}

/*----------------------------------------------------------------------------*/

template < typename _T >
typename SharedQueue< _T >::Node * SharedQueue< _T >::get_tail ()
{
    std::lock_guard< std::mutex > tailLock( m_tailMutex );
    return m_pTail;
}

/*----------------------------------------------------------------------------*/
