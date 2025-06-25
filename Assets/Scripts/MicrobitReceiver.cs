using System;
using System.IO;
using System.Net.Sockets;
using System.Threading;
using UnityEngine;

public class MicrobitTCPReceiver : MonoBehaviour
{
    private TcpClient client;
    private StreamReader reader;
    private Thread receiveThread;
    private bool running = false;

    void Start()
    {
        try
        {
            client = new TcpClient("127.0.0.1", 12345);
            reader = new StreamReader(client.GetStream());
            running = true;
            receiveThread = new Thread(ReceiveLoop);
            receiveThread.Start();
            Debug.Log("Connected to C++ bridge.");
        }
        catch (Exception e)
        {
            Debug.LogError("TCP connection failed: " + e.Message);
        }
    }

    void ReceiveLoop()
    {
        while (running)
        {
            try
            {
                string line = reader.ReadLine();
                if (int.TryParse(line, out int address))
                {
                    Debug.Log("Received Address: " + address);
                    // 必要に応じて処理を分岐
                }
            }
            catch (Exception ex)
            {
                Debug.LogWarning("Receive error: " + ex.Message);
            }
        }
    }

    void OnApplicationQuit()
    {
        running = false;
        receiveThread?.Join();
        reader?.Close();
        client?.Close();
    }
}
