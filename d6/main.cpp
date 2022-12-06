#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <set>

#define BUFSIZE 1024

using namespace std;

/** used in processing */
char *data;
int32_t cnt;
size_t length;
/** add additional */

int32_t find_sequence(const char *cstr, size_t count, size_t seq_len)
{
    set<char> seq;
    int32_t loc = -1;
    for (size_t i = seq_len - 1; i < count; i++)
    {
        /** clear set if not empty */
        if (!seq.empty())
            seq.clear();
        /** insert sequence length of chars into set */
        for (size_t k = 0; k < seq_len; k++)
            seq.insert(cstr[i - k]);
        /** check if all chars were unique, i.e. inserted */
        if (seq.size() == seq_len)
        {
            loc = (int32_t)(i + 1);
            break;
        }
    }
    return loc;
}

void read_input(const char *filename)
{
    ifstream ifs(filename, ifstream::in);

    if (!ifs.is_open())
    {
        exit(1);
    }
    ifs.seekg(0, ifstream::end);
    length = ifs.tellg();
    ifs.seekg(0, ifstream::beg);

    data = new char[length];

    ifs.read(data, length);
    cnt = (int32_t)ifs.gcount();
    length = ifs.gcount();
    ifs.close();
}

void write_output(const char *filename, int32_t task1, int32_t task2)
{
    ofstream ofs(filename, ofstream::out);
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
    return find_sequence(data, length, 4);
}
int32_t process_task2(void)
{
    return find_sequence(data, length, 14);
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

    cout << "Task 1 result: " << task1 << "\n";
    cout << "Task 2 result: " << task2 << "\n";

    delete[] data;
    /** add delete to all data here */
    return 0;
}