#include "helpers.h"
#include <map>
#include <omnetpp.h>
#include <sstream>
using namespace omnetpp;
using namespace std;

class MailClient : public cSimpleModule {
private:
  int addr = 0;
  int smtpAddr = 3;
  int pop3Addr = 4;
  string recipient;
  string emailSubject;
  string myEmail;
  cMessage *sendEvt = nullptr;
  cMessage *checkEvt = nullptr;

protected:
  void initialize() override {
    addr = par("address");
    smtpAddr = par("smtpAddr");
    pop3Addr = par("pop3Addr");
    recipient = par("recipient").stdstringValue();
    emailSubject = par("emailSubject").stdstringValue();

    // Create email address based on client address
    stringstream ss;
    ss << "user" << addr << "@mail.com";
    myEmail = ss.str();

    // Schedule send event
    sendEvt = new cMessage("sendMail");
    scheduleAt(simTime() + SimTime(par("sendAt").doubleValue()), sendEvt);

    // Schedule check event
    checkEvt = new cMessage("checkMail");
    scheduleAt(simTime() + SimTime(par("checkAt").doubleValue()), checkEvt);

    EV_INFO << "MailClient[" << addr << "] (" << myEmail << ") initialized\n";
    EV_INFO << "  Will send email at " << par("sendAt").doubleValue() << "s\n";
    EV_INFO << "  Will check mail at " << par("checkAt").doubleValue() << "s\n";
  }

  void handleMessage(cMessage *msg) override {
    if (msg->isSelfMessage()) {
      if (msg == sendEvt) {
        // Send email to SMTP server
        auto *mail = mk("MAIL_SEND", MAIL_SEND, addr, smtpAddr);
        mail->addPar("from").setStringValue(myEmail.c_str());
        mail->addPar("to").setStringValue(recipient.c_str());
        mail->addPar("subject").setStringValue(emailSubject.c_str());

        stringstream body;
        body << "This is a test email from " << myEmail << " sent at time "
             << simTime();
        mail->addPar("body").setStringValue(body.str().c_str());
        mail->addPar("size").setLongValue(1500 +
                                          intuniform(0, 3500)); // 1.5-5KB

        EV_INFO << "MailClient[" << addr << "] Sending email:\n";
        EV_INFO << "  From: " << myEmail << "\n";
        EV_INFO << "  To: " << recipient << "\n";
        EV_INFO << "  Subject: " << emailSubject << "\n";

        send(mail, "ppp$o");
        return;
      } else if (msg == checkEvt) {
        // Check for new mail at POP3 server
        auto *check = mk("MAIL_CHECK", MAIL_CHECK, addr, pop3Addr);
        check->addPar("user").setStringValue(myEmail.c_str());

        EV_INFO << "MailClient[" << addr << "] Checking mail for " << myEmail
                << "\n";
        send(check, "ppp$o");
        return;
      }
    }

    switch (msg->getKind()) {
    case MAIL_ACK: {
      long msgId = msg->par("messageId").longValue();
      EV_INFO << "MailClient[" << addr
              << "] Email sent successfully (ID: " << msgId << ")\n";
      break;
    }
    case MAIL_RETRIEVE: {
      EV_INFO << "MailClient[" << addr << "] Received email:\n";
      EV_INFO << "  From: " << msg->par("from").stringValue() << "\n";
      EV_INFO << "  To: " << msg->par("to").stringValue() << "\n";
      EV_INFO << "  Subject: " << msg->par("subject").stringValue() << "\n";
      EV_INFO << "  Body: " << msg->par("body").stringValue() << "\n";
      EV_INFO << "  Size: " << msg->par("size").longValue() << " bytes\n";
      break;
    }
    case MAIL_NONE: {
      EV_INFO << "MailClient[" << addr << "] No new mail available\n";
      break;
    }
    default:
      EV_WARN << "MailClient[" << addr << "] unexpected kind=" << msg->getKind()
              << "\n";
    }
    delete msg;
  }

  void finish() override {
    cancelAndDelete(sendEvt);
    cancelAndDelete(checkEvt);
    sendEvt = nullptr;
    checkEvt = nullptr;
  }
};
Define_Module(MailClient);
