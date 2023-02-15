#pragma once
#include <thread>
#include <string>
#include <mutex>
#include <condition_variable>
#include <unordered_map>
#include "ConnectionHandler.h"
#include "event.h"
using std::string;


// TODO: implement the STOMP protocol
class StompProtocol
{
private:
int sub_id;
int receipt_id;//both need to be made uniquely by the client
bool isLoggedIn;
string CurrentUsername;
string Subscription;
string UnSubscription;
std::unordered_map<std::string, int> subNameTosubId;
std::map<string,std::vector<string>> gameNameToUsernames;
std::map<std::pair<string,string>,std::vector<Event>> UsernameAndGameToEventsVector;
std::map<std::pair<string,string>,Event> currentEventPerGame; 


public:
StompProtocol(string host,short port,string username);
ConnectionHandler connectionHandler;
void socketRead();
void keyboardRead();
string mapToString(std::map<string,string>);
string mapToStringv2(std::map<string,string>);
string BuildSummary(string game_name,string user);
void updateEvent(std::pair<string,string> username_and_gameName,Event newEvent);
};
