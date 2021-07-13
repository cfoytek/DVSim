#include "Node.h"

Node::Node(int id) {
  this->id = id;
}

Packet* Node::preparePacket(int packetType,
                            int srcNode,
                            int destNode,
                            bool tracePacket) {
  Packet* newPacket = new Packet(packetType, srcNode, destNode, tracePacket);
  std::ostringstream sb;
  sb << "Node " << id << " is preparing a new packet to Node " << destNode
     << " of type " << Logger::packetCodeToString(packetType);
  if (packetType == DVPACKET) {
    sb << ".\n\tThe packet is a DV packet, the delay to the destination is "
       << getDelayToNeighbor(destNode);
    // Iterate routing table and add each entry to packet, use split horizon
    // to avoid counting to infinity prob.
    int rtDestNode;
    int rtCost;
    rTableIt it;
    for (it = rTable.begin(); it != rTable.end(); it++) {
      rtDestNode = it->getDestNode();
      rtCost = it->getCost();
      // Implement split horizon, never send an rt entry to a node if it's
      // the next hop in the entry, also a node should never send the cost
      // to a node to that node
      if (it->getNextHop() != destNode && rtDestNode != destNode) {
        sb << "\n\t\t*Adding DV Update Table Entry Pair(dest,cost): "
           << rtDestNode << "," << rtCost;
        newPacket->addDVUpdateEntry(rtDestNode, rtCost);
      }
    }
    Logger::getInstance().Log(LOG_ALL_DV_EVENTS,
                              EventManager::getInstance().getCurrentTime(),
                              sb.str());
    return newPacket;
  } else if (packetType == DATAPACKET) {
    Logger::getInstance().Log(LOG_ALL_DV_EVENTS,
                              EventManager::getInstance().getCurrentTime(),
                              sb.str());
    return newPacket;
  } else {
    return NULL;
  }
}

bool Node::disconnectFromNeighbor(int neighborId) {
  // Delete node from list of neighbors
  int neighborsErased = neighbors.erase(neighborId);
  // Iterate routing table to find all nodes where the neighbor
  // is the next hop, the router no longer knows how to reach any of
  // these nodes, is this correct?
  rTableIt it;
  if (neighborsErased == 1) {
    for (it = rTable.begin(); it != rTable.end(); it++) {
      if (it->getNextHop() == neighborId) {
        // set cost to node to INF
        updateRTEntryToNode(it->getDestNode(), INF, it->getNextHop());
        // Tell Event Manager that your routing table has updated
        EventManager::getInstance().onRTableUpdated(this->id);
      }
    }
    std::ostringstream sb;
    sb << "Node " << id << " disconnected itself from Node " << neighborId;
    Logger::getInstance().Log(LOG_ALL_NETWORK_EVENTS, sb.str());
    // return true for successful deletion
    return true;
  }
  return false;
}

bool Node::addNeighbor(Node* newNeighbor, int cost, double linkDelay) {
  Neighbor neighborStruct;
  int newNeighborId = newNeighbor->getId();
  neighborStruct.node = newNeighbor;
  neighborStruct.cost = cost;
  neighborStruct.delay = linkDelay;
  if (neighbors.count(newNeighborId) == 0) {
    std::ostringstream sb;
    sb << "Adding a neighbor to Node " << id << "\'s list of neighbors: "
       << "\n\t\tNeighbor: Node " << newNeighborId << "\n\t\tCost: " << cost
       << "\n\t\tDelay: " << linkDelay;
    neighbors.insert(std::make_pair(newNeighborId, neighborStruct));
    Logger::getInstance().Log(LOG_EVERYTHING, sb.str());
    // Initialize the new neighbor in the Node's routing table
    updateRTEntryToNode(newNeighborId, cost, newNeighborId);
    return true;
  } else
    return false;
}

std::vector<int> Node::getAllNeighbors() {
  neighborIt it;
  std::vector<int> listOfNeighbors;
  for (it = neighbors.begin(); it != neighbors.end(); it++) {
    listOfNeighbors.push_back(it->first);
  }
  return listOfNeighbors;
}

int Node::getId() {
  return this->id;
}

