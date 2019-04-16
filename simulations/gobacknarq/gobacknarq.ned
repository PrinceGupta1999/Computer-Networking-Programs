simple Sender
{
    parameters:
        @display("i=block/routing");
    gates:
        input in;
        output out;
}

simple Receiver
{
    parameters:
        @display("i=block/process");
    gates:
        input in;
        output out;
}

network GoBackNARQ
{
    @display("bgb=200,197");
    submodules:
        sender: Sender {
            parameters:
                @display("i=,cyan;p=39,153");
        }
        receiver: Receiver {
            parameters:
                @display("i=,gold;p=156,38");
        }
    connections:
        sender.out --> {  delay = 500ms; } --> receiver.in;
        sender.in <-- {  delay = 500ms; } <-- receiver.out;
}
