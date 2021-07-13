#include "NetworkBuilder.h"
NetworkBuilder::NetworkBuilder() {}

bool NetworkBuilder::isNodeCreated(int nodeId) {
  std::ostringstream sb;
  if (network.empty()) {
    // std::cout << "Node list is empty, creating node." << std::endl;
    sb << "Network, creating Node " << nodeId;
    Logger::getInstance().Log(LOG_EVERYTHING, sb.str());
    return false;
  } else {
    nodeIterator it;
    for (it = network.begin(); it < network.end(); it++) {
      if ((*it)->getId() == nodeId) {
        // std::cout << "Node already exists, dont need to create it." <<
        // std::endl;
        sb << "Node " << nodeId << " already exists, don't need to create it";
        Logger::getInstance().Log(LOG_EVERYTHING, sb.str());
        return true;
      }
    }
  }
  // std::cout << "List is not empty, but node doesn't exist, so create it." <<
  // std::endl;
  sb << "Node does not exist, creating Node " << nodeId;
  Logger::getInstance().Log(LOG_EVERYTHING, sb.str());
  return false;
}

void NetworkBuilder::createNode(int nodeId) {
  Node* newNode = new Node(nodeId);
  if (!network.empty()) {
    nodeIterator it;
    for (it = network.begin(); it != network.end(); it++) {
      if (it + 1 == network.end()) {
        // std::cout << "Got to the end of the vector, adding new node to the
        // end." << std::endl;
        network.insert(network.end(), newNode);
        // std::cout << "Inserted new node at the end of the vector, sorting
        // list." << std::endl;
        std::sort(network.begin(), network.end(), compLessNodePtr);
        break;
      }
    }
  } else {
    network.push_back(newNode);
  }
}

void NetworkBuilder::addEdge(int srcNode,
                             int destNode,
                             int linkCost,
                             double linkDelay) {
  std::ostringstream sb;
  Node* srcNodePtr = NULL;
  Node* destNodePtr = NULL;
  nodeIterator it;
  if (!network.empty()) {
    for (it = network.begin(); it < network.end(); it++) {
      if ((*it)->getId() == srcNode) {
        srcNodePtr = *it;
      } else if ((*it)->getId() == destNode) {
        destNodePtr = *it;
      }
    }
    // Add the edge to the srcNode & destNode's list of neighbors
    if (srcNodePtr != NULL && destNodePtr != NULL) {
      srcNodePtr->addNeighbor(destNodePtr, linkCost, linkDelay);
      destNodePtr->addNeighbor(srcNodePtr, linkCost, linkDelay);

      sb << "Created edge between nodes " << srcNode << " and " << destNode;
      Logger::getInstance().Log(LOG_EVERYTHING, sb.str());
    } else {
      sb << "Tried to create edge between nodes " << srcNode << " and "
         << destNode << " but one or both of the edges are nonexistant.";
      Logger::getInstance().Log(LOG_EVERYTHING, sb.str());
      throw "Cannot create Edge to non-existant node.";
    }
  }
}

int NetworkBuilder::getNumNodes() {
  return network.size();
}

void NetworkBuilder::printNodes() {
  nodeIterator it;
  for (it = network.begin(); it < network.end(); it++) {
    std::cout << "Node " << (*it)->getId() << std::endl;
  }
}

void NetworkBuilder::printNetwork() {
  nodeIterator it;
  std::cout << "\n\n The network is as follows: \n";
  for (it = network.begin(); it < network.end(); it++) {
    std::cout << "Node " << (*it)->getId() << " has the the neighbors: \n";
    std::cout << "Neighbor\tCost\tDelay\n";
    std::cout << (*it)->neighborhoodToString();
  }
}

std::vector<Node*> NetworkBuilder::getNetwork() {
  return network;
}

bool compLessNodePtr(Node* lhs, Node* rhs) {
  return (lhs->getId() < rhs->getId());
}
