#include <iostream>
#include <fstream>
#include <algorithm>
#include <list>
#include <stack>
#include <map>
#include <string>
#include <cstring>
#include <cassert>

using namespace std;

#define BUFSIZE 1024
#define NUMSCAP 12
#define EMPTY -1

/** item types */
enum itype_t
{
    CONSTANT,   // integer constant
    LIST_START, // marker for list start, '['
    LIST_STOP,  // marker for list stop, ']'
    DELIMITER,  // item seperator, ','
    NONE_ITEM,  // empty list marker
};
/** Comparison pass/fail conditions */
enum check_t
{
    UNDEFINED,     // default, none of the other cases match
    TRUE_INT,      // lhs < rhs
    TRUE_LIST,     // lhs[n] < rhs[n]
    TRUE_LENGTH,   // lhs.length < rhs.length
    FALSE_INT,     // lhs > rhs
    FALSE_LIST,    // lhs[n] > rhs[n]
    FALSE_LENGTH,  // lhs.length > rhs.length
    NONE_ITEM_DUO, // indecisive flag, i.e., lhs == rhs == NONE_ITEM
};
/** Convert char to itype_t, supports LIST_START, LIST_STOP and DELIMITER */
static map<char, itype_t> char_to_itype = {
    {'[', itype_t::LIST_START},
    {']', itype_t::LIST_STOP},
    {',', itype_t::DELIMITER},
};
/** Convert itype_t to char, supports LIST_START, LIST_STOP, DELIMITER and NONE_ITEM */
static map<itype_t, char> itype_to_char = {
    {itype_t::LIST_START, '['},
    {itype_t::LIST_STOP, ']'},
    {itype_t::DELIMITER, ','},
    {itype_t::NONE_ITEM, '_'},
};
/** map check_t to string for debugging purposes */
static map<check_t, string> check_to_string = {
    {check_t::UNDEFINED, "check_t::UNDEFINED"},
    {check_t::TRUE_INT, "check_t::TRUE_INT"},
    {check_t::TRUE_LIST, "check_t::TRUE_LIST"},
    {check_t::TRUE_LENGTH, "check_t::TRUE_LENGTH"},
    {check_t::FALSE_INT, "check_t::FALSE_INT"},
    {check_t::FALSE_LIST, "check_t::FALSE_LIST"},
    {check_t::FALSE_LENGTH, "check_t::FALSE_LENGTH"},
};
/** list item */
typedef struct item_t
{
    int32_t value; // value of item
    itype_t itype; // item type of item

} item_t;
/** data packet content */
typedef struct packet_t
{
    list<item_t> lhs; // left-hand-side of packet
    list<item_t> rhs; // right-hand-side of packet
} packet_t;
list<packet_t> *data;
int32_t cnt;
size_t length;
/** add additional */

