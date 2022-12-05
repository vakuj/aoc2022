#include <iostream>
#include <fstream>
#include <algorithm>
#include <deque>

#define BUFSIZE 1024
using namespace std;

/** used in processing */

deque<char> *cargo;
typedef struct move_t
{
    int32_t from;
    int32_t to;
    int32_t count;
} move_t;
move_t *moves;
int32_t csize;
int32_t msize;

int32_t cnt;
size_t length;
/** add additional */

void to_cargo(const char *cstr, size_t count)
{
    for (size_t i = 0; i < count; i += 4)
    {
        if (cstr[i + 1] >= 'A' && cstr[i + 1] <= 'Z')
        {
            cargo[i / 4].push_front(cstr[i + 1]);
            if ((i / 4) + 1 > (size_t)csize)
                csize = (int32_t)(i / 4) + 1;
        }
    }
}
inline void to_move(const char *cstr)
{
    int32_t n = sscanf(cstr, "move %d from %d to %d", &moves[msize].count, &moves[msize].from, &moves[msize].to);
    msize++;
    if (n != 3)
        exit(1);
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

    cargo = new deque<char>[length];
    moves = new move_t[length];

    char *indata = new char[BUFSIZE];
    // int32_t ctr = 0;
    bool movements = false;
    msize = 0;
    csize = 0;
    while (!ifs.eof())
    {
        int next = ifs.peek();
        if (next == '\n')
        {
            /** forward without storing */
            movements = true;
            ifs.getline(indata, BUFSIZE);
        }
        else
        {
            /** store for later processing */
            ifs.getline(indata, BUFSIZE);
            if (!movements)
            {
                if (indata[1] != '1')
                    to_cargo(indata, ifs.gcount());
            }
            else
            {
                to_move(indata);
            }
        }
    }

    ifs.close();
    delete[] indata;
}

void write_output(const char *filename, char *task1, char *task2)
{
    ofstream ofs(filename, ofstream::out);
    ofs << "Task 1 result: " << task1 << "\n";
    ofs << "Task 2 result: " << task2 << "\n";
    ofs.close();
}
void process_task()
{
    /** Do something to process the task in general */
}
void CrateMover9000(move_t move)
{
    for (int32_t i = 0; i < move.count; i++)
    {
        if (!cargo[move.from - 1].empty())
        {
            cargo[move.to - 1].push_back(cargo[move.from - 1].back());
            cargo[move.from - 1].pop_back();
        }
    }
}
void CrateMover9001(move_t move)
{
    deque<char> tmp;
    for (int32_t i = 0; i < move.count; i++)
    {
        if (!cargo[move.from - 1].empty())
        {
            tmp.push_front(cargo[move.from - 1].back());
            cargo[move.from - 1].pop_back();
        }
    }
    for (int32_t i = 0; i < move.count; i++)
    {
        cargo[move.to - 1].push_back(tmp.front());
        tmp.pop_front();
    }
}

char *process_task1(void)
{
    for (int32_t i = 0; i < msize; i++)
    {
        CrateMover9000(moves[i]);
    }
    char *top = new char[BUFSIZE];
    for (int32_t i = 0; i < csize; i++)
    {
        top[i] = cargo[i].back();
    }
    top[csize] = '\0';
    return top;
}
char *process_task2(void)
{
    for (int32_t i = 0; i < msize; i++)
    {
        CrateMover9001(moves[i]);
    }
    char *top = new char[BUFSIZE];
    for (int32_t i = 0; i < csize; i++)
    {
        top[i] = cargo[i].back();
    }
    top[csize] = '\0';
    return top;
}
int main()
{
    /** use this for testing examples */
    // read_input("test.txt");

    /** use this to run input */
    read_input("input.txt");

    // process_task();
    deque<char> *cargo_copy = new deque<char>[length];
    copy(cargo, cargo + length, cargo_copy);
    char *task1 = process_task1();
    copy(cargo_copy, cargo_copy + length, cargo);

    char *task2 = process_task2();

    /** store output to file */
    write_output("output.txt", task1, task2);

    cout << "Task 1 result: " << task1 << "\n";
    cout << "Task 2 result: " << task2 << "\n";

    delete[] cargo;
    delete[] moves;
    delete[] task1;
    delete[] task2;
    delete[] cargo_copy;
    /** add delete to all data here */
    return 0;
}