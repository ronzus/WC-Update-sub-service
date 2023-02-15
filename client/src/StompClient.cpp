
#include "../include/StompProtocol.h"
#include <thread>
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <fstream>
#include <vector>
using std::string;
bool shut=false;
bool logged_in=true;//socket wise
bool connectedwait=true;
bool subscription=false;
bool unsubscription=false;
std::mutex m_mutex;
std::mutex m;
std::condition_variable waitForlogin;
std::condition_variable waitforConnected;




int main(int argc, char *argv[])
{
    // TODO: implement the STOMP client
      std::vector<std::string> result;
    while(1){
        const short bufsize = 1024;
        char buf[bufsize];

        std::cin.getline(buf, bufsize);
        std::string line(buf);
        std::string command = line.substr(0, line.find_first_of(" "));
        std::istringstream iss(line);
        for (std::string s; iss >> s;)
        {
            result.push_back(s);
        }
        if(result.at(0)=="login"){
            break;
        }
        else{
            std::cerr << "you must login first!" << std::endl;
            result.clear();
        }

    }
    std::istringstream ibs(result.at(1));
    std::vector<std::string> result2;
     for (std::string s; std::getline(ibs, s, ':');)
         {
         result2.push_back(s);
         }
    StompProtocol protocol(result2.at(0), stoi(result2.at(1)),result.at(2));
    if (!protocol.connectionHandler.connect()) {
        std::cerr << "Cannot connect to Socket" << std::endl;
        return 1;
    }
    protocol.connectionHandler.sendFrameAscii("CONNECT\naccept-version:1.2\nhost:stomp.cs.bgu.ac.il\nlogin:" + result.at(2)+"\npasscode:" + result.at(3) + "\n\0", '\0');    

    std::thread socketThread(&StompProtocol::socketRead, &protocol);
    std::unique_lock<std::mutex> mlock(m_mutex);
                    waitforConnected.wait(mlock, []
                    { return !(connectedwait); });
    mlock.unlock();
    std::thread keyboardThread(&StompProtocol::keyboardRead, &protocol);


    socketThread.join();
    keyboardThread.join();

    return 0;

}
StompProtocol::StompProtocol(string host,short port,string username): sub_id(0), receipt_id(0),isLoggedIn(true),CurrentUsername(username),Subscription(""),UnSubscription(""),subNameTosubId(),gameNameToUsernames(),UsernameAndGameToEventsVector(),currentEventPerGame(),connectionHandler(host,port){};


void StompProtocol::socketRead()
{

    while (1)
    {
        
        std::string msgFrame;
        this->connectionHandler.getFrameAscii(msgFrame, '\0');
        while (msgFrame != "")
        {
            std::string header =msgFrame.substr(0,msgFrame.find_first_of("\n"));
            if(header=="ERROR"){
                shut=true;
            }
            else if(header=="CONNECTED"){
                connectedwait=false;
                waitforConnected.notify_all();
                std::cerr << "Login successful " << "\n"<< std::endl;
            }
            else if(header=="RECEIPT"&&shut){
                msgFrame="";
                break;
            }
             
            else if(header=="RECEIPT"&&subscription){ 
             std::cerr << "Joined channel "<<this->Subscription <<"\n"<< std::endl;
             subscription=false;
            }
            else if(header=="RECEIPT"&&unsubscription){
                std::cerr << "Exited channel "<<this->UnSubscription << std::endl;
                unsubscription=false;

            }

            else if(header=="MESSAGE"){
               
                string x = msgFrame.substr(msgFrame.find_first_of("\n\n")+1);
              
                Event newEvent(x);
                updateEvent(std::make_pair(newEvent.get_username(),newEvent.get_team_a_name()+"_"+newEvent.get_team_b_name()),newEvent);
                this->UsernameAndGameToEventsVector[std::make_pair(newEvent.get_username(),newEvent.get_team_a_name()+"_"+newEvent.get_team_b_name())].push_back(newEvent);
            }
            msgFrame="";
            if(shut!=true){
            this->connectionHandler.getFrameAscii(msgFrame, '\0');
            }
            
        }
        if(msgFrame==""&&shut)
            {
                this->connectionHandler.close();
                logged_in=false;
                this->isLoggedIn = false;
                shut=false;
                std::unique_lock<std::mutex> ma(m);
                waitForlogin.wait(ma, []
                    { return logged_in; });

            }
    }
    
}

