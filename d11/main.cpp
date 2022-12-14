#include <iostream>
#include <fstream>
#include <algorithm>

#include <map>
#include <list>
#include <string>
#include <cstring>
#include <cassert>
using namespace std;

struct MONKEY
{
public:
    typedef struct item_t
    {
        int32_t id;
        size_t value;
    } item_t;
    enum fn_t
    {
        ADD,
        MULT,
        SQUARE,
    } fn_sel = fn_t::ADD;
    int32_t id = 0;
    int32_t fn_const = 0;
    int32_t m_true = 0;
    int32_t m_false = 0;
    int32_t item_test = 1;

private:
    size_t BIG_CONSTANT = 223092870; // prod of each prime up to 23, e.g. 2*3*5*7*...*23
    size_t eval_ctr = 0;
    list<size_t> items;
    list<size_t> start_items;

    size_t eval_item(size_t val, bool relief)
    {
        eval_ctr++;
        size_t new_val = 0;
        switch (fn_sel)
        {
        case fn_t::ADD:
            new_val = val + fn_const;
            break;
        case fn_t::MULT:
            new_val = val * fn_const;
            break;
        case fn_t::SQUARE:
            new_val = val * val;
            break;
        default:
            new_val = val;
            break;
        }
        new_val = relief ? (new_val / 3) : (new_val % BIG_CONSTANT);
        return new_val;
    }

public:
    void ping_start(void) { start_items.assign(items.begin(), items.end()); }
    void reset_items(void)
    {
        eval_ctr = 0;
        items.assign(start_items.begin(), start_items.end());
    }
    void clear_values(void) { items.clear(); }
    void print_monkey(void)
    {
        cout << "Monkey " << id << ":\n";
        cout << "  Items: ";
        for (auto it : items)
            cout << it << ", ";
        cout << "\n";
        cout << "  Operator: new = ";
        if (fn_sel == fn_t::ADD)
            cout << "current + " << fn_const;
        if (fn_sel == fn_t::MULT)
            cout << "current * " << fn_const;
        if (fn_sel == fn_t::SQUARE)
            cout << "current * current";
        cout << "\n";
        cout << "  Test: divisible by " << item_test << "\n";
        cout << "    If true: throw to monkey id " << m_true << "\n";
        cout << "    If false: throw to monkey id " << m_false << "\n";
    }
    size_t get_inspect(void) { return eval_ctr; }
    void append_value(size_t value) { items.push_back(value); }
    bool append_item(item_t item)
    {
        if (item.id != id)
            return false;
        items.push_back(item.value);
        return true;
    }
    void eval_items(list<item_t> *throws, bool relief = true)
    {
        assert(throws != NULL);
        size_t val;
        while (!items.empty())
        {
            val = eval_item(items.front(), relief);
            if (val % item_test == 0)
                throws->push_back({m_true, val});
            else
                throws->push_back({m_false, val});
            items.pop_front();
        }
    }
};

#define BUFSIZE 1024
/** used in processing */
map<int32_t, MONKEY> data;
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

    list<int32_t> items;
    MONKEY m;

    char *indata = new char[BUFSIZE];
    char *ptr = NULL;
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
            // Read monkey id
            ifs.getline(indata, BUFSIZE);
            if (1 != sscanf(indata, "Monkey %d:", &m.id))
                break;
            // Read item list
            ifs.getline(indata, BUFSIZE);
            m.clear_values(); // clear current list
            ptr = strtok(indata + 16, " ,:");
            while (ptr != NULL)
            {
                m.append_value(atoi(ptr));
                ptr = strtok(NULL, " ,");
            }
            // Read operator type and constant
            ifs.getline(indata, BUFSIZE);
            if (string("  Operation: new = old * old").compare(indata) == 0)
            {
                m.fn_const = 1;
                m.fn_sel = MONKEY::fn_t::SQUARE;
            }
            else
            {
                if (1 == sscanf(indata, "  Operation: new = old * %d", &m.fn_const))
                    m.fn_sel = MONKEY::fn_t::MULT;
                else if (1 == sscanf(indata, "  Operation: new = old + %d", &m.fn_const))
                    m.fn_sel = MONKEY::fn_t::ADD;
            }
            // Read test value
            ifs.getline(indata, BUFSIZE);
            sscanf(indata, "  Test: divisible by %d", &m.item_test);
            // Read throw to id if true
            ifs.getline(indata, BUFSIZE);
            sscanf(indata, "    If true: throw to monkey %d", &m.m_true);
            // Read throw to id if false
            ifs.getline(indata, BUFSIZE);
            sscanf(indata, "    If false: throw to monkey %d", &m.m_false);

            data[m.id] = m;
            data[m.id].ping_start();
            ctr++;
        }
    }
    cnt = ctr;

    ifs.close();
    delete[] indata;
}

void write_output(const char *filename, size_t task1, size_t task2)
{
    ofstream ofs(filename, ofstream::out);
    ofs << "Task 1 result: " << task1 << "\n";
    ofs << "Task 2 result: " << task2 << "\n";
    ofs.close();
}

void play(size_t rounds, list<size_t> *times, bool relief)
{
    list<MONKEY::item_t> *throws = new list<MONKEY::item_t>();
    for (size_t k = 0; k < rounds; k++)
    {
        for (int32_t i = 0; i < cnt; i++)
        {
            data[i].eval_items(throws, relief);
            while (!throws->empty())
            {
                data[throws->front().id].append_item(throws->front());
                throws->pop_front();
            }
            if (k == rounds - 1)
                times->push_back(data[i].get_inspect());
        }
    }
    delete throws;
}

void process_task()
{
    // prints the monkey content
    for (auto it = data.begin(); it != data.end(); ++it)
        it->second.print_monkey();
    cout << "\n";
}

size_t process_task1(void)
{
    size_t m_business = 1;
    list<size_t> times;

    play(20, &times, true);

    times.sort();
    m_business *= times.back();
    times.pop_back();
    m_business *= times.back();

    return m_business;
}
size_t process_task2(void)
{
    size_t m_business = 1;
    list<size_t> times;

    for (auto &it : data)
        it.second.reset_items();

    play(10000, &times, false);

    times.sort();
    m_business *= times.back();
    times.pop_back();
    m_business *= times.back();

    return m_business;
}
int main()
{
    /** use this for testing examples */
    // read_input("test.txt");

    /** use this to run input */
    read_input("input.txt");

    // process_task();

    size_t task1 = process_task1();
    size_t task2 = process_task2();

    /** store output to file */
    write_output("output.txt", task1, task2);

    cout << "Task 1 result: " << task1 << "\n";
    cout << "Task 2 result: " << task2 << "\n";

    // delete[] data;
    /** add delete to all data here */
    return 0;
}