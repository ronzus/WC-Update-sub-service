package bgu.spl.net.impl.stomp;

import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentLinkedQueue;

public class User {
    
    public String userName;
    public String passCode;
    public boolean isLoggedIn;
    public ConcurrentHashMap<String,Channel> subscriptions;//subscription ids is the Strings kept here
    public ConcurrentHashMap<Channel,String> ChannelToSubId;
    public int currentCID;     


    public User(String user_Name ,String pass_Code){
        this.userName=user_Name;
        this.passCode=pass_Code;
        this.isLoggedIn=false;
        this.subscriptions=new ConcurrentHashMap<>();
        this.currentCID=-1;
        this.ChannelToSubId=new ConcurrentHashMap<>();
        System.out.println(userName);
    }



    public boolean ConnectToUser(int connectionId){
        if(isLoggedIn){
        return false;
        }
        isLoggedIn=true;
        currentCID=connectionId;
        return true;

    }
    public void addSubscription(Channel channel,String subId){
        subscriptions.put(subId,channel);
        ChannelToSubId.put(channel, subId);
        
    }

    public void DisconnectFromUser(){
        this.subscriptions=new ConcurrentHashMap<>();
        this.ChannelToSubId=new ConcurrentHashMap<>();
        isLoggedIn=false;
        currentCID=-1;
        
    }
    
}
