#ifndef DEFINES_H
#define DEFINES_H

// Defined constants
static const int INF = 9999;

// Packet codes
static const int DVPACKET = 0;
static const int DATAPACKET = 1;

// Event codes
static const int SENDDV = 0;
static const int RECVDV = 1;
static const int SENDTRIGDV = 2;
static const int FWDDATAPKT = 4;
// Special case event codes
static const int RMLINKBTWNNODES = 6;

// Log level codes
// Log only the assignment results to a result file, don't log anything else
static const int LOG_RESULTS = 0;
// Log a router's RTable every time it changes and the packet that cause the
// change's DVUpdate table
static const int LOG_RT_CHANGES = 1;
// Log all DV packets sent, recieved, and any RT changes
static const int LOG_ALL_DV_EVENTS = 2;
// Log all packets sent, recieved, their hops along the network, disconnection
// events, RT changes, and any neighbor changes the occur after the network is
// created
static const int LOG_ALL_NETWORK_EVENTS = 3;
// Log everything above as well as the creation of the network & event queue
static const int LOG_EVERYTHING = 4;
#endif
