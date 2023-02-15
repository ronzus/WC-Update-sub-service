package bgu.spl.net.impl.stomp;

import bgu.spl.net.srv.BlockingConnectionHandler;
import bgu.spl.net.srv.ConnectionHandler;
import bgu.spl.net.srv.Connections;
import bgu.spl.net.srv.NonBlockingConnectionHandler;
import bgu.spl.net.srv.Server;

import java.io.IOException;
import java.util.*;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.concurrent.atomic.AtomicInteger;

public class ConnectionsImpl<T> implements Connections<T> {
    public ConcurrentHashMap<Integer,ConnectionHandler<T>> activeClients;
    public ConcurrentHashMap <Integer,User> clientIDtoUser;//<clientId,User>
    public ConcurrentHashMap <String,User> regUsers; //<userName,User>
    public ConcurrentHashMap <String,Channel> channels;//<nameofchannel,Channel>

    public AtomicInteger id;
    public AtomicInteger msg_id;

    public ConnectionsImpl(){
        this.activeClients = new ConcurrentHashMap<>();
        this.id=new AtomicInteger(0);
        this.clientIDtoUser=new ConcurrentHashMap<>();
        this.regUsers=new ConcurrentHashMap<>();
        this.msg_id=new AtomicInteger(0);
        channels=new ConcurrentHashMap<>();
     
    }
 
    public int Connect(ConnectionHandler<T> newCH){

        activeClients.put(id.intValue(),newCH);
        int ret=id.intValue();
        id.set(id.intValue()+1);
        return ret;

    }



    @Override
    public boolean send(int connectionId, T msg) {
        ConnectionHandler<T> CurrentCH = activeClients.get((connectionId));
        if(CurrentCH==null){
            return false;
        }
        CurrentCH.send(msg);
        return true;
    }


    @Override
    public void send(String channel, T msg) {
        
            for(int subscriberId :channels.get(channel).subscribersConnectionIds){
            
                send(subscriberId,msg);
            }

    }

    @Override
    public void disconnect(int connectionId) {
    
        for(Channel chan:channels.values()){
            if(chan.subscribersConnectionIds.contains(connectionId)){
                chan.subscribersConnectionIds.remove(connectionId);
            }
        }
        regUsers.remove(clientIDtoUser.get(connectionId).userName);
        clientIDtoUser.get(connectionId).DisconnectFromUser();
        clientIDtoUser.remove(connectionId);
        try {
            activeClients.get(connectionId).close();
        } catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        
        activeClients.remove(connectionId);
 
    }

}
