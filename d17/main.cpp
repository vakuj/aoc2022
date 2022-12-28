#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <array>
#include <map>
#include <list>
#include <bitset>
#include <cstring>
#include <cassert>

using namespace std;

#define BUFSIZE 1024
#define WIDTH 8
#define HEIGHT 8192
#define cave_t array<bitset<WIDTH>, HEIGHT>
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
        if (npos.x + grid_size[rock].x > WIDTH - 1)
            return true;
        // blocked by other frozen rock?
        for (auto it : shape_map[rock])
            if (cave[it.y + npos.y].test(it.x + npos.x))
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
            if (cave[it.y + npos.y].test(it.x + npos.x))
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
            cave->at(it.y + pos.y).set(it.x + pos.x);
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

void write_output(const char *filename, size_t task1, size_t task2)
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
/**
 * @brief Moves the rock in accordance to the game
 *
 * @param rock pointer to the rock to move
 * @param down true for moving down, pointed to value is flipped upon use, and set to false if rock is frozen.
 * @param move_ctr the index of the next movement direction from data. is incremented and mod-ed to the cnt of movements in data.
 * @param cave pointer to where the movement should occur.
 * @return true if the rock froze due to the current movement, else false.
 */
bool make_move(ROCK *rock, bool *down, int32_t *move_ctr, cave_t *cave)
{
    if (*down)
    {
        rock->move_rock(dir_t::DOWN, cave);
        *down = false;
    }
    else
    {
        rock->move_rock(data->at((*move_ctr)++), cave);
        *move_ctr %= cnt;
        *down = true;
    }
    if (rock->isFrozen())
    {
        *down = false;
        return true;
    }
    return false;
}
/**
 * @brief Spawns a new rock for the pointer rock, according to the next rock_shape. highest_point is evaluted and updated if needed.
 *
 * @param rock pointer to the rock to be spawned
 * @param rock_shape pointer to the rock_shape which is incremented and mod-ed to real rock shapes.
 * @param highest_point pointer to the highest_pointed, by the current rock before spawned if new high point is reached.
 */
void spawn_new_rock(ROCK *rock, rock_t *rock_shape, int32_t *highest_point)
{
    // update highest point
    *highest_point = rock->getHighest() < *highest_point ? *highest_point : rock->getHighest();
    // calculate next spawn position
    coord_t nextSpawn = {2, *highest_point + 4};
    // update rock shape
    *rock_shape = (rock_t)((int32_t)*rock_shape + 1);
    if (*rock_shape == rock_t::NONE)
        *rock_shape = rock_t::HLINE;
    // make new rock
    *rock = ROCK(*rock_shape, nextSpawn);
}
/**
 * @brief returns true if repeating pattern of pattern_size length and at offset is found
 *
 * @param cave pointer to where to check if pattern is found
 * @param offset from the start where to search for pattern
 * @param pattern_size the length of the pattern
 * @return true if pattern is detected
 */
inline bool hasPattern(cave_t *cave, int32_t offset, int32_t pattern_size)
{
    return (0 == memcmp(cave->data() + offset, cave->data() + offset + pattern_size, pattern_size));
}
/**
 * @brief searches for next appearance of element at cave[offset] starting at start+1
 *
 * @param cave where to search for next element
 * @param offset to the element to search for
 * @param start starting point (+1) where to begin the search (default 0)
 * @return int32_t the number of element iterated through before found. (-1 on failure)
 */
inline int32_t patternSize(cave_t *cave, int32_t offset, int32_t start = 0)
{
    for (int32_t i = start + 1; i < HEIGHT / 2; i++)
    {
        if (cave->at(offset) == cave->at(offset + i))
            return i;
    }
    return -1;
}
/**
 * @brief finds first occurance of repeating pattern, requires 3 repeats
 *
 * @param cave pointer to where to search for pattern
 * @param pattern_size pointer to a hint of the lower bound for size of pattern to search for, is updated if a more suitable length is found.
 * @return int32_t the offset to the first pattern start from beginning (-1 on failure)
 */
int32_t searchPattern(cave_t *cave, int32_t *pattern_size)
{
    int32_t offset = 0;
    if (*pattern_size <= 0)
        *pattern_size = 0;
    int32_t poff = 0; // pattern offset
    while ((offset + *pattern_size) < HEIGHT / 2)
    {
        poff = *pattern_size;                   // initialize pattern size at hint
        poff = patternSize(cave, offset, poff); // find in
        while (poff >= 0)
        {
            if (poff > 1) // ignore <= 1 search patterns
            {
                // check that (this and next) and (next and next+) are the same pattern
                if (hasPattern(cave, offset, poff) && hasPattern(cave, offset + poff, poff))
                {
                    *pattern_size = poff;
                    return offset;
                }
            }
            poff = patternSize(cave, offset, poff); // find the next pattern offset
        }
        offset++;
    }
    return -1;
}
/**
 * @brief Prints the content of the cave for every row which contains any rock elements
 *
 * @param cave to print
 */
