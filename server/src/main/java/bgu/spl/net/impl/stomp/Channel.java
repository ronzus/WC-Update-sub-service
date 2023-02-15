package bgu.spl.net.impl.stomp;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.concurrent.atomic.AtomicBoolean;

public class Channel {

    ConcurrentLinkedQueue<Integer> subscribersConnectionIds;
    String Name;


    public Channel(String nameString){
        subscribersConnectionIds=new ConcurrentLinkedQueue<>();
        Name=nameString;
    } 

    public void subscribe(int connectionId){
    subscribersConnectionIds.add(connectionId);
    
    }






}
