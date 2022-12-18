#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <cassert>
using namespace std;
#define BUFSIZE 1024
/** used in processing */
typedef struct coord_t
{
    int32_t x;
    int32_t y;
} coord_t;
typedef struct pair_t
{
    coord_t sensor;
    coord_t beacon;
    int32_t distance;
} pair_t;
list<pair_t> *data;
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

    data = new list<pair_t>();
    pair_t sb = {{0, 0}, {0, 0}, 0};
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
            ifs.getline(indata, BUFSIZE);
            if (4 != sscanf(indata, "Sensor at x=%d, y=%d: closest beacon is at x=%d, y=%d", &sb.sensor.x, &sb.sensor.y, &sb.beacon.x, &sb.beacon.y))
                assert(0 && "undefined behavior");
            data->push_back(sb);
            ctr++;
        }
    }
    cnt = ctr;
    ifs.close();
    delete[] indata;
}

void write_output(const char *filename, size_t task1, int32_t task2)
{
    ofstream ofs(filename, ofstream::out);
    ofs << "Task 1 result: " << task1 << "\n";
    ofs << "Task 2 result: " << task2 << "\n";
    ofs.close();
}
/**
 * @brief calculates the absolute distance between two integers
 *
 * @param a first integer
 * @param b second integer
 * @return int32_t the absolute distance
 */
inline int32_t abs_distance(int32_t a, int32_t b)
{
    return (a > b) ? (a - b) : (b - a);
}
/**
 * @brief calculates the manhattan distance between two points
 *
 * @param a first point
 * @param b second point
 * @return int32_t manhattan distance
 */
inline int32_t abs_distance(coord_t a, coord_t b)
{
    return (abs_distance(a.x, b.x) + abs_distance(a.y, b.y));
}
void process_task()
{
    /** calculate the distances */
    for (auto it = data->begin(); it != data->end(); it++)
        it->distance = abs_distance(it->sensor, it->beacon);
}
size_t find_impossible_at_y(int32_t target_y)
{
    set<int32_t> impossible;
    for (auto it : *data)
    {
        int32_t dist = abs_distance(it.sensor.y, target_y);
        if (dist < it.distance)
            for (int32_t dx = 0; dx < (it.distance - dist) * 2; dx++)
                impossible.insert((it.sensor.x - it.distance + dist + dx));
    }
    return impossible.size();
}
coord_t find_impossible_coord(int32_t upper_bound)
{
    map<int32_t, map<int32_t, bool>> impossible;
    coord_t current = {0, 0};
    for (auto it : *data)
    {
        for (int32_t dx = 0; dx < it.distance * 2; dx++)
        {
            for (int32_t dy = 0; dy < it.distance * 2; dy++)
            {
                current.x = it.sensor.x - it.distance + dx;
                current.y = it.sensor.y - it.distance + dy;
                if ((0 <= current.x && current.x < upper_bound) && (0 <= current.y && current.y < upper_bound))
                    if (abs_distance(current, it.sensor) <= it.distance)
                        impossible[current.x][current.y] = true;
            }
        }
    }
    current.x = 0;
    current.y = 0;
    for (int32_t x = 0; x < upper_bound; x++)
    {
        if (impossible.count(x) < (size_t)upper_bound)
        {
            current.x = x;
            for (int32_t y = 0; y < upper_bound; y++)
            {
                if (impossible[x].count(y) == 0)
                {
                    current.y = y;
                    break;
                }
            }
        }
        if (current.y != 0)
            break;
    }
    return current;
}
size_t process_task1(void)
{
    // return find_impossible_at_y(10);
    return find_impossible_at_y(2000000);
}
int32_t process_task2(void)
{
    /** Do something to process the task specific to 2 */
    // coord_t c = find_impossible_coord(20);
    coord_t c = find_impossible_coord(4000000);
    return c.x * 4000000 + c.y;
}
int main()
{
    /** use this for testing examples */
    // read_input("test.txt");

    /** use this to run input */
    read_input("input.txt");

    process_task();

    size_t task1 = process_task1();
    int32_t task2 = process_task2();

    /** store output to file */
    write_output("output.txt", task1, task2);

    cout << "Task 1 result: " << task1 << "\n";
    cout << "Task 2 result: " << task2 << "\n";

    delete data;
    /** add delete to all data here */
    return 0;
}