#include "impl/QueueImpl.h"

#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>

/*----------------------------------------------------------------------------*/

struct SharedQueueImpl::Node
{
    IData * data;
    std::unique_ptr<Node> next;
};

/*----------------------------------------------------------------------------*/

struct SharedQueueImpl::ImplData
{
    ImplData ( std::size_t _size )
        :   m_pHead( std::make_unique< Node >() )
        ,   m_pTail( m_pHead.get() )
        ,   m_currentQueueSizeLockable( 0 )
        ,   m_queueSize( _size )
    {
    }

    std::mutex m_headMutex;
    std::unique_ptr< Node > m_pHead;
    std::mutex m_tailMutex;
    mutable std::mutex m_sizeMutex;
    Node * m_pTail;
    std::condition_variable m_pushDataCond;
    std::condition_variable m_popDataCond;

    mutable std::size_t m_currentQueueSizeLockable;
    const std::size_t m_queueSize;
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

SharedQueueImpl::SharedQueueImpl ( std::size_t _size )
    :   pImplData( std::make_unique< ImplData >( _size ) )
{
}

/*----------------------------------------------------------------------------*/

SharedQueueImpl::~SharedQueueImpl ()
{
    while ( pImplData->m_pHead.get() != get_tail() )
    {
        auto pOldHead = std::move( pImplData->m_pHead );
        pImplData->m_pHead = std::move( pOldHead->next );
        delete pOldHead->data;
    }
}

/*----------------------------------------------------------------------------*/

int SharedQueueImpl::count () const noexcept
{
    std::lock_guard< std::mutex > lck( pImplData->m_sizeMutex );
    return pImplData->m_currentQueueSizeLockable;
}

/*----------------------------------------------------------------------------*/

void SharedQueueImpl::enqueue ( IData * _pNewValue )
{
    {
        std::unique_lock< std::mutex > lck( pImplData->m_sizeMutex );
        pImplData->m_popDataCond.wait( lck, [ & ] {
            return
                    pImplData->m_currentQueueSizeLockable
                <   pImplData->m_queueSize
            ;
        } );
        ++pImplData->m_currentQueueSizeLockable;
    }

    std::unique_ptr< Node > pNewNode( std::make_unique< Node >() );
    {
        std::lock_guard< decltype( pImplData->m_tailMutex ) > tailLock(
            pImplData->m_tailMutex
        );
        pImplData->m_pTail->data = _pNewValue;
        Node * const pNewTail = pNewNode.get();
        pImplData->m_pTail->next = std::move( pNewNode );
        pImplData->m_pTail = pNewTail;
    }
    pImplData->m_headMutex.lock();
    pImplData->m_pushDataCond.notify_one();
    pImplData->m_headMutex.unlock();
}

/*----------------------------------------------------------------------------*/

bool SharedQueueImpl::enqueue ( IData * _pNewValue, int _millisecondsTimeout )
{
    using namespace std::chrono;

    {
        std::unique_lock< std::mutex > lck( pImplData->m_sizeMutex );
        bool notFull = pImplData->m_popDataCond.wait_for(
                lck
            ,   milliseconds( _millisecondsTimeout )
            ,   [ this ]
            {
                return
                        pImplData->m_currentQueueSizeLockable
                    <   pImplData->m_queueSize
                ;
            }
        );
        if ( !notFull )
            return false;

        ++pImplData->m_currentQueueSizeLockable;
    }
    EndScopeNotifier notifier( pImplData->m_pushDataCond );

    std::unique_ptr< Node > pNewNode( std::make_unique< Node >() );
    {
        std::lock_guard< decltype( pImplData->m_tailMutex ) > tailLock(
            pImplData->m_tailMutex
        );
        pImplData->m_pTail->data = _pNewValue;
        Node * const pNewTail = pNewNode.get();
        pImplData->m_pTail->next = std::move( pNewNode );
        pImplData->m_pTail = pNewTail;
    }
    return true;
}

/*----------------------------------------------------------------------------*/

IData * SharedQueueImpl::dequeue ()
{

    EndScopeNotifier notifier( pImplData->m_popDataCond );

    std::unique_lock< std::mutex > headLock( pImplData->m_headMutex );
    pImplData->m_pushDataCond.wait( headLock, [ this ] {
        return pImplData->m_pHead.get() != get_tail();
    } );

    {
        std::lock_guard< std::mutex > lck( pImplData->m_sizeMutex );
        --pImplData->m_currentQueueSizeLockable;
    }

    auto pOldHead = std::move( pImplData->m_pHead );
    pImplData->m_pHead = std::move( pOldHead->next );
    return pOldHead->data;
}

/*----------------------------------------------------------------------------*/

IData * SharedQueueImpl::dequeue ( int _millisecondsTimeout )
{
    EndScopeNotifier notifier( pImplData->m_popDataCond );

    using namespace std::chrono;

    bool popped;

    std::unique_lock< std::mutex > headLock( pImplData->m_headMutex );
    popped = pImplData->m_pushDataCond.wait_for(
            headLock
        ,   milliseconds( _millisecondsTimeout )
        ,   [ this ]
        {
            return pImplData->m_pHead.get() != get_tail();
        }
    );

    if ( !popped )
        return nullptr;

    {
        std::lock_guard< std::mutex > lck( pImplData->m_sizeMutex );
        --pImplData->m_currentQueueSizeLockable;
    }

    auto pOldHead = std::move( pImplData->m_pHead );
    pImplData->m_pHead = std::move( pOldHead->next );
    return pOldHead->data;
}

/*----------------------------------------------------------------------------*/

SharedQueueImpl::Node * SharedQueueImpl::get_tail ()
{
    std::lock_guard< std::mutex > tailLock( pImplData->m_tailMutex );
    return pImplData->m_pTail;
}

/*----------------------------------------------------------------------------*/
