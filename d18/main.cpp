#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <array>
#include <map>

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
     * @brief elementwise division by constant
     *
     * @param c constant to divide with
     * @return coord_t result
     */
    coord_t operator/(int32_t c) { return {this->x / c, this->y / c, this->z / c}; }
    /**
     * @brief elementwise multiplication by constant
     *
     * @param c constant
     * @return coord_t result
     */
    coord_t operator*(int32_t c) { return {this->x * c, this->y * c, this->z * c}; }
    /**
     * @brief elementwise comparison.
     *
     * @param b to be compared with
     * @return true if all are equal, else false
     */
    bool operator==(coord_t &b) { return (this->x == b.x && this->y == b.y && this->z == b.z); }
    /**
     * @brief elementwise inverse comparison
     *
     * @param b to be compared with
     * @return true if non are equal, else false
     */
    bool operator!=(coord_t &b) { return (this->x != b.x && this->y != b.y && this->z != b.z); }
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
public:
    enum id_t
    {
        UNDEF,  // undefined
        LAVA,   // lava element
        CLOSED, // air enclosed in lava
        OPEN,   // exposed to open air
    };
    using x_t = array<id_t, GRIDSIZE>;
    using xy_t = array<array<id_t, GRIDSIZE>, GRIDSIZE>;
    using xyz_t = array<array<array<id_t, GRIDSIZE>, GRIDSIZE>, GRIDSIZE>;

private:
    xyz_t *xyz; // xyz grid to mark lava
    /**
     * @brief char equivalent of id.
     * UNDEF = ., LAVA = X, OPEN =  , CLOSED = #. @ is returned if outside of bound.
     *
     * @param id to convert to char
     * @return char equivalent of id
     */
    static char id_to_char(id_t id)
    {
        if (id_t::UNDEF == id)
            return '.';
        if (id_t::LAVA == id)
            return 'X';
        if (id_t::OPEN == id)
            return ' ';
        if (id_t::CLOSED == id)
            return '#';
        return '@';
    }

public:
    GRID() { xyz = new xyz_t(); }
    ~GRID() { delete xyz; }
    /**
     * @brief set LAVA in xyz grid at location a
     *
     * @param a location to set true
     */
    void set(coord_t a) { (*xyz)[a.z][a.y][a.x] = id_t::LAVA; }
    /**
     * @brief set coordinate a in xyz grid to id
     *
     * @param a coordinate to set
     * @param id it to set it to
     */
    void set(coord_t a, id_t id) { (*xyz)[a.z][a.y][a.x] = id; }
    /**
     * @brief reset (UNDEF) in xyz grid for location a
     *
     * @param a location to reset
     */
    void reset(coord_t a) { (*xyz)[a.z][a.y][a.x] = id_t::UNDEF; }
    /**
     * @brief retrieves data at location a in xyz grid
     *
     * @param a location to retrieve
     * @return true location is set, else not set.
     */
    id_t operator[](coord_t a) { return (*xyz)[a.z][a.y][a.x]; }
    /**
     * @brief Get the xy grid at location z
     *
     * @param z location to get xy grid.
     * @return xy_t retrieved xy grid.
     */
    xy_t get_xy(const size_t z) { return (*xyz)[z]; }
    /**
     * @brief Get the x grid at location y,z.
     *
     * @param y coordinate
     * @param z coordinate
     * @return x_t grid data a y,z
     */
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
                if (el == id_t::LAVA)
                    return true;
        return false;
    }
    /**
     * @brief returns true if c is inside of bounds of xyz data
     *
     * @param c coordinate to check
     * @return true coordinate is inside bounds, else false
     */
    bool isInside(coord_t c)
    {
        if (c.x < 0 || c.x > GRIDSIZE - 1)
            return false;
        if (c.y < 0 || c.y > GRIDSIZE - 1)
            return false;
        if (c.z < 0 || c.z > GRIDSIZE - 1)
            return false;
        return true;
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
                        os << id_to_char(el);
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
    if ((*grid)[src] == GRID::id_t::UNDEF || (*grid)[src] == GRID::id_t::OPEN)
        return 1;
    return 0;
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
/**
 * @brief Set all bounding neighbours of c to id if c is currently undefined.
 * Recursively calls along normal, splitting of to its two orthogonal directions,
 * anti-parallels included. Does not go along negative of norm.
 *
 * @param c coordinate to set
 * @param norm normal to follow
 * @param id the value to set
 */
void set_to_id(coord_t c, coord_t norm, GRID::id_t id)
{
    if (!grid->isInside(c)) // coordinate c outside of bounds -> return directly
        return;
    if ((*grid)[c] == GRID::id_t::UNDEF) // element is undefined
    {
        grid->set(c, id); // set current to id
        if (norm == normX || norm * -1 == normX)
        {
            // go along x-direction, and split to y and z
            set_to_id(c + norm, norm, id);

            set_to_id(c + normY, normY, id);
            set_to_id(c - normY, normY * -1, id);

            set_to_id(c + normZ, normZ, id);
            set_to_id(c - normZ, normZ * -1, id);
        }
        if (norm == normY || norm * -1 == normY)
        {
            // go along y-direction, and split to x and z
            set_to_id(c + normX, normX, id);
            set_to_id(c - normX, normX * -1, id);

            set_to_id(c + norm, norm, id);

            set_to_id(c + normZ, normZ, id);
            set_to_id(c - normZ, normZ * -1, id);
        }
        if (norm == normZ || norm * -1 == normZ)
        {
            // go along z-direction, and split to x and y
            set_to_id(c + normX, normX, id);
            set_to_id(c - normX, normX * -1, id);

            set_to_id(c + normY, normY, id);
            set_to_id(c - normY, normY * -1, id);

            set_to_id(c + norm, norm, id);
        }
    }
    return;
}
int32_t process_task2(void)
{
    // set all outside elements to open starting from corner
    coord_t corner = {0, 0, 0};
    grid->set(corner, GRID::id_t::OPEN);
    set_to_id(corner + normX, normX, GRID::id_t::OPEN);

    // set any remaining undefined elements as closed
    for (int32_t x = 0; x < GRIDSIZE; x++)
        for (int32_t y = 0; y < GRIDSIZE; y++)
            for (int32_t z = 0; z < GRIDSIZE; z++)
                if ((*grid)[{x, y, z}] == GRID::id_t::UNDEF)
                    grid->set({x, y, z}, GRID::id_t::CLOSED);

    cout << *grid;
    return count_free_neighbours();
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