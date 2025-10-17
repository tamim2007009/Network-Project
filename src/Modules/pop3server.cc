#include "helpers.h"
#include <map>
#include <omnetpp.h>
#include <sstream>
#include <vector>
using namespace omnetpp;
using namespace std;

// Structure to store email messages
struct Email {
  string from;
  string to;
  string subject;
  string body;
  long size;
  simtime_t arrivedAt;
};

class POP3Server : public cSimpleModule {
private:
  int addr = 0;
  map<string, vector<Email>> mailboxes; // user email -> list of emails
  long totalMailCount = 0;              // total emails stored across all users

protected:
  void initialize() override {
    addr = par("address");
    totalMailCount = 0;
    EV_INFO << "POP3Server[" << addr << "] initialized\n";
  }

  void handleMessage(cMessage *msg) override {
    if (msg->getKind() == MAIL_STORE) {
      // Store email from SMTP server
      string from = msg->par("from").stringValue();
      string to = msg->par("to").stringValue();
      string subject = msg->par("subject").stringValue();
      string body = msg->par("body").stringValue();
      long size = msg->par("size").longValue();

      Email email;
      email.from = from;
      email.to = to;
      email.subject = subject;
      email.body = body;
      email.size = size;
      email.arrivedAt = simTime();

      mailboxes[to].push_back(email);
      totalMailCount++;

      EV_INFO << "POP3Server[" << addr << "] Stored email for " << to << "\n";
      EV_INFO << "  Mailbox size (" << to << "): " << mailboxes[to].size()
              << " message(s)\n";
      EV_INFO << "  Total stored (all users): " << totalMailCount << "\n";
    } else if (msg->getKind() == MAIL_CHECK) {
      // Client checking for mail
      long src = SRC(msg);
      string user = msg->par("user").stringValue();

      EV_INFO << "POP3Server[" << addr << "] Mail check from " << user << "\n";

      auto it = mailboxes.find(user);
      if (it != mailboxes.end() && !it->second.empty()) {
        // Retrieve and send the first email
        Email email = it->second.front();
        it->second.erase(it->second.begin());
        if (totalMailCount > 0)
          totalMailCount--; // keep total in sync

        auto *retrieve = mk("MAIL_RETRIEVE", MAIL_RETRIEVE, addr, src);
        retrieve->addPar("from").setStringValue(email.from.c_str());
        retrieve->addPar("to").setStringValue(email.to.c_str());
        retrieve->addPar("subject").setStringValue(email.subject.c_str());
        retrieve->addPar("body").setStringValue(email.body.c_str());
        retrieve->addPar("size").setLongValue(email.size);

        double retTime = par("retrievalTime").doubleValue();
        sendDelayed(retrieve, SimTime(retTime), "ppp$o");

        EV_INFO << "POP3Server[" << addr << "] Sending email to " << user
                << "\n";
        EV_INFO << "  " << it->second.size()
                << " message(s) remaining in mailbox (" << user << ")\n";
        EV_INFO << "  Total remaining (all users): " << totalMailCount << "\n";
      } else {
        // No mail available
        auto *none = mk("MAIL_NONE", MAIL_NONE, addr, src);
        sendDelayed(none, SimTime(par("retrievalTime").doubleValue()), "ppp$o");

        EV_INFO << "POP3Server[" << addr << "] No mail for " << user << "\n";
      }
    } else {
      EV_WARN << "POP3Server[" << addr << "] unexpected kind=" << msg->getKind()
              << "\n";
    }
    delete msg;
  }
};
Define_Module(POP3Server);
