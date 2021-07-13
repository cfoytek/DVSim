#include "Packet.h"

Packet::Packet(int type, int source, int dest) {
  packetType = type;
  sourceNode = source;
  destNode = dest;
  DVUpdateTableSize = 0;
}

Packet::Packet(int type, int source, int dest, bool isTracked) {
  packetType = type;
  sourceNode = source;
  destNode = dest;
  traceMe = isTracked;
  DVUpdateTableSize = 0;
}

void Packet::addDVUpdateEntry(int destNode, int cost) {
  DVUpdateEntry dvEntry;
  dvEntry.destNode = destNode;
  dvEntry.cost = cost;
  dvTable.push_back(dvEntry);
  DVUpdateTableSize++;
}

int Packet::getPacketType() {
  return packetType;
}

int Packet::getSourceNode() {
  return sourceNode;
}

int Packet::getDestNode() {
  return destNode;
}

int Packet::getDVUPdateTableSize() {
  return DVUpdateTableSize;
}

bool Packet::isTrackedPacket() {
  return traceMe;
}

std::pair<int, int> Packet::getDVEntryPair(int entryIndex) {
  DVUpdateEntry entry = dvTable.at(entryIndex);

  return std::make_pair(entry.destNode, entry.cost);
}
