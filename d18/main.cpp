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
    int32_t x;
    int32_t y;
    int32_t z;
    coord_t operator+(coord_t &b) { return {this->x + b.x, this->y + b.y, this->z + b.z}; }
    // coord_t operator*(int32_t c) { return {c * this->x, c * this->y, c * this->z}; }
    coord_t operator-(coord_t &b) { return {this->x - b.x, this->y - b.y, this->z - b.z}; }
    friend ostream &operator<<(ostream &os, const coord_t &a)
    {
        os << a.x << ", " << a.y << ", " << a.z << endl;
        return os;
    }
} coord_t;
static coord_t normX = {1, 0, 0};
static coord_t normY = {0, 1, 0};
static coord_t normZ = {0, 0, 1};
struct GRID
{
    using xyz_t = array<array<array<bool, GRIDSIZE>, GRIDSIZE>, GRIDSIZE>;
    using yz_t = array<array<bool, GRIDSIZE>, GRIDSIZE>;

private:
    xyz_t *xyz;

public:
    GRID() { xyz = new xyz_t(); }
    ~GRID() { delete xyz; }
    void set(coord_t a) { (*xyz)[a.x][a.y][a.z] = true; }
    void reset(coord_t a) { (*xyz)[a.x][a.y][a.z] = false; }
    void flip(coord_t a) { (*xyz)[a.x][a.y][a.z] = !(*xyz)[a.x][a.y][a.z]; }
    bool operator[](coord_t a) { return (*xyz)[a.x][a.y][a.z]; }
    yz_t get_yz(const size_t x) { return (*xyz)[x]; }
    bool any_at_x(const size_t x)
    {
        for (auto col : this->get_yz(x))
            for (auto el : col)
                if (el)
                    return true;
        return false;
    }
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