void print_cave(cave_t cave)
{
    for (size_t y = HEIGHT - 1; y > 0; y--)
    {
        if (cave[y - 1].any())
        {
            for (int32_t x = 0; x < 7; x++)
            {
                if (cave[y - 1].test(x))
                    cout << "#";
                else
                    cout << ".";
            }
            cout << " : " << y - 1 << "\n";
        }
    }
}
size_t process_task1(void)
{
    size_t rcnt = 2022;
    /** init cave with a floor */
    cave_t *cave = new cave_t();
    cave->at(0).set();
    coord_t nextSpawn = {2, 4};
    rock_t rock_shape = rock_t::HLINE;
    int32_t highest_point = 0;
    int32_t ctr = 0;
    bool next_down = false;
    ROCK current_rock = ROCK(rock_shape, nextSpawn);
    list<ROCK> rocks;
    while (rocks.size() < rcnt)
    {
        if (make_move(&current_rock, &next_down, &ctr, cave))
        {
            rocks.push_back(current_rock);
            spawn_new_rock(&current_rock, &rock_shape, &highest_point);
        }
    }
    /** uncomment to print cave */
    // print_cave(*cave);
    delete cave;
    return highest_point;
}
/**
 * @brief Idea: First simulate many rocks falling. From the many rocks cave, extract repeating pattern including also the preamble
 * before the first occurance of the pattern. Then calculate the how many patterns are needed for the big rock tower. From that extract
 * the height and also add the height for the preamble and any partial pattern heights.
 *
 * @param psize hint to the lower bound of expected pattern length. 40 works for test case, and 2000 works for the real case.
 * @return size_t the rock tower height
 */
size_t process_task2(int32_t psize)
{
#define ROCKMANY 10000
#define ROCKMAX 1000000000000

    size_t rock_count = ROCKMANY; // start by simulating many rocks

    /** init cave with a floor */
    cave_t *cave = new cave_t();
    cave->at(0).set();

    coord_t nextSpawn = {2, 4};
    rock_t rock_shape = rock_t::HLINE;

    ROCK current_rock = ROCK(rock_shape, nextSpawn);
    bool next_down = false;
    int32_t ctr = 0;
    int32_t highest_point = 0;
    int32_t poffset = 0;
    while (rock_count > 0)
    {
        if (make_move(&current_rock, &next_down, &ctr, cave))
        {
            spawn_new_rock(&current_rock, &rock_shape, &highest_point);
            rock_count--;
            if (highest_point > HEIGHT - 100)
            {
                // with many rocks added try find a pattern
                poffset = searchPattern(cave, &psize);
                break;
            }
        }
    }
    assert(poffset != -1);                                    // -1 indicates failure to find pattern
    cave_t *preamble = new cave_t();                          // for the preamble
    cave_t *pattern = new cave_t();                           // for the pattern
    copy_n(cave->begin(), poffset, preamble->begin());        // copy of the cave layout for later comparison with the preamble
    copy_n(cave->begin() + poffset, psize, pattern->begin()); // copy of the cave layout for later comparison with the pattern

    /** uncomment for print pattern and preamble */
    // cout << "preamble\n";
    // print_cave(*preamble);
    // cout << "repeating pattern\n";
    // print_cave(*pattern);

    // clear the cave to restart the rock falling
    for (auto it = cave->begin(); it != cave->end(); it++)
        it->reset();
    cave->at(0).set();

    // re-initialize all to starting point
    ctr = 0;
    rock_shape = rock_t::HLINE;
    nextSpawn = {2, 4};
    next_down = false;
    current_rock = ROCK(rock_shape, nextSpawn);
    highest_point = 0;

    list<ROCK> first_part; // contains the rocks for preamble
    list<ROCK> repeat;     // contains the rocks for the repeating pattern

    while (true)
    {
        if (make_move(&current_rock, &next_down, &ctr, cave))
        {
            first_part.push_back(current_rock); // store preamble rocks for later use
            spawn_new_rock(&current_rock, &rock_shape, &highest_point);
            if (equal(preamble->begin(), preamble->begin() + poffset, cave->begin())) // check if preamble is complete
                break;
        }
    }
    while (true)
    {
        if (make_move(&current_rock, &next_down, &ctr, cave))
        {
            repeat.push_back(current_rock); // store repeating rocks for later use
            spawn_new_rock(&current_rock, &rock_shape, &highest_point);
            if (equal(pattern->begin(), pattern->begin() + psize, cave->begin() + poffset)) // check if pattern is complete
                break;
        }
    }

    size_t rock_tower_height = 0;                                 // init rock tower height at 0
    size_t rocks = ROCKMAX - first_part.size();                   // remove rocks from preamble
    rock_tower_height += (size_t)psize * (rocks / repeat.size()); // multiply pattern size with amount of repetitions and add to height
    rocks %= repeat.size();                                       // take the remainder of the height
    if (rocks == 0)
    {
        // add the pattern offset to the height if no remainder
        rock_tower_height += (size_t)poffset - 1;
    }
    else
    {
        // add the remaining rocks, height includes the pattern offset
        highest_point = 0;
        auto it = repeat.begin();
        while (rocks > 0)
        {
            highest_point = it->getHighest() < highest_point ? highest_point : it->getHighest();
            it++;
            rocks--;
        }
        rock_tower_height += (size_t)highest_point;
    }
    delete cave;
    delete pattern;
    delete preamble;
    return rock_tower_height;
}
int main()
{
    /** use this for testing examples */
    // read_input("test.txt");

    /** use this to run input */
    read_input("input.txt");

    process_task();

    size_t task1 = process_task1();

    /** use this for the testing data set */
    // size_t task2 = process_task2(40);

    /** use this for the real data set */
    size_t task2 = process_task2(2000);

    /** store output to file */
    write_output("output.txt", task1, task2);

    cout << "Task 1 result: " << task1 << "\n";
    cout << "Task 2 result: " << task2 << "\n";

    delete data;
    /** add delete to all data here */
    return 0;
}