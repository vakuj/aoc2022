#include <iostream>
#include <fstream>
#include <algorithm>
#include <map>
#include <set>
#include <cstring>
#include <cassert>
using namespace std;

#define BUFSIZE 1024
/** used in processing */
typedef struct pos_t
{
    int32_t x;
    int32_t y;
} pos_t;
enum dir_t
{
    UP,
    LEFT,
    RIGHT,
    DOWN,
    ULEFT,
    URIGHT,
    DLEFT,
    DRIGHT,
    IGNORE
};
typedef struct move_t
{
    int32_t step;
    dir_t dir;
} move_t;
static map<char, dir_t> char_to_dir = {
    {'D', dir_t::DOWN},
    {'U', dir_t::UP},
    {'R', dir_t::RIGHT},
    {'L', dir_t::LEFT}};
move_t *data;
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

    data = new move_t[length];

    char *indata = new char[BUFSIZE];
    int32_t ctr = 0;
    int32_t len = 0;
    char d = '\0';
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
            if (2 == sscanf(indata, "%c %d", &d, &len))
            {
                data[ctr].step = len;
                data[ctr].dir = char_to_dir[d];
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

void print_history(map<int32_t, map<int32_t, int32_t>> hist)
{
    int32_t width = 0;
    int32_t height = 0;
    // find the range of data points
    for (auto x : hist)
    {
        if (x.first < 0)
            width = (-2 * x.first > width) ? -2 * x.first : width;
        else
            width = (2 * x.first > width) ? 2 * x.first : width;
        for (auto y : x.second)
        {
            if (y.first < 0)
                height = (-2 * y.first > height) ? -2 * y.first : height;
            else
                height = (2 * y.first > height) ? 2 * y.first : height;
        }
    }
    // add some padding
    height += 10;
    width += 10;
    for (int32_t j = -height / 2; j < height / 2; j++)
    {
        for (int32_t i = -width / 2; i < width / 2; i++)
        {
            if (hist.count(i) > 0)
            {
                if (hist[i].count(j) > 0)
                    cout << "#";
                else
                    cout << ".";
            }
            else
                cout << ".";
        }
        cout << "\n";
    }
    cout << "===\n";
}
void process_task()
{
    /** Do something to process the task in general */
}

inline bool is_touching(pos_t hp, pos_t tp)
{
    for (int32_t dx = -1; dx < 2; dx++)
        for (int32_t dy = -1; dy < 2; dy++)
            if ((hp.x - dx == tp.x) && (hp.y - dy == tp.y))
                return true;

    return false;
}
dir_t find_direction(pos_t *head, pos_t *tail)
{
    if (is_touching(*head, *tail))
        return dir_t::IGNORE;

    dir_t dir = dir_t::IGNORE;
    int32_t dx = head->x - tail->x;
    int32_t dy = head->y - tail->y;
    if (dx == 0)
    {
        if (dy > 0)
            dir = dir_t::UP;
        else if (dy < 0)
            dir = dir_t::DOWN;
        else // dy == 0
            dir = dir_t::IGNORE;
    }
    else if (dx > 0)
    {
        if (dy == 0)
            dir = dir_t::RIGHT;
        else if (dy > 0)
            dir = dir_t::URIGHT;
        else // dy < 0
            dir = dir_t::DRIGHT;
    }
    else // dx < 0
    {
        if (dy == 0)
            dir = dir_t::LEFT;
        else if (dy > 0)
            dir = dir_t::ULEFT;
        else // dy < 0
            dir = dir_t::DLEFT;
    }
    return dir;
}

void update_knot(pos_t *knot, dir_t dir)
{
    assert(knot != NULL);

    switch (dir)
    {
    case dir_t::DOWN:
        knot->y--;
        break;
    case dir_t::UP:
        knot->y++;
        break;
    case dir_t::LEFT:
        knot->x--;
        break;
    case dir_t::RIGHT:
        knot->x++;
        break;
    case dir_t::ULEFT:
        knot->y++;
        knot->x--;
        break;
    case dir_t::URIGHT:
        knot->y++;
        knot->x++;
        break;
    case dir_t::DLEFT:
        knot->y--;
        knot->x--;
        break;
    case dir_t::DRIGHT:
        knot->y--;
        knot->x++;
        break;
    default:
        break;
    }
}
int32_t process_task1(void)
{
    pos_t *knots = new pos_t[2];
    knots[0] = {0, 0};
    knots[1] = {0, 0};
    dir_t dir = dir_t::IGNORE;
    int32_t tmoves = 0;
    map<int32_t, map<int32_t, int32_t>> thist;
    for (int32_t i = 0; i < cnt; i++)
    {
        for (int32_t j = 0; j < data[i].step; j++)
        {
            update_knot(knots, data[i].dir);
            dir = find_direction(knots, knots + 1);
            update_knot(knots + 1, dir);

            thist[knots[1].x][knots[1].y] += 1;
        }
    }
    // print_history(thist);
    for (auto x : thist)
        tmoves += (int32_t)x.second.size();
    delete[] knots;
    return tmoves;
}
int32_t process_task2(void)
{
    int32_t tmoves = 0;
    int32_t n = 10;
    dir_t dir = dir_t::IGNORE;
    pos_t *knots = new pos_t[n];
    for (int32_t k = 0; k < n; k++)
        knots[k] = {0, 0};
    map<int32_t, map<int32_t, int32_t>> thist;
    for (int32_t i = 0; i < cnt; i++)
    {
        for (int32_t j = 0; j < data[i].step; j++)
        {
            dir = dir_t::IGNORE;
            update_knot(knots, data[i].dir);
            for (int k = 0; k < n - 1; k++)
            {
                dir = find_direction(knots + k, knots + k + 1);
                update_knot(knots + k + 1, dir);
            }
            thist[knots[n - 1].x][knots[n - 1].y] += 1;
        }
    }
    // print_history(thist);
    for (auto x : thist)
        tmoves += (int32_t)x.second.size();
    delete[] knots;
    return tmoves;
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

    delete[] data;
    /** add delete to all data here */
    return 0;
}