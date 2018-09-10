﻿using System;

namespace Qml.Net.Hosting
{
    class Program
    {
        public class TestObject
        {
            public void TestMethod()
            {
                Console.WriteLine("test method");
            }
        }
        
        static int Main(string[] _)
        {
            return Host.Run(_, (args, app, engine, runCallback) =>
            {
                QQmlApplicationEngine.RegisterType<TestObject>("test");
                return runCallback();
            });
        }
    }
}
