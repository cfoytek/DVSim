Christopher Foytek

------Building The Project------
You build the project using "make", you can also clean the project for complete rebuild using "make clean"

------Debugging------
The program has some built-in debug logging functions. However if debugging is turned on, running the program will not print to the Results.txt file.
There are 5 logging levels defined in defines.h:

	* LOG_RESULTS logs only the required results for the assignment and nothing else.

	* LOG_RT_CHANGES logs the time, DV packet that caused the change, and all Routing Table 
	  entries that are changed when a node processes a DV packet

	* LOG_ALL_DV_EVENTS logs the sending, recieving, and any RT changes that DV packets make
	  to a node

	* LOG_ALL_NETWORK_EVENTS logs DV and data packet transactions, all RT entries changed and
	  the associated node, as well as any changes to the topology of the network after it has
	  been created.
	
	* LOG_EVERYTHING logs everything that LOG_ALL_NETWORK_EVENTS logs as well as the creation
	  and deletion of the network by the NetworkBuilder. At one time this also logged changes
	  to the EventQueue, but that quickly leads to extremely large log files, so it was excluded
	  from the final submission.

To change the logging level, change the LOCAL_LOG_LEVEL const in DistanceVectorSimulator.cpp to one of the above codes.
Logs are saved to a text file that starts with "log_", with a generated time code attached to the end.

------Navigating The Code------
The main entry point for the program is located in DistanceVectorSimulator.cpp. A quick rundown of the header files:
	* NetworkBuilder.h - Helper class that builds the network, and passes a vector of pointers to Node objects to the EventManager
	* EventManager.h - Mediates packet transactions between nodes through the scheduling of Events
	* Node.h - Framework for a node on the network. Maintains two containers, a map containing its neighbors, and a vector containing
		   RoutingTableEntries
	* RoutingTableEntry.h - A wrapper class for an entry in a node's routing table
	* Event.h - A wrapper class for a simulator event
	* EventQueue.h - A priority queue implemented with an ordered Linked List that holds a list of Events
	* Packet.h - A wrapper class for a packet, has a vector of DVUpdateEntry structs that acts as the destination, cost pairs
	* defines.h - Contains globally defined static values
	* Logger.h - A class used for both creating/adding to the results document, and for logging debug events throughout the course of the program.
		*The implementation of this logger turned out to be a pretty naiive and inflexible approach. If I were to implement it
		 again, I would use stream operators so that logging of complex strings with many variables would be simpler.