#include <iostream>
#include <fstream>
#include <algorithm>
#include <map>

#define BUFSIZE 1024
/** used in processing */
std::map<char, int32_t> *comp1;
std::map<char, int32_t> *comp2;
int32_t cnt;
size_t length;
/** add additional */

int32_t charPrio(char chr)
{
    if (chr >= 'A' && chr <= 'Z')
        return (int32_t)((chr - 'A') + 27);
    if (chr >= 'a' && chr <= 'z')
        return (int32_t)((chr - 'a') + 1);
    return -1;
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

    comp1 = new std::map<char, int32_t>[length];
    comp2 = new std::map<char, int32_t>[length];

    char *indata = new char[BUFSIZE];
    int32_t ctr = 0;
    while (!ifs.eof())
    {
        ifs.getline(indata, BUFSIZE);
        for (int32_t i = 0; i < ifs.gcount(); i++)
        {
            if (i < ifs.gcount() / 2)
            {
                if (charPrio(indata[i]) > 0)
                    comp1[ctr][indata[i]] += 1;
            }
            else
            {
                if (charPrio(indata[i]) > 0)
                    comp2[ctr][indata[i]] += 1;
            }
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
    int32_t sum = 0;
    for (int32_t i = 0; i < cnt; i++)
    {
        auto it = comp1[i].begin();
        while (it != comp1[i].end())
        {
            if (comp2[i].count(it->first) > 0)
            {
                sum += charPrio(it->first);
            }
            it++;
        }
    }
    return sum;
}
inline bool inThree(char item, int32_t offset)
{
    return (((comp1[offset + 1].count(item) > 0) ||
             (comp2[offset + 1].count(item) > 0)) &&
            ((comp1[offset + 2].count(item) > 0) ||
             (comp2[offset + 2].count(item) > 0)));
}
int32_t process_task2(void)
{
    int32_t sum = 0;
    for (int32_t i = 0; i < cnt; i += 3)
    {
        auto it = comp1[i].begin();
        while (it != comp1[i].end())
        {
            if (inThree(it->first, i))
            {
                sum += charPrio(it->first);
            }
            it++;
        }
        it = comp2[i].begin();
        while (it != comp2[i].end())
        {
            if (inThree(it->first, i))
            {
                sum += charPrio(it->first);
            }
            it++;
        }
    }

    return sum;
}
int main()
{
    /** use this for testing examples */
    // read_input("test.txt");

    /** use this to run input */
    read_input("input.txt");

    // process_task();

    int32_t task1 = process_task1();
    int32_t task2 = process_task2();

    /** store output to file */
    write_output("output.txt", task1, task2);

    std::cout << "Task 1 result: " << task1 << "\n";
    std::cout << "Task 2 result: " << task2 << "\n";

    delete[] comp1;
    delete[] comp2;
    /** add delete to all data here */
    return 0;
}