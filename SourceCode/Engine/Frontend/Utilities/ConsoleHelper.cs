// Copyright 2016-2017 ?????????????. All Rights Reserved.
using System;

namespace Engine.Frontend
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
