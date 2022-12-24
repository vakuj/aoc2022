#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <map>
#include <list>

using namespace std;

#define BUFSIZE 1024

#define cave_t map<int32_t, map<int32_t, bool>>
typedef struct coord_t
{
    int32_t x;
    int32_t y;
} coord_t;
enum rock_t
{
    HLINE,  // - shaped rock
    CROSS,  // + shaped rock
    LSHAPE, // _| shaped rock
    VLINE,  // | shaped rock
    SQUARE, // # shaped rock
    NONE,
};
enum dir_t
{
    LEFT,
    RIGHT,
    DOWN,
};
static map<dir_t, char> dir_to_char = {
    {LEFT, '<'},
    {RIGHT, '>'},
    {DOWN, 'v'},
};
static map<rock_t, vector<coord_t>> shape_map = {
    {HLINE, {{0, 0}, {1, 0}, {2, 0}, {3, 0}}},
    {CROSS, {{1, 0}, {1, 1}, {1, 2}, {0, 1}, {2, 1}}},
    {LSHAPE, {{0, 0}, {1, 0}, {2, 0}, {2, 1}, {2, 2}}},
    {VLINE, {{0, 0}, {0, 1}, {0, 2}, {0, 3}}},
    {SQUARE, {{0, 0}, {0, 1}, {1, 1}, {1, 0}}},
};
static map<rock_t, coord_t> grid_size = {
    {HLINE, {4, 1}},
    {CROSS, {3, 3}},
    {LSHAPE, {3, 3}},
    {VLINE, {1, 4}},
    {SQUARE, {2, 2}},
};
struct ROCK
{
private:
    coord_t pos; // coordinate to lower left corner
    rock_t rock; // rock type
    bool frozen; // true if rock is frozen cave

public:
    ROCK()
    {
        spawn_rock(rock_t::CROSS, {0, 0});
    }
    ROCK(rock_t nrock, coord_t spos)
    {
        spawn_rock(nrock, spos);
    }
    ~ROCK() {}
    void spawn_rock(rock_t nrock, coord_t spos)
    {
        frozen = false;
        rock = nrock;
        pos = spos;
    }
    coord_t block_by_wall(coord_t npos)
    {
        coord_t grid = grid_size[rock];
        if (npos.x < 0)
            npos.x = 0;
        if (npos.x + grid.x > 7)
            npos.x--;
        return npos;
    }
    coord_t block_by_frozen(coord_t npos, dir_t dir, map<int32_t, int32_t> cfloor)
    {
        if (dir != dir_t::DOWN)
        {
            vector<coord_t> shape = shape_map[rock];
            for (auto it : shape)
            {
                if (npos.y + it.y <= cfloor[it.x + npos.x])
                {
                    switch (dir)
                    {
                    case dir_t::LEFT:
                        npos.x++;
                        break;
                    case dir_t::RIGHT:
                        npos.x--;
                        break;
                    default:
                        break;
                    }
                }
            }
        }
        return npos;
    }
    bool collision_rock(coord_t npos, cave_t cave)
    {
        vector<coord_t> shape = shape_map[rock];
        bool collision = false;
        for (auto it : shape)
        {
            if (cave.count(it.x + npos.x) != 0)
                if (cave[it.x + npos.x].count(it.y + npos.y) != 0)
                    collision = true;
            if (collision)
                break;
        }
        return collision;
    }
    bool collision_floor(coord_t npos, dir_t dir, map<int32_t, int32_t> cfloor)
    {
        if (dir == dir_t::DOWN)
        {
            vector<coord_t> shape = shape_map[rock];
            for (auto it : shape)
            {
                if (cfloor[npos.x + it.x] >= npos.y + it.y)
                    return true;
            }
        }
        return false;
    }
    void setFrozenPos(cave_t *cave)
    {
        if (frozen)
        {
            vector<coord_t> shape = shape_map[rock];
            for (auto it : shape)
            {
                if (cave->count(it.x + pos.x) != 0)
                    cave->at(it.x + pos.x)[it.y + pos.y] = true;
            }
        }
    }
    void update_floor(map<int32_t, int32_t> *cfloor)
    {
        if (frozen)
        {
            vector<coord_t> shape = shape_map[rock];
            for (auto it : shape)
                if (cfloor->at(it.x + pos.x) < it.y + pos.y)
                    cfloor->at(it.x + pos.x) = it.y + pos.y;
        }
    }
    void move_rock(dir_t dir, cave_t *cave, map<int32_t, int32_t> *cfloor)
    {
        coord_t npos = pos;
        switch (dir)
        {
        case dir_t::LEFT:
            npos.x--;
            break;
        case dir_t::RIGHT:
            npos.x++;
            break;
        case dir_t::DOWN:
            npos.y--;
        default:
            break;
        }
        npos = block_by_wall(npos);
        // if (!collision_rock(npos, *cave))
        npos = block_by_frozen(npos, dir, *cfloor);
        if (!collision_floor(npos, dir, *cfloor))
            pos = npos;
        else
        {
            frozen = true;
            // setFrozenPos(cave);
            update_floor(cfloor);
        }
    }
    bool isFrozen(void) { return frozen; }
    coord_t getPos(void) { return pos; }
    int32_t getHighest(void) { return pos.y + grid_size[rock].y - 1; }
};

