#pragma once
namespace windows
{
#include <windows.h>
}
using namespace windows;
#include <bits/stdc++.h>
#include <unistd.h>
#include "Thread.cpp"

using namespace std;
using namespace std::chrono;
class MemoryThread : public Thread
{

public:
    double maxMemory = 0;

    unsigned long long getTotalSystemMemory()
    {
#if defined(_WIN32)
        MEMORYSTATUSEX status;
        status.dwLength = sizeof(status);
        GlobalMemoryStatusEx(&status);
        return status.ullTotalPhys;

// #el
#elif defined(__unix__) || defined(__unix) || defined(unix) || (defined(__APPLE__) && defined(__MACH__))
        long pages = sysconf(_SC_PHYS_PAGES);
        long page_size = sysconf(_SC_PAGE_SIZE);
        return pages * page_size;

#endif
    }

    void computeMemory()
    {
        //------------need to work line 23 to 27----
        //   Runtime.getRuntime().gc();
        double used = (double)getTotalSystemMemory() / (double)(1024 * 1024);
        if (maxMemory < used)
            maxMemory = used;
    }

    // @Override
    void *run()
    {
        while (true)
        {
            computeMemory();
            try
            {
                auto start = high_resolution_clock::now();

                std::this_thread::sleep_for(chrono::milliseconds(100));

                auto stop = high_resolution_clock::now();
                auto duration = duration_cast<milliseconds>(stop - start);
                if (duration.count() != 100)
                {
                    throw "Interrupted Exception";
                }
            }

            catch (...)
            {
                cout << "Interrupted Exception: level=severe" << endl;
            }
        }
    }
};
