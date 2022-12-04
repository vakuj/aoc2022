#include <iostream>
#include <fstream>
#include <cstdio>
#include <set>

#define BUFSIZE 1024
/** used in processing */
std::set<int32_t> *range1;
std::set<int32_t> *range2;
int32_t cnt;
size_t length;
/** add additional */

inline void set_range(std::set<int32_t> *range, int32_t offset, int32_t a, int32_t b)
{
    for (int32_t i = a; i < b + 1; i++)
    {
        range[offset].insert(i);
    }
}

bool contains_all(std::set<int32_t> r1, std::set<int32_t> r2)
{
    bool contains = true;   // assume the smaller range is fully contained
    std::set<int32_t> R, r; // local small(r) and large(R) ranges

    // find smallest range to iterate through
    if (r1.size() < r2.size())
    {
        R = r2;
        r = r1;
    }
    else
    {
        R = r1;
        r = r2;
    }
    auto it = r.begin();
    while (it != r.end() && contains) // iterate through the small range
    {
        if (R.count(*it) == 0)
            contains = false; // item in r is not in R
        it++;
    }
    return contains;
}
bool contains_any(std::set<int32_t> r1, std::set<int32_t> r2)
{
    bool contains = false; // assume items of r1 is not in r2
    auto it = r1.begin();
    while (it != r1.end() && !contains) // iterate through r1
    {
        if (r2.count(*it) > 0)
            contains = true; // item in r1 found in r2
        it++;
    }
    return contains;
}

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

    range1 = new std::set<int32_t>[length];
    range2 = new std::set<int32_t>[length];

    char *indata = new char[BUFSIZE];
    int32_t ctr = 0;
    int32_t a, b, x, y;
    while (!ifs.eof())
    {
        ifs.getline(indata, BUFSIZE);
        /** store for later processing */
        if (4 == std::sscanf(indata, "%d-%d,%d-%d", &a, &b, &x, &y))
        {
            set_range(range1, ctr, a, b);
            set_range(range2, ctr, x, y);
        }
        ctr++;
    }
    cnt = ctr;
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
    /** Do something to process the task in general */
}
int32_t process_task1(void)
{
    int32_t count = 0;
    for (int32_t i = 0; i < cnt; i++)
    {
        count += contains_all(range1[i], range2[i]) ? 1 : 0;
    }
    return count;
}
int32_t process_task2(void)
{
    int32_t count = 0;
    for (int32_t i = 0; i < cnt; i++)
    {
        count += contains_any(range1[i], range2[i]) ? 1 : 0;
    }

    return count;
}
int main()
{
    /** use this for testing examples */
    // read_input("test.txt");

    /** use this to run input */
    read_input("input.txt");

    process_task();

    int32_t task1 = process_task1();
    int32_t task2 = process_task2();

    /** store output to file */
    write_output("output.txt", task1, task2);

    std::cout << "Task 1 result: " << task1 << "\n";
    std::cout << "Task 2 result: " << task2 << "\n";

    /** add delete to all data here */
    delete[] range1;
    delete[] range2;

    return 0;
}