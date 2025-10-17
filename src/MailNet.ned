package web_mail_transfer;

channel P2P extends ned.DatarateChannel
{
    datarate = 100Mbps;
    delay = 0.5ms;
}

simple Router
{
    parameters:
        string routes = default("");   // e.g. "1:0,2:1,3:2,4:3"
        @display("i=device/router");
    gates:
        inout pppg[4];                 // <-- fixed-size vector gate (0..3)
}

simple MailClient
{
    parameters:
        int address;
        int smtpAddr = default(3);        // SMTP server address
        int pop3Addr = default(4);        // POP3 server address
        string recipient = default("user2@mail.com");
        string emailSubject = default("Test Email");
        double sendAt @unit(s) = default(1s);
        double checkAt @unit(s) = default(5s);
        @display("i=device/laptop");
    gates:
        inout ppp;
}

simple SMTPServer
{
    parameters:
        int address;
        int pop3Addr = default(4);        // POP3 server to forward mail
        double processingTime @unit(s) = default(10ms);
        @display("i=device/server");
    gates:
        inout ppp;
}

simple POP3Server
{
    parameters:
        int address;
        double retrievalTime @unit(s) = default(5ms);
        int maxMailSizeBytes = default(5000);
        @display("i=device/server");
    gates:
        inout ppp;
}

network MailNet
{
    submodules:
        r: Router {
            parameters:
                routes = "1:0,2:1,3:2,4:3";
                @display("p=420,200");
        }
        client1: MailClient {
            parameters:
                address = 1;
                smtpAddr = 3;
                pop3Addr = 4;
                recipient = "user2@mail.com";
                emailSubject = "Hello from Client1";
                sendAt = 1s;
                checkAt = 8s;
                @display("p=120,120");
        }
        client2: MailClient {
            parameters:
                address = 2;
                smtpAddr = 3;
                pop3Addr = 4;
                recipient = "user1@mail.com";
                emailSubject = "Reply from Client2";
                sendAt = 3s;
                checkAt = 10s;
                @display("p=120,280");
        }
        smtp: SMTPServer {
            parameters:
                address = 3;
                pop3Addr = 4;
                processingTime = 10ms;
            @display("p=720,120");
        }
        pop3: POP3Server {
            parameters:
                address = 4;
                retrievalTime = 5ms;
                maxMailSizeBytes = 5000;
            @display("p=720,280");
        }
    connections:
        client1.ppp <--> P2P <--> r.pppg[0];
        client2.ppp <--> P2P <--> r.pppg[1];
        smtp.ppp    <--> P2P <--> r.pppg[2];
        pop3.ppp    <--> P2P <--> r.pppg[3];
}

