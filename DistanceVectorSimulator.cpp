/*
* This program is a Discrete-time event simulator for distance vector packet
* routing algorithm. It was created for the CS4310 - Computer Networks class
* at Texas State University.
*                                           -Created by Christopher Foytek 2016
*/

#include <iostream>
#include <cmath>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <unistd.h>
#include "src/defines.h"
#include "src/NetworkBuilder.h"
#include "src/EventManager.h"
#include "src/Logger.h"

using namespace std;

const int LOCAL_LOG_LEVEL = LOG_RESULTS;
int maxClock;
double currentClock;
int topologyNumber;
string inFile;
int srcNode;
int destNode;
int linkCost;
double linkDelay;

int main(int argc, char* argv[]);

void initializeNetwork(string);
double convertNumInput(string input);

int main(int argc, char* argv[]) {
  if (argc != 3) {
    cout << "\n\nInvalid Arguments\n";
    cout << "\tUsage: " << argv[0] << " <topology file> <time>\n" << endl;
    return -1;
  } else {
    inFile = argv[1];
    ifstream inF(inFile.c_str());
    string numString(argv[2]);
    maxClock = convertNumInput(numString);
    if (!(inF.good()) || maxClock == -1) {
      cout << "\n\nInvalid Arguments\n";
      cout << "\tUsage: " << argv[0] << " <topology file> <time>\n" << endl;
      return -1;
    }
    inF.close();
  }

  // Initialize logger for generating results and debug logging
  cout << "Initializing logger to save program results.\n";
  Logger::getInstance().intializeLogger(LOCAL_LOG_LEVEL, inFile);

  // Initialize the network
  cout << "\nCreating network for " << inFile << endl;
  initializeNetwork(inFile);

  // Initialize Event Manager with topology special requirements
  switch (topologyNumber) {
    case 1:
      cout << "Initializing special network event to disconnect two nodes at "
           << "time 10\n";
      EventManager::getInstance().scheduleEvent(10, RMLINKBTWNNODES, 0, 2,
                                                NULL);
      cout << "Initializing special network event to send data packet at time"
           << " 10\n";
      EventManager::getInstance().scheduleEvent(10, FWDDATAPKT, 0, 3, NULL);
      break;
    case 2:
      cout << "Initializing special network event to disconnect two nodes at "
              "time"
           << " 20\n";
      EventManager::getInstance().scheduleEvent(20, RMLINKBTWNNODES, 5, 7,
                                                NULL);
      cout << "Initializing special network event to send data packet at time"
           << " 20\n";
      EventManager::getInstance().scheduleEvent(20, FWDDATAPKT, 0, 7, NULL);
      break;
    case 3:
      cout << "Initializing special network event to disconnect two nodes at "
              "time"
           << " 30\n";
      EventManager::getInstance().scheduleEvent(30, RMLINKBTWNNODES, 9, 17,
                                                NULL);
      cout << "Initializing special network event to send data packet at time"
           << " 30\n";
      EventManager::getInstance().scheduleEvent(30, FWDDATAPKT, 0, 23, NULL);
      break;
  }

  // Initialize event manager with DV events after network is created
  cout << "\n\nInitializing Event Manager with first periodic updates.\n";
  EventManager::getInstance().createInitialDVEvents();
  currentClock = EventManager::getInstance().getCurrentTime();
  cout << "Processing network events until time " << maxClock;
  while (currentClock <= maxClock) {
    EventManager::getInstance().processNextEvent();
    currentClock = EventManager::getInstance().getCurrentTime();
  }
  // Log the routing tables for all nodes
  cout << "\nEvent Manager finished processing events,"
       << "logging results to file.";
  EventManager::getInstance().logNetworkReport();
  cout << "\nCleaning up memory.";
  EventManager::getInstance().destroyNetwork();
  cout << "\nResults added to the Results.txt file. Press any key to exit...";
  getchar();
}

void initializeNetwork(string inFile) {
  // Create a new network builder object
  NetworkBuilder network;
  // Remember topology number for special event generation
  if (inFile == "topology1.txt")
    topologyNumber = 1;
  else if (inFile == "topology2.txt")
    topologyNumber = 2;
  else if (inFile == "topology3.txt")
    topologyNumber = 3;
  else
    topologyNumber = 0;
  // Load topology from input file and build network
  cout << "Reading topology file.\n";
  ifstream topologyFile;
  topologyFile.open(inFile.c_str());
  int counter = 1;
  while (!topologyFile.eof()) {
    topologyFile >> srcNode >> destNode >> linkCost >> linkDelay;
    if (!network.isNodeCreated(srcNode)) {
      cout << "Creating Node " << srcNode << endl;
      network.createNode(srcNode);
    }
    if (!network.isNodeCreated(destNode)) {
      cout << "Creating Node " << destNode << endl;
      network.createNode(destNode);
    }
    cout << "Creating connection between Nodes " << srcNode << " and "
         << destNode << endl;
    network.addEdge(srcNode, destNode, linkCost, linkDelay);
  }
  // Initialize Event Manager with newly created network
  cout << "Success! Network created. Passing network to EventManager\n";
  EventManager::getInstance().setNetwork(network.getNetwork());
  cout << "Event Manager now owns the network. The network will be logged to "
       << "the log file if debug logging is turned on.\n\n";
  Logger::getInstance().Log(LOG_ALL_NETWORK_EVENTS,
                            EventManager::getInstance().networkToString());
}

double convertNumInput(string input) {
  double stringNum;
  stringstream numString(input);
  if (!(numString >> stringNum))
    return -1;
  return stringNum;
}