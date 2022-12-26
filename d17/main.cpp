#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <map>
#include <list>

using namespace std;

#define BUFSIZE 1024

#define cave_t vector<vector<bool>>
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
static map<rock_t, char> rock_fill_map = {
    {HLINE, '-'},
    {CROSS, '+'},
    {LSHAPE, 'L'},
    {VLINE, 'I'},
    {SQUARE, '#'},
};
static map<rock_t, vector<coord_t>>
    shape_map = {
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
    /**
     * @brief Construct a new ROCK object
     * defaults to rock_t::HLINE and position x=2, y=4
     */
    ROCK()
    {
        spawn_rock(rock_t::HLINE, {2, 4});
    }
    /**
     * @brief Construct a new ROCK object with specified rock type and position
     *
     * @param nrock the rock type of the new rock
     * @param spos positon of the new rock
     */
    ROCK(rock_t nrock, coord_t spos)
    {
        spawn_rock(nrock, spos);
    }
    /**
     * @brief Destroy the ROCK object
     *
     */
    ~ROCK() {}
    /**
     * @brief Spawns a rock of type nrock at position spos, is called by ctors
     *
     * @param nrock the rock type of the spawned rock
     * @param spos the spawn position
     */
    void spawn_rock(rock_t nrock, coord_t spos)
    {
        frozen = false;
        rock = nrock;
        pos = spos;
    }
    /**
     * @brief returns the state of member variable frozen
     *
     * @return true rock is frozen cannot move anymore
     */
    bool isFrozen(void) { return frozen; }
    /**
     * @brief returns the current positon of rock
     *
     * @return coord_t x,y coordinates of the rock
     */
    coord_t getPos(void) { return pos; }
    /**
     * @brief returns the highest point of the rock object
     *
     * @return int32_t the highest point (y) of rock.
     */
    int32_t getHighest(void) { return pos.y + grid_size[rock].y - 1; }
    /**
     * @brief  checks if the new position npos is possible based on the cave layout
     * should only be checked if moving side-ways.
     *
     * @param npos new position to check
     * @param cave the cave layout
     * @return true if the new position is blocked
     */
    bool is_blocked_movement(coord_t npos, cave_t cave)
    {
        // blocked by left wall?
        if (npos.x < 0)
            return true;
        // blocked by right wall?
        if (npos.x + grid_size[rock].x > 7)
            return true;
        // blocked by other frozen rock?
        for (auto it : shape_map[rock])
            if (cave[it.x + npos.x][it.y + npos.y])
                return true;

        return false;
    }
    /**
     * @brief checks if the new position of the rock has collided with anything.
     * should only be checked if moving down.
     *
     * @param npos
     * @param cave
     * @return true
     * @return false
     */
    bool collision_rock(coord_t npos, cave_t cave)
    {
        for (auto it : shape_map[rock])
        {
            if (cave[it.x + npos.x][it.y + npos.y])
                return true;
        }
        return false;
    }
    /**
     * @brief Sets the frozen position of the rock in cave. Also sets frozen to true.
     *
     * @param cave the pointer to the cave where the rock should be frozen in place.
     */
    void setFrozenPos(cave_t *cave)
    {
        frozen = true;
        for (auto it : shape_map[rock])
            cave->at(it.x + pos.x)[it.y + pos.y] = true;
    }
    /**
     * @brief Moves the rock based on dir and updates cave accordingly. Does nothing if rock already frozen.
     *
     * @param dir the direction in which the rock should move.
     * @param cave the pointer to the cave where the rock moves.
     */
    void move_rock(dir_t dir, cave_t *cave)
    {
        if (!frozen)
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
            if (dir != dir_t::DOWN)
            {
                if (!is_blocked_movement(npos, *cave))
                    pos = npos;
            }
            else
            {
                if (!collision_rock(npos, *cave))
                    pos = npos;
                else
                {
                    frozen = true;
                    setFrozenPos(cave);
                }
            }
        }
    }
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
}
int32_t process_task1(void)
{
    /** init cave with a floor */
    size_t rcnt = 2023;
    cave_t *cave = new cave_t(8);
    for (int32_t x = 0; x < 7; x++)
    {
        cave->at(x).resize(rcnt * 4, false);
        cave->at(x)[0] = true;
    }
    coord_t nextSpawn = {2, 4};
    rock_t rock_shape = rock_t::HLINE;
    list<ROCK> rocks;
    rocks.push_back(ROCK(rock_shape, nextSpawn));
    ROCK current_rock = rocks.back();
    bool next_down = false;
    int32_t ctr = 0;
    int32_t highest_point = 0;
    while (rocks.size() < rcnt)
    {
        if (next_down)
        {
            current_rock.move_rock(dir_t::DOWN, cave);
            next_down = false;
        }
        else
        {
            current_rock.move_rock(data->at(ctr++), cave);
            // ctr++;
            next_down = true;
        }
        if (current_rock.isFrozen())
        {
            // update highest point, keep old if current rock fell below
            highest_point = current_rock.getHighest() < highest_point ? highest_point : current_rock.getHighest();
            // update spawn position
            nextSpawn.x = 2;
            nextSpawn.y = highest_point + 4;
            // update rock type
            rock_shape = (rock_t)((int32_t)rock_shape + 1);
            if (rock_shape == rock_t::NONE)
                rock_shape = rock_t::HLINE;
            // store rock
            rocks.push_back(ROCK(rock_shape, nextSpawn));
            // pick current rock
            current_rock = rocks.back();
            // next movement is down
            next_down = false;
        }
        ctr %= cnt;
    }
    // for (int32_t y = highest_point; y >= 0; y--)
    // {
    //     for (int32_t x = 0; x < 7; x++)
    //     {
    //         if (cave->at(x)[y])
    //             cout << "#";
    //         else
    //             cout << ".";
    //     }
    //     cout << "\n";
    // }

    delete cave;
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

    delete data;
    /** add delete to all data here */
    return 0;
}