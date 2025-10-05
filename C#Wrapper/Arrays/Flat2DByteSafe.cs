using System.Collections;
using System.Runtime.InteropServices;
using System.Text;

namespace C_Wrapper.Arrays
{
    using size_t = UInt64;
    public class Flat2DByteSafe : IList<byte>
    {
        private size_t Width, Height, Length;

        private readonly byte[] Array;

        private readonly bool AllowChangeSize;
        private size_t index;

        public Flat2DByteSafe(Flat2DByte flatArray)
        {
            this.Width = flatArray.Width;
            this.Height = flatArray.Height;
            this.Length = this.Width * this.Height;

            this.AllowChangeSize = false;

            IntPtr DataPtr = APIWrapper.Flat2DByte_GetDataPtr(flatArray.Ptr);
            int Size = (int)(Width * Height);

            this.Array = new byte[Size];

            Marshal.Copy(DataPtr, this.Array, 0, Size);
        }

        public Flat2DByteSafe(size_t Width, size_t Height)
        {
            this.Width = Width;
            this.Height = Height;
            this.Length = Width * Height;

            this.AllowChangeSize = true;

            this.Array = new byte[Length];
        }

        public byte this[int index] { get => Array[index]; set => Array[index] = value; }
        public byte this[size_t x, size_t y]
        {
            get
            {
                return Array[y * Width + x];
            }
            set
            {
                Array[y * Width + x] = value;
            }
        }

        public int Count => (int)Length;

        public bool IsReadOnly => false;

        public void Add(byte item)
        {
            if (this.AllowChangeSize == false) throw new NotImplementedException();
            Array[index] = item;
            index++;
        }

        public void Clear()
        {
            for (size_t x = 0; x < Length; x++) { Array[x] = 0; }
        }

        public bool Contains(byte item)
        {
            for (size_t x = 0; x < Length; x++)
            {
                if (Array[x] == item) return true;
            }
            return false;
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
            if (this.AllowChangeSize == false) throw new NotImplementedException();
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

        public override string ToString()
        {
            StringBuilder sb = new();

            for (int x = 0; x < (int)this.Length; x++)
            {
                if (x != 0 && x % (int)Width == 0) sb.AppendLine();
                sb.Append($"{this.Array[x]} ");
            }

            return sb.ToString();
        }
    }
}
