#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <map>
#include <list>
using namespace std;
#define BUFSIZE 1024
/** used in processing */
enum dir_t
{
    UP,
    RIGHT,
    DOWN,
    LEFT,
};
enum rotate_t
{
    CW,    // clockwise
    CCW,   // counter clockwise
    UNDEF, // undefined rotation
};
typedef struct move_t
{
    int32_t step;
    rotate_t rot;
} move_t;
static map<rotate_t, map<dir_t, dir_t>> rotation_map = {
    {
        rotate_t::CW,
        {
            {dir_t::UP, dir_t::RIGHT},
            {dir_t::RIGHT, dir_t::DOWN},
            {dir_t::DOWN, dir_t::LEFT},
            {dir_t::LEFT, dir_t::UP},
        },
    },
    {
        rotate_t::CCW,
        {
            {dir_t::UP, dir_t::LEFT},
            {dir_t::LEFT, dir_t::DOWN},
            {dir_t::DOWN, dir_t::RIGHT},
            {dir_t::RIGHT, dir_t::UP},
        },
    },
};
struct state_t
{
public:
    typedef struct pos_t
    {
        int32_t row;
        int32_t col;
    } pos_t;

private:
    pos_t pos;
    pos_t bounds;
    dir_t dir;
    state_t &move(int32_t n)
    {
        if (n > 0)
        {
            switch (dir)
            {
            case dir_t::LEFT:
                pos.col -= n;
                break;
            case dir_t::RIGHT:
                pos.col += n;
                break;
            case dir_t::UP:
                pos.row -= n;
                break;
            case dir_t::DOWN:
                pos.row += n;
                break;
            default:
                break;
            }
            return *this;
        }
        if (n < 0)
        {
            n = -1 * n;
            switch (dir)
            {
            case dir_t::LEFT:
                pos.col += n;
                break;
            case dir_t::RIGHT:
                pos.col -= n;
                break;
            case dir_t::UP:
                pos.row += n;
                break;
            case dir_t::DOWN:
                pos.row -= n;
                break;
            default:
                break;
            }
            return *this;
        }
        return *this;
    }
    bool inbounds(void)
    {
        return ((0 <= pos.row && pos.row < bounds.row) &&
                (0 <= pos.col && pos.col < bounds.col));
    }
    void coerce_to_bounds(void)
    {
        pos.col = pos.col < 0 ? bounds.col - 1 : pos.col;
        pos.col = pos.col >= bounds.col ? 0 : pos.col;
        pos.row = pos.row < 0 ? bounds.row - 1 : pos.row;
        pos.row = pos.row >= bounds.row ? 0 : pos.row;
    }
    bool is_free(vector<vector<char>> *ind) { return (ind->at(pos.row)[pos.col] != '#'); }
    bool is_void(vector<vector<char>> *ind) { return (ind->at(pos.row)[pos.col] == ' '); }

public:
    void set_pos(pos_t pos) { this->pos = pos; }
    void set_dir(dir_t dir) { this->dir = dir; }
    void set_bounds(pos_t bounds) { this->bounds = bounds; }
    /**
     * @brief updates dir according to rot and current dir
     * by using rotation_map
     *
     * @param rot rotation to apply
     */
    void rotate(rotate_t rot) { this->dir = rotation_map[rot][dir]; }
    pos_t get_pos(void) { return pos; }
    dir_t get_dir(void) { return dir; }
    /**
     * @brief prefix increment this->pos in this->dir,
     *
     * @return state_t& ref to new state
     */
    state_t &operator++() { return move(1); }
    /**
     * @brief postfix increment of this->pos in this->dir
     *
     * @return state_t previous position
     */
    state_t operator++(int)
    {
        state_t old = *this;
        operator++();
        return old;
    }
    /**
     * @brief prefix decrement this->pos in this->dir.
     *
     * @return state_t& ref to new state
     */
    state_t &operator--() { return move(-1); }
    /**
     * @brief postfix decrement of this->pos in this->dir
     *
     * @return state_t previous position
     */
    state_t operator--(int)
    {
        state_t old = *this;
        operator--();
        return old;
    }
    /**
     * @brief fast forwards through void segment of input device ind
     *
     * @param ind input device data
     */
    void fastfwd(vector<vector<char>> *ind)
    {
        if (!inbounds())
            coerce_to_bounds();
        while (is_void(ind))
        {
            (*this)++;
            if (!inbounds())
                coerce_to_bounds();
        }
    }
    /**
     * @brief Checks if new state is ok. if not ok state is reset to
     * previous good position, i.e. state before latest state++ call
     *
     * @param ind input_device
     * @return true new state is ok, else false.
     */
    bool is_ok(vector<vector<char>> *ind)
    {
        pos_t orig = get_pos();
        if (!inbounds())
            coerce_to_bounds();
        if (is_void(ind))
            fastfwd(ind); // move through the void
        if (!is_free(ind))
        {
            // reset position if wall encountered
            set_pos(orig);
            (*this)--;
            return false;
        }
        return true;
    }
};
vector<move_t> *moves;
vector<vector<char>> *input_device;
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

    moves = new vector<move_t>();
    input_device = new vector<vector<char>>();
    vector<char> *chars = new vector<char>();

    char *indata = new char[BUFSIZE];
    bool movements = false;
    while (!ifs.eof())
    {
        int next = ifs.peek();
        if (next == '\n')
        {
            /** forward without storing */
            ifs.getline(indata, BUFSIZE);
            movements = true;
        }
        else
        {
            /** store for later processing */
            if (!movements)
            {
                ifs.getline(indata, BUFSIZE);
                chars->clear();
                chars->resize(ifs.gcount() - 1);
                for (int32_t i = 0; i < ifs.gcount() - 1; i++)
                {
                    switch (indata[i])
                    {
                    case ' ':
                    case '.':
                    case '#':
                        chars->at(i) = (indata[i]);
                        break;
                    default:
                        break;
                    }
                }
                input_device->push_back(*chars);
            }
            else
            {
                move_t m = {0, rotate_t::UNDEF};
                string str = "";
                bool forward = true;
                while (!ifs.eof())
                {
                    if (forward)
                    {
                        str.clear();
                        while (isdigit(ifs.peek()))
                            str.push_back((char)ifs.get());
                        m.step = stoi(str);
                        m.rot = rotate_t::UNDEF;
                    }
                    else
                    {
                        auto c = (char)ifs.get();
                        if (c == 'R')
                            m.rot = rotate_t::CW;
                        else if (c == 'L')
                            m.rot = rotate_t::CCW;
                        else
                            m.rot = rotate_t::UNDEF;
                        m.step = 0;
                    }
                    moves->push_back(m);
                    forward = !forward;
                }
            }
        }
    }

    ifs.close();
    delete[] indata;
    delete chars;
}