/** used in processing */
vector<dir_t> *data;
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

    data = new vector<dir_t>();

    // char *indata = new char[BUFSIZE];
    char c;
    int32_t ctr = 0;
    while (ifs.get(c))
    {
        switch (c)
        {
        case '<':
            data->push_back(dir_t::LEFT);
            break;
        case '>':
            data->push_back(dir_t::RIGHT);
            break;
        default:
            ctr--;
            break;
        }
        ctr++;
    }
    cnt = ctr;
    ifs.close();
    // delete[] indata;
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
        cout << dir_to_char[it];
    cout << "\n";
}
int32_t process_task1(void)
{
    /** Do something to process the task specific to 1 */
    /** init cave with a floor */
    cave_t *cave = new cave_t();
    map<int32_t, int32_t> *cfloor = new map<int32_t, int32_t>();
    for (int32_t x = 0; x < 7; x++)
    {
        (*cave)[x][0] = true;
        (*cfloor)[x] = 0;
        // cave->at(x)[0] = true;
    }
    coord_t nextSpawn = {2, 5};
    rock_t rock_shape = rock_t::HLINE;
    list<ROCK> rocks;
    rocks.push_back(ROCK(rock_shape, nextSpawn));
    ROCK current_rock = rocks.back();
    bool next_down = true;
    int32_t ctr = 0;
    size_t rcnt = 2022;
    while (rocks.size() < rcnt)
    {
        cout << "current position: " << current_rock.getPos().x << ", " << current_rock.getPos().y;
        if (next_down)
        {
            current_rock.move_rock(dir_t::DOWN, cave, cfloor);
            next_down = false;
            cout << " moving DOWN\n";
        }
        else
        {
            cout << " moving " << (data->at(ctr) == dir_t::LEFT ? "LEFT" : "RIGHT") << "\n";
            current_rock.move_rock(data->at(ctr), cave, cfloor);
            ctr++;
            next_down = true;
        }
        // if (current_rock.isFrozen() && rocks.size() >= rcnt)
        //     break;
        if (current_rock.isFrozen())
        {
            next_down = true;
            nextSpawn.x = 2;
            nextSpawn.y = current_rock.getHighest() + 5;
            rock_shape = (rock_t)((int32_t)rock_shape + 1);
            if (rock_shape == rock_t::NONE)
                rock_shape = rock_t::HLINE;
            rocks.push_back(ROCK(rock_shape, nextSpawn));
            current_rock = rocks.back();
            cout << "rocks spawned: " << rocks.size() << ", new rock: ";
            cout << rock_shape << " at ";
            cout << current_rock.getPos().x << ", " << current_rock.getPos().y << "\n";
        }
        ctr %= cnt;
    }
    int32_t highest_point = 0;
    for (auto it : *cfloor)
        highest_point = it.second > highest_point ? it.second : highest_point;
    cout << current_rock.getPos().x << ", " << current_rock.getPos().y << "\n";
    for (int32_t y = highest_point; y >= 0; y--)
    {
        for (int32_t x = 0; x < 7; x++)
        {

            if (cfloor->at(x) == y)
                cout << "#";
            else if (cave->at(x).count(y) != 0)
                cout << "#";
            else
                cout << ".";
        }
        cout << "\n";
    }

    delete cave;
    delete cfloor;
    return highest_point;
}
int32_t process_task2(void)
{
    /** Do something to process the task specific to 2 */
    return 0;
}
int main()
{
    /** use this for testing examples */
    read_input("test.txt");

    /** use this to run input */
    // read_input("input.txt");

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