#include "RoutingTableEntry.h"

RoutingTableEntry::RoutingTableEntry(int destinationNode,
                                     int costToDest,
                                     int nextHop) {
  destNode = destinationNode;
  cost = costToDest;
  this->nextHop = nextHop;
}

RoutingTableEntry::~RoutingTableEntry() {}

int RoutingTableEntry::getNextHop() {
  return nextHop;
}

int RoutingTableEntry::getCost() {
  return cost;
}

int RoutingTableEntry::getDestNode() const {
  return destNode;
}