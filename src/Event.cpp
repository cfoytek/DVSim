#include "Event.h"

Event::Event()
{
}

Event::Event(double eventTime, int eventCode, int sourceNode, int destinationNode,
              Packet * packet)
{
  time = eventTime;
  eventType = eventCode;
  srcNode = sourceNode;
  destNode = destinationNode;
  pkt = packet;
}

Event::~Event()
{
}

double Event::getTime() const
{
  return time;
}

int Event::getEventCode()
{
  return eventType;
}

int Event::getsrcNode()
{
  return srcNode;
}

int Event::getDestNode()
{
  return destNode;
}

Packet *Event::getPacket()
{
  return pkt;
}
