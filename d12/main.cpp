#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <list>

using namespace std;
#define BUFSIZE 1024
/** used in processing */
typedef struct loc_t
{
    int32_t row;
    int32_t col;
} loc_t;
typedef struct cost_t
{
    int32_t left = 0;
    int32_t right = 0;
    int32_t up = 0;
    int32_t down = 0;
} cost_t;
enum dir_t
{
    LEFT,
    RIGHT,
    UP,
    DOWN,
    UNDEF
};
vector<vector<char>> data;
vector<vector<cost_t>> cost;
loc_t start, stop;
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

    vector<char> row;
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

            for (int32_t i = 0; i < ifs.gcount(); i++)
            {
                if (indata[i] != '\n' && indata[i] != '\0')
                    row.push_back(indata[i]);
            }
            data.push_back(row);
            row.clear();
            ctr++;
            /** store for later processing */
            // data[ctr++] = atoi(indata);
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
    char c;
    cost_t cst;
    vector<cost_t> row;
    // find start and stop
    for (int32_t i = 0; i < (int32_t)data.size(); i++)
    {
        for (int32_t j = 0; j < (int32_t)data[0].size(); j++)
        {
            c = data[i][j];
            if (c == 'S')
            {
                c = 'a';
                data[i][j] = 'a';
                start.row = i;
                start.col = j;
            }
            if (c == 'E')
            {
                c = 'z';
                data[i][j] = 'z';
                stop.row = i;
                stop.col = j;
            }
        }
    }
    // calculate cost of moving up,down,left,rigt from i,j
    // if outside of bounds: mark with INT32_MAX
    for (int32_t i = 0; i < (int32_t)data.size(); i++)
    {
        for (int32_t j = 0; j < (int32_t)data[0].size(); j++)
        {
            c = data[i][j];
            if (i - 1 >= 0)
                cst.up = (int32_t)(data[i - 1][j] - c);
            else
                cst.up = INT32_MAX;
            if (i + 1 < (int32_t)data.size())
                cst.down = (int32_t)(data[i + 1][j] - c);
            else
                cst.down = INT32_MAX;
            if (j - 1 >= 0)
                cst.left = (int32_t)(data[i][j - 1] - c);
            else
                cst.left = INT32_MAX;
            if (j + 1 < (int32_t)data[0].size())
                cst.right = (int32_t)(data[i][j + 1] - c);
            else
                cst.right = INT32_MAX;
            row.push_back(cst);
        }
        cost.push_back(row);
        row.clear();
    }
}
inline bool isSet(loc_t loc, vector<vector<bool>> *pool)
{
    return (pool->data()[loc.row][loc.col]);
}
inline bool isValidMove(loc_t loc, dir_t dir)
{
    bool valid = false;
    cost_t cst = cost[loc.row][loc.col];
    switch (dir)
    {
    case dir_t::UP:
        valid = ((cst.up <= 1));
        break;
    case dir_t::DOWN:
        valid = ((cst.down <= 1));
        break;
    case dir_t::LEFT:
        valid = ((cst.left <= 1));
        break;
    case dir_t::RIGHT:
        valid = ((cst.right <= 1));
        break;
    default:
        break;
    }
    return valid;
}
inline loc_t nextMove(loc_t curr, dir_t dir)
{
    loc_t next = curr;
    switch (dir)
    {
    case dir_t::DOWN:
        next.row += 1;
        break;
    case dir_t::UP:
        next.row -= 1;
        break;
    case dir_t::RIGHT:
        next.col += 1;
        break;
    case dir_t::LEFT:
        next.col -= 1;
        break;
    default:
        break;
    }
    return next;
}
/**
 * @brief Dijkstra's algorithm-ish
 *
 * @param source starting point
 * @param MAX_VALUE value used to initialize distances
 * @return vector<vector<int32_t>>* pointer to 2D-vector containing distance map
 */
vector<vector<int32_t>> *walk(loc_t source, const int32_t MAX_VALUE = INT32_MAX)
{
    static vector<dir_t> dirs = {dir_t::UP, dir_t::DOWN, dir_t::LEFT, dir_t::RIGHT}; // Set of movement diretions
    vector<vector<int32_t>> *dist = new vector<vector<int32_t>>(data.size());        // map of all distances from source
    vector<vector<bool>> *added = new vector<vector<bool>>(data.size());             // map of all added nodes
    list<loc_t> *nodes = new list<loc_t>();                                          // list of nodes destined for processing

    // Initialize maps
    for (int32_t x = 0; x < (int32_t)data.size(); x++)
    {
        added->data()[x].resize(data[0].size());
        dist->data()[x].resize(data[0].size());
        for (int32_t y = 0; y < (int32_t)data[0].size(); y++)
        {
            added->data()[x][y] = false;
            dist->data()[x][y] = MAX_VALUE;
        }
    }

    // set source to 0 dist, add source to nodes and mark source as set
    dist->data()[source.row][source.col] = 0;
    nodes->push_back(source);
    added->data()[source.row][source.col] = true;

    loc_t current, neighbour;
    int32_t ndist;
    while (!nodes->empty())
    {
        // take first node and remove it from nodes
        current = nodes->front();
        nodes->pop_front();

        for (size_t i = 0; i < dirs.size(); i++) // iterate each neighbour to current node
        {
            if (isValidMove(current, dirs[i])) // is movement to neighbour valid?
            {
                neighbour = nextMove(current, dirs[i]);
                if (!isSet(neighbour, added)) // is neighbour already added?
                {
                    // Nope: add to nodes and mark as set
                    nodes->push_back(neighbour);
                    added->data()[neighbour.row][neighbour.col] = true;
                }
                // calculate new distance to neighbour and update neighbours if new distance is less than before?
                ndist = dist->data()[current.row][current.col] + 1;
                if (ndist <= dist->data()[neighbour.row][neighbour.col])
                    dist->data()[neighbour.row][neighbour.col] = ndist;
            }
        }
    }
    delete added;
    delete nodes;

    return dist;
}
int32_t process_task1(void)
{
    int32_t steps = 0;
    static int32_t MAX_VALUE = (int32_t)(data.size() * data[0].size());
    vector<vector<int32_t>> *dist = walk(start, MAX_VALUE);
    steps = dist->data()[stop.row][stop.col];
    delete dist;
    return steps;
}
int32_t process_task2(void)
{
    static int32_t MAX_VALUE = (int32_t)(data.size() * data[0].size());
    list<int32_t> steps;
    for (int32_t x = 0; x < (int32_t)data.size(); x++)
    {
        for (int32_t y = 0; y < (int32_t)data[0].size(); y++)
        {
            if (data[x][y] == 'a')
            {
                auto dist = walk({x, y}, MAX_VALUE);
                steps.push_back(dist->data()[stop.row][stop.col]);
                delete dist;
            }
        }
    }
    steps.sort();
    return steps.front();
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

    // delete data;
    /** add delete to all data here */
    return 0;
}