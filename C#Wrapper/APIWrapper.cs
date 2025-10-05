using System.Runtime.InteropServices;



namespace C_Wrapper
{
    using size_t = UInt64;
    //using ptr = IntPtr;
    public class APIWrapper
    {
        private const string DllName = "C++DLL"; // Имя вашей DLL

        // ==================== HeightMap API ====================

        // Создать объект HeightMap и вернуть указатель на него
        //[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        //public static extern IntPtr HeightMap_Create(size_t width, size_t height, [MarshalAs(UnmanagedType.Bool)] bool setRandomValue = true);

        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr HeightMap_Create(size_t width, size_t height, [MarshalAs(UnmanagedType.U8)] size_t threadCount,[MarshalAs(UnmanagedType.Bool)] bool setRandomValue = true);


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

        // Выполнить count шагов симуляции
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void HeightMap_Tick(IntPtr obj, [MarshalAs(UnmanagedType.U8)] size_t count);
        
        // Выполнить count шагов симуляции
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void HeightMap_TickMT(IntPtr obj, [MarshalAs(UnmanagedType.U8)] size_t count);

        // Получить указатель на матрицу
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr HeightMap_GetMatrix(IntPtr obj);

        // Получить ширину карты
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern size_t HeightMap_GetWidth(IntPtr obj);

        // Получить высоту карты
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern size_t HeightMap_GetHeight(IntPtr obj);

        // Сделать хорошо
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern size_t HeightMap_MakeGood(IntPtr obj, int type = 1);
        
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void HeightMap_Normalize(IntPtr obj);

        // Уничтожить объект HeightMap
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void HeightMap_Destroy(IntPtr obj);

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
}