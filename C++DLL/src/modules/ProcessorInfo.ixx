#include <windows.h>
#include <iostream>

export module CPUInfo;

export namespace CpuHelper {

    struct CpuInfo {
        size_t L1_size;
        size_t L2_size;
        size_t L3_size;
        short cores;
        short threads;

        bool Init()
        {
            DWORD length = 0;
            // Сначала получаем размер буфера, необходимый для хранения информации
            GetLogicalProcessorInformationEx(RelationAll, nullptr, &length);

            // Выделяем память под буфер
            auto buffer = new BYTE[length];
            if (!GetLogicalProcessorInformationEx(RelationAll, reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(buffer), &length)) {
                printf("Ошибка получения информации о процессоре.\n");
                delete[] buffer;
                return false;
            }

            DWORD offset = 0;
            DWORD processorCoreCount = 0;
            DWORD threadCount = 0;
            DWORD l1CacheSize = 0;
            DWORD l2CacheSize = 0;
            DWORD l3CacheSize = 0;

            // неидеальный скрипт, зависит от порядка выдаваемых групп ядер системой
            // т.к. она может выдать то P ядра сначала, то E
            while (offset < length) {
                auto info = reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(buffer + offset);

                if (info->Relationship == RelationProcessorCore) {
                    processorCoreCount++;
                    threadCount += info->Processor.GroupCount;
                }
                else if (info->Relationship == RelationCache) {
                    switch (info->Cache.Level) {
                    case 1:
                        if (info->Cache.Type == CacheData || info->Cache.Type == CacheUnified) {
                            l1CacheSize = info->Cache.CacheSize;
                        }
                        break;
                    case 2:
                        l2CacheSize = info->Cache.CacheSize;
                        break;
                    case 3:
                        l3CacheSize = info->Cache.CacheSize;
                        break;
                    }
                }

                offset += info->Size;
            }

            // Перезапустим цикл для более точного подсчета потоков
            offset = 0;
            threadCount = 0;
            while (offset < length) {
                auto info = reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(buffer + offset);
                if (info->Relationship == RelationProcessorCore) {
                    // Подсчитываем количество битов (логических процессоров) в маске
                    WORD groupCount = info->Processor.GroupCount;
                    for (WORD i = 0; i < groupCount; ++i) {
                        KAFFINITY mask = info->Processor.GroupMask[i].Mask;
                        // Подсчитываем установленные биты в маске
                        while (mask) {
                            threadCount += mask & 1;
                            mask >>= 1;
                        }
                    }
                }
                offset += info->Size;
            }

            this->cores = (short)processorCoreCount;
            this->threads = (short)threadCount;

            this->L1_size = l1CacheSize;
            this->L2_size = l2CacheSize;
            this->L3_size = l3CacheSize;

            delete[] buffer;
            return true;
        }
        friend std::ostream& operator <<(std::ostream& stream, CpuInfo* info) {
            stream << "Cores / Threads: " << info->cores << " / " << info->threads << '\n';
            stream << "Caches L1 / L2 / L3 " << info->L1_size << ' ' << info->L2_size << ' ' << \
                info->L3_size << '\n';
            return stream;
        }

        friend std::ostream& operator <<(std::ostream& stream, CpuInfo info) {
            stream << "Cores / Threads: " << info.cores << " / " << info.threads << '\n';
            stream << "Caches L1 / L2 / L3 in bytes " << info.L1_size << ' ' << info.L2_size << ' ' << \
                info.L3_size << '\n';
            return stream;
        }
    };
}