int Node::processPacket(Packet* packet) {
  std::ostringstream sb;
  if (!packet) {
    sb << "Node " << this->id << " received a packet that is null.";
    Logger::getInstance().Log(LOG_ALL_NETWORK_EVENTS,
                              EventManager::getInstance().getCurrentTime(),
                              sb.str());
    throw std::invalid_argument("Can't process a null packet.");
  }
  if (packet->getPacketType() == DVPACKET) {
    bool isRTableChanged = false;
    int pktSrcNode = packet->getSourceNode();
    int dvEntryNode;
    int dvEntryCost;
    sb << "Node " << this->id << " received a packet of type "
       << Logger::packetCodeToString(DVPACKET) << " from Node" << pktSrcNode;
    Logger::getInstance().Log(LOG_ALL_DV_EVENTS,
                              EventManager::getInstance().getCurrentTime(),
                              sb.str());
    // Iterate through each entry in the packet's DV update content
    for (int i = 0; i < packet->getDVUPdateTableSize(); i++) {
      // Get a pair containing the contents of the entry
      std::pair<int, int> packetUpdateEntry = packet->getDVEntryPair(i);
      dvEntryNode = packetUpdateEntry.first;
      dvEntryCost = packetUpdateEntry.second;
      // If cost to node + cost to sender < cost to node in rTable, update the
      // rTable with the new cost
      int summedCost = dvEntryCost + getCostToNode(pktSrcNode);
      if ((summedCost < getCostToNode(dvEntryNode))) {
        updateRTEntryToNode(dvEntryNode, summedCost, packet->getSourceNode());
        isRTableChanged = true;
      }
      // Check to see if the node needs to update an entry to INF
      int nextHopToDVEntryNode = getNextHopToDest(packetUpdateEntry.first);
      if (dvEntryCost == INF && nextHopToDVEntryNode == pktSrcNode) {
        updateRTEntryToNode(dvEntryNode, INF, nextHopToDVEntryNode);
        isRTableChanged = true;
      } else {
        // do nothing because the entry is useless
        continue;
      }
    }
    if (isRTableChanged == true) {
      EventManager::getInstance().onRTableUpdated(this->id);
    }
    // Delete the packet to avoid memory leaks
    delete packet;
    return -1;
  } else if (packet->getPacketType() == DATAPACKET) {
    if (packet->getDestNode() == this->id) {
      sb << "Packet arrived at Node " << this->id;
      Logger::getInstance().Log(
          LOG_RESULTS, EventManager::getInstance().getCurrentTime(), sb.str());
      Logger::getInstance().Log(LOG_ALL_NETWORK_EVENTS,
                                EventManager::getInstance().getCurrentTime(),
                                sb.str());
      delete packet;
      return -1;
    } else {
      int nextHop = getNextHopToDest(packet->getDestNode());
      if (nextHop != -1) {
        sb << "Packet destined to Node " << packet->getDestNode()
           << " arrived at Node " << this->id << ". Forwarding packet to Node "
           << nextHop;
        Logger::getInstance().Log(LOG_RESULTS,
                                  EventManager::getInstance().getCurrentTime(),
                                  sb.str());
        Logger::getInstance().Log(LOG_ALL_NETWORK_EVENTS,
                                  EventManager::getInstance().getCurrentTime(),
                                  sb.str());
        return nextHop;
      }
    }
  }
  Logger::getInstance().Log(LOG_ALL_NETWORK_EVENTS,
                            EventManager::getInstance().getCurrentTime(),
                            "There was an unknown error processing the packet");
  return -2;
}

Node::Neighbor Node::getNeighborById(int id) {
  neighborIt it;
  Node::Neighbor neighborWithId;

  it = neighbors.find(id);
  if (it != neighbors.end()) {
    neighborWithId = it->second;
    return neighborWithId;
  } else {
    std::ostringstream sb;
    sb << "Node " << id << " is unreachable "
       << "by Node " << this->id << ". Perhaps you got disconnected?";

    throw std::logic_error(sb.str());
  }
}

