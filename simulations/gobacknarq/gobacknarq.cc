#include <stdio.h>
#include <string.h>
#include <omnetpp.h>
#include <bits/stdc++.h>
#include "packet_m.h"

using namespace omnetpp;
using namespace std;

class Sender : public cSimpleModule
{
  private:
    simtime_t timeout, pipelineDelay, delay;  // timeout
    int seq, frameNumber;  // message sequence number
    int windowSize, ackSeq; // For Discarding multiple almost simultaneous acks with same seq;
    queue<Packet *> messages; // the sliding window of messages that has to be re-sent on timeout
    queue<Packet *> timeouts; // Managing timeouts
  public:
    Sender();
    virtual ~Sender();

  protected:
    virtual void generateNewMessage();
    virtual void sendMessages();
    virtual void initialize() override;
    virtual void handleMessage(cMessage *tmsg) override;
};

Define_Module(Sender);

Sender::~Sender()
{
    while(!messages.empty())
    {
        Packet *message = messages.front();
        messages.pop();
        delete message;
    }
    while(!timeouts.empty())
    {
        Packet *timeoutEvent = timeouts.front();
        cancelAndDelete(timeoutEvent);
        timeouts.pop();
    }
}
Sender::Sender()
{

}
void Sender::initialize()
{
    // Initialize variables.
    seq = -1;
    windowSize = 3;
    frameNumber = 0;
    pipelineDelay = 0.1;
    timeout = 1 + pipelineDelay * (windowSize + 1);
    delay = 0.0;
    ackSeq = 0;
    // Send Queue of Messages
    while(messages.size() < windowSize)
        generateNewMessage();
    sendMessages();
}

void Sender::handleMessage(cMessage *tmsg)
{
    Packet *msg = check_and_cast<Packet *>(tmsg);
    EV << msg << endl;
    if(msg->isSelfMessage()) // msg transmitted from Sender => timeout msg->isSelfMessage()
    {
        Packet *timeoutEvent, *message;
        while(!timeouts.empty()) // Removing Successfully Transmitted Messages and their Timeouts
        {
            EV << "Checking if "<< messages.front() <<" has timed out\n";
            message = messages.front();
            timeoutEvent = timeouts.front();
            cancelEvent(timeoutEvent);
            timeouts.pop();
            if(timeoutEvent == msg)
                break;
            EV << "Timer cancelled and Message Deleted"<<message->getName()<<"\n";
            delete timeoutEvent;
            delete message;
            messages.pop();
        }
        if(messages.size() > 0) // Cancelling Timers of messages to be sent again
        {
            queue<Packet *> temptimeouts;
            while(!timeouts.empty())
            {
                cancelEvent(timeouts.front());
                EV << "Timer cancelled "<<timeouts.front()->getName()<<"\n";
                temptimeouts.push(timeouts.front());
                timeouts.pop();
            }
            timeouts.push(timeoutEvent);
            while(!temptimeouts.empty())
            {
                timeouts.push(temptimeouts.front());
                temptimeouts.pop();
            }
        }

        while(messages.size() < windowSize) // Preparing Window of Messages
                generateNewMessage();
        sendMessages();
    }
    else {
        if (uniform(0, 1) < 0.1) { // Ack Lost
            EV << "\"Losing\" Acknowledgement " << msg << endl;
            bubble("Acknowledgement lost");
        }
        else if(ackSeq == msg->getSeq())
        {
            EV << "Discarding Acknowledgement" << msg << endl;
            bubble("Acknowledgement Siscarded");
        }
        else {
            ackSeq = msg->getSeq();
            Packet *message, *timeoutEvent;
            // message arrived
            // Acknowledgment received!
            EV << "Received: " << msg->getName() << "\n";
            while(!messages.empty()) // Removing Successfully Transmitted Messages and Their Timeouts
            {
                EV << "Checking if "<< messages.front() <<" was transmitted\n";
                message = messages.front();
                timeoutEvent = timeouts.front();
                cancelEvent(timeoutEvent);
                timeouts.pop();
                if(msg->getSeq() == message->getSeq())
                    break;
                EV << "Message Deleted and Timer cancelled "<<message->getName()<<"\n";
                delete message;
                delete timeoutEvent;
                messages.pop();
            }
            if(messages.size() > 0)
            {
                queue<Packet *> temptimeouts;
                while(!timeouts.empty())
                {
                    EV << "Timer cancelled "<<timeouts.front()->getName()<<"\n";
                    cancelEvent(timeouts.front());
                    temptimeouts.push(timeouts.front());
                    timeouts.pop();
                }
                timeouts.push(timeoutEvent);
                while(!temptimeouts.empty())
                {
                    timeouts.push(temptimeouts.front());
                    temptimeouts.pop();
                }
            }

            while(messages.size() < windowSize) // Preparing Window of Messages
                    generateNewMessage();
            sendMessages();
        }
        delete msg;
    }
}

void Sender::generateNewMessage()
{
    // Generate a message with a different name every time.
    char msgname[20];
    sprintf(msgname, "message-%d", ++seq);
    Packet *msg = new Packet(msgname);
    msg->setSeq(frameNumber);
    messages.push(msg);
    sprintf(msgname, "timeout-message-%d", seq);
    // Generate corresponding timeout
    Packet *timeoutEvent = new Packet(msgname);
    timeoutEvent->setSeq(frameNumber);
    timeouts.push(timeoutEvent);
    frameNumber = (frameNumber + 1) % (windowSize + 1);
}

void Sender::sendMessages()
{
    delay = 0.0;
    for(int i = 0; i < windowSize; i++)
    {
        // Send Message and Set Timeout
        Packet *message = messages.front(), *copy = message->dup();
        EV << "Sending Message: " << message <<"\n";
        sendDelayed(copy, delay, "out");
        Packet *timeoutEvent = timeouts.front();
        scheduleAt(simTime() + (delay + timeout), timeoutEvent);
        delay += pipelineDelay;
        messages.pop();
        messages.push(message);
        timeouts.pop();
        timeouts.push(timeoutEvent);
    }
}
/**
 * Sends back an acknowledgement -- or not.
 */
class Receiver : public cSimpleModule
{
    private:
        int seq, frameCount, windowSize;
    protected:
        virtual void handleMessage(cMessage *msg) override;
        virtual void initialize() override;
};

Define_Module(Receiver);

void Receiver::initialize()
{
    seq = frameCount = 0; // Initializing Seq Number to receive
    windowSize = 3;
}
void Receiver::handleMessage(cMessage *tmsg)
{
    Packet *msg = check_and_cast<Packet *>(tmsg);
    if (uniform(0, 1) < 0.1) {
        EV << "\"Losing\" message " << msg << endl;
        bubble("message lost");
        delete msg;
    }
    else {
        EV << msg << " received.\n";
        if(msg->getSeq() != seq)
        {
            EV << msg << " not in sequence. Discarding ...\n";
            bubble("message discarded");
            delete msg;
            char msgname[20];
            sprintf(msgname, "ack-%d", seq);
            Packet *ack = new Packet(msgname);
            ack->setSeq(seq);
            send(ack, "out");
            frameCount = 0;
        }
        else
        {
            frameCount++;
            ++seq;
            seq %= (windowSize + 1);
            delete msg;
            if(frameCount == windowSize)
            {
                char msgname[20];
                sprintf(msgname, "ack-%d", seq);
                Packet *ack = new Packet(msgname);
                ack->setSeq(seq);
                send(ack, "out");
                EV << " Sending ack: " << ack << ".\n";
                frameCount = 0;
            }
        }
    }
}
