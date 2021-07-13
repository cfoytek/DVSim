#include "EventQueue.h"

EventQueue::EventQueue() {}

bool EventQueue::push(Event networkEvent) {
  ListItem* curr = head;

  ListItem* newEvent = new ListItem;
  newEvent->e = networkEvent;

  // If the list is empty or the first item is bigger than new item, add item
  // to the front of the list
  if (!head || head->e > networkEvent) {
    newEvent->next = head;
    head = newEvent;
    numEvents++;
    return true;
  } else {  // Else we'll iterate through the list and add item where
            // appropriate
    // Find node before our insertion point
    while (curr->next && curr->next->e <= networkEvent) {
      curr = curr->next;
    }
    // Insert node after curr
    newEvent->next = curr->next;
    curr->next = newEvent;
    numEvents++;
    return true;
  }
  return false;
}

Event EventQueue::top() {
  if (!head) {
    Logger::getInstance().Log(
        LOG_ALL_NETWORK_EVENTS,
        "ERROR: Someone tried to access the queue while it was empty.");
    throw std::out_of_range("The queue is empty!");
  }

  return head->e;
}

bool EventQueue::pop() {
  if (!head)
    return false;
  else {
    ListItem* tmp = head->next;
    delete head;
    head = tmp;
    numEvents--;
    return true;
  }
  return false;
}

bool EventQueue::empty() {
  return numEvents > 0;
}

int EventQueue::getNumEvents() {
  return numEvents;
}

EventQueue::ListItem::ListItem() {}
