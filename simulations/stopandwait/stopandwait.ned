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

network StopAndWait
{
    submodules:
        sender: Sender {
            parameters:
                @display("i=,cyan");
        }
        receiver: Receiver {
            parameters:
                @display("i=,gold");
        }
    connections:
        sender.out --> {  delay = 100ms; } --> receiver.in;
        sender.in <-- {  delay = 100ms; } <-- receiver.out;
}
