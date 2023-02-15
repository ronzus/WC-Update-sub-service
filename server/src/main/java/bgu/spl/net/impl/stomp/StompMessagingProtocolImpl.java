package bgu.spl.net.impl.stomp;

import java.util.concurrent.ConcurrentHashMap;

import javax.swing.border.LineBorder;

import bgu.spl.net.api.StompMessagingProtocol;
import bgu.spl.net.srv.Connections;


public class StompMessagingProtocolImpl<T> implements StompMessagingProtocol<T>{
    ConnectionsImpl<String> connectionsC;
    int connectionId1;
    User currentUser;
    boolean disconnect;


    
    @Override
    public void start(int connectionId, Connections<T> connections) {
        this.connectionId1=connectionId;
        this.connectionsC=(ConnectionsImpl<String>) connections;
        this.currentUser=null;
        this.disconnect=false;

    }

    @Override
    public void process(String message) {
        String command=message.substring(0,message.indexOf("\n"));
        String[] args=message.substring(message.indexOf("\n")+1).split("\n");
        ConcurrentHashMap<String,String> linesF = new ConcurrentHashMap<>();
        String msgbody="";
        for(String line:args){
            if(linesF.get("destination")!=null&command.equals("SEND")){
                if(line.equals("")){
                    msgbody=msgbody+line;
                }
                else{
                    msgbody=msgbody+line+"\n";
                }
            }
            else if(!line.equals("")){
                 linesF.put(line.split(":")[0], line.split(":")[1]);
            }
        
        }

        switch (command) {
            case "CONNECT":
            handleCONNECT(linesF);
               break;
             
            case "SUBSCRIBE":
             handleSUBSCRIBE(linesF);
                break;  

            case "UNSUBSCRIBE":
             handleUNSUBSCRIBE(linesF); 
                break;

            case "SEND":
            // String msgBody=message.substring(message.indexOf(""));
              handleSEND(linesF,msgbody);
                break;

            case "DISCONNECT":
              handleDISCONNECT(linesF);  
                break;
                   
            default:
            {
                connectionsC.send(connectionId1, "ERROR\nmessage:invalid command\n\nThe command:\n----\n"+command);
                disconnect_pr();
                
            }
            
                break;
        }


    }

    @Override
    public boolean shouldTerminate() {

        return disconnect;
    }
  

    public void handleCONNECT(ConcurrentHashMap<String,String> linesF){

        if(connectionsC.regUsers.get(linesF.get("login")) != null){ //the user already registered  
            if(!linesF.get("passcode").equals(connectionsC.regUsers.get(linesF.get("login")).passCode)){ // the password is incorrect
                if(linesF.containsKey("receipt")){
                    sendReceipt(linesF.get("receipt"));
                }
                connectionsC.send(connectionId1,"Wrong password");
            }
            else if(connectionsC.regUsers.containsKey(linesF.get("login"))){
                if(linesF.containsKey("receipt")){
                    sendReceipt(linesF.get("receipt"));
                }
                connectionsC.send(connectionId1,"User already logged in");
            }
            else{
            connectionsC.regUsers.put(linesF.get("login"), new User(linesF.get("login"), linesF.get("passcode")));
            currentUser = connectionsC.regUsers.get(linesF.get("login"));
            currentUser.ConnectToUser(connectionId1);
            connectionsC.clientIDtoUser.put(connectionId1,currentUser);
            
            if(linesF.containsKey("receipt")){
                sendReceipt(linesF.get("receipt"));
            }
            connectionsC.send(connectionId1,"CONNECTED\nversion:1.2\n\n\u0000");
            }   
        }
        else if(connectionsC.regUsers.get(linesF.get("login"))==null){
            connectionsC.regUsers.put(linesF.get("login"), new User(linesF.get("login"), linesF.get("passcode")));
            currentUser = connectionsC.regUsers.get(linesF.get("login"));
            currentUser.ConnectToUser(connectionId1);
            connectionsC.clientIDtoUser.put(connectionId1,currentUser);
            if(linesF.containsKey("receipt")){
                sendReceipt(linesF.get("receipt"));
            }
            connectionsC.send(connectionId1,"CONNECTED\nversion:1.2\n\n\u0000");
            
        }
        
        }


