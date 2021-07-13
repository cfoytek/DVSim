#ifndef NODE_H
#define NODE_H

#include "EventManager.h"
#include "RoutingTableEntry.h"
#include "Packet.h"
#include "Logger.h"
#include <vector>
#include <map>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <utility>
#include <iomanip>

class EventManager;
class RoutingTableEntry;
class Packet;

class Node {
public:

private:
  int id;
  struct Neighbor {
    Node *node;
    int cost;
    double delay;
  };
  std::map<int, Neighbor> neighbors;
  typedef std::map<int, Neighbor>::iterator neighborIt;
  std::vector<RoutingTableEntry> rTable;
  typedef std::vector<RoutingTableEntry>::iterator rTableIt;
  Neighbor getNeighborById(int);
  void updateRTEntryToNode(int destNode, int cost, int nextHop);
  int getCostToNode(int nodeId);


public:
  /**
   * Used to initialize a new node with the specified id.
   */
  Node(int id);

  /**
   * Returns the id of the current node object
   * @return The id of the current node.
   */
  int getId();

  /**
   * Processes the recieved packet and returns the index of the next hop if the
   * packet is not destined to the current node. If the packet changes the state
   * of the node, the node will notify the EventManager with the new state
   * information by creating triggered update DVPacket events with the new RTable.
   * @param  Packet The packet to be processed.
   * @return        Return the index of the next hop node if the packet is not
   *                        destined for the current node, else returns -1.
   */

  int processPacket(Packet *packet);

  /**
   * Prepares a packet of a certain packet type and returns it to the caller.
   * If the packet type is DVpacket this function will implement split horizon
   * to avoid the counting to infinity problem.
   * @param  packetType The type of packet to be sent. 0 for DVpacket, 1 for
   *                      DataPacket
   * @param  srcNode    The index of the node from which the packet originates.
   * @param  destNode   The index of the node to which the packet is destined.
   *                            This value does not have to be a neighbor of the
   *                            node from which the packet originates.
   * @return            Returns a pointer to an object of type Packet that is
                                the prepared packet ready to send, if the
                                creation is unsuccessful, returns null.
   */
  Packet *preparePacket(int packetType, int srcNode, int destNode, bool tracePacket);

  //Set cost to Neighbor to 999, delete from list of neighbors, notify EventManager
  //that your RTable has changed by creating tiggered update DV packets.
  /**
   * Disconnects from the specified neighbor
   * @param  neighborId The id of the neighbor to be disconnected.
   * @return            Returns a boolean value that is true upon successfully
   *                            disconnecting from the neighbor, and false if
   *                            the neighbor does not exist.
   */
  bool disconnectFromNeighbor(int neighborId);

  /**
   * Adds a new neighbor to the node's list of neighbors. Neighbors are always
   * created in pairs, and are usually created by the Netowrk Builder.
   * @param  newNeighbor A pointer to the node that you are adding as a neighbor
   * @param  cost        The cost of transmitting across the link between this
   *                             node and the new neighbor.
   * @param  linkDelay   The delay in seconds that it takes to send information
   *                             across the link between this node and the new
   *                             neighbor.
   * @return             Returns a boolean value that is true upon successfully
   *                             addition of the new neighbor, and false if the
   *                             neighbor already exists.
   */
  bool addNeighbor(Node *newNeighbor, int cost, double linkDelay);
  std::vector<int> getAllNeighbors();
  std::string neighborToString(int id);
  std::string neighborhoodToString();
  std::string rTableToString();
  double getDelayToNeighbor(int neighborId);
  int getNextHopToDest(int destNode);
};



#endif
