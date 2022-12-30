#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <array>

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
    /**
     * @brief print content of a as "a.x, a.y, a.z" to os. appends endl.
     *
     * @param os ostream to use for printing
     * @param a to be printed to os
     * @return ostream& os
     */
    friend ostream &operator<<(ostream &os, const coord_t &a)
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
    using xyz_t = array<array<array<bool, GRIDSIZE>, GRIDSIZE>, GRIDSIZE>;
    using yz_t = array<array<bool, GRIDSIZE>, GRIDSIZE>;

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
    void set(coord_t a) { (*xyz)[a.x][a.y][a.z] = true; }
    /**
     * @brief reset (false) in xyz grid for location a
     *
     * @param a location to reset
     */
    void reset(coord_t a) { (*xyz)[a.x][a.y][a.z] = false; }
    /**
     * @brief flips state in location a (true -> false, false -> true)
     *
     * @param a location to flip
     */
    void flip(coord_t a) { (*xyz)[a.x][a.y][a.z] = !(*xyz)[a.x][a.y][a.z]; }
    /**
     * @brief retrieves data at location a in xyz grid
     *
     * @param a location to retrieve
     * @return true location is set, else not set.
     */
    bool operator[](coord_t a) { return (*xyz)[a.x][a.y][a.z]; }
    /**
     * @brief Get the yz grid at location x
     *
     * @param x location to get yz grid.
     * @return yz_t retrieved yz grid.
     */
    yz_t get_yz(const size_t x) { return (*xyz)[x]; }
    /**
     * @brief checks if any are yz grid at location x of xyz grid.
     *
     * @param x location to check in xyz grid
     * @return true at least 1 element is set, else no elements are set
     */
    bool any_at_x(const size_t x)
    {
        for (auto col : this->get_yz(x))
            for (auto el : col)
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
        for (size_t x = 0; x < GRIDSIZE; x++)
        {
            if (g.any_at_x(x))
            {
                os << "At x = " << x << endl;
                for (auto col : g.get_yz(x))
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
    delete grid;
    /** add delete to all data here */
    return 0;
}