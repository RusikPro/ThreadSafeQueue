#ifndef __SHAREDQUEUE_TEST_UTILITIES_H__
#define __SHAREDQUEUE_TEST_UTILITIES_H__

/*----------------------------------------------------------------------------*/

#include <boost/date_time/posix_time/posix_time.hpp>

#include <mutex>
#include <string>
#include <thread>

/*----------------------------------------------------------------------------*/

inline std::string now_str()
{
    const boost::posix_time::ptime now =
        boost::posix_time::microsec_clock::local_time();

    const boost::posix_time::time_duration td = now.time_of_day();
    const long hours        = td.hours();
    const long minutes      = td.minutes();
    const long seconds      = td.seconds();
    const long milliseconds = td.total_milliseconds() -
                              ((hours * 3600 + minutes * 60 + seconds) * 1000);
    char buf[40];
    sprintf(buf, "%02ld:%02ld:%02ld.%03ld",
        hours, minutes, seconds, milliseconds);

    return buf;
}

/*----------------------------------------------------------------------------*/

std::recursive_mutex g_coutResource;

inline void thread_safe_cout_ ()
{
    std::cout << std::endl;
}

template<typename _FirstT, typename ..._RestT >
inline void thread_safe_cout_ ( _FirstT && _first, _RestT && ..._rest )
{
    std::cout << std::forward< _FirstT >( _first ) << " ";
    thread_safe_cout_( std::forward< _RestT >( _rest )... );
}

template<typename _FirstT, typename ..._RestT >
inline void thread_safe_cout ( _FirstT && _first, _RestT && ..._rest )
{
    std::unique_lock< std::recursive_mutex > lock( g_coutResource );

    std::cout << now_str() << " [" << std::this_thread::get_id() << "] ";
    thread_safe_cout_(
        std::forward< _FirstT >( _first ), std::forward< _RestT >( _rest )...
    );
}

/*----------------------------------------------------------------------------*/

inline void sleep_for ( std::size_t _milliseconds )
{
    std::this_thread::sleep_for( std::chrono::milliseconds( _milliseconds ) );
    thread_safe_cout( _milliseconds, "milliseconds passed" );
}

/*----------------------------------------------------------------------------*/

#endif // __SHAREDQUEUE_TEST_UTILITIES_H__