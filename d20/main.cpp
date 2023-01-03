#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <list>
#include <cassert>

using namespace std;

#define BUFSIZE 1024

/** used in processing */
typedef struct item_t
{
    int32_t value;
    int32_t touched;
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
    item_t item = {0, 0};
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
inline int32_t truncate_index(int32_t position, int32_t index)
{
    index += position;
    if (index > cnt)
        return (index + 1) % cnt;
    if (index < -cnt)
        return (index - 2) % cnt;
    if (index <= 0)
        return (index - 1) % cnt;
    return index;
}
void move_item(list<item_t> *items, item_t item, int32_t from)
{
    int32_t index = truncate_index(from, item.value);
    item.touched += 1;
    auto it = items->begin();
    advance(it, index);
    items->insert(it, item);
}
int32_t find_next(list<item_t> *items, item_t *item)
{
    auto it = items->begin();
    int32_t index = 0;
    while (it != items->end())
    {
        if (!it->touched)
        {
            if (it->value == 0) // only touch zeros, as they will not move anywhere
                it->touched += 1;
            else
            {
                *item = *it;
                items->erase(it);
                return index;
            }
        }
        it++;
        index++;
    }
    return -1;
}
int32_t find_zero(list<item_t> *items)
{
    list<item_t>::iterator it;
    int32_t i;
    for (it = items->begin(), i = 0; it != items->end(); i++, it++)
    {
        if (it->value == 0)
            return i;
    }
    return -1;
}
int32_t first_non_zero(list<item_t> *items, int32_t index)
{
    auto it = items->begin();
    index %= cnt;
    advance(it, index);
    while (it->value == 0)
    {
        it++;
        if (it == items->end())
            it = items->begin();
    }
    return it->value;
}
void decrypt(list<item_t> *items)
{
    const int32_t key = 811589153;
    for (auto it = items->begin(); it != items->end(); it++)
    {
        it->value *= key;
    }
}
void process_task()
{
    /** Do something to process the task in general */
}
int32_t process_task1(void)
{
    /** Do something to process the task specific to 1 */
    // print_list(data);
    list<item_t> *items = new list<item_t>();
    items->assign(data->begin(), data->end());

    int32_t index = 0;
    item_t item;
    while (true)
    {
        index = find_next(items, &item);
        if (index < 0)
            break;
        if (item.value != 0)
            move_item(items, item, index);
        // print_list(data);
    }
    index = find_zero(items);
    assert(index >= 0);
    int32_t i1 = first_non_zero(items, index + 1000);
    int32_t i2 = first_non_zero(items, index + 2000);
    int32_t i3 = first_non_zero(items, index + 3000);

    cout << index << ": " << i1 << ", " << i2 << ", " << i3 << endl;
    delete items;
    return i1 + i2 + i3;
}
int32_t process_task2(void)
{
    /** Do something to process the task specific to 2 */
    list<item_t> *items = new list<item_t>();
    items->assign(data->begin(), data->end());
    // decrypt(items);
    cout << uset.size() << ", " << items->size() << endl;

    delete items;
    return 0;
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

    delete data;
    /** add delete to all data here */
    return 0;
}