void parsePacket(const char *cstr, size_t count, list<item_t> *items)
{
    assert(items != NULL); // Panic if items is NULL
    assert(cstr != NULL);  // Panic if cstr is NULL

    char *num = new char[NUMSCAP];        // temporary holder for integers as chars
    char *ptr = num;                      // pointer to where to store next integer in num;
    items->clear();                       // clear items to ensure empty;
    item_t item = {0, itype_t::CONSTANT}; // current item temporary holder
    for (size_t i = 0; i < count; i++)    // iterate through count chars and parse to item
    {
        if (cstr[i] == '[') // list marker start detected
        {
            item.value = 0;
            item.itype = char_to_itype[cstr[i]];
            items->push_back(item);
        }
        else if (cstr[i] == ']') // list marker end detected
        {
            /**
             * if ptr is not at num home -> must be a stored number
             * handle it and continue.
             */
            if (ptr != num)
            {
                item.value = atoi(num);
                item.itype = itype_t::CONSTANT;
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
                item.itype = itype_t::NONE_ITEM;
                items->push_back(item);
            }
            item.value = 0;
            item.itype = char_to_itype[cstr[i]];
            ;
            items->push_back(item);
        }
        else if (cstr[i] == ',')
        {
            /** delimiter detected -> store num if ptr is not at num home */
            if (ptr != num)
            {
                item.value = atoi(num);
                item.itype = itype_t::CONSTANT;
                memset(num, '\0', NUMSCAP);
                ptr = num;
                items->push_back(item);
            }
            item.value = 0;
            item.itype = char_to_itype[cstr[i]];
            items->push_back(item);
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
    list<item_t> *lhs = new list<item_t>();
    list<item_t> *rhs = new list<item_t>();
    packet_t new_pack;

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
            parsePacket(indata, ifs.gcount(), lhs);

            ifs.getline(indata, BUFSIZE); // RHS
            parsePacket(indata, ifs.gcount(), rhs);
            new_pack.lhs.assign(lhs->begin(), lhs->end());
            new_pack.rhs.assign(rhs->begin(), rhs->end());
            data->push_back(new_pack);

            ctr++;
        }
    }
    cnt = ctr;
    ifs.close();
    delete[] indata;
    delete rhs;
    delete lhs;
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
    for (auto lit = items->begin(); lit != items->end(); lit++)
    {
        if (lit->itype == CONSTANT)
            cout << lit->value;
        else
            cout << itype_to_char[lit->itype];
    }
    cout << "\n";
}
void process_task()
{
    /** prints data packets */
    for (auto it = data->begin(); it != data->end(); it++)
    {
        print_list(&(it->lhs));
        print_list(&(it->rhs));
        cout << "\n";
    }
}
/**
 * @brief Fetches the next items from the list iterator and inserts the item into dst.
 * Content of dst may differ depending on what first item is:
 * (a) Item is CONSTANT a signular item is inserted. Same for NONE_ITEM item type.
 * (b) Item is LIST_START a sequence of items are insterted until LIST_START and LIST_STOP reach balance (excludes first LIST_START and last LIST_STOP).
 * (c) Item is LIST_STOP or DELIMITER advances it but no insertion to dst.
 *
 * @param it iterator reference. is advanced for each item parsed
 * @param dst reference to insertion destination (push_back). is cleared before each before parsing.
 */
void fetch_next(list<item_t>::iterator &it, list<item_t> *dst)
{
    assert(dst != NULL);
    dst->clear();
    if (it->itype == itype_t::CONSTANT || it->itype == itype_t::NONE_ITEM)
    {
        dst->push_back(*it);
        it++;
    }
    else if (it->itype == itype_t::LIST_START)
    {
        it++;
        int32_t balance = 1;
        while (balance != 0)
        {
            if (it->itype == itype_t::LIST_START)
                balance++;
            if (it->itype == itype_t::LIST_STOP)
                balance--;

            dst->push_back(*it);
            it++;
        }
        dst->pop_back();
    }
    else if (it->itype == itype_t::LIST_STOP || it->itype == itype_t::DELIMITER)
        it++;
}
/**
 * @brief Compares left item to right item.
 * Only compares CONSTANT-to-CONSTANT, CONSTANT-to-NONE_ITEM and NONE_ITEM-to-CONSTANT.
 *
 * @param left item to compare to right
 * @param right item to compare to left
 * @return check_t outcome of comparison
 */
check_t item_compare(item_t left, item_t right)
{
    if (left.itype == itype_t::CONSTANT && right.itype == itype_t::CONSTANT)
    {
        if (left.value < right.value)
            return check_t::TRUE_INT;
        if (left.value > right.value)
            return check_t::FALSE_INT;
        return check_t::UNDEFINED;
    }
    if (left.itype == itype_t::NONE_ITEM && right.itype == itype_t::NONE_ITEM)
        return check_t::NONE_ITEM_DUO;
    if (left.itype == itype_t::CONSTANT && right.itype == itype_t::NONE_ITEM)
        return check_t::FALSE_LENGTH;
    if (left.itype == itype_t::NONE_ITEM && right.itype == itype_t::CONSTANT)
        return check_t::TRUE_LENGTH;
    return check_t::UNDEFINED;
}
/**
 * @brief Compares left list of items to right list of items.
 *        Makes recursive call to itself if sub-list is found.
 *
 * @param left list of items
 * @param right list of items
 * @return check_t outcome of the comparison
 */
