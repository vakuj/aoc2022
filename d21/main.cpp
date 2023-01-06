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
    int64_t value = 0;
    bool exec = false;

    bool execute(int64_t a, int64_t b)
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
    void set_value(int64_t _value) { value = _value; }
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
    bool is_constant(void) { return fun == fun_t::CON; }
    string get_depend(bool first = true)
    {
        if (first)
            return id_a;
        return id_b;
    }
    bool try_execute(map<string, int64_t> *executed)
    {
        if (fun == fun_t::CON)
        {
            exec = true;
            if (executed->count(id) == 0)
                executed->insert(pair<string, int64_t>(id, value));
            else
                executed->at(id) = value;
            return true;
        }
        if (executed->count(id_a) == 0)
            return false;
        if (executed->count(id_b) == 0)
            return false;
        if (execute(executed->at(id_a), executed->at(id_b)))
        {
            if (executed->count(id) == 0)
                executed->insert(pair<string, int64_t>(id, value));
            else
                executed->at(id) = value;
        }
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

void write_output(const char *filename, int64_t task1, int64_t task2)
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
int64_t process_task1(void)
{
    map<string, int64_t> executed;
    list<MONKEY> pending;
    list<MONKEY> monkeys;
    monkeys.assign(data->begin(), data->end());
    auto it = monkeys.begin();
    while (it != monkeys.end())
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
    assert(executed.count("root") > 0);
    return executed["root"];
}
int64_t monkey_fun(int64_t value, list<MONKEY> *exec_order, map<string, int64_t> *executed)
{
    auto it = exec_order->begin();
    while (it != exec_order->end())
    {
        if (it->get_id() == "humn")
            it->set_value(value);
        it->try_execute(executed);
        it++;
    }
    return executed->at("root");
}
int64_t correct_human(list<MONKEY> *exec_order, map<string, int64_t> *executed, int64_t constant = 150)
{
    auto f = [exec_order, executed, constant](int64_t x)
    {
        return monkey_fun(x, exec_order, executed) - 2 * constant;
    };
    cout << constant << endl;
    int64_t a = (1L << 50), b = -1 * a - 1, c;
    int64_t fa = f(a), fb = f(b), fc;
    size_t ctr = 0;
    while (ctr < 100)
    {
        ctr++;
        c = (a + b) / 2;
        fc = f(c);
        cout << ctr << ": " << a << ", " << c << ", " << b;
        cout << " -> " << fa << ", " << fc << ", " << fb << endl;
        if (fc == 0)
            return c;
        if (fc >= 0)
        {
            if (fa >= 0)
            {
                a = c;
                fa = fc;
            }
            else
            {
                b = c;
                fb = fc;
            }
        }
        else
        {
            if (fa < 0)
            {
                a = c;
                fa = fc;
            }
            else
            {
                b = c;
                fb = fc;
            }
        }
    }
    assert(0 && "fail");
    return 0;
}

int64_t process_task2(void)
{
    /** Do something to process the task specific to 2 */
    map<string, int64_t> executed;
    list<MONKEY> pending;
    list<MONKEY> exec_order;
    list<MONKEY> monkeys;
    monkeys.assign(data->begin(), data->end());
    MONKEY root;
    MONKEY humn;
    auto it = monkeys.begin();
    while (it != monkeys.end())
    {
        if (it->get_id() == "root")
            root = *it;
        else if (it->get_id() == "humn")
            humn = *it;
        else
        {
            if (!it->try_execute(&executed))
                pending.push_back(*it);
        }
        it++;
    }
    /** should parse all non humn related monkeys */
    MONKEY monkey;
    size_t psize = pending.size();
    size_t pctr = 0;
    while (pending.size() > 0)
    {
        pctr = 0;
        while (pctr < psize)
        {
            monkey = pending.front();
            pending.pop_front();
            if (!monkey.try_execute(&executed))
                pending.push_back(monkey);
            pctr++;
        }
        if (pending.size() == psize)
            break;
        psize = pending.size();
    }
    int64_t constant = 0;
    if (executed.count(root.get_depend()))
        constant = executed[root.get_depend()];
    else
        constant = executed[root.get_depend(false)];
    assert(constant != 0);

    /** now parse all humn related monkeys and set to exec order as they get executed */
    humn.try_execute(&executed);
    exec_order.push_front(humn);
    while (pending.size() > 0)
    {
        monkey = pending.front();
        pending.pop_front();
        if (!monkey.try_execute(&executed))
            pending.push_back(monkey);
        else
            exec_order.push_back(monkey);
    }
    root.try_execute(&executed);
    exec_order.push_back(root);

    for (auto it : executed)
        cout << it.first << ": " << it.second << endl;
    // string id_a = exec_order->back().get_depend(true);
    // string id_b = exec_order->back().get_depend(false);
    // int64_t constant = 0;

    int64_t answer = correct_human(&exec_order, &executed, constant);

    assert(executed.count("root") > 0);
    return answer;
}
int main()
{
    /** use this for testing examples */
    // read_input("test.txt");

    /** use this to run input */
    read_input("input.txt");
    // 3582317956032 too high
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