void StompProtocol::keyboardRead()
{
   
    while (1)
    {
        const short bufsize = 1024;
        char buf[bufsize];
        std::cin.getline(buf, bufsize);
        std::string line(buf);
        std::string command = line.substr(0, line.find_first_of(" "));
        std::istringstream iss(line);
        std::vector<std::string> result;
        for (std::string s; iss >> s;)
        {
            result.push_back(s);
        }
//==================================================================================================================================================

        if (!this->isLoggedIn)
        {
            if (command == "login")
            {
                    std::istringstream iss(result.at(2));
                    std::vector<std::string> result2;
                    for (std::string s; std::getline(iss, s, ':');)
                    {
                        result2.push_back(s);
                    }
                    if(!this->connectionHandler.connect()){
                        std::cerr << "Cannot connect to Socket" << std::endl;
                    }
                    logged_in=true;
                    waitForlogin.notify_all();
                    this->connectionHandler.sendFrameAscii("CONNECT\naccept-version:1.2\nhost:stomp.cs.bgu.ac.il\nlogin:" + result.at(2)+"\npasscode:" + result.at(3) + "\n\0", '\0');    
                    std::unique_lock<std::mutex> mlock(m_mutex);
                    waitforConnected.wait(mlock, []
                    { return !(connectedwait); });
                    this->isLoggedIn=true; 
                    shut=false;
                    this->receipt_id=0;
                    this->CurrentUsername=result.at(2);
                    this->subNameTosubId.clear();
                    this->gameNameToUsernames.clear();
                    this->UsernameAndGameToEventsVector.clear();
            }
            else
            {
                std::cerr << "you must login first" << std::endl;
            }
        }
//==================================================================================================================================================
        else
        {
             if (command == "login")
            {
             std::cerr << "client is already logged in" << std::endl;
            }

            else if (command == "join")
            {
            
                connectionHandler.sendFrameAscii("SUBSCRIBE\ndestination:" + line.substr(line.find_first_of(" ") + 1) + "\nid:" + std::to_string(this->sub_id) + "\nreceipt:" + std::to_string(this->receipt_id) + "\n\0", '\0');
                subscription=true;
                this->Subscription=result.at(1);
                this->subNameTosubId.insert(std::make_pair(line.substr(line.find_first_of(" ") + 1), this->sub_id));
                this->sub_id++;
                this->receipt_id++;
                this->gameNameToUsernames[result.at(1)];  
                this->gameNameToUsernames.at(result.at(1)).push_back(CurrentUsername);
                this->currentEventPerGame[std::make_pair(CurrentUsername,result.at(1))]=Event();

            }
            else if (command == "report")
            {
               

              names_and_events nne = parseEventsFile(result.at(1));
                for(auto event: nne.events){
                    int time = event.get_time();
                    std::string timeString = std::to_string(time);
                    std::string general_updates = mapToString(event.get_game_updates());
                    std::string teama_updates = mapToString(event.get_team_a_updates());
                    std::string teamb_updates = mapToString(event.get_team_b_updates());
                    std::string currentEv = this->CurrentUsername+"\n"+"team a:"+nne.team_a_name+"\n"+"team b:"+nne.team_b_name+"\n"+"event name:"+event.get_name()+"\n"+"time:"+timeString+"\n"+"general game updates:"+general_updates+"\n"+"team a updates:"+teama_updates+"\n"+"team b updates:"+teamb_updates+"\n"+"description:\n"+event.get_discription();
                    connectionHandler.sendFrameAscii("SEND\ndestination:" +nne.team_a_name+"_"+nne.team_b_name+"\n\n"+"user:"+currentEv+"\n\0",'\0');
                }     
                
                 
            }
            else if (command == "exit")
            {
                try{
                connectionHandler.sendFrameAscii("UNSUBSCRIBE\nid:" + std::to_string(this->subNameTosubId.at(line.substr(line.find_first_of(" ") + 1))) + "\nreceipt:" + std::to_string(this->receipt_id) + "\n\0", '\0');
                unsubscription=true;
                this->UnSubscription=result.at(1);
                this->receipt_id++;
                for(string username:this->gameNameToUsernames.at(result.at(1))){
                this->currentEventPerGame.erase(std::make_pair(username,result.at(1)));
                this->UsernameAndGameToEventsVector.erase(std::make_pair(username,result.at(1)));
                }
                
                }
                  catch(const std::out_of_range& e){
                    std::cout << "you weren't subscribed to the Channel!"<<std::endl;
                  }
            }
            else if (command == "logout")
            {
                connectionHandler.sendFrameAscii("DISCONNECT\nreceipt:" + std::to_string(this->receipt_id) + "\0", '\0');
                shut=true;
            }
            else if (command == "summary")
            { 
                string summary=BuildSummary(result.at(1),result.at(2));
                if(summary!=""){
                std::fstream strm;
                strm.open(result.at(3),std::ios::out|std::ios_base::trunc);
                strm<<summary;
                strm.close();
                }

            }
        
        }
    }
}
string StompProtocol::BuildSummary(string game_name,string user){
    try{
  string retvalue="";
  std::vector<Event> myEvents=this->UsernameAndGameToEventsVector.at(std::make_pair(user,game_name));
  Event mostRecent =this->currentEventPerGame.at(std::make_pair(user,game_name));
  retvalue.append(mostRecent.get_team_a_name()+" vs "+mostRecent.get_team_b_name()+"\n");
  retvalue.append("Game stats:\n");
  retvalue.append("General stats:\n");
  retvalue.append(mapToStringv2(mostRecent.get_game_updates())+"\n");
  retvalue.append(mostRecent.get_team_a_name()+" stats:\n");
  retvalue.append(mapToStringv2(mostRecent.get_team_a_updates())+"\n");
  retvalue.append(mostRecent.get_team_b_name()+" stats:\n");
  retvalue.append(mapToStringv2(mostRecent.get_team_b_updates())+"\n");
  retvalue.append("Game event reports:\n\n");

  for(Event& ev:myEvents){
    retvalue.append(std::to_string(ev.get_time())+" - "+ev.get_name()+":\n\n");
    retvalue.append(ev.get_discription()+"\n\n");
  }
  return retvalue;
    }
    catch(const std::out_of_range& e){
     std::cerr << "Invalid command" << std::endl;
     return "";
    }

}
string StompProtocol::mapToString(std::map<std::string, std::string> map){
    std::string output = "\n";
    for (auto x : map) {
        output = output +"\t"+x.first+":"+x.second+"\n";
    }
     output = output.substr(0,output.size()-1);
    return output;
}
string StompProtocol::mapToStringv2(std::map<std::string, std::string> map){
    std::string output = "";
    for (auto x : map) {
        output = output+x.first+":"+x.second+"\n";
    }
     output=output.substr(0,output.size()-1);
    return output;
}

void StompProtocol::updateEvent(std::pair<string,string> username_and_gameName,Event newEvent){
    Event& event = currentEventPerGame[username_and_gameName];
    string game=username_and_gameName.second;

     std::istringstream iss(game);
     std::vector<std::string> result2;
    for (std::string s; std::getline(iss, s, '_');)
     {
        result2.push_back(s);
    }
    event.Get_team_a_name()=result2.at(0);
    event.Get_team_b_name()=result2.at(1);
    for(auto x : newEvent.get_game_updates()){
        event.Get_game_updates()[x.first] = x.second;
    }
    for(auto x : newEvent.get_team_a_updates()){
        event.Get_team_a_updates()[x.first] = x.second;
    }
    for(auto x : newEvent.get_team_b_updates()){
        event.Get_team_b_updates()[x.first] = x.second;
    }
}