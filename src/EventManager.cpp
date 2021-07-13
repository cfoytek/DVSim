#include "EventManager.h"

void EventManager::setNetwork(std::vector<Node*> network) {
  nodeList = network;
}

void EventManager::processNextEvent() {
  Event e = eventQueue.top();
  eventQueue.pop();
  currentTime = e.getTime();
  Packet* newPacket;
  Node* srcNode = getNodeById(e.getsrcNode());
  Node* destNode = getNodeById(e.getDestNode());
  int nextHop;
  switch (e.getEventCode()) {
    case SENDDV:
      // Make source node prepare a packet to the destination node
      newPacket = srcNode->preparePacket(DVPACKET, e.getsrcNode(),
                                         e.getDestNode(), false);
      try {
        // Try scheduling a recieve event for the destination
        scheduleEvent(
            currentTime + srcNode->getDelayToNeighbor(e.getDestNode()), RECVDV,
            e.getsrcNode(), e.getDestNode(), newPacket);
      } catch (std::logic_error e) {
        // If an error is returned, the node is unreachable, so don't create any
        // more periodic updates for it
        std::ostringstream sb;
        Logger::getInstance().Log(LOG_ALL_DV_EVENTS, e.what());
      }

      // Schedule the next DV event at time + 1
      scheduleEvent(currentTime + 1, SENDDV, e.getsrcNode(), e.getDestNode(),
                    NULL);
      break;
    case RECVDV:
      destNode->processPacket(e.getPacket());
      break;
    case SENDTRIGDV:
      // Send packet
      newPacket = srcNode->preparePacket(DVPACKET, e.getsrcNode(),
                                         e.getDestNode(), false);
      scheduleEvent(currentTime + srcNode->getDelayToNeighbor(e.getDestNode()),
                    RECVDV, e.getsrcNode(), e.getDestNode(), newPacket);

      // Assume network converged until another node's RT changes & they send
      // TDV
      lastNodeToConverge = e.getsrcNode();
      if (!rmLinkAnomalyOccurred) {
        controlMessagesForInitialConverge++;
        initialNetworkConvergenceTime = currentTime;
      } else {
        controlMessagesPostAnomaly++;
        convergenceTimeAfterAnomaly = currentTime - timeOfNetworkAnomaly;
      }
      networkConvergeStatus = true;
      break;
    case FWDDATAPKT:
      // If there's no packet associated, with the event we create it first
      if (e.getPacket() == NULL) {
        newPacket =
            new Packet(DATAPACKET, e.getsrcNode(), e.getDestNode(), true);
        Logger::getInstance().Log(LOG_RESULTS, "---Data Packet TraceRoute---");
      } else {
        newPacket = e.getPacket();
      }
      // Getting the next hop this way makes the first log of sending the packet
      // appear before anything else in the results
      nextHop = srcNode->processPacket(newPacket);
      // If the function returns -1, it's the dest so do nothing, else
      if (nextHop != -1) {
        // Get the delay to the next hop in the route
        double delayToNextHop =
            getNodeById(e.getsrcNode())->getDelayToNeighbor(nextHop);
        // Schdeule event for the next hop to recieve, process, and fwd the
        // Packet
        scheduleEvent(currentTime + delayToNextHop, FWDDATAPKT, nextHop,
                      e.getDestNode(), newPacket);
      }
      break;
    case RMLINKBTWNNODES:
      // Print the initially converged RTables and the time taken/control
      // messages sent
      Logger::getInstance().Log(LOG_RESULTS, "---Initial Routing Tables---");
      nodeIterator it;
      std::ostringstream stringBuilder;
      for (it = nodeList.begin(); it != nodeList.end(); it++) {
        stringBuilder << "\n\nRouting Table for Node " << (*it)->getId()
                      << ":\n";
        stringBuilder << (*it)->rTableToString();
        Logger::getInstance().Log(LOG_RESULTS, stringBuilder.str());
        stringBuilder.str("");
      }
      stringBuilder << "\n\nNetwork converged at time "
                    << initialNetworkConvergenceTime << std::endl
                    << "The number of triggered DV messages sent was "
                    << controlMessagesForInitialConverge << std::endl
                    << "The last node to converge was Node "
                    << lastNodeToConverge << std::endl;
      Logger::getInstance().Log(LOG_RESULTS, stringBuilder.str());

      // Remove link between the two nodes
      getNodeById(e.getsrcNode())->disconnectFromNeighbor(e.getDestNode());
      getNodeById(e.getDestNode())->disconnectFromNeighbor(e.getsrcNode());
      disconnectedNode1 = e.getsrcNode();
      disconnectedNode2 = e.getDestNode();
      // Set flags for tracking the network anomaly
      timeOfNetworkAnomaly = e.getTime();
      rmLinkAnomalyOccurred = true;
      break;
  }
}

