#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>

#define BUFSIZE 1024
int32_t *data;
int32_t *count;
int32_t *sumcal;
size_t elves;
size_t length;

void read_input(const char *filename)
{
    std::ifstream ifs(filename, std::ifstream::in);

    if (!ifs.is_open())
    {
        exit(1);
    }
    ifs.seekg(0, std::ifstream::end);
    length = ifs.tellg();
    ifs.seekg(0, std::ifstream::beg);

    data = new int32_t[length];
    count = new int32_t[length];
    elves = 0;
    char *indata = new char[BUFSIZE];
    int32_t ctr = 0;
    while (!ifs.eof())
    {
        int next = ifs.peek();
        if (next == '\n')
        {
            count[elves++] = ctr;
            ifs.ignore();
        }
        else
        {
            ifs.getline(indata, BUFSIZE);
            data[ctr++] = atoi(indata);
        }
    }
    count[elves++] = ctr;
    ifs.close();
    delete[] indata;
}

void write_output(const char *filename, int32_t task1, int32_t task2)
{
    std::ofstream ofs(filename, std::ofstream::out);
    ofs << "Task 1 result: " << task1 << "\n";
    ofs << "Task 2 result: " << task2 << "\n";
    ofs.close();
}
void process_task()
{
    int32_t offset = 0;
    sumcal = new int32_t[elves];
    for (size_t i = 0; i < elves; i++)
    {
        sumcal[i] = 0;
        for (int32_t j = offset; j < count[i]; j++)
        {
            sumcal[i] += data[j];
        }
        offset = count[i];
    }
    std::sort(sumcal, sumcal + elves);
}
int32_t sum_n_max(size_t n)
{
    int32_t maxcal = 0;
    n = n < elves ? n : elves;
    for (size_t i = elves - n; i < elves; i++)
    {
        maxcal += sumcal[i];
    }
    return maxcal;
}

inline int32_t process_task1(void)
{
    return sum_n_max(1);
}
inline int32_t process_task2(void)
{
    return sum_n_max(3);
}
int main()
{

    read_input("input.txt");
    process_task();
    int32_t task1 = process_task1();
    int32_t task2 = process_task2();
    write_output("output.txt", task1, task2);

    std::cout << "Task 1 result: " << task1 << "\n";
    std::cout << "Task 2 result: " << task2 << "\n";

    delete[] data;
    delete[] count;
    delete[] sumcal;
    return 0;
}