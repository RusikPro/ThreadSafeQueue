# ThreadSafeQueue

A C++ implementation of a thread-safe queue that supports multiple producer and consumer threads, designed for efficient and synchronized data sharing between threads. This project includes core functionality, factory creation methods, and comprehensive testing, including stress and performance benchmarks.

---

## Requirements

- **C++ Standard**: Requires C++17 or higher.
- **Boost Libraries**:
  - `system`
  - `filesystem`
  - `unit_test_framework`
- **CMake**: Version 2.8.7 or higher.
- **POSIX Threads**: Links with `-lpthread` for multi-threaded support.

---

## Features

- **Thread-Safe Queue**: Implements a thread-safe queue using `std::mutex` and `std::condition_variable` to ensure proper synchronization.
- **Multiple Implementations**: Includes a base interface (`IQueue`) and two implementations:
  - Standard Shared Queue
  - Shared Queue using PImpl idiom
- **Factory Design Pattern**: Simplifies queue creation through a `QueueFactory`.
- **Testing**: Comprehensive tests for correctness, performance, and stress scenarios using the Boost.Test framework.

---

## Project Structure

The project is modular, with components organized as follows:

```plaintext
ThreadSafeQueue/
├── src/                                # Source files
│   ├── impl/                           # Implementation files
│   │   ├── QueueImpl.cpp               # Implementation of QueueImpl
│   │   ├── QueueImpl.h                 # Header for QueueImpl (using PImple idion)
│   │   ├── SharedQueue.cpp             # Implementation of SharedQueue
│   │   ├── SharedQueue.h               # Header for SharedQueue
│   │   ├── SharedQueuePImpl.h          # Header for SharedQueue (using PImpl idiom)
│   └── include/
│       ├── IQueue.h                    # Queue interface definition
│       ├── QueueFactory.h              # Factory for creating queue instances
├── test/                               # Test suite
│   ├── PerformanceTests.cpp            # Performance benchmarks
│   ├── SharedQueue.cpp                 # Validation tests for SharedQueue
│   ├── StressTests.cpp                 # Stress testing the queue
│   ├── utilities.hpp                   # Helper utilities for testing
│   └── CMakeLists.txt                  # CMake configuration for test suite
├── .gitignore                          # Git ignore rules
├── CMakeLists.txt                      # Root CMake configuration
└── README.md                           # Project documentation
```

## Setup and Build Instructions

The project can be set up and built using the following steps:

1. Clone the repository:
```bash
git clone <repository-url>
cd ThreadSafeQueue/
```
2. Create a build directory:
```bash
mkdir build
cd build
```

3. Run CMake to configure the project:
```bash
cmake ..
```
4. Build the project:
```bash
make
```

5. Run the tests:
```bash
ctest
```


### Queue Interface (`IQueue`)

The queue interface includes the following methods:

```plaintext
1. int count() const
   - Returns the number of items in the queue.

2. void enqueue(T* value)
   - Enqueues an item (blocks if full).

3. bool enqueue(T* value, int timeout_ms)
   - Attempts to enqueue within a timeout.

4. T* dequeue()
   - Dequeues an item (blocks if empty).

5. T* dequeue(int timeout_ms)
   - Attempts to dequeue within a timeout.
```
