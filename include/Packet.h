//This class defines a packet. Each packet has a packet type: 0 for DVPacket
//1 for Data Packet. If the packet is a DV packet, the packet will contain
//the sender's Routing Table. Each packet also has an original sender and
//destination field. These are used by the nodes to route the packet to the
//proper destination.
#ifndef PACKET_H
#define PACKET_H

#include "defines.h"
#include "RoutingTableEntry.h"
#include <vector>
#include <utility>

class Packet {
private:
  int packetType;
  int sourceNode;
  int destNode;
  bool traceMe;
  struct DVUpdateEntry {
    int destNode;
    int cost;
  };
  typedef std::vector<DVUpdateEntry> DVUpdateTable;
  DVUpdateTable dvTable;
  int DVUpdateTableSize;
public:
  Packet(int type, int source, int dest);
  Packet(int type, int source, int dest, bool isTracked);
  void addDVUpdateEntry(int destNode, int cost);
  int getPacketType();
  int getSourceNode();
  int getDestNode();
  int getDVUPdateTableSize();
  bool isTrackedPacket();
  std::pair<int, int> getDVEntryPair(int entryIndex);
};
#endif
