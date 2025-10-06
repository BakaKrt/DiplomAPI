# DiplomAPI

Представляет DLL на C++ и C#
C++DLL.dll написан на C++ (соглашение вызовов Cdecl), C#Wrapper на C#

## С#Wrapper:
везде используется using size_t = UInt64
Вся реализация завязана на **использовании памяти**, **выделенной в DLL** C++ (кроме Safe реализации), поэтому _требуется_ using или dispose() для освобождения памяти.
```c#
class HeightMap : IDisposable
```
### Конструктор:
```c#
HeightMap(size_t width, size_t height, size_t threadCount = 2, bool setRandom = true)
```
- `width` (`size_t`): Ширина создаваемой карты высот. Константа.
- `height` (`size_t`): Высота создаваемой карты высот. Константа.
- `threadCount` (`size_t`): Количество потоков, в которые будет обрабатывать метод TickMT. 0 для автоопределения по количеству потоков системы. По умолчанию 2. Нельзя изменить для уже созданного объекта!
- `setRandom` (`bool`): Стоит заполнить массив случайными значениями от 0 до 255. По умолчанию true.

### Tick в одном потоке
```c#
void Tick(size_t count = 1)
```
- `count` (`size_t`): Количество итераций. Если необходимо сделать несколько итераций за раз. По умолчанию 1.
Работает довольно быстро примерно до размера матрицы 150*150 [на процессоре Ryzen 7 5700x3d]

### Tick в многопопотоке
```c#
void TickMT(size_t count = 1)
```
- `count` (`size_t`): Количество итераций. Если необходимо сделать несколько итераций за раз. По умолчанию 1.
Работает быстро при размере матрицы более 150*150 [на процессоре Ryzen 7 5700x3d]

### Метод "Сделать хорошо"
```c#
void MakeGood(int type = 0)
```
- `type` (`size_t`): Тип. Всего 3, считая с 0 до 2


### Нормализация массива в диапазоне от 0 до 255
```c#
void Normalize()
```

## Пример использования
```c#
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
            map = new HeightMap(SizeX, SizeY, 0);
            map.TickMT(2);
            map.Normalize();
            map[0, 0] = 0; // Можно, идёт обращение напрямую к памяти
            Console.WriteLine(map[0, 0]); // Аналогично
            map.Dispose();
        }
    }
}

```