void EventManager::onRTableUpdated(int updatedNode) {
  // A node's Routing table was updated, which means the network is not
  // converged
  networkConvergeStatus = false;
  Node* updatedNodePtr = getNodeById(updatedNode);
  std::vector<int> updateNodeNeighbors = updatedNodePtr->getAllNeighbors();
  for (int i = 0; i < updateNodeNeighbors.size(); i++) {
    std::ostringstream sb;
    int neighborId = updateNodeNeighbors.at(i);
    sb << "Creating event to send triggered DV from Node " << updatedNode
       << " to Node " << neighborId;
    Logger::getInstance().Log(LOG_ALL_DV_EVENTS, currentTime, sb.str());
    scheduleEvent(currentTime, SENDTRIGDV, updatedNode, neighborId, NULL);
  }
}

std::string EventManager::networkToString() {
  nodeIterator it;
  std::ostringstream stringBuilder;
  stringBuilder << "\n\n The network is as follows: \n";
  for (it = nodeList.begin(); it < nodeList.end(); it++) {
    stringBuilder << "Node " << (*it)->getId() << " has the the neighbors: \n";
    stringBuilder << "Neighbor\tCost\tDelay\n";
    stringBuilder << (*it)->neighborhoodToString();
  }
  return stringBuilder.str();
}

void EventManager::logNetworkReport() {
  nodeIterator it;
  std::ostringstream stringBuilder;
  if (rmLinkAnomalyOccurred) {
    Logger::getInstance().Log(LOG_RESULTS,
                              "\n---Routing Tables After Connection Loss---");
    stringBuilder << "The connection between Nodes " << disconnectedNode1
                  << " and " << disconnectedNode2
                  << " was lost at time = " << timeOfNetworkAnomaly
                  << " causing the network to reconverge."
                  << "\nThe Network reconverged after "
                  << convergenceTimeAfterAnomaly << std::endl
                  << "The number of triggered DV messages sent was "
                  << controlMessagesPostAnomaly << std::endl
                  << "The last node to converge was Node " << lastNodeToConverge
                  << std::endl;
    Logger::getInstance().Log(LOG_RESULTS, stringBuilder.str());
    Logger::getInstance().Log(
        LOG_RESULTS,
        "The disconnection changed the routing tables of the nodes"
        " to the following:\n");
    stringBuilder.str("");
    for (it = nodeList.begin(); it != nodeList.end(); it++) {
      stringBuilder << "\n\nRouting Table for Node " << (*it)->getId() << ":\n";
      stringBuilder << (*it)->rTableToString();
      Logger::getInstance().Log(LOG_RESULTS, stringBuilder.str());
      stringBuilder.str("");
    }
  }
}

void EventManager::scheduleEvent(double time,
                                 int eventCode,
                                 int srcNode,
                                 int destNode,
                                 Packet* packet) {
  Event e(time, eventCode, srcNode, destNode, packet);
  std::ostringstream sb;
  sb << "Adding new event to the event queue of type "
     << Logger::eventCodeToString(eventCode) << " from Node " << srcNode
     << " to Node " << destNode << ". Event is scheduled for time " << time;
  Logger::getInstance().Log(LOG_ALL_NETWORK_EVENTS, currentTime, sb.str());
  eventQueue.push(e);
}

void EventManager::createInitialDVEvents() {
  nodeIterator it;
  std::vector<int> nodeNeighborIds;
  for (it = nodeList.begin(); it != nodeList.end(); it++) {
    nodeNeighborIds = (*it)->getAllNeighbors();
    for (int i = 0; i < nodeNeighborIds.size(); i++) {
      std::ostringstream sb;
      sb << "Creating event to send periodic DV packet from Node "
         << (*it)->getId() << " to Node " << nodeNeighborIds.at(i);
      Logger::getInstance().Log(LOG_ALL_DV_EVENTS, getCurrentTime(), sb.str());
      scheduleEvent(0, SENDDV, (*it)->getId(), nodeNeighborIds.at(i), NULL);
    }
  }
}

double EventManager::getCurrentTime() {
  return currentTime;
}

void EventManager::destroyNetwork() {
  nodeIterator it;
  for (it = nodeList.begin(); it != nodeList.end(); it++) {
    std::ostringstream sb;
    sb << "Attempting to free memory of Node " << (*it)->getId();
    Logger::getInstance().Log(LOG_EVERYTHING, sb.str());
    delete *it;
    Logger::getInstance().Log(LOG_EVERYTHING, "Node deleted.");
  }
}

Node* EventManager::getNodeById(int nodeIndex) {
  nodeIterator it;
  for (it = nodeList.begin(); it != nodeList.end(); it++) {
    if ((*it)->getId() == nodeIndex)
      return *it;
  }
  return NULL;
}