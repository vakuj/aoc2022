#include <iostream>
#include <fstream>
#include <algorithm>

#include <list>
#include <set>

#include <cassert>
using namespace std;
#define BUFSIZE 1024
/** used in processing */
/** coordinate data type with x and y*/
typedef struct coord_t
{
    int32_t x;
    int32_t y;
} coord_t;
/** sensor and beacon pair coordinates, includes distance between the pair */
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

void write_output(const char *filename, size_t task1, size_t task2)
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
inline bool inBounds(coord_t c, int32_t upper_bound)
{
    return ((0 <= c.x && c.x < upper_bound) && (0 <= c.y && c.y < upper_bound));
}
coord_t find_impossible_coord(int32_t upper_bound)
{
    /**
     * Idea:
     *  Add the data points just outside of the current
     *  boundary as shown below (the 1s, 2s, 3s and 4s).
     *  These will be data points will serve as possible
     *  candidates as possible locations where the can
     *  beacon exists.
     *
     *    2nd   1   1st
     *         2#1
     *        2###1
     *       2#####1
     *      2###X###4
     *       3#####4
     *        3###4
     *         3#4
     *    3rd   3   4th
     *
     *  When the set of possibles have been created, iterate
     *  through each point and compare distance to all sensors.
     *  If the current point is longer away from each of the
     *  sensors than their individual reach (sensor->beacon distance)
     *  then the current point is the one which holds the missing
     *  beacon.
     */

    list<coord_t> possible;
    coord_t current = {0, 0};
    // find and add all boundary elements as possible candidates
    for (auto it : *data)
    {
        current = it.sensor;
        current.x += it.distance + 1;
        // add boundary of 1st quadrant
        for (int32_t dx = 0; dx < it.distance + 1; dx++)
        {
            current.x--;
            current.y++;
            if (inBounds(current, upper_bound))
                possible.push_back(current);
        }
        // add boundary of 2nd quadrant
        for (int32_t dx = 0; dx < it.distance + 1; dx++)
        {
            current.x--;
            current.y--;
            if (inBounds(current, upper_bound))
                possible.push_back(current);
        }
        // add boundary of 3rd quadrant
        for (int32_t dx = 0; dx < it.distance + 1; dx++)
        {
            current.x++;
            current.y--;
            if (inBounds(current, upper_bound))
                possible.push_back(current);
        }
        // add boundary of 4th quadrant
        for (int32_t dx = 0; dx < it.distance + 1; dx++)
        {
            current.x++;
            current.y++;
            if (inBounds(current, upper_bound))
                possible.push_back(current);
        }
    }
    current.x = 0;
    current.y = 0;
    bool candidate;
    for (auto pit : possible)
    {
        candidate = true; // assume this is the "one"
        for (auto it : *data)
        {
            if (abs_distance(pit, {it.sensor}) <= it.distance)
                candidate = candidate ? false : candidate; // turns out the initial assumption was wrong.
        }
        if (candidate)
        {
            // assumption was correct -> beacon found!
            current = pit;
            break;
        }
    }
    return current;
}
size_t process_task1(void)
{
    return find_impossible_at_y(2000000);
}
size_t process_task2(void)
{
    coord_t c = find_impossible_coord(4000000);
    return ((size_t)c.x * 4000000 + (size_t)c.y);
}
int main()
{
    /** use this for testing examples */
    // read_input("test.txt");
    // size_t task1 = find_impossible_at_y(10);
    // coord_t c = find_impossible_coord(20);
    // size_t task2 = ((size_t)c.x * 4000000 + (size_t)c.y);

    /** use this to run input */
    read_input("input.txt");

    process_task();

    size_t task1 = process_task1();
    size_t task2 = process_task2();

    /** store output to file */
    write_output("output.txt", task1, task2);

    cout << "Task 1 result: " << task1 << "\n";
    cout << "Task 2 result: " << task2 << "\n";

    delete data;
    /** add delete to all data here */
    return 0;
}