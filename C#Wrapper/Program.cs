using C_Wrapper.Arrays;


namespace C_Wrapper
{
    using size_t = UInt64;
    internal class Program
    {
        static unsafe void Main(string[] args)
        {
            size_t SizeX = 10, SizeY = 10;

            HeightMap map = new(SizeX, SizeY);
            Console.WriteLine($"{map}\n");

            map.MakeGood(1);

            Console.WriteLine($"{map}\n");

            map.Dispose();
        }
    }
}
