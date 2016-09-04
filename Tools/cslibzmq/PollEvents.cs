namespace ZeroMQ.Interop
{
    using System;

    [Flags]
    public enum PollEvents : short
    {
        None = 0x0,
        PollIn = 0x1,
        PollOut = 0x2,
        PollErr = 0x4
    }
}
