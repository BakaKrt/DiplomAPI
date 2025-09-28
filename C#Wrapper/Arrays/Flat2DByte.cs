using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace C_Wrapper.Arrays
{
    using size_t = UInt64;
    public class Flat2DByte : IDisposable
    {

        public IntPtr _ptr { get; private set; } = IntPtr.Zero;
        private unsafe byte* _dataPtr = null;

        public readonly size_t Width;
        public readonly size_t Height;


        public unsafe Flat2DByte(size_t Width, size_t Height)
        {
            this.Width = Width;
            this.Height = Height;
            _ptr = APIWrapper.CreateFlat2DByte(Width, Height);

            if (_ptr == IntPtr.Zero)
            {
                throw new NullReferenceException($"{nameof(Flat2DByte)} creation with dimensions: {Width}, {Height} got a nullptr");
            }

            _dataPtr = (byte*)APIWrapper.Flat2DByte_GetDataPtr(_ptr);
        }

        public Flat2DByteSafe ToSafe(bool DeleteThis = false)
        {
            if (DeleteThis == false)
            {
                return new Flat2DByteSafe(this);
            }
            else
            {
                Flat2DByteSafe SafeBytes = new(this);
                this.Dispose();
                return SafeBytes;
            }
        }

        public unsafe byte this[size_t x]
        {
            get
            {
#if DEBUG
                CheckForRange(x); // Проверка только в Debug
#endif
                return _dataPtr[x];
            }
            set
            {
#if DEBUG
                CheckForRange(x); // Проверка только в Debug
#endif
                _dataPtr[x] = value;
            }
        }

        public unsafe byte this[size_t x, size_t y]
        {
            get
            {
#if DEBUG
                CheckForRange(x, y); // Проверка только в Debug
#endif
                return _dataPtr[y * Width + x];
            }
            set
            {
#if DEBUG
                CheckForRange(x, y); // Проверка только в Debug
#endif
                _dataPtr[y * Width + x] = value;
            }
        }

        private unsafe void CheckForRange(UInt64 x)
        {
            if (_dataPtr == null || _ptr == IntPtr.Zero) { throw new ObjectDisposedException(nameof(Flat2DByte)); }
            if (x >= this.Width * Height || x < 0) { throw new IndexOutOfRangeException(); }
        }

        private unsafe void CheckForRange(UInt64 x, UInt64 y)
        {
            if (_dataPtr == null || _ptr == IntPtr.Zero) { throw new ObjectDisposedException(nameof(Flat2DByte)); }
            if (x >= this.Width || x < 0) { throw new IndexOutOfRangeException(); }
            if (y >= this.Height || y < 0) { throw new IndexOutOfRangeException(); }
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
            Console.WriteLine($"{nameof(Flat2DByte)}:Dispose");
        }

        protected virtual unsafe void Dispose(bool disposing)
        {
            if (disposing && _ptr != IntPtr.Zero)
            {
                APIWrapper.DestroyFlat2DByte(_ptr);
                _ptr = IntPtr.Zero;
                _dataPtr = null;
            }
        }

        public override string ToString()
        {
            StringBuilder sb = new();

            UInt64 Length = this.Width * Height;

            for (UInt64 x = 0; x < Length; x++)
            {
                if (x != 0 && x % Width == 0) sb.AppendLine();
                sb.Append($"{this[x]} ");
            }

            return sb.ToString();
        }
    }
}
