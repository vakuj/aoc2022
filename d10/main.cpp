#include <iostream>
#include <fstream>
#include <algorithm>

#include <stack>
#include <list>
using namespace std;
#define BUFSIZE 1024
#define WIDTH 40
#define HEIGHT 6
/** used in processing */
enum instr_t
{
    NOOP,
    ADDX,
    IGNORE,
};
typedef struct op_t
{
    instr_t instr;
    int32_t value;
} op_t;
list<op_t> *data;
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

    data = new list<op_t>();
    int32_t d = 0;
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
            if (1 == sscanf(indata, "addx %d", &d))
                data->push_back({instr_t::ADDX, d});
            else
                data->push_back({instr_t::NOOP, 0});
        }
    }
    cnt = ctr;
    ifs.close();
    delete[] indata;
}

void write_output(const char *filename, int32_t task1, char *task2)
{
    ofstream ofs(filename, ofstream::out);
    ofs << "Task 1 result: " << task1 << "\n";
    ofs << "Task 2 result:\n";
    for (size_t i = 0; i < WIDTH * HEIGHT; i++)
        ofs << task2[i] << (i % WIDTH == WIDTH - 1 ? "\n" : "");
    ofs << "\n";
    ofs.close();
}
void process_task(stack<op_t> *op_stack)
{
    for (auto it = data->rbegin(); it != data->rend(); it++)
    {
        switch (it->instr)
        {
        case instr_t::ADDX:
            op_stack->push(*it);
            op_stack->push({instr_t::NOOP, 0});
            break;
        case instr_t::NOOP:
            op_stack->push({instr_t::NOOP, 0});
        default:
            break;
        }
    }
}
inline void atomic_exec_instruct(stack<op_t> *ops, int32_t *xreg)
{
    if (ops->top().instr == instr_t::ADDX)
        *xreg += ops->top().value;
    ops->pop();
}
inline void update_pixel(char *image, int32_t xreg, int32_t clk_cycle)
{
    clk_cycle %= WIDTH * HEIGHT;
    int32_t hpos = clk_cycle % WIDTH;
    for (int32_t i = -1; i < 2; i++)
        if (xreg + i == hpos)
            image[clk_cycle] = '#';
}
int32_t process_task1(void)
{
    int32_t xreg = 1;
    stack<op_t> atomic_stack;
    int32_t strength = 0;
    int32_t ctr = 0;
    process_task(&atomic_stack);
    while (!atomic_stack.empty())
    {
        atomic_exec_instruct(&atomic_stack, &xreg);
        if (ctr % 40 == 19)
            strength += (ctr + 1) * xreg;
        ctr++;
    }
    return strength;
}

char *process_task2(void)
{
    int32_t xreg = 1;
    stack<op_t> atomic_stack;

    char *image = new char[WIDTH * HEIGHT];
    for (size_t i = 0; i < WIDTH * HEIGHT; i++)
        image[i] = '.';

    int32_t ctr = 0;
    process_task(&atomic_stack);
    update_pixel(image, xreg, ctr++);
    while (!atomic_stack.empty())
    {
        atomic_exec_instruct(&atomic_stack, &xreg);
        update_pixel(image, xreg, ctr++);
    }
    return image;
}
int main()
{
    /** use this for testing examples */
    // read_input("test.txt");

    /** use this to run input */
    read_input("input.txt");

    int32_t task1 = process_task1();
    char *task2 = process_task2();

    /** store output to file */
    write_output("output.txt", task1, task2);

    cout << "Task 1 result: " << task1 << "\n";
    cout << "Task 2 result:\n";
    for (size_t i = 0; i < WIDTH * HEIGHT; i++)
        cout << task2[i] << (i % WIDTH == WIDTH - 1 ? "\n" : "");
    cout << "\n";

    delete data;
    delete[] task2;
    /** add delete to all data here */
    return 0;
}