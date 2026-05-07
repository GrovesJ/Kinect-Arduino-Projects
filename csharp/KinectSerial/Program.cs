using System;
using System.IO.Ports;
using Microsoft.Kinect;

class Program
{
    static KinectSensor sensor;
    static SerialPort serial;
    static DepthImagePixel[] depthPixels;

    static void Main(string[] args)
    {
        string comPort = args.Length > 0 ? args[0] : "COM5";

        if (KinectSensor.KinectSensors.Count == 0)
        {
            Console.WriteLine("No Kinect sensor detected.");
            return;
        }

        try
        {
            serial = new SerialPort(comPort, 115200);
            serial.Open();
        }
        catch (Exception ex)
        {
            Console.WriteLine($"Failed to open serial port ({comPort}): {ex.GetType().Name}: {ex.Message}");
            Console.WriteLine("Common causes: another program (Arduino Serial Monitor) is using the port, or insufficient permissions.");
            Console.WriteLine("Try: close other apps using the port, run the app as Administrator, or choose a different COM port.");
            return;
        }

        sensor = KinectSensor.KinectSensors[0];
        sensor.DepthStream.Enable(DepthImageFormat.Resolution320x240Fps30);
        depthPixels = new DepthImagePixel[sensor.DepthStream.FramePixelDataLength];

        sensor.DepthFrameReady += Sensor_DepthFrameReady;
        sensor.Start();

        Console.WriteLine("Running. Press Enter to quit.");
        Console.ReadLine();

        sensor.Stop();
        if (serial != null)
        {
            try
            {
                if (serial.IsOpen)
                {
                    serial.Close();
                }
                serial.Dispose();
            }
            catch (Exception) { }
        }
    }

    static void Sensor_DepthFrameReady(object sender, DepthImageFrameReadyEventArgs e)
    {
        using (DepthImageFrame frame = e.OpenDepthImageFrame())
        {
            if (frame == null)
            {
                return;
            }

            frame.CopyDepthImagePixelDataTo(depthPixels);

            int width = frame.Width;
            int height = frame.Height;
            int cx = width / 2;
            int cy = height / 2;

            int box = 20;
            int count = 0;
            int sum = 0;

            for (int y = cy - box / 2; y < cy + box / 2; y++)
            {
                for (int x = cx - box / 2; x < cx + box / 2; x++)
                {
                    int idx = (y * width) + x;
                    int depth = depthPixels[idx].Depth;

                    if (depth > 0)
                    {
                        sum += depth;
                        count++;
                    }
                }
            }

            if (count == 0)
            {
                return;
            }

            int avgDepthMm = sum / count;
            int speed = MapClamp(avgDepthMm, 500, 3000, 255, 0);

            serial.Write(new byte[] { (byte)speed }, 0, 1);
            Console.WriteLine($"Depth {avgDepthMm} mm -> Speed {speed}");
        }
    }

    static int MapClamp(int v, int inMin, int inMax, int outMin, int outMax)
    {
        if (v < inMin)
        {
            v = inMin;
        }
        if (v > inMax)
        {
            v = inMax;
        }

        return (v - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
    }
}
