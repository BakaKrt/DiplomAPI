﻿using System.Text;

namespace C_Wrapper.Arrays
{
    using size_t = UInt64;
    public class HeightMap : IDisposable
    {
        public IntPtr Ptr { get; private set; } = IntPtr.Zero;
        private Flat2DByte _dataPtr;

        public readonly size_t Width;
        public readonly size_t Height;

        /// <summary>
        /// Стандартный конструктор
        /// </summary>
        /// <param name="width">Ширина</param>
        /// <param name="height">Высота</param>
        /// <param name="threadCount">Количество потоков (0 для максимального количества согласно системе), по умолчанию 2</param>
        /// <param name="setRandom">Случайное заполнение значениями, по умолчанию true</param>
        /// <exception cref="NullReferenceException"></exception>
        public unsafe HeightMap(size_t width, size_t height, size_t threadCount = 2, bool setRandom = true)
        {
            this.Width = width;
            this.Height = height;

            Ptr = APIWrapper.HeightMap_Create(width, height, threadCount, setRandom);
            if (Ptr == IntPtr.Zero)
            {
                throw new NullReferenceException($"{nameof(Flat2DByte)} creation with dimensions: {width}, {height} got a nullptr");
            }

            _dataPtr = new Flat2DByte(APIWrapper.HeightMap_GetMatrix(Ptr));
        }

        public void Tick(size_t count = 1)
        {
            APIWrapper.HeightMap_Tick(Ptr, count);
            _dataPtr = new Flat2DByte(APIWrapper.HeightMap_GetMatrix(Ptr));
        }

        public void TickMT(size_t count = 1)
        {
            APIWrapper.HeightMap_TickMT(Ptr, count);
            _dataPtr = new Flat2DByte(APIWrapper.HeightMap_GetMatrix(Ptr));
        }

        public bool SetRules(bool[] array)
        {
            if (array.Length == 8)
            {
                APIWrapper.HeightMap_SetRules(Ptr, array);
                return true;
            }
            return false;
        }

        public void MakeGood(int type = 0)
        {
            APIWrapper.HeightMap_MakeGood(Ptr, type);
        }

        public void Normalize()
        {
            APIWrapper.HeightMap_Normalize(Ptr);
        }

        public unsafe byte this[size_t x]
        {
            get => _dataPtr[x];
            set
            {
                _dataPtr[x] = value;
            }
        }

        public unsafe byte this[size_t x, size_t y]
        {
            get
            {
                return _dataPtr[y * Width + x];
            }
            set
            {
                _dataPtr[y * Width + x] = value;
            }
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
            Console.WriteLine($"{nameof(HeightMap)}:Dispose");
        }

        protected virtual unsafe void Dispose(bool disposing)
        {
            if (disposing && Ptr != IntPtr.Zero)
            {
                APIWrapper.HeightMap_Destroy(Ptr);
                Ptr = IntPtr.Zero;
                _dataPtr = null!;
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
