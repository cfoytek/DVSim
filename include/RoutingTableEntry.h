#ifndef ROUTING_TABLE_ENTRY_H
#define ROUTING_TABLE_ENTRY_H
#include <vector>
class RoutingTableEntry
{
public:
  RoutingTableEntry(int destinationNode, int costToDest, int nextHop);
  ~RoutingTableEntry();
  int getNextHop();
  int getCost();
  int getDestNode() const;
private:
    int destNode;
    int cost;
    int nextHop;
};
inline bool operator<(const RoutingTableEntry &lhs, const RoutingTableEntry &rhs) {
  return lhs.getDestNode() < rhs.getDestNode(); 
}
inline bool operator>(const RoutingTableEntry &lhs, const RoutingTableEntry &rhs) {
  return operator<(rhs, lhs); 
}
inline bool operator<=(const RoutingTableEntry &lhs, const RoutingTableEntry &rhs) {
  return !operator>(lhs, rhs);
}
inline bool operator>=(const RoutingTableEntry &lhs, const RoutingTableEntry &rhs) {
  return !operator<(lhs, rhs);
}
#endif