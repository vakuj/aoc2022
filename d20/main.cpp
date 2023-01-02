#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <list>

using namespace std;

#define BUFSIZE 1024
/** used in processing */
typedef struct item_t
{
    int32_t value;
    bool touched;
} item_t;

list<item_t> *data;
int32_t cnt;
size_t length;
/** add additional */

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

    data = new list<item_t>();
    item_t item = {0, false};
    char *indata = new char[BUFSIZE];
    int32_t ctr = 0;
    while (!ifs.eof())
    {
        int next = ifs.peek();
        if (next == '\n')
        {
            /** forward without storing */
            ifs.getline(indata, BUFSIZE);
        }
        else
        {
            ifs.getline(indata, BUFSIZE);
            /** store for later processing */
            if (1 == sscanf(indata, "%d", &item.value))
            {
                data->push_back(item);
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
    ofstream ofs(filename, ofstream::out);
    ofs << "Task 1 result: " << task1 << "\n";
    ofs << "Task 2 result: " << task2 << "\n";
    ofs.close();
}
void print_list(list<item_t> *items)
{
    for (auto it : *items)
    {
        cout.width(2);
        cout << it.value << ", ";
    }
    cout << endl;
}
int32_t truncate_index(int32_t position, int32_t index)
{
    index = position + index;

    if (index < 0)
        return (cnt - 1 + index);
    if (index > 0)
        return index % cnt;
    return index;
}
void move_item(item_t item, int32_t from)
{
    int32_t index = truncate_index(from, item.value);
    item.touched = true;
    auto it = data->begin();
    advance(it, index);
    data->insert(it, item);
}
int32_t find_next(item_t *item)
{
    auto it = data->begin();
    int32_t index = 0;
    while (it != data->end())
    {
        if (!it->touched)
        {
            if (it->value == 0) // only touch zeros, as they will not move anywhere
                it->touched = true;
            else
            {
                *item = *it;
                data->erase(it);
                return index;
            }
        }
        it++;
        index++;
    }
    return -1;
}
void process_task()
{
    /** Do something to process the task in general */
}
int32_t process_task1(void)
{
    /** Do something to process the task specific to 1 */
    print_list(data);
    int32_t index = 0;
    item_t item;
    while (true)
    {
        index = find_next(&item);
        if (index < 0)
            break;
        if (item.value != 0)
            move_item(item, index);
        print_list(data);
    }
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

    cout << "Task 1 result: " << task1 << "\n";
    cout << "Task 2 result: " << task2 << "\n";

    delete data;
    /** add delete to all data here */
    return 0;
}