namespace C_Wrapper
{
    internal class Program
    {
        static unsafe void Main(string[] args)
        {
            Console.WriteLine("Hello, World!");
            Flat2DByte Matrix = new(10, 10);
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
