﻿using AwesomeSockets.Domain.Sockets;
using AwesomeSockets.Sockets;
using Network;
using Network.Converter;
using Network.Enums;
using Network.Extensions;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
using System.Runtime.Serialization.Formatters.Binary;
using System.Text;
using System.Threading.Tasks;

namespace RE4MP
{
    public class Server
    {
        private ServerConnectionContainer serverConnectionContainer;

        public void StartServer(Trainer trainer)
        {
            Console.WriteLine("Please enter the server port and press return:");
            string port = Console.ReadLine();


            ISocket listenSocket = AweSock.TcpListen(int.Parse(port));

            Console.WriteLine("Server started");
            ISocket client = AweSock.TcpAccept(listenSocket);

            var inBuf = AwesomeSockets.Buffers.Buffer.New(99999);
            var outBuf = AwesomeSockets.Buffers.Buffer.New(99999);

            while(true)
            {
                //get message
                Tuple<int, EndPoint> received = AweSock.ReceiveMessage(client, inBuf);
                AwesomeSockets.Buffers.Buffer.FinalizeBuffer(inBuf);

                //parse message
                var res = Utils.Deserialize(AwesomeSockets.Buffers.Buffer.GetBuffer(inBuf));
                AwesomeSockets.Buffers.Buffer.ClearBuffer(inBuf);

                //act on message
                //Console.WriteLine(string.Join(", ", res["pos_ally"]));
                this.HandleInputData(res, trainer);

                //get response data
                var outputData = this.GetOutputData(trainer);

                //write to buffer
                AwesomeSockets.Buffers.Buffer.ClearBuffer(outBuf);
                AwesomeSockets.Buffers.Buffer.Add(outBuf, Utils.ObjectToByteArray(outputData));
                AwesomeSockets.Buffers.Buffer.FinalizeBuffer(outBuf);

                //respond
                int bytesSent = AweSock.SendMessage(client, outBuf);
                Console.WriteLine("sent data");
            }

            Console.ReadLine();
        }

        private Dictionary<string, byte[]> GetOutputData(Trainer trainer)
        {
            var outputData = new Dictionary<string, byte[]>();

            outputData.Add("write_pos_ally", trainer.GET_POS_ALLY());

            return outputData;
        }

        private void HandleInputData(Dictionary<string, byte[]> data, Trainer trainer)
        {
            trainer.WRITE_POS_ALLY(data["write_pos_ally"]);
        }
    }
}