void write_output(const char *filename, int32_t task1, int32_t task2)
{
    ofstream ofs(filename, ofstream::out);
    ofs << "Task 1 result: " << task1 << "\n";
    ofs << "Task 2 result: " << task2 << "\n";
    ofs.close();
}
void print_inputdevice(void)
{
    for (auto row : *input_device)
    {
        for (auto el : row)
            cout << el;
        cout << endl;
    }
}
void process_task()
{
    /** ensure rows are equally long, pad with ' ' if necessary */
    size_t max_size = 0;
    for (auto row : *input_device)
        max_size = row.size() > max_size ? row.size() : max_size;

    for (auto row = input_device->begin(); row != input_device->end(); row++)
        while (row->size() < max_size)
            row->push_back(' ');
}
int32_t password(state_t state)
{
    int32_t passw = 0;
    passw += (state.get_pos().row + 1) * 1000;
    passw += (state.get_pos().col + 1) * 4;
    switch (state.get_dir())
    {
        // case dir_t::RIGHT: passw += 0; break;
    case dir_t::DOWN:
        passw += 1;
        break;
    case dir_t::LEFT:
        passw += 2;
        break;
    case dir_t::UP:
        passw += 3;
        break;
    default:
        break;
    }
    return passw;
}
int32_t process_task1(void)
{

    state_t state;
    // initialize positon at corner and direction right
    state.set_pos({0, 0});
    state.set_dir(dir_t::RIGHT);
    // set constrantes to movement
    state.set_bounds({(int32_t)input_device->size(), (int32_t)input_device->at(0).size()});
    // fast forward through void
    state.fastfwd(input_device);

    for (auto it : *moves)
    {
        if (it.rot == rotate_t::UNDEF)
        {
            int32_t ctr = it.step;
            while (ctr > 0)
            {
                state++;                        // advance state
                if (!state.is_ok(input_device)) // check if move is ok, state is reset if not ok.
                    break;
                ctr--;
            }
        }
        else
            state.rotate(it.rot); // apply rotation
    }
    return password(state);
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

    delete moves;
    delete input_device;
    /** add delete to all data here */
    return 0;
}