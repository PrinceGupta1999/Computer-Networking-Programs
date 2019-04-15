#include <stdio.h>
#include <string.h>
#include <omnetpp.h>

using namespace omnetpp;

/**
 * In the previous model we just created another packet if we needed to
 * retransmit. This is OK because the packet didn't contain much, but
 * in real life it's usually more practical to keep a copy of the original
 * packet so that we can re-send it without the need to build it again.
 */
class Sender : public cSimpleModule
{
  private:
    int seq;  // message sequence number
    cMessage *message;  // message that has to be re-sent on timeout

  public:
    Sender();
    virtual ~Sender();

  protected:
    virtual cMessage *generateNewMessage();
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Sender);

Sender::Sender()
{
    message = nullptr;
}

Sender::~Sender()
{
    delete message;
}

void Sender::initialize()
{
    // Initialize variables.
    seq = 0;
    // Generate and send initial message.
    EV << "Sending initial message\n";
    message = generateNewMessage();
    send(message, "out");
}

void Sender::handleMessage(cMessage *msg)
{
    // message arrived
    // Acknowledgment received!
    EV << "Received: " << msg->getName() << "\n";
    delete msg;
    // Ready to send another one.
    message = generateNewMessage();
    send(message, "out");
}

cMessage* Sender::generateNewMessage()
{
    // Generate a message with a different name every time.
    char msgname[20];
    sprintf(msgname, "message-%d", ++seq);
    cMessage *msg = new cMessage(msgname);
    return msg;
}

/**
 * Sends back an acknowledgement -- or not.
 */
class Receiver : public cSimpleModule
{
  protected:
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Receiver);

void Receiver::handleMessage(cMessage *msg)
{
    EV << msg << " received, sending back an acknowledgement.\n";
    delete msg;
    send(new cMessage("ack"), "out");
}
