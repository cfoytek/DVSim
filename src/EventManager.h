#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H

// Includes
#include "defines.h"
#include "Node.h"
#include "Event.h"
#include "Packet.h"
#include "EventQueue.h"
#include <sstream>
#include <string>
#include <vector>
#include <queue>
#include <functional>

// Forward declarations
class Node;
class Packet;
class Event;
class EventQueue;

class EventManager {
 public:
  /*
   * Create Singleton class because we need one and only
   * one EventManager for the simulator
   */
  static EventManager& getInstance() {
    static EventManager instance;

    return instance;
  };
  void setNetwork(std::vector<Node*>);
  // Get next event from queue and process it,
  // and schedule new events as needed
  void processNextEvent();
  /**
   * Prepares DV packets from the updated node to all its neighbors with the
   * contents of the packets being the updated node's new RTable.
   * @param updatedNode The id of the updated node.
   */
  void onRTableUpdated(int updatedNode);
  std::string networkToString();
  void logNetworkReport();
  void scheduleEvent(double time,
                     int eventCode,
                     int srcNode,
                     int destNode,
                     Packet* packet);
  void createInitialDVEvents();
  double getCurrentTime();
  void destroyNetwork();

 private:
  EventQueue eventQueue;
  std::vector<Node*> nodeList;
  typedef std::vector<Node*>::iterator nodeIterator;
  double currentTime;
  double initialNetworkConvergenceTime;
  int controlMessagesForInitialConverge = 0;
  int hopsForDataPkt = 0;
  int lastNodeToConverge;
  double convergenceTimeAfterAnomaly;
  int controlMessagesPostAnomaly = 0;
  double timeOfNetworkAnomaly;
  int disconnectedNode1;
  int disconnectedNode2;
  bool networkConvergeStatus;
  bool rmLinkAnomalyOccurred;
  Node* getNodeById(int nodeIndex);
  EventManager() {}
  ~EventManager() {}
  EventManager(EventManager const&);
  void operator=(EventManager const&);
};

#endif
