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
    int64_t value;
    int32_t touched;
    int32_t id;
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
    item_t item = {0, 0, 0};
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
            if (1 == sscanf(indata, "%ld", &item.value))
            {
                item.id = ctr++;
                data->push_back(item);
            }
        }
    }
    cnt = ctr;
    ifs.close();
    delete[] indata;
}

void write_output(const char *filename, int64_t task1, int64_t task2)
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
inline int64_t truncate_index(int64_t from, int64_t index)
{
    index %= cnt;
    cout.width(3);
    cout << index << ", ";
    index += from;
    if (index >= cnt)
        return (index + 1) % cnt;
    if (index <= -cnt)
        return (index - 2) % cnt;
    if (index <= 0)
        return (index - 1) % cnt;
    return index;
}
void move_item(list<item_t> *items, item_t item, int64_t from)
{
    int64_t index = truncate_index(from, item.value);
    cout.width(12);
    cout << item.value << ", ";
    cout.width(5);
    cout << from << ", ";
    cout.width(5);
    cout << index << endl;
    auto it = items->begin();
    advance(it, index);
    items->insert(it, item);
}
int32_t find_next_id(int32_t id, list<item_t> *items, item_t *item)
{
    auto it = items->begin();
    int32_t index = 0;
    while (it != items->end())
    {
        if (it->id == id)
        {
            it->touched += 1;
            *item = *it;
            if (it->value != 0)
                items->erase(it);
            return index;
        }
        it++;
        index++;
    }
    return -1;
}
int32_t find_next(list<item_t> *items, item_t *item)
{
    auto it = items->begin();
    int32_t index = 0;
    while (it != items->end())
    {
        if (!it->touched)
        {
            it->touched += 1;
            if (it->value != 0) // only touch zeros, as they will not move anywhere
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
int64_t first_non_zero(list<item_t> *items, int32_t index)
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
        it->value *= key;
}
void process_task()
{
    /** Do something to process the task in general */
}
int64_t process_task1(void)
{
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
    }
    print_list(items);
    index = find_zero(items);
    assert(index >= 0);
    int64_t i1 = first_non_zero(items, index + 1000);
    int64_t i2 = first_non_zero(items, index + 2000);
    int64_t i3 = first_non_zero(items, index + 3000);

    // cout << index << ": " << i1 << ", " << i2 << ", " << i3 << endl;
    delete items;
    return i1 + i2 + i3;
}
int64_t process_task2(void)
{
    list<item_t> *items = new list<item_t>();
    items->assign(data->begin(), data->end());
    decrypt(items);
    int32_t index = 0;
    item_t item;
    print_list(items);
    for (int32_t id = 0; id < (int32_t)items->size(); id++)
    {
        index = find_next_id(id, items, &item);
        assert(index >= 0);
        if (item.value != 0)
            move_item(items, item, index);
    }
    print_list(items);
    index = find_zero(items);
    assert(index >= 0);
    int64_t i1 = first_non_zero(items, index + 1000);
    int64_t i2 = first_non_zero(items, index + 2000);
    int64_t i3 = first_non_zero(items, index + 3000);
    cout << index << ": " << i1 << ", " << i2 << ", " << i3 << endl;

    delete items;
    return i1 + i2 + i3;
}
int main()
{
    /** use this for testing examples */
    read_input("test.txt");

    /** use this to run input */
    // read_input("input.txt");

    process_task();

    int64_t task1 = process_task1();
    // int64_t task1 = 0;
    int64_t task2 = process_task2();

    /** store output to file */
    write_output("output.txt", task1, task2);

    cout << "Task 1 result: " << task1 << "\n";
    cout << "Task 2 result: " << task2 << "\n";

    delete data;
    /** add delete to all data here */
    return 0;
}