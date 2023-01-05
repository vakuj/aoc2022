#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <list>

using namespace std;
#define BUFSIZE 1024

/** used in processing */
vector<int32_t> *data;
int32_t cnt;
size_t length;
/** add additional */

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

    data = new vector<int32_t>();
    int32_t id_nbr, ore_ore, clay_ore, obsi_ore, obsi_clay, geod_ore, geod_obsidian;

    char *indata = new char[BUFSIZE];
    int32_t ctr = 0;
    while (!ifs.eof())
    {
        int next = ifs.peek();
        if (next == '\n')
        {
            /** forward without storing */
        }
        else
        {
            /** store for later processing */
            ifs.getline(indata, BUFSIZE);
            if (7 == sscanf(indata, "Blueprint %d: Each ore robot costs %d ore. Each clay robot costs %d ore. Each obsidian robot costs %d ore and %d clay. Each geode robot costs %d ore and %d obsidian.",
                            &id_nbr, &ore_ore, &clay_ore, &obsi_ore, &obsi_clay, &geod_ore, &geod_obsidian))
            {

                data->push_back(id_nbr);
                ctr++;
            }
        }
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
    /** Do something to process the task specific to 1 */
    return 0;
}
int32_t process_task2(void)
{
    /** Do something to process the task specific to 2 */
    return 0;
}
int main()
{
    /** use this for testing examples */
    read_input("test.txt");

    /** use this to run input */
    // read_input("input.txt");

    process_task();

    int32_t task1 = process_task1();
    int32_t task2 = process_task2();

    /** store output to file */
    write_output("output.txt", task1, task2);

    std::cout << "Task 1 result: " << task1 << "\n";
    std::cout << "Task 2 result: " << task2 << "\n";

    delete data;
    /** add delete to all data here */
    return 0;
}