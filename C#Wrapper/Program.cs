using C_Wrapper.Arrays;


namespace C_Wrapper
{
    internal class Program
    {
        static unsafe void Main(string[] args)
        {
            Console.WriteLine("Hello, World!");

            UInt64 SizeX = 10, SizeY = 10;

            Flat2DByte Matrix = new(SizeX, SizeY);

            Random rand = new();

            for(UInt64 x = 0; x < SizeX * SizeY; x++)
            {
                Matrix[x] = (byte)rand.Next(255);
            }

            //Matrix[0, 0] = 255;
            //Matrix[0, 1] = 1;

            Console.WriteLine(Matrix);


            var t = Matrix.ToSafe(true);

            Console.WriteLine("\n\n\n");

            Console.WriteLine(t);

            //Console.WriteLine(Matrix);
        }
    }
}
