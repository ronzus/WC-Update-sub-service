# SPL_Assignment_3
The Third Assignment in the course "Systems Programming" Ben-Gurion University 2023-1.

This assignment was about creating a 'community led' service for the world cup,
Where users can subscribe to a game channel and report and receive reports
about the game to and from the other subscribed users.

The server is implemented in Java and supports both Thread-Per-Client (TPC) and the Reactor DP,
choosing which one according to arguments given on startup. 
The client is implemented in C++.

All communication between the clients and the server is according to
STOMP ‘Simple-Text-Oriented-Messaging-Protocol’.
