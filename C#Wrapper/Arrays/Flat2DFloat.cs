using System.Text;

namespace C_Wrapper.Arrays
{
    using size_t = UInt64;
    public class Flat2DFloat : IDisposable
    {
        public IntPtr Ptr { get; private set; } = IntPtr.Zero;
        public unsafe byte* DataPtr { get; private set; } = null;

        public readonly size_t Width;
        public readonly size_t Height;


        public unsafe Flat2DFloat(size_t width, size_t height)
        {
            this.Width = width;
            this.Height = height;
            Ptr = APIWrapper.CreateFlat2DFloat(width, height);

            if (Ptr == IntPtr.Zero)
            {
                throw new NullReferenceException($"{nameof(Flat2DFloat)} creation with dimensions: {width}, {height} got a nullptr");
            }

            DataPtr = (byte*)APIWrapper.Flat2DFloat_GetDataPtr(Ptr);
        }

        public unsafe Flat2DFloat(IntPtr pointer)
        {
            this.Ptr = pointer;
            this.Width = APIWrapper.Flat2DFloat_GetWidth(pointer);
            this.Height = APIWrapper.Flat2DFloat_GetHeight(pointer);
            this.DataPtr = (byte*)APIWrapper.Flat2DFloat_GetDataPtr(Ptr);
        }

        public static Flat2DFloat FromPointer(IntPtr pointer)
        {
            return new Flat2DFloat(pointer);
        }

        //public Flat2DFloatSafe ToSafe(bool deleteThis = false)
        //{
        //    if (deleteThis == false)
        //    {
        //        return new Flat2DFloatSafe(this);
        //    }
        //    else
        //    {
        //        Flat2DFloatSafe SafeBytes = new(this);
        //        this.Dispose();
        //        return SafeBytes;
        //    }
        //}

        public unsafe byte this[size_t x]
        {
            get
            {
#if DEBUG
                CheckForRange(x); // Проверка только в Debug
#endif
                return DataPtr[x];
            }
            set
            {
#if DEBUG
                CheckForRange(x); // Проверка только в Debug
#endif
                DataPtr[x] = value;
            }
        }

        public unsafe byte this[size_t x, size_t y]
        {
            get
            {
#if DEBUG
                CheckForRange(x, y); // Проверка только в Debug
#endif
                return DataPtr[y * Width + x];
            }
            set
            {
#if DEBUG
                CheckForRange(x, y); // Проверка только в Debug
#endif
                DataPtr[y * Width + x] = value;
            }
        }

        private unsafe void CheckForRange(size_t x)
        {
            if (DataPtr == null || Ptr == IntPtr.Zero) { throw new ObjectDisposedException(nameof(Flat2DFloat)); }
            if (x >= this.Width * Height || x < 0) { throw new IndexOutOfRangeException(); }
        }

        private unsafe void CheckForRange(size_t x, size_t y)
        {
            if (DataPtr == null || Ptr == IntPtr.Zero) { throw new ObjectDisposedException(nameof(Flat2DFloat)); }
            if (x >= this.Width || x < 0) { throw new IndexOutOfRangeException(); }
            if (y >= this.Height || y < 0) { throw new IndexOutOfRangeException(); }
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
            Console.WriteLine($"{nameof(Flat2DFloat)}:Dispose");
        }

        protected virtual unsafe void Dispose(bool disposing)
        {
            if (disposing && Ptr != IntPtr.Zero)
            {
                APIWrapper.DestroyFlat2DFloat(Ptr);
                Ptr = IntPtr.Zero;
                DataPtr = null;
            }
        }

        public override string ToString()
        {
            StringBuilder sb = new();

            size_t Length = this.Width * Height;

            for (size_t x = 0; x < Length; x++)
            {
                if (x != 0 && x % Width == 0) sb.AppendLine();
                sb.Append($"{this[x]} ");
            }

            return sb.ToString();
        }
    }
}
