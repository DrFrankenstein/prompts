using System;
using System.IO;
using System.Collections.Generic;
using System.Linq;

namespace Aoc1
{
    static class Program
    {
        static IEnumerable<char> Characters(TextReader input)
        {
            int c;
            while ((c = input.Read()) > 0)
                yield return (char) c;
        }

        static int RunThruElevator(this IEnumerable<int> commands)
        {
            int floor = 0, step = 0;
            bool beenInBasement = false;
            foreach (int command in commands)
            {
                floor += command;
                ++step;

                if (!beenInBasement && floor == -1)
                {
                    Console.WriteLine("Note: first entered basement at step {0}", step);
                    beenInBasement = true;
                }
            }

            return floor;
        }

        static void Main(string[] args)
        {
            var floor = Characters(Console.In)
                .Select((ch) =>
                {
                    switch (ch)
                    {
                        case '(': return 1;
                        case ')': return -1;
                        default: return 0;
                    }
                })
                .RunThruElevator()
            ;

            Console.WriteLine("Floor: {0}", floor);
        }
    }
}