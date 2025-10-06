using C_Wrapper.Arrays;


namespace C_Wrapper
{
    using size_t = UInt64;
    internal class Program
    {
        static void Main(string[] args)
        {
            size_t SizeX = 10, SizeY = 10;
            size_t threadCount = 2;
            HeightMap map = new(SizeX, SizeY, threadCount);
            Console.WriteLine($"{map}\n");

            map.MakeGood(1);

            Console.WriteLine($"{map}\n");

            map.Dispose();

            SizeX = 1000 * 16; SizeY = 1000 * 16;
            map = new HeightMap(SizeX, SizeY, threadCount);
            map.TickMT(2);
            map.Normalize();
            map[0, 0] = 0; // Можно, идёт обращение напрямую к памяти
            Console.WriteLine(map[0, 0]); // Аналогично
            map.Dispose();
        }
    }
}
