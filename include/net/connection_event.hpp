#ifndef CONNECTION_EVENT_HPP
#define CONNECTION_EVENT_HPP

#include <cstdint>

// Platform-agnostic event representation
struct ConnectionEvent {
  enum EventType { READ = 0x01, WRITE = 0x02, ERROR = 0x04, CLOSE = 0x08 };

  uintptr_t ident; // File descriptor
  EventType type;  // Event type
  int flags;       // Event flags
  int filter;      // Event filter
  int fflags;      // Filter flags
  int64_t data;    // Event data
  void *udata;     // User data
};

#endif