    public void handleSUBSCRIBE(ConcurrentHashMap<String,String> linesF){
        if(connectionsC.channels.get(linesF.get("destination"))==null){
            
            connectionsC.channels.put(linesF.get("destination"), new Channel(linesF.get("destination")));
            connectionsC.channels.get(linesF.get("destination")).subscribe(connectionId1);
            currentUser.addSubscription(connectionsC.channels.get(linesF.get("destination")), linesF.get("id"));
            if(linesF.containsKey("receipt")){
                sendReceipt(linesF.get("receipt"));
            }
        

        }
        else if(connectionsC.channels.get(linesF.get("destination")).subscribersConnectionIds.contains(connectionId1)){
            if(linesF.containsKey("receipt")){
                sendReceipt(linesF.get("receipt"));
            }
            connectionsC.send(connectionId1,("ERROR\nmessage:The user is already subscribed to this channel\nwhere was the error met:->SUBSCRIBE-"));
            
            disconnect_pr();
        }
        else{//if channel exists but user isnt inside of
            connectionsC.channels.get(linesF.get("destination")).subscribe(connectionId1);
            currentUser.addSubscription(connectionsC.channels.get(linesF.get("destination")), linesF.get("id"));
            if(linesF.containsKey("receipt")){
                sendReceipt(linesF.get("receipt"));
            }
        }
        
         
    }

    public void handleUNSUBSCRIBE(ConcurrentHashMap<String,String> linesF){
         
        if(currentUser.subscriptions.containsKey(linesF.get("id"))){
            Channel c=currentUser.subscriptions.get(linesF.get("id"));
        if(c.subscribersConnectionIds.contains(connectionId1)){
        c.subscribersConnectionIds.remove(connectionId1);
        currentUser.subscriptions.remove((linesF.get("id")));
        if(linesF.containsKey("receipt")){
            sendReceipt(linesF.get("receipt"));
        }
        }
        else{
            if(linesF.containsKey("receipt")){
                sendReceipt(linesF.get("receipt"));
            }
            connectionsC.send(connectionId1,("ERROR \nmessage:The user isn't subscribed to this channel\nwhere was the error met:->UNSUBSCRIBE-"));
            disconnect_pr();
        }
        }
        else{
            if(linesF.containsKey("receipt")){
                sendReceipt(linesF.get("receipt"));
            }
            connectionsC.send(connectionId1,("ERROR \nmessage:The user isn't subscribed to this channel\nwhere was the error met:->UNSUBSCRIBE-"));
            disconnect_pr();       
        }
    
    }


      public void handleSEND(ConcurrentHashMap<String,String> linesF,String msgBody) {

          if (connectionsC.channels.get(linesF.get("destination")) == null) {
            if(linesF.containsKey("receipt")){
                sendReceipt(linesF.get("receipt"));
            }
              connectionsC.send(connectionId1,
                      ("ERROR\nmessage:channel does'nt exist\nwhere was the error met:->SEND\n->destination header malformed"));
              disconnect_pr();

          } else if (!connectionsC.channels.get(linesF.get("destination")).subscribersConnectionIds.contains(connectionId1)) {
            if(linesF.containsKey("receipt")){
                sendReceipt(linesF.get("receipt"));
            }
              connectionsC.send(connectionId1,
                      ("ERROR\nmessage:user isn't subscribed to this channel\nwhere was the error met:->SEND->\ndestination doesn't exist"));
              disconnect_pr();

          } else {
            if(linesF.containsKey("receipt")){
                sendReceipt(linesF.get("receipt"));
            }
              sendMSGtoSubcription(linesF.get("destination"),msgBody);
          }
        
      }
  

    public void handleDISCONNECT(ConcurrentHashMap<String,String> linesF){
        if(linesF.containsKey("receipt")){
            sendReceipt(linesF.get("receipt"));
        }
        disconnect_pr();
      
    }

    public void disconnect_pr(){
        currentUser.DisconnectFromUser();
        disconnect=true;
        connectionsC.disconnect(connectionId1);
        

    }
    
    public void sendMSGtoSubcription(String Channel,String msg){
        for(int connectionId_subscriber: connectionsC.channels.get(Channel).subscribersConnectionIds){
            connectionsC.send(connectionId_subscriber, "MESSAGE\nsubscription:"+connectionsC.clientIDtoUser.get(connectionId_subscriber).ChannelToSubId.get(connectionsC.channels.get(Channel))+"\nmessage-id:"+connectionsC.msg_id.getAndIncrement()+"\ndestination:"+Channel+"\n \n"+msg);  

        }
        
        

    }
    public void sendReceipt(String receiptID){
        connectionsC.activeClients.get(connectionId1).send("RECEIPT\nreceipt-id:"+receiptID+"\n \u0000");

    }
  

}
