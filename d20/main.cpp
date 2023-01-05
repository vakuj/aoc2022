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
/**
 * @brief prints values from list of items
 *
 * @param items ptr to list of items
 */
void print_list(list<item_t> *items)
{
    for (auto it : *items)
    {
        cout.width(2);
        cout << it.value << ", ";
    }
    cout << endl;
}
/**
 * @brief Re-evaluates the from + index to bounds within list
 *
 * @param from original positon
 * @param index index increment
 * @return int64_t truncated index
 */
inline int64_t truncate_index(int64_t from, int64_t index)
{
    index += from;
    index %= (cnt - 1);
    if (index == 0)
        index = -1;
    else if (index < 0)
        index--;
    return index;
}
/**
 * @brief moves item to from old to new position in list of items. original item should
 * by caller.
 *
 * @param items ptr to list of items
 * @param item the item to insert
 * @param from original position
 */
void move_item(list<item_t> *items, item_t item, int64_t from)
{
    int64_t index = truncate_index(from, item.value);
    auto it = items->begin();
    advance(it, index);
    items->insert(it, item);
}
/**
 * @brief finds item with id in list of items. touched is incremented. if item value is 0, the
 * item is not removed. is removed for non-zero item values.
 *
 * @param id id to find
 * @param items ptr to list of items
 * @param item the found item
 * @return int32_t index where found
 */
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
/**
 * @brief finds the next item in list of items which has not been touched. if next item
 * has value zero, the item is only touched, and not removed.
 *
 * @param items ptr to list of items
 * @param item the next item with touched incremented, and removed from list
 * @return int32_t index where found.
 */
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
/**
 * @brief finds the index of first item with value 0 in list of items
 *
 * @param items ptr to list to items
 * @return int32_t index of item with value zero
 */
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
/**
 * @brief finds the first non-zero value item in list of items starting at index.
 * searches starts over from begining of list if end is reached and not found.
 *
 * @param items ptr to list of items
 * @param index index to start at
 * @return int64_t value of found item
 */
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
/**
 * @brief applies decryption key to each item value in list of items
 *
 * @param items ptr to list of items
 */
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
    // print_list(items);
    while (true)
    {
        index = find_next(items, &item);
        if (index < 0)
            break;
        if (item.value != 0)
            move_item(items, item, index);
        // print_list(items);
    }
    // print_list(items);
    index = find_zero(items);
    assert(index >= 0);
    int64_t i1 = first_non_zero(items, index + 1000);
    int64_t i2 = first_non_zero(items, index + 2000);
    int64_t i3 = first_non_zero(items, index + 3000);

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

    for (size_t i = 0; i < 10; i++)
    {
        for (int32_t id = 0; id < (int32_t)items->size(); id++)
        {
            index = find_next_id(id, items, &item);
            assert(index >= 0);
            if (item.value != 0)
                move_item(items, item, index);
        }
    }
    index = find_zero(items);
    assert(index >= 0);
    int64_t i1 = first_non_zero(items, index + 1000);
    int64_t i2 = first_non_zero(items, index + 2000);
    int64_t i3 = first_non_zero(items, index + 3000);

    delete items;
    return i1 + i2 + i3;
}
int main()
{
    /** use this for testing examples */
    // read_input("test.txt");

    /** use this to run input */
    read_input("input.txt");

    process_task();

    int64_t task1 = process_task1();
    int64_t task2 = process_task2();

    /** store output to file */
    write_output("output.txt", task1, task2);

    cout << "Task 1 result: " << task1 << "\n";
    cout << "Task 2 result: " << task2 << "\n";

    delete data;
    /** add delete to all data here */
    return 0;
}