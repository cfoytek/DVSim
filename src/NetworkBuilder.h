#ifndef NETWORK_BUILDER_H
#define NETWORK_BUILDER_H

#include "Node.h"
#include "EventManager.h"
#include "Logger.h"
#include <vector>
#include <sstream>
#include <iostream>
#include <algorithm>

class NetworkBuilder {
 private:
  double linkDelay;
  std::vector<Node*> network;
  typedef std::vector<Node*>::iterator nodeIterator;

 public:
  NetworkBuilder();
  bool isNodeCreated(int nodeId);
  void createNode(int nodeId);
  void addEdge(int srcNode, int destNode, int linkCost, double linkDelay);
  void printNetwork();
  int getNumNodes();
  void printNodes();
  std::vector<Node*> getNetwork();
  void initializeNodeRoutingTables();
};
bool compLessNodePtr(Node* lhs, Node* rhs);
#endif
