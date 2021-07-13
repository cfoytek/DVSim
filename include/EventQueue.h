#ifndef EVENT_LIST_H
#define EVENT_QUEUE_H
#include "Event.h"
#include "Logger.h"
#include <sstream>
#include <stdexcept>

class Event;
class Logger;
class EventQueue {
 private:
  class ListItem {
   public:
    ListItem();
    Event e;
    ListItem* next;
  };
  ListItem* head;
  int numEvents = 0;

 public:
  EventQueue();
  // Insert sorted by event time, with events that have the same time
  // being inserted in sequential order.
  bool push(Event);
  // Returns the top event
  Event top();
  bool pop();
  bool empty();
  int getNumEvents();
};
#endif  // !EVENT_QUEUE_H
