#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <array>
#include <list>

using namespace std;
#define BUFSIZE 1024
#define GRIDSIZE 25
/** used in processing */
typedef struct coord_t
{
    int32_t x; // x-coordinate
    int32_t y; // y-coordinate
    int32_t z; // z-coordinate
    /**
     * @brief elementwise add b to this, no bounds checking
     *
     * @param b to be added to this
     * @return coord_t
     */
    coord_t operator+(coord_t &b) { return {this->x + b.x, this->y + b.y, this->z + b.z}; }
    /**
     * @brief elementwise subtraction of this and b
     *
     * @param b to be subtracted from this
     * @return coord_t
     */
    coord_t operator-(coord_t &b) { return {this->x - b.x, this->y - b.y, this->z - b.z}; }
    coord_t operator/(int32_t c) { return {this->x / c, this->y / c, this->z / c}; }
    coord_t operator*(int32_t c) { return {this->x * c, this->y * c, this->z * c}; }
    bool operator==(coord_t &b) { return (this->x == b.x && this->y == b.y && this->z == b.z); }
    bool operator!=(coord_t &b) { return !(this->operator==(b)); }
    /**
     * @brief print content of a as "a.x, a.y, a.z" to os. appends endl.
     *
     * @param os ostream to use for printing
     * @param a to be printed to os
     * @return ostream& os
     */
    friend ostream &
    operator<<(ostream &os, const coord_t &a)
    {
        os << a.x << ", " << a.y << ", " << a.z << endl;
        return os;
    }
} coord_t;
static coord_t normX = {1, 0, 0}; // normal to x-plane
static coord_t normY = {0, 1, 0}; // normal to y-plane
static coord_t normZ = {0, 0, 1}; // normal to z-plane
struct GRID
{
    using x_t = array<bool, GRIDSIZE>;
    using xy_t = array<array<bool, GRIDSIZE>, GRIDSIZE>;
    using xyz_t = array<array<array<bool, GRIDSIZE>, GRIDSIZE>, GRIDSIZE>;

private:
    xyz_t *xyz; // xyz grid to mark lava

public:
    GRID() { xyz = new xyz_t(); }
    ~GRID() { delete xyz; }
    /**
     * @brief set true in xyz grid at location a
     *
     * @param a location to set true
     */
    void set(coord_t a) { (*xyz)[a.z][a.y][a.x] = true; }
    /**
     * @brief reset (false) in xyz grid for location a
     *
     * @param a location to reset
     */
    void reset(coord_t a) { (*xyz)[a.z][a.y][a.x] = false; }
    /**
     * @brief flips state in location a (true -> false, false -> true)
     *
     * @param a location to flip
     */
    void flip(coord_t a) { (*xyz)[a.z][a.y][a.x] = !(*xyz)[a.z][a.y][a.x]; }
    /**
     * @brief retrieves data at location a in xyz grid
     *
     * @param a location to retrieve
     * @return true location is set, else not set.
     */
    bool operator[](coord_t a) { return (*xyz)[a.z][a.y][a.x]; }
    /**
     * @brief Get the yz grid at location x
     *
     * @param x location to get yz grid.
     * @return yz_t retrieved yz grid.
     */
    xy_t get_xy(const size_t z) { return (*xyz)[z]; }
    x_t get_x(const size_t y, const size_t z) { return (*xyz)[z][y]; }
    /**
     * @brief checks if any are yz grid at location x of xyz grid.
     *
     * @param x location to check in xyz grid
     * @return true at least 1 element is set, else no elements are set
     */
    bool any_at_z(const size_t z)
    {
        for (auto col : this->get_xy(z))
            for (auto el : col)
                if (el)
                    return true;
        return false;
    }
    bool any_at_yz(const size_t y, const size_t z)
    {
        for (auto el : this->get_x(y, z))
            if (el)
                return true;
        return false;
    }
    /**
     * @brief prints each non-empty yz grid of xyz grid. appends endl.
     *
     * @param os ostream to print to
     * @param g GRID to print
     * @return ostream& returns os
     */
    friend ostream &operator<<(ostream &os, GRID &g)
    {
        for (size_t z = 0; z < GRIDSIZE; z++)
        {
            if (g.any_at_z(z))
            {
                os << "At z = " << z << endl;
                for (auto col : g.get_xy(z))
                {
                    for (auto el : col)
                        os << (el ? "X" : ".");
                    os << endl;
                }
                os << endl;
            }
        }
        return os;
    }
};
vector<coord_t> *data;
GRID *grid;
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

    data = new vector<coord_t>();
    coord_t t = {0, 0, 0};

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
            if (3 == sscanf(indata, "%d,%d,%d", &t.x, &t.y, &t.z))
            {
                data->push_back(t);
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
void process_task()
{
    /** sets data into girds */
    grid = new GRID();

    for (auto it : *data)
    {
        grid->set(it);
    }
    cout << "Grid:\n";
    cout << *grid;
    cout << endl;
}
/**
 * @brief return 1 if grid is free in coordinate src
 *
 * @param src coordinate to check
 * @return int32_t 1 if free in src, else 0
 */
inline int32_t try_add_to_count(coord_t src)
{
    return !(*grid)[src] ? 1 : 0;
}
int32_t count_free_neighbours(void)
{
    int32_t free_count = 0;
    for (auto it : *data)
    {
        // checks if free cube side from all directions
        free_count += try_add_to_count(it + normX);
        free_count += try_add_to_count(it - normX);
        free_count += try_add_to_count(it + normY);
        free_count += try_add_to_count(it - normY);
        free_count += try_add_to_count(it + normZ);
        free_count += try_add_to_count(it - normZ);
    }
    return free_count;
}
int32_t process_task1(void)
{
    return count_free_neighbours();
}
void grow_outward(coord_t c)
{
    if (!(*grid)[c])
    {
        grid->set(c);
        grow_outward(c - normX);
        grow_outward(c + normX);
        grow_outward(c - normY);
        grow_outward(c + normY);
        grow_outward(c - normZ);
        grow_outward(c + normZ);
    }
}
bool look_outward(coord_t c, coord_t norm)
{
    if (norm == normX || norm * -1 == normX)
        if (c.x <= 1 || c.x >= GRIDSIZE - 1)
            return false;
    if (norm == normY || norm * -1 == normY)
        if (c.y <= 1 || c.y >= GRIDSIZE - 1)
            return false;
    if (norm == normZ || norm * -1 == normZ)
        if (c.z <= 1 || c.z >= GRIDSIZE - 1)
            return false;
    if (!(*grid)[c])
        return look_outward(c + norm, norm);
    return true;
}
inline bool isTrapped(coord_t c)
{
    if ((*grid)[c])
        return false;
    return look_outward(c, normX) && look_outward(c, normX * -1) &&
           look_outward(c, normY) && look_outward(c, normY * -1) &&
           look_outward(c, normZ) && look_outward(c, normZ * -1);
}
int32_t process_task2(void)
{
    /** Do something to process the task specific to 2 */
    coord_t center = {0, 0, 0};
    for (auto it : *data)
        center = center + it;
    center = center / (int32_t)data->size();
    cout << "center: " << center;
    // current problem is that pockets of size two or more
    // if (isTrapped(center))
    //     cout << "center is trapped: " << center;

    grow_outward(center);

    for (int32_t z = 1; z < GRIDSIZE - 1; z++)
    {
        for (int32_t y = 1; y < GRIDSIZE - 1; y++)
        {
            for (int32_t x = 1; x < GRIDSIZE - 1; x++)
            {
                if (isTrapped({x, y, z}))
                    grid->set({x, y, z});
            }
        }
        /* code */
    }

    cout << *grid;
    return count_free_neighbours();
}
int main()
{
    /** use this for testing examples */
    // read_input("test.txt");

    /** use this to run input */
    read_input("input.txt");
    // task 2: 2830, 2925 too high. 2160 too low. 2548, 2490 ??

    process_task();

    int32_t task1 = process_task1();
    int32_t task2 = process_task2();

    /** store output to file */
    write_output("output.txt", task1, task2);

    cout << "Task 1 result: " << task1 << "\n";
    cout << "Task 2 result: " << task2 << "\n";

    delete data;
    delete grid;
    /** add delete to all data here */
    return 0;
}