#ifndef MODULES_HELPERS_H_
#define MODULES_HELPERS_H_

#include <map>
#include <omnetpp.h>
#include <sstream>
using namespace omnetpp;
using namespace std;

/*
Message kinds:
  10 = MAIL_SEND       - Client sends email to SMTP server
  11 = MAIL_STORE      - SMTP forwards email to POP3 for storage
  12 = MAIL_ACK        - SMTP acknowledges receipt to client
  20 = MAIL_CHECK      - Client checks for new mail at POP3 server
  21 = MAIL_RETRIEVE   - POP3 sends email to client
  22 = MAIL_NONE       - POP3 indicates no mail available

For all messages we set:
  par("src") : long  logical sender address
  par("dst") : long  logical destination address

Plus:
  MAIL_SEND:     par("from"): string (sender email)
                 par("to"): string (recipient email)
                 par("subject"): string
                 par("body"): string
                 par("size"): long (bytes)

  MAIL_STORE:    par("from"): string
                 par("to"): string
                 par("subject"): string
                 par("body"): string
                 par("size"): long

  MAIL_ACK:      par("messageId"): long

  MAIL_CHECK:    par("user"): string (email address to check)

  MAIL_RETRIEVE: par("from"): string
                 par("to"): string
                 par("subject"): string
                 par("body"): string
                 par("size"): long

  MAIL_NONE:     (no additional params)
*/

enum {
  MAIL_SEND = 10,
  MAIL_STORE = 11,
  MAIL_ACK = 12,
  MAIL_CHECK = 20,
  MAIL_RETRIEVE = 21,
  MAIL_NONE = 22
};

static cMessage *mk(const char *name, int kind, long src, long dst) {
  auto *m = new cMessage(name, kind);
  m->addPar("src").setLongValue(src);
  m->addPar("dst").setLongValue(dst);
  return m;
}
static inline long SRC(cMessage *m) { return m->par("src").longValue(); }
static inline long DST(cMessage *m) { return m->par("dst").longValue(); }

#endif /* MODULES_HELPERS_H_ */
