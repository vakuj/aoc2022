#include <iostream>
#include <fstream>
#include <algorithm>
#include <list>
#include <stack>
#include <cstring>
#include <cassert>

using namespace std;

#define BUFSIZE 1024
#define NUMSCAP 12
#define EMPTY -1
/** used in processing */

typedef struct item_t
{
    int32_t value;
    int32_t level;
} item_t;
typedef struct packet_t
{
    list<item_t> lhs;
    list<item_t> rhs;
} packet_t;
list<packet_t> *data;
int32_t cnt;
size_t length;
/** add additional */

void parsePacket(const char *cstr, size_t count, list<item_t> *items)
{
    assert(items != NULL); // Panic if items is NULL
    assert(cstr != NULL);  // Panic if cstr is NULL

    char *num = new char[NUMSCAP];     // temporary holder for integers as chars
    char *ptr = num;                   // pointer to where to store next integer in num;
    items->clear();                    // clear items to ensure empty;
    item_t item = {0, 0};              // current item temporary holder
    for (size_t i = 0; i < count; i++) // iterate through count chars and parse to item
    {
        if (cstr[i] == '[')      // list marker start detected
            item.level++;        // increase item level
        else if (cstr[i] == ']') // list marker end detected
        {
            /**
             * if ptr is not at num home -> must be a stored number
             * handle it and continue.
             */
            if (ptr != num)
            {
                item.value = atoi(num);
                memset(num, '\0', NUMSCAP);
                ptr = num;
                items->push_back(item);
            }
            /**
             * Check to see if the item was a empty list, that is, '[' was previous to ']'
             * store a negtive value to indicate this.
             */
            if (cstr[i - 1] == '[')
            {
                item.value = EMPTY;
                items->push_back(item);
            }
            item.level--; // decrease item level
        }
        else if (cstr[i] == ',')
        {
            /** delimiter detected -> store num if ptr is not at num home */
            if (ptr != num)
            {
                item.value = atoi(num);
                memset(num, '\0', NUMSCAP);
                ptr = num;
                items->push_back(item);
            }
        }
        else
        {
            /**
             * char is not one of "[]," -> must be number or null ('\0').
             * store to ptr position and increase if not null.
             */
            if (cstr[i] != '\0')
                *(ptr++) = cstr[i];
        }
    }
    delete[] num;
    return;
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

    data = new list<packet_t>();
    list<item_t> lhs;
    list<item_t> rhs;

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
            /** store for later processing */
            ifs.getline(indata, BUFSIZE); // LHS
            parsePacket(indata, ifs.gcount(), &lhs);

            ifs.getline(indata, BUFSIZE); // RHS
            parsePacket(indata, ifs.gcount(), &rhs);
            data->push_back({lhs, rhs});

            ctr++;
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
void process_task()
{
    /** Do something to process the task in general */
    // for (auto it = data->begin(); it != data->end(); it++)
    // {
    //     for (auto lit = it->lhs.begin(); lit != it->lhs.end(); lit++)
    //     {
    //         cout.width(lit->level);
    //         cout << lit->value << "\n";
    //     }
    //     for (auto rit = it->rhs.begin(); rit != it->rhs.end(); rit++)
    //     {
    //         cout.width(rit->level);
    //         cout << rit->value << "\n";
    //     }
    // }
}
int32_t process_task1(void)
{
    /**
     * If both values are `integers`, the `lower integer should come first`.
     *  - If the `left integer < right integer`, the inputs are `in the right order`.
     *  - If the `left integer > right integer`, the inputs are `not in the right order`.
     *  - If `left integer == right integer`; continue checking the next part of the input.
     *
     * If both values are `lists`, compare the `first value of each list`, then `the second value`, and `so on`.
     *  - If the `left list runs out of items first`, the inputs are in the `right order`.
     *  - If the `right list runs out of items first`, the inputs are `not in the right order`.
     *  - If the `lists are the same length` and `no comparison makes a decision` about the order, continue checking the `next part of the input`.
     *
     * If exactly one value is an integer, convert the integer to a list which contains that integer as its only value, then retry the comparison.
     *  - For example, if comparing `[0,0,0]` and `2`, convert the right value to `[2]`; the result is then found by instead comparing `[0,0,0]` and `[2]`.
     */
    enum check_t
    {
        UNDEFINED,
        TRUE_INT,
        TRUE_LIST,
        TRUE_LENGTH,
        FALSE_INT,
        FALSE_LIST,
        FALSE_LENGTH,
    };
    packet_t front;
    stack<item_t> lstack, rstack;
    item_t litem, ritem;
    int32_t current_level = 1;
    bool right_order;
    check_t check = check_t::UNDEFINED;
    int32_t ctr = 1;
    while (!data->empty())
    {
        right_order = false;
        check = check_t::UNDEFINED;
        front = data->front();
        data->pop_front();
        while (!front.lhs.empty() && !front.rhs.empty())
        {
            litem = front.lhs.front();
            ritem = front.rhs.front();
            front.lhs.pop_front();
            front.rhs.pop_front();
            if (litem.level == ritem.level)
            {
                if (litem.value < ritem.value)
                {
                    check = check_t::TRUE_INT;
                    break;
                }
                if (litem.value > ritem.value)
                {
                    check = check_t::FALSE_INT;
                    break;
                }
            }
            else
            {
                if (litem.value == EMPTY && ritem.value == EMPTY)
                {
                    if (litem.level < ritem.level)
                    {
                        check = check_t::TRUE_LIST;
                        break;
                    }
                    else
                    {
                        check = check_t::FALSE_LIST;
                        break;
                    }
                }
                else if (litem.level < ritem.level)
                {
                    /** probably should advance rhs */
                    if (litem.value < ritem.value)
                    {
                        check = check_t::TRUE_INT;
                        break;
                    }
                    if (litem.value > ritem.value)
                    {
                        check = check_t::FALSE_INT;
                        break;
                    }
                }
                else
                {
                    /** probably should advance lhs */
                    if (litem.value < ritem.value)
                    {
                        check = check_t::TRUE_INT;
                        break;
                    }
                    if (litem.value > ritem.value)
                    {
                        check = check_t::FALSE_INT;
                        break;
                    }
                }
            }
        }
        if (check == check_t::UNDEFINED)
        {
            if (front.lhs.empty() && !front.rhs.empty())
                check = check_t::TRUE_LENGTH;
            else
                check = check_t::FALSE_LENGTH;
        }
        if (check > check_t::UNDEFINED && check <= check_t::TRUE_LENGTH)
            cout << ctr << ", ";
        ctr++;
    }
    cout << "\n";
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