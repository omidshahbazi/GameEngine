// Copyright 2012-2015 ?????????????. All Rights Reserved.
using System;

namespace Frontend
{
    static class ConsoleHelper
    {
        public static void WriteLineInfo(string Text)
        {
            Console.ForegroundColor = ConsoleColor.White;
            Console.WriteLine(Text);
        }

        public static void WriteLineWarning(string Text)
        {
            Console.ForegroundColor = ConsoleColor.Yellow;
            Console.WriteLine(Text);
        }

        public static void WriteLineError(string Text)
        {
            Console.ForegroundColor = ConsoleColor.Red;
            Console.WriteLine(Text);
        }
    }
}
