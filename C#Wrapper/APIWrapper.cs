using System.Runtime.InteropServices;



namespace C_Wrapper
{
    using size_t = UInt64;
    //using ptr = IntPtr;
    public class APIWrapper
    {
        private const string DllName = "C++DLL"; // Имя вашей DLL

        #region HeightMap
        /// <summary>
        /// Создать объект HeightMap и вернуть указатель на него
        /// </summary>
        /// <param name="width"></param>
        /// <param name="height"></param>
        /// <param name="threadCount"></param>
        /// <param name="setRandomValue"></param>
        /// <returns></returns>
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr HeightMap_Create(size_t width, size_t height, [MarshalAs(UnmanagedType.U8)] size_t threadCount,[MarshalAs(UnmanagedType.Bool)] bool setRandomValue = true);


        /// <summary>
        /// Установить правила
        /// </summary>
        /// <param name="obj"></param>
        /// <param name="rules"></param>
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void HeightMap_SetRules(IntPtr obj, [MarshalAs(UnmanagedType.LPArray, SizeConst = 8)] bool[] rules);

        /// <summary>
        /// Установить правила по отдельным параметрам
        /// </summary>
        /// <param name="obj"></param>
        /// <param name="r1"></param>
        /// <param name="r2"></param>
        /// <param name="r3"></param>
        /// <param name="r4"></param>
        /// <param name="r5"></param>
        /// <param name="r6"></param>
        /// <param name="r7"></param>
        /// <param name="r8"></param>
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

        /// <summary>
        /// Выполнить count шагов симуляции
        /// </summary>
        /// <param name="obj"></param>
        /// <param name="count"></param>
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void HeightMap_Tick(IntPtr obj, [MarshalAs(UnmanagedType.U8)] size_t count);
        
        /// <summary>
        /// Выполнить count шагов симуляции
        /// </summary>
        /// <param name="obj"></param>
        /// <param name="count"></param>
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void HeightMap_TickMT(IntPtr obj, [MarshalAs(UnmanagedType.U8)] size_t count);

        /// <summary>
        /// Получить указатель на матрицу
        /// </summary>
        /// <param name="obj"></param>
        /// <returns></returns>
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr HeightMap_GetMatrix(IntPtr obj);

        /// <summary>
        /// Получить ширину карты
        /// </summary>
        /// <param name="obj"></param>
        /// <returns></returns>
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern size_t HeightMap_GetWidth(IntPtr obj);

        /// <summary>
        /// Получить высоту карты
        /// </summary>
        /// <param name="obj"></param>
        /// <returns></returns>
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern size_t HeightMap_GetHeight(IntPtr obj);

        /// <summary>
        /// Сделать хорошо
        /// </summary>
        /// <param name="obj"></param>
        /// <param name="type"></param>
        /// <returns></returns>
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern size_t HeightMap_MakeGood(IntPtr obj, int type = 1);
        
        /// <summary>
        /// Нормализация карты высот
        /// </summary>
        /// <param name="obj"></param>
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void HeightMap_Normalize(IntPtr obj);

        /// <summary>
        /// Уничтожить объект HeightMap
        /// </summary>
        /// <param name="obj"></param>
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void HeightMap_Destroy(IntPtr obj);

        #endregion

        #region Flat2DByte
        /// <summary>
        /// Создать объект Flat2DByte и вернуть указатель на него
        /// </summary>
        /// <param name="width"></param>
        /// <param name="height"></param>
        /// <returns></returns>
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr CreateFlat2DByte(size_t width, size_t height);

        /// <summary>
        /// Установить значение
        /// </summary>
        /// <param name="obj"></param>
        /// <param name="x"></param>
        /// <param name="y"></param>
        /// <param name="value"></param>
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void Flat2DByte_SetData(IntPtr obj, size_t x, size_t y, byte value);

        /// <summary>
        /// Получить значение
        /// </summary>
        /// <param name="obj"></param>
        /// <param name="x"></param>
        /// <param name="y"></param>
        /// <returns></returns>
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern byte Flat2DByte_GetData(IntPtr obj, size_t x, size_t y);

        /// <summary>
        /// Получить указатель на данные
        /// </summary>
        /// <param name="obj"></param>
        /// <returns></returns>
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr Flat2DByte_GetDataPtr(IntPtr obj);

        /// <summary>
        /// Получить размер данных
        /// </summary>
        /// <param name="obj"></param>
        /// <returns></returns>
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern size_t Flat2DByte_GetDataSize(IntPtr obj);

        /// <summary>
        /// Получить ширину
        /// </summary>
        /// <param name="obj"></param>
        /// <returns></returns>
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern size_t Flat2DByte_GetWidth(IntPtr obj);

        /// <summary>
        /// Получить высоту
        /// </summary>
        /// <param name="obj"></param>
        /// <returns></returns>
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern size_t Flat2DByte_GetHeight(IntPtr obj);

        /// <summary>
        /// Уничтожить объект Flat2DByte
        /// </summary>
        /// <param name="obj"></param>
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void DestroyFlat2DByte(IntPtr obj);
        #endregion

        #region Flat2DFloat
        /// <summary>
        /// Создать объект Flat2DByte и вернуть указатель на него
        /// </summary>
        /// <param name="width"></param>
        /// <param name="height"></param>
        /// <returns></returns>
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr CreateFlat2DFloat(size_t width, size_t height);

        /// <summary>
        /// Установить значение
        /// </summary>
        /// <param name="obj"></param>
        /// <param name="x"></param>
        /// <param name="y"></param>
        /// <param name="value"></param>
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void Flat2DFloat_SetData(IntPtr obj, size_t x, size_t y, byte value);

        /// <summary>
        /// Получить значение
        /// </summary>
        /// <param name="obj"></param>
        /// <param name="x"></param>
        /// <param name="y"></param>
        /// <returns></returns>
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern byte Flat2DFloat_GetData(IntPtr obj, size_t x, size_t y);

        /// <summary>
        /// Получить указатель на данные
        /// </summary>
        /// <param name="obj"></param>
        /// <returns></returns>
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr Flat2DFloat_GetDataPtr(IntPtr obj);

        /// <summary>
        /// Получить размер данных
        /// </summary>
        /// <param name="obj"></param>
        /// <returns></returns>
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern size_t Flat2DFloat_GetDataSize(IntPtr obj);

        /// <summary>
        /// Получить ширину
        /// </summary>
        /// <param name="obj"></param>
        /// <returns></returns>
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern size_t Flat2DFloat_GetWidth(IntPtr obj);

        /// <summary>
        /// Получить высоту
        /// </summary>
        /// <param name="obj"></param>
        /// <returns></returns>
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern size_t Flat2DFloat_GetHeight(IntPtr obj);

        /// <summary>
        /// Уничтожить объект Flat2DFloat
        /// </summary>
        /// <param name="obj"></param>
        [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void DestroyFlat2DFloat(IntPtr obj);
        #endregion
    }
}