# Web-Based Mail Transfer Protocol

## Project Overview

This OMNeT++ simulation implements a web-based mail transfer protocol similar to SMTP and POP3, enabling email communication between multiple clients through dedicated mail servers.

## File Structure

### Network Definition Files

- **`MailNet.ned`** - Main network topology defining the mail transfer system
- **`package.ned`** - Package definition (web_mail_transfer)
- **`omnetpp.ini`** - Simulation configuration file

### Implementation Files (Modules/)

- **`mailclient.cc`** - MailClient module implementation
- **`smtpserver.cc`** - SMTP server module implementation
- **`pop3server.cc`** - POP3 server module implementation
- **`router.cc`** - Router module implementation
- **`helpers.h`** - Message types, enums, and helper functions

## Network Architecture

```
   MailClient1 (user1@mail.com)
        |
        |--- Router --- SMTPServer
        |                   |
   MailClient2          POP3Server
   (user2@mail.com)
```

### Network Components

1. **MailClient (2 instances)**

   - Address: 1 (client1), 2 (client2)
   - Sends emails via SMTP server
   - Checks and retrieves emails from POP3 server
   - Email addresses: user1@mail.com, user2@mail.com

2. **SMTPServer**

   - Address: 3
   - Receives emails from clients
   - Sends acknowledgments
   - Forwards emails to POP3 for storage
   - Processing time: 10ms

3. **POP3Server**

   - Address: 4
   - Stores emails in per-user mailboxes
   - Delivers emails when clients check mail
   - Retrieval time: 5ms
   - Max mail size: 5KB

4. **Router**
   - Routes messages between all nodes
   - Routing table: "1:0,2:1,3:2,4:3"

## Message Types

| Message Type  | Code | Description                               |
| ------------- | ---- | ----------------------------------------- |
| MAIL_SEND     | 10   | Client sends email to SMTP server         |
| MAIL_STORE    | 11   | SMTP forwards email to POP3 for storage   |
| MAIL_ACK      | 12   | SMTP acknowledges receipt to client       |
| MAIL_CHECK    | 20   | Client checks for new mail at POP3 server |
| MAIL_RETRIEVE | 21   | POP3 sends email to client                |
| MAIL_NONE     | 22   | POP3 indicates no mail available          |

## Email Message Parameters

Each email contains:

- **from**: Sender email address (string)
- **to**: Recipient email address (string)
- **subject**: Email subject line (string)
- **body**: Email body content (string)
- **size**: Email size in bytes (1.5KB - 5KB)

## Simulation Flow

### Timeline (default configuration):

```
t = 1.0s:  Client1 sends email to user2@mail.com
t = 1.01s: SMTP processes and forwards to POP3
t = 3.0s:  Client2 sends email to user1@mail.com
t = 3.01s: SMTP processes and forwards to POP3
t = 8.0s:  Client1 checks mail → receives email from Client2
t = 10.0s: Client2 checks mail → receives email from Client1
```

### Communication Sequence

1. **Sending Email:**

   ```
   MailClient → SMTP: MAIL_SEND (email data)
   SMTP → MailClient: MAIL_ACK (acknowledgment)
   SMTP → POP3: MAIL_STORE (email data)
   ```

2. **Retrieving Email:**
   ```
   MailClient → POP3: MAIL_CHECK (user email)
   POP3 → MailClient: MAIL_RETRIEVE (email data) OR MAIL_NONE
   ```

## Network Configuration

- **Channel Type**: P2P (Point-to-Point)
- **Data Rate**: 100 Mbps
- **Propagation Delay**: 0.5ms
- **Simulation Time**: 15 seconds

## Configurable Parameters

### MailClient Parameters:

- `address`: Node address (1, 2)
- `smtpAddr`: SMTP server address (default: 3)
- `pop3Addr`: POP3 server address (default: 4)
- `recipient`: Recipient email address
- `emailSubject`: Email subject line
- `sendAt`: Time to send email
- `checkAt`: Time to check for mail

### SMTPServer Parameters:

- `address`: Server address (3)
- `pop3Addr`: POP3 server address (4)
- `processingTime`: Email processing time (default: 10ms)

### POP3Server Parameters:

- `address`: Server address (4)
- `retrievalTime`: Email retrieval time (default: 5ms)
- `maxMailSizeBytes`: Maximum email size (default: 5000 bytes)

## Building and Running

1. Open the project in OMNeT++ IDE
2. Build the project (Ctrl+B)
3. Run the simulation (F5)
4. View results in the event log and message animation

## Key Features

✅ Two-way email communication between clients  
✅ SMTP protocol for sending emails  
✅ POP3 protocol for retrieving emails  
✅ Mailbox storage with per-user queues  
✅ Acknowledgment system for sent emails  
✅ Realistic network delays and processing times  
✅ Email metadata (from, to, subject, body, size)

---

## What to Expect (Logs and Results)

- SMTP logs each incoming MAIL_SEND and immediately sends MAIL_ACK back to the client.
- POP3 logs mailbox sizes per user and the total stored (all users):
  - After both emails are stored: Total should reach 2
  - After client1 retrieves: Total should be 1
  - After client2 retrieves: Total should be 0
- Results are written under `results/` (e.g., `General-#0.sca`) with run metadata and parameters.

## Troubleshooting

- No mail retrieved?

  - Ensure `sendAt` < corresponding `checkAt` so storage happens before the client checks.
  - Verify `recipient` matches the peer client’s email (user1@mail.com / user2@mail.com).
  - Check the router `routes` string covers destinations 1..4.

- Total stored isn’t 2 after both sends?
  - POP3 stores per recipient; the total is the sum across both mailboxes. Check SMTP ACK logs to confirm both sends occurred.

## Windows Command-Line (optional)

If you prefer CLI over the IDE, run these inside the OMNeT++ environment:

```powershell
# Build
make

# Run with GUI (Qtenv) or console (Cmdenv)
# opp_run -u Qtenv -n . -l . omnetpp.ini
# opp_run -u Cmdenv -n . -l . omnetpp.ini
```

## Differences from HTTP-DNS Project

| Original (HTTP-DNS)      | New (Mail Transfer)    |
| ------------------------ | ---------------------- |
| PC module                | MailClient module      |
| DNS server               | SMTP server            |
| HTTP server              | POP3 server            |
| DNS queries              | Email sending          |
| HTTP requests            | Email retrieval        |
| 3 network nodes          | 4 network nodes        |
| One-way request/response | Two-way email exchange |

## Future Enhancements

Potential improvements:

- Multiple mailbox support per client
- Email priorities and spam filtering
- Attachment support with larger sizes
- IMAP protocol for server-side storage
- Email encryption/security
- Bounce messages for invalid addresses
- Mail server redundancy and load balancing

---

_Package: web_mail_transfer_  
_Network: MailNet_  
_OMNeT++ Version: 6.x+_
