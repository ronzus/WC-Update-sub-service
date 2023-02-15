#include "../include/event.h"
#include "../include/json.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
using json = nlohmann::json;

Event::Event(std::string username,std::string team_a_name, std::string team_b_name, std::string name, int time,
             std::map<std::string, std::string> game_updates, std::map<std::string, std::string> team_a_updates,
             std::map<std::string, std::string> team_b_updates, std::string discription)
    : username(username),team_a_name(team_a_name), team_b_name(team_b_name), name(name),
      time(time), game_updates(game_updates), team_a_updates(team_a_updates),
      team_b_updates(team_b_updates), description(discription)
{
}

Event::~Event()
{
}

const std::string &Event::get_team_a_name() const
{
    return this->team_a_name;
}
 std::string &Event::Get_team_a_name() 
{
    return this->team_a_name;
}

const std::string &Event::get_team_b_name() const
{
    return this->team_b_name;
}
 std::string &Event::Get_team_b_name() 
{
    return this->team_b_name;
}

const std::string &Event::get_name() const
{
    return this->name;
}

int Event::get_time() const
{
    return this->time;
}

const std::map<std::string, std::string> &Event::get_game_updates() const
{
    return this->game_updates;
}

 std::map<std::string, std::string> &Event::Get_game_updates()
{
    return this->game_updates;
}

const std::map<std::string, std::string> &Event::get_team_a_updates() const
{
    return this->team_a_updates;
}

 std::map<std::string, std::string> &Event::Get_team_a_updates()
{
    return this->team_a_updates;
}

const std::map<std::string, std::string> &Event::get_team_b_updates() const
{
    return this->team_b_updates;
}

std::map<std::string, std::string> &Event::Get_team_b_updates()
{
    return this->team_b_updates;
}

const std::string &Event::get_discription() const
{
    return this->description;
}

Event::Event() :username(""), team_a_name(""), team_b_name(""), name(""), time(0), game_updates(), team_a_updates(), team_b_updates(), description("")
{
}

Event::Event(const std::string &frame_body) :username(""), team_a_name(""), team_b_name(""), name(""), time(0), game_updates(), team_a_updates(), team_b_updates(), description("")
{
    if(frame_body!=""){
      
std::istringstream ibs(frame_body);
    std::vector<std::string> result;
    
     for (std::string s; std::getline(ibs, s, '\n');)
         {
             if(s.find_first_not_of("\t") != 0){
                s.erase(std::remove(s.begin(), s.end(), '\t'), s.end());
             }
            result.push_back(s);
           
         }
    
    std::map<std::string, std::string> my_map;
    std::string key, value;

    for(std::string s:result)
    {
        std::stringstream ss(s);
        if(std::getline(ss, key, ':') && std::getline(ss, value, ':')) 
        {
            my_map[key] = value;
        }
    }
    username = my_map["user"];
    team_a_name = my_map["team a"];
    team_b_name = my_map["team b"];
    name = my_map["event name"];
    time = std::stoi(my_map["time"]);
    
    std::map<std::string, std::string> Game_updates;
    std::map<std::string, std::string> Team_a_updates;
    std::map<std::string, std::string> Team_b_updates;

    int i = 9;
    while(result[i]!=("team a updates:")){// team a:japan
        std::stringstream ss(result[i]);
        if(std::getline(ss, key, ':') && std::getline(ss, value, ':')) 
        {
            Game_updates[key] = value;
        }
        i++;
    }

    while(result[i]!=("team b updates:")){
        std::stringstream ss(result[i]);
        if(std::getline(ss, key, ':') && std::getline(ss, value, ':')) 
        {
            Team_a_updates[key] = value;
        }
        i++;
    }

     while(result[i]!=("description:")){
        std::stringstream ss(result[i]);
        if(std::getline(ss, key, ':') && std::getline(ss, value, ':')) 
        {
            Team_b_updates[key] = value;
        }
        i++;
    }

    game_updates = Game_updates;
    team_a_updates = Team_a_updates;
    team_b_updates = Team_b_updates;

    std::size_t Description = frame_body.find("description");
    std::string new_Description = frame_body.substr(Description+13);
    description = new_Description;
    }
}

names_and_events parseEventsFile(std::string json_path)
{
    std::ifstream f(json_path);
    json data = json::parse(f);

    std::string team_a_name = data["team a"];
    std::string team_b_name = data["team b"];

    // run over all the events and convert them to Event objects
    std::vector<Event> events;
    for (auto &event : data["events"])
    {
        std::string name = event["event name"];
        int time = event["time"];
        std::string description = event["description"];
        std::map<std::string, std::string> game_updates;
        std::map<std::string, std::string> team_a_updates;
        std::map<std::string, std::string> team_b_updates;
        for (auto &update : event["general game updates"].items())
        {
            if (update.value().is_string())
                game_updates[update.key()] = update.value();
            else
                game_updates[update.key()] = update.value().dump();
        }

        for (auto &update : event["team a updates"].items())
        {
            if (update.value().is_string())
                team_a_updates[update.key()] = update.value();
            else
                team_a_updates[update.key()] = update.value().dump();
        }

        for (auto &update : event["team b updates"].items())
        {
            if (update.value().is_string())
                team_b_updates[update.key()] = update.value();
            else
                team_b_updates[update.key()] = update.value().dump();
        }
        
        events.push_back(Event("pureEvent-undefined",team_a_name, team_b_name, name, time, game_updates, team_a_updates, team_b_updates, description));
    }
    names_and_events events_and_names{team_a_name, team_b_name, events};

    return events_and_names;
}

const std::string &Event::get_username() const{
    return this->username;

}