check_t list_to_list_compare(list<item_t> *left, list<item_t> *right)
{
    /** Uncomment for debug print */
    // print_list(left);
    // print_list(right);
    check_t check = check_t::UNDEFINED;
    list<item_t>::iterator lit, rit;
    list<item_t> *lproc = new list<item_t>();
    list<item_t> *rproc = new list<item_t>();
    lit = left->begin();
    rit = right->begin();
    while (lit != left->end() && rit != right->end())
    {
        fetch_next(lit, lproc);
        fetch_next(rit, rproc);
        if (lproc->size() == 1 && rproc->size() == 1) // each hold a constant
            check = item_compare(lproc->front(), rproc->front());
        else if (lproc->size() >= 1 && rproc->size() >= 1)
            check = list_to_list_compare(lproc, rproc);
        // else // empty lproc && rproc -> do nothing
        if (check == check_t::NONE_ITEM_DUO)
        {
            if (left->size() == 1 && right->size() > 1)
                check = check_t::TRUE_LIST;
            else if (left->size() > 1 && right->size() == 1)
                check = check_t::FALSE_LIST;
            else
                check = check_t::UNDEFINED;
        }
        if (check != check_t::UNDEFINED)
            break;
    }
    if (check == check_t::UNDEFINED)
    {
        if (lit == left->end() && rit != right->end())
            check = check_t::TRUE_LIST;
        if (lit != left->end() && rit == right->end())
            check = check_t::FALSE_LIST;
    }
    delete lproc;
    delete rproc;
    return check;
}
/**
 * @brief simple list compare suitable for sort algorithm of list<list<item_t>> type
 *
 * @param left left hand side of comparison
 * @param right right hand side of comparison
 * @return true if left is before right, else false if right should be before left
 */
bool list_compare(list<item_t> &left, list<item_t> &right)
{
    check_t check = list_to_list_compare(&left, &right);
    return !(check_t::FALSE_INT <= check && check <= check_t::FALSE_LENGTH);
}
int32_t process_task1(void)
{
    check_t check = check_t::UNDEFINED;
    int32_t ctr = 1;
    int32_t sum_of_true = 0;
    list<packet_t>::iterator it = data->begin();
    while (it != data->end())
    {
        check = check_t::UNDEFINED;

        check = list_to_list_compare(&(it->lhs), &(it->rhs));
        /** Uncomment for debug print */
        // cout << ctr << ": " << check_to_string[check] << "\n";
        if (check > check_t::UNDEFINED && check <= check_t::TRUE_LENGTH)
            sum_of_true += ctr;
        ctr++;
        it++;
    }
    return sum_of_true;
}
int32_t process_task2(void)
{
    // construct the two divider packets
    const char *div1_str = "[[2]]";
    const char *div2_str = "[[6]]";
    list<item_t> *div1 = new list<item_t>();
    list<item_t> *div2 = new list<item_t>();
    parsePacket(div1_str, 6, div1);
    parsePacket(div2_str, 6, div2);
    // organize all data to a list of lists of items.
    list<list<item_t>> all_packets;
    for (auto it = data->begin(); it != data->end(); it++)
    {
        all_packets.push_back(it->rhs);
        all_packets.push_back(it->lhs);
    }
    // don't forget the divider packets
    all_packets.push_front(*div1);
    all_packets.push_back(*div2);
    // do some sorting
    all_packets.sort(list_compare);
    int32_t offset_div1 = 0;
    int32_t offset_div2 = 0;
    int32_t ctr = 1;
    // find offsets of divider packets
    for (auto it : all_packets)
    {
        // compare algorithm causes UNDEFINED if lists are exact match
        if (offset_div1 == 0)
            offset_div1 = (list_to_list_compare(&it, div1) == check_t::UNDEFINED) ? ctr : offset_div1;
        if (offset_div2 == 0)
            offset_div2 = (list_to_list_compare(&it, div2) == check_t::UNDEFINED) ? ctr : offset_div2;
        ctr++;
        if (offset_div1 > 0 && offset_div2 > 0)
            break;
    }
    // clean up and return the offsets multiplied with each other.
    delete div1;
    delete div2;
    return offset_div1 * offset_div2;
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

    cout << "Task 1 result: " << task1 << "\n";
    cout << "Task 2 result: " << task2 << "\n";

    delete data;
    /** add delete to all data here */
    return 0;
}