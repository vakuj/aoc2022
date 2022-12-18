#include <iostream>
#include <fstream>
#include <algorithm>

#include <vector>
#include <list>
#include <stack>
#include <map>

#include <cstring>
#include <cassert>

using namespace std;

#define BUFSIZE 1024
#define CAVE_SIZE 1000
/** types identifiable by scan */
enum stype_t
{
    FREE,  // Air pocket (not occupied by anything)
    ROCK,  // Rock wall
    SAND,  // Sand unit
    FLOOR, // Floor marker
    CONT,  // Continuous flow of sand
};
/** coordinate type with x and y */
typedef struct coord_t
{
    int32_t x;
    int32_t y;
} coord_t;
/** convertes stype_t to char for visualization */
static map<stype_t, char> stype_to_char = {
    {FREE, '.'},
    {ROCK, '#'},
    {SAND, 'o'},
    {FLOOR, 'X'},
    {CONT, '~'},
};
list<list<coord_t>> *data;
vector<vector<stype_t>> *cave;
int32_t low_point = 0;
int32_t cave_width, cave_height;
int32_t cnt;
size_t length;
/** add additional */

void parse_coord(char *cstr, list<coord_t> *dst)
{
    const char *tokens = " ->";
    char *ptr;
    ptr = strtok(cstr, tokens);
    coord_t coord = {0, 0};
    dst->clear();
    while (ptr != NULL)
    {
        if (2 != sscanf(ptr, "%d,%d", &coord.x, &coord.y))
            assert(0 && "Unexpected");
        low_point = (coord.y > low_point) ? coord.y : low_point;
        dst->push_back(coord);
        ptr = strtok(NULL, tokens);
    }
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

    data = new list<list<coord_t>>();
    list<coord_t> *coords = new list<coord_t>();

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
            parse_coord(indata, coords);
            data->push_back(*coords);
            ctr++;
        }
    }
    cnt = ctr;
    ifs.close();
    delete[] indata;
    delete coords;
}
void write_output(const char *filename, int32_t task1, int32_t task2)
{
    ofstream ofs(filename, ofstream::out);
    ofs << "Task 1 result: " << task1 << "\n";
    ofs << "Task 2 result: " << task2 << "\n";
    ofs.close();
}

void insert_rock_walls(list<coord_t> src, vector<vector<stype_t>> *dst)
{
    auto it = src.begin();
    coord_t start, stop;
    start = *it;
    it++;
    while (it != src.end())
    {

        stop = *it;
        if (start.x < stop.x)
            for (int32_t x = start.x; x <= stop.x; x++)
                dst->data()[x][start.y] = stype_t::ROCK;
        else
            for (int32_t x = stop.x; x <= start.x; x++)
                dst->data()[x][start.y] = stype_t::ROCK;
        if (start.y < stop.y)
            for (int32_t y = start.y; y <= stop.y; y++)
                dst->data()[start.x][y] = stype_t::ROCK;
        else
            for (int32_t y = stop.y; y <= start.y; y++)
                dst->data()[start.x][y] = stype_t::ROCK;
        start = stop;
        it++;
    }
}
inline bool isInBounds(coord_t t)
{
    return ((0 <= t.x && t.x < cave_width) && (0 <= t.y && t.y < cave_height));
}
inline bool isSame(coord_t a, coord_t b)
{
    return (a.x == b.x && a.y == b.y);
}
void write_cave(const char *filename, coord_t start, coord_t stop)
{
    ofstream ofs(filename, ofstream::out);

    for (int32_t y = start.y; y < stop.y; y++)
    {
        for (int32_t x = start.x; x < stop.x; x++)
        {
            if (isInBounds({x, y}))
                ofs << stype_to_char[cave->data()[x][y]];
        }
        ofs << "\n";
    }
    ofs.close();
}
void clear_sand(bool hasFloor)
{
    // remove all but rock walls
    for (int32_t x = 0; x < cave_width; x++)
        for (int32_t y = 0; y < cave_height; y++)
            cave->data()[x][y] = (cave->data()[x][y] != stype_t::ROCK) ? stype_t::FREE : stype_t::ROCK;

    // fill in floor if needed
    if (hasFloor)
        for (int32_t x = 0; x < cave_width; x++)
            cave->data()[x][cave_height - 1] = stype_t::FLOOR;
}
coord_t make_sand_physics(coord_t current)
{
    if (!isInBounds({current.x, current.y + 1}))
        return {current.x, current.y + 1}; // return out-of-bound coord if next is out-of-bounds

    if (cave->data()[current.x][current.y + 1] == stype_t::FREE) // check down
        return {current.x, current.y + 1};

    if (cave->data()[current.x - 1][current.y + 1] == stype_t::FREE) // check down left
        return {current.x - 1, current.y + 1};

    if (cave->data()[current.x + 1][current.y + 1] == stype_t::FREE) // check down right
        return {current.x + 1, current.y + 1};

    return current;
}
int32_t simulate_sand_physics(coord_t inlet, bool hasFloor = false)
{
    clear_sand(hasFloor);
    stack<coord_t> prev;
    coord_t next;
    prev.push(inlet);
    int32_t ctr = 0;
    while (!prev.empty())
    {
        next = make_sand_physics(prev.top());
        if (!isInBounds(next))
            break;
        if (!isSame(next, prev.top()))
            prev.push(next);
        else
        {
            cave->data()[next.x][next.y] = stype_t::SAND;
            prev.pop();
            ctr++;
        }
    }
    while (!prev.empty())
    {
        // unwind the stack and fill in with continuous stream.
        cave->data()[prev.top().x][prev.top().y] = stype_t::CONT;
        prev.pop();
    }
    return ctr;
}
void process_task()
{
    /** Construct initial cave */
    cave_width = CAVE_SIZE;
    cave_height = low_point + 3;
    cave = new vector<vector<stype_t>>(cave_width);
    for (auto it = cave->begin(); it != cave->end(); it++)
        it->resize(cave_height);
    for (auto it = data->begin(); it != data->end(); it++)
        insert_rock_walls(*it, cave);
}
int32_t process_task1(void)
{
    return simulate_sand_physics({500, 0});
}
int32_t process_task2(void)
{
    return simulate_sand_physics({500, 0}, true);
}
int main()
{
    /** use this for testing examples */
    // read_input("test.txt");

    /** use this to run input */
    read_input("input.txt");

    process_task();

    int32_t task1 = process_task1();
    write_cave("Cave_pt1.txt", {0, 0}, {cave_width, cave_height});
    int32_t task2 = process_task2();
    write_cave("Cave_pt2.txt", {0, 0}, {cave_width, cave_height});

    /** store output to file */
    write_output("output.txt", task1, task2);

    cout << "Task 1 result: " << task1 << "\n";
    cout << "Task 2 result: " << task2 << "\n";

    delete data;
    delete cave;
    /** add delete to all data here */
    return 0;
}