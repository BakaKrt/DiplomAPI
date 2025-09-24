using System.Collections;
using System.Data.Common;
using System.Drawing;
using System.Runtime.InteropServices;
using System.Text;

namespace C_Wrapper
{
    using size_t = UInt64;
    //using ptr = IntPtr;
    public class APIWrapper
    {
        private const string DllName = "C++DLL"; // Имя вашей DLL

        // ==================== HeightMap API ====================

        // Создать объект HeightMap и вернуть указатель на него
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr CreateHeightMap(size_t width, size_t height, [MarshalAs(UnmanagedType.Bool)] bool setRandomValue);

        // Установить правила
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void HeightMap_SetRules(IntPtr obj, [MarshalAs(UnmanagedType.LPArray, SizeConst = 8)] bool[] rules);

        // Установить правила по отдельным параметрам
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void HeightMap_SetRulesEx(IntPtr obj,
                                                       [MarshalAs(UnmanagedType.Bool)] bool r1,
                                                       [MarshalAs(UnmanagedType.Bool)] bool r2,
                                                       [MarshalAs(UnmanagedType.Bool)] bool r3,
                                                       [MarshalAs(UnmanagedType.Bool)] bool r4,
                                                       [MarshalAs(UnmanagedType.Bool)] bool r5,
                                                       [MarshalAs(UnmanagedType.Bool)] bool r6,
                                                       [MarshalAs(UnmanagedType.Bool)] bool r7,
                                                       [MarshalAs(UnmanagedType.Bool)] bool r8);

        // Выполнить один шаг симуляции
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void HeightMap_Tick(IntPtr obj);

        // Получить указатель на матрицу
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr HeightMap_GetMatrix(IntPtr obj);

        // Получить ширину карты
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern size_t HeightMap_GetWidth(IntPtr obj);

        // Получить высоту карты
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern size_t HeightMap_GetHeight(IntPtr obj);

        // Уничтожить объект HeightMap
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void DestroyHeightMap(IntPtr obj);

        // ==================== Flat2DByte API ====================

        // Создать объект Flat2DByte и вернуть указатель на него
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr CreateFlat2DByte(size_t width, size_t height);

        // Установить значение
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void Flat2DByte_SetData(IntPtr obj, size_t x, size_t y, byte value);

        // Получить значение
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern byte Flat2DByte_GetData(IntPtr obj, size_t x, size_t y);

        // Получить указатель на данные
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr Flat2DByte_GetDataPtr(IntPtr obj);

        // Получить размер данных
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern size_t Flat2DByte_GetDataSize(IntPtr obj);

        // Получить ширину
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern size_t Flat2DByte_GetWidth(IntPtr obj);

        // Получить высоту
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern size_t Flat2DByte_GetHeight(IntPtr obj);

        // Уничтожить объект Flat2DByte
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void DestroyFlat2DByte(IntPtr obj);
    }

    public class Flat2DByte: IDisposable
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

    public class Flat2DByteSafe: IList<byte>
    {
        private size_t Width, Height, Length;

        private readonly byte[] Array;

        public Flat2DByteSafe(Flat2DByte flatArray)
        {
            this.Width = flatArray.Width;
            this.Height = flatArray.Height;

            this.Length = this.Width * this.Height;
            
            IntPtr DataPtr = APIWrapper.Flat2DByte_GetDataPtr(flatArray._ptr);
            int Size = (int)(Width * Height);

            this.Array = new byte[Size];

            Marshal.Copy(DataPtr, this.Array, 0, Size);
        }

        public Flat2DByteSafe(size_t Width, size_t Height)
        {
            this.Width = Width;
            this.Height = Height;
            this.Length = Width * Height;

            this.Array = new byte[Length];
        }

        public byte this[int index] { get => throw new NotImplementedException(); set => throw new NotImplementedException(); }

        public int Count => (int)Length;

        public bool IsReadOnly => false;

        public void Add(byte item)
        {
            throw new NotImplementedException();
        }

        public void Clear()
        {
            throw new NotImplementedException();
        }

        public bool Contains(byte item)
        {
            throw new NotImplementedException();
        }

        public void CopyTo(byte[] array, int arrayIndex)
        {
            throw new NotImplementedException();
        }

        public IEnumerator<byte> GetEnumerator()
        {
            throw new NotImplementedException();
        }

        public int IndexOf(byte item)
        {
            throw new NotImplementedException();
        }

        public void Insert(int index, byte item)
        {
            throw new NotImplementedException();
        }

        public bool Remove(byte item)
        {
            throw new NotImplementedException();
        }

        public void RemoveAt(int index)
        {
            throw new NotImplementedException();
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            return GetEnumerator();
        }
    }
}