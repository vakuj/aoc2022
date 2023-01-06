#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <map>
#include <list>
#include <string>
#include <cassert>

using namespace std;

#define BUFSIZE 1024
/** used in processing */
struct MONKEY
{
    enum fun_t
    {
        CON, // id = constant
        ADD, // id = id_a + id_b
        SUB, // id = id_a - id_b
        MUL, // id = id_a * id_b
        DIV, // id = id_a / id_b
    };

private:
    string id;
    string id_a;
    string id_b;
    fun_t fun = fun_t::CON;
    int64_t value;
    bool exec = false;

    bool execute(int32_t a, int32_t b)
    {
        exec = true;
        switch (fun)
        {
        case fun_t::ADD:
            value = a + b;
            break;
        case fun_t::SUB:
            value = a - b;
            break;
        case fun_t::MUL:
            value = a * b;
            break;
        case fun_t::DIV:
            value = a / b;
            break;
        default:
            exec = false;
            break;
        }
        return exec;
    }
    string get_fun(void)
    {
        string str = id;
        str += ": ";
        str += id_a;
        switch (fun)
        {
        case fun_t::CON:
            str += to_string(value);
            break;
        case fun_t::ADD:
            str += " + ";
            break;
        case fun_t::SUB:
            str += " - ";
            break;
        case fun_t::MUL:
            str += " * ";
            break;
        case fun_t::DIV:
            str += " / ";
            break;
        default:
            break;
        }
        str += id_b;
        if (exec)
        {
            str += " -> ";
            str += to_string(value);
        }
        return str;
    }

public:
    MONKEY() {}
    MONKEY(const char *indata)
    {
        // zczc: 2
        // ptdq: humn - dvpt
        char *_id = new char[5];
        char *_ida = new char[5];
        char *_idb = new char[5];
        if (2 == sscanf(indata, "%4s: %ld", _id, &value))
        {
            fun = fun_t::CON;
            id = string(_id);
            id_a = "";
            id_b = "";
        }
        else
        {
            char c = ' ';
            if (4 == sscanf(indata, "%4s: %4s %c %4s", _id, _ida, &c, _idb))
            {
                switch (c)
                {
                case '+':
                    fun = fun_t::ADD;
                    break;
                case '-':
                    fun = fun_t::SUB;
                    break;
                case '*':
                    fun = fun_t::MUL;
                    break;
                case '/':
                    fun = fun_t::DIV;
                    break;

                default:
                    break;
                }
            }
            id = string(_id);
            id_a = string(_ida);
            id_b = string(_idb);
        }
        delete[] _id;
        delete[] _ida;
        delete[] _idb;
    }
    ~MONKEY() {}

    void set_id(string _id) { id = _id; }
    string get_id(void) { return id; }
    bool get_exec(void) { return exec; }
    void set_value(int32_t _value) { value = _value; }
    int64_t get_value(void) { return value; }
    void set_fun(fun_t _fun, string _id_a, string _id_b)
    {
        fun = _fun;
        if (_fun != fun_t::CON)
        {
            id_a = _id_a;
            id_b = _id_b;
        }
        else
        {
            id_a = "";
            id_b = "";
        }
    }
    bool try_execute(map<string, int32_t> *executed)
    {
        if (exec)
            return true;
        if (fun == fun_t::CON)
        {
            exec = true;
            executed->insert(pair<string, int32_t>(id, value));
            return true;
        }
        if (executed->count(id_a) == 0)
            return false;
        if (executed->count(id_b) == 0)
            return false;
        if (execute(executed->at(id_a), executed->at(id_b)))
            executed->insert(pair<string, int32_t>(id, value));
        return exec;
    }
    friend ostream &operator<<(ostream &os, MONKEY &m)
    {
        os << m.get_fun();
        return os;
    }
};

list<MONKEY> *data;
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

    data = new list<MONKEY>();

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
            data->push_back(MONKEY(indata));
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
    for (auto it : *data)
        cout << it << endl;
}
int32_t process_task1(void)
{
    /** Do something to process the task specific to 1 */
    map<string, int32_t> executed;
    list<MONKEY> pending;
    auto it = data->begin();
    while (it != data->end())
    {
        if (!it->try_execute(&executed))
            pending.push_back(*it);
        it++;
    }
    MONKEY monkey;
    while (pending.size() > 0)
    {
        monkey = pending.front();
        pending.pop_front();
        if (!monkey.try_execute(&executed))
        {
            pending.push_back(monkey);
        }
    }
    // for (auto it : executed)
    //     cout << it.first << ": " << it.second << endl;
    if (executed.count("root") == 0)
        return 0;
    cout << executed["root"] << endl;
    return executed["root"];
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
    // 571353930 too low
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