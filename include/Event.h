#ifndef EVENT_H
#define EVENT_H

#include "Packet.h"

class Event
{
public:
  Event();
  Event(double eventTime, int eventCode, int sourceNode, int destinationNode,
        Packet *packet);
  ~Event();
  double getTime() const;
  int getEventCode();
  int getsrcNode();
  int getDestNode();
  Packet *getPacket();
private:
  double time;
  int eventType;
  int srcNode;
  int destNode;
  Packet *pkt; //This can be null in the case that there is no packet being sent
  //That is, when the EventType is RecieveDV or RecieveData
};
/**
*Frustratingly, the priority queue is doing a pairwise comparison of each
*field in the class instead of using these overloaded operators.....
*/
inline bool operator<(const Event &lhs, const Event &rhs) {
  return lhs.getTime() < rhs.getTime(); 
}
inline bool operator>(const Event &lhs, const Event &rhs) {
  return operator<(rhs, lhs); 
}
inline bool operator<=(const Event &lhs, const Event &rhs) {
  return !operator>(lhs, rhs);
}
inline bool operator>=(const Event &lhs, const Event &rhs) {
  return !operator<(lhs, rhs);
}
#endif // !EVENT_H