void Node::updateRTEntryToNode(int destNode, int cost, int nextHop) {
  RoutingTableEntry rTableEntry(destNode, cost, nextHop);
  rTableIt it;
  if (rTable.empty()) {
    rTable.push_back(rTableEntry);
  } else {
    // Search the routing table for entry, if found update and break from loop
    for (it = rTable.begin(); it != rTable.end(); it++) {
      if (it->getDestNode() == destNode) {
        *it = rTableEntry;
        std::ostringstream sb;
        sb << "Node " << this->id << " is updating a routing table entry"
           << "\n\t\tDest: " << it->getDestNode()
           << "\t\tNew:" << rTableEntry.getDestNode()
           << "\n\t\tCost: " << it->getCost()
           << "\t\tNew:" << rTableEntry.getCost()
           << "\n\t\tNext: " << it->getNextHop()
           << "\t\tNew:" << rTableEntry.getNextHop();
        Logger::getInstance().Log(LOG_RT_CHANGES,
                                  EventManager::getInstance().getCurrentTime(),
                                  sb.str());
        return;
      }
    }
    // If not, add the entry to the routing table
    std::ostringstream sb;
    sb << "Node " << this->id << " is updating a routing table entry"
       << "\n\t\tDest: " << it->getDestNode()
       << "\t\tNew:" << rTableEntry.getDestNode()
       << "\n\t\tCost: " << it->getCost() << "\t\tNew:" << rTableEntry.getCost()
       << "\n\t\tNext: " << it->getNextHop()
       << "\t\tNew:" << rTableEntry.getNextHop();
    Logger::getInstance().Log(
        LOG_RT_CHANGES, EventManager::getInstance().getCurrentTime(), sb.str());
    rTable.push_back(rTableEntry);
    std::sort(rTable.begin(), rTable.end());
  }
}

int Node::getCostToNode(int nodeId) {
  rTableIt it;
  for (it = rTable.begin(); it != rTable.end(); it++) {
    if (it->getDestNode() == nodeId) {
      return it->getCost();
    }
  }
  return INF;
}

int Node::getNextHopToDest(int destNode) {
  rTableIt it;
  for (it = rTable.begin(); it != rTable.end(); it++) {
    if (it->getDestNode() == destNode)
      return it->getNextHop();
  }
  return -1;
}

double Node::getDelayToNeighbor(int neighborId) {
  try {
    Neighbor neighborAtId = getNeighborById(neighborId);
    return neighborAtId.delay;
  } catch (std::logic_error e) {
    Logger::getInstance().Log(LOG_ALL_NETWORK_EVENTS, e.what());
    return -1;
  }
}

std::string Node::neighborToString(int id) {
  Node::Neighbor neighborWithId;
  std::ostringstream stringBuilder;

  try {
    neighborWithId = getNeighborById(id);
  } catch (const std::invalid_argument& e) {
    stringBuilder << e.what();
    return stringBuilder.str();
  }

  stringBuilder << id << "\t" << neighborWithId.cost << "\t"
                << neighborWithId.delay;
  return stringBuilder.str();
}

std::string Node::neighborhoodToString() {
  neighborIt it;
  std::ostringstream stringBuilder;
  for (it = neighbors.begin(); it != neighbors.end(); it++) {
    stringBuilder << neighborToString(it->first) << std::endl;
  }
  return stringBuilder.str();
}

std::string Node::rTableToString() {
  std::ostringstream stringBuilder;
  rTableIt it;
  char separator = ' ';
  stringBuilder << std::left << std::setw(15) << std::setfill(separator)
                << "Destination";
  stringBuilder << std::left << std::setw(10) << std::setfill(separator)
                << "Cost";
  stringBuilder << std::left << std::setw(10) << std::setfill(separator)
                << "Next Hop\n";
  for (it = rTable.begin(); it != rTable.end(); it++) {
    int destNode = it->getDestNode();
    int cost = it->getCost();
    int nextHop = it->getNextHop();
    stringBuilder << std::left << std::setw(15) << std::setfill(separator)
                  << destNode;
    stringBuilder << std::left << std::setw(10) << std::setfill(separator)
                  << cost;
    stringBuilder << std::left << std::setw(10) << std::setfill(separator)
                  << nextHop << std::endl;
  }
  return stringBuilder.str();
}
