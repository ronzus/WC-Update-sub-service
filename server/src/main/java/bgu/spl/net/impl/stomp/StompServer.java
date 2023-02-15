package bgu.spl.net.impl.stomp;

import bgu.spl.net.srv.Server;

public class StompServer{

    public static void main(String[] args) {
        // TODO: implement this
    
        if (args.length == 0) {
            args = new String[]{"127.0.0.1", "tpc"};
        }

     if(args[1].equals("tpc")){
        try{
        Server.threadPerClient(
                7777, //port
                () -> new StompMessagingProtocolImpl<String>(), //protocol factory
                LineMessageEncoderDecoder::new //message encoder decoder factory
        ).serve();
        }
        catch(NumberFormatException e)
        {

        }
     }
     else{
        try{

        Server.reactor(
                Runtime.getRuntime().availableProcessors(),
                7777, //port
                () -> new StompMessagingProtocolImpl<String>(), //protocol factory
                LineMessageEncoderDecoder::new //message encoder decoder factory
        ).serve();
        }
        catch(NumberFormatException e)
        {

        }


    // }
}

}
}
