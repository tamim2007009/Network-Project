
#include "helpers.h"
#include <map>
#include <omnetpp.h>
#include <sstream>
using namespace omnetpp;
using namespace std;

class SMTPServer : public cSimpleModule {
private:
  int addr = 0;
  int pop3Addr = 4; // POP3 server address for mail storage
  long messageIdCounter = 1000;

protected:
  void initialize() override {
    addr = par("address");
    pop3Addr = par("pop3Addr");
    messageIdCounter = 1000;
    EV_INFO << "SMTPServer[" << addr << "] initialized, forwarding to POP3["
            << pop3Addr << "]\n";
  }

  void handleMessage(cMessage *msg) override {
    if (msg->getKind() == MAIL_SEND) {
      long src = SRC(msg);
      string from = msg->par("from").stringValue();
      string to = msg->par("to").stringValue();
      string subject = msg->par("subject").stringValue();
      string body = msg->par("body").stringValue();
      long size = msg->par("size").longValue();

      EV_INFO << "SMTPServer[" << addr << "] Received email:\n";
      EV_INFO << "  From: " << from << "\n";
      EV_INFO << "  To: " << to << "\n";
      EV_INFO << "  Subject: " << subject << "\n";
      EV_INFO << "  Size: " << size << " bytes\n";

      // Send acknowledgment back to sender
      auto *ack = mk("MAIL_ACK", MAIL_ACK, addr, src);
      ack->addPar("messageId").setLongValue(messageIdCounter++);
      send(ack, "ppp$o");

      // Forward email to POP3 server for storage
      double procTime = par("processingTime").doubleValue();
      auto *store = mk("MAIL_STORE", MAIL_STORE, addr, pop3Addr);
      store->addPar("from").setStringValue(from.c_str());
      store->addPar("to").setStringValue(to.c_str());
      store->addPar("subject").setStringValue(subject.c_str());
      store->addPar("body").setStringValue(body.c_str());
      store->addPar("size").setLongValue(size);

      sendDelayed(store, SimTime(procTime), "ppp$o");

      EV_INFO << "SMTPServer[" << addr << "] Forwarding to POP3 after "
              << procTime << "s processing\n";
    } else {
      EV_WARN << "SMTPServer[" << addr << "] unexpected kind=" << msg->getKind()
              << "\n";
    }
    delete msg;
  }
};
Define_Module(SMTPServer);
