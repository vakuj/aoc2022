#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <list>

#define BUFSIZE 1024
/** used in processing */
using namespace std;

enum dir_t
{
    NORTH,
    SOUTH,
    WEST,
    EAST,
};

typedef struct elf_t
{
    int32_t x;
    int32_t y;
} elf_t;
vector<vector<bool>> *data;
vector<elf_t> *elfs;
int32_t cnt;
size_t length;
/** add additional */

void read_input(const char *filename)
{
    std::ifstream ifs(filename, std::ifstream::in);

    if (!ifs.is_open())
    {
        exit(1);
    }
    ifs.seekg(0, std::ifstream::end);
    length = ifs.tellg();
    ifs.seekg(0, std::ifstream::beg);

    data = new vector<vector<bool>>();
    elfs = new vector<elf_t>();

    vector<bool> *row = new vector<bool>();

    char *indata = new char[BUFSIZE];
    int32_t ctr = 0;
    while (!ifs.eof())
    {
        int next = ifs.peek();
        if (next == '\n')
        {
            /** forward without storing */
            ifs.ignore(BUFSIZE);
        }
        else
        {
            ifs.getline(indata, BUFSIZE);
            /** store for later processing */
            row->resize(ifs.gcount());
            for (int32_t i = 0; i < ifs.gcount() - 1; i++)
            {
                if (indata[i] == '.')
                    row->at(i) = false;
                if (indata[i] == '#')
                {
                    row->at(i) = true;
                    elfs->push_back({i, ctr});
                }
            }
            data->push_back(*row);
            ctr++;
            row->clear();
        }
    }
    cnt = ctr;

    ifs.close();
    delete[] indata;
    delete row;
}

void write_output(const char *filename, int32_t task1, int32_t task2)
{
    std::ofstream ofs(filename, std::ofstream::out);
    ofs << "Task 1 result: " << task1 << "\n";
    ofs << "Task 2 result: " << task2 << "\n";
    ofs.close();
}
void process_task()
{
    /** Do something to process the task in general */
}
bool is_free(vector<elf_t> *current, elf_t elf, dir_t dir)
{
    for (auto ce : *current)
    {
        if (dir == dir_t::NORTH)
        {
            if (elf.y - 1 == ce.y)
                if (elf.x == ce.x || elf.x - 1 == ce.x || elf.x + 1 == ce.x)
                    return false;
        }
        if (dir == dir_t::SOUTH)
        {
            if (elf.y + 1 == ce.y)
                if (elf.x == ce.x || elf.x - 1 == ce.x || elf.x + 1 == ce.x)
                    return false;
        }
        if (dir == dir_t::WEST)
        {
            if (elf.x - 1 == ce.x)
                if (elf.y == ce.y || elf.y - 1 == ce.y || elf.y + 1 == ce.y)
                    return false;
        }
        if (dir == dir_t::EAST)
        {
            if (elf.x + 1 == ce.x)
                if (elf.y == ce.y || elf.y - 1 == ce.y || elf.y + 1 == ce.y)
                    return false;
        }
    }
    return true;
}

bool update_pos(vector<elf_t> *current, vector<elf_t> *new_pos, int32_t offset, list<dir_t> order)
{
    // should not move if each of the neighbours are free
    elf_t curr_pos = current->at(offset);
    bool cont = false;
    for (auto ce : *current)
    {
        if (ce.x == curr_pos.x - 1)
        {
            if (ce.y == curr_pos.y - 1 || ce.y == curr_pos.y || ce.y == curr_pos.y + 1)
            {
                cont = true;
                break;
            }
        }
        if (ce.x == curr_pos.x)
        {
            if (ce.y == curr_pos.y - 1 || ce.y == curr_pos.y + 1)
            {
                cont = true;
                break;
            }
        }
        if (ce.x == curr_pos.x + 1)
        {
            if (ce.y == curr_pos.y - 1 || ce.y == curr_pos.y || ce.y == curr_pos.y + 1)
            {
                cont = true;
                break;
            }
        }
    }
    if (!cont)
        goto early_return; // jump to end if no neighbours

    for (auto dir : order)
    {
        if (is_free(current, curr_pos, dir))
        {
            if (dir == dir_t::NORTH)
            {
                new_pos->at(offset) = {curr_pos.x, curr_pos.y - 1};
                break;
            }
            if (dir == dir_t::SOUTH)
            {
                new_pos->at(offset) = {curr_pos.x, curr_pos.y + 1};
                break;
            }
            if (dir == dir_t::WEST)
            {
                new_pos->at(offset) = {curr_pos.x - 1, curr_pos.y};
                break;
            }
            if (dir == dir_t::EAST)
            {
                new_pos->at(offset) = {curr_pos.x + 1, curr_pos.y};
                break;
            }
        }
    }
early_return:
    return cont;
}
elf_t minmax_range_elf(vector<elf_t> *e, elf_t *mine, elf_t *maxe)
{
    elf_t range = {0, 0};
    *mine = *maxe;
    *maxe = {0, 0};
    for (auto ce : *e)
    {
        maxe->x = ce.x > maxe->x ? ce.x : maxe->x;
        maxe->y = ce.y > maxe->y ? ce.y : maxe->y;
        mine->x = ce.x < mine->x ? ce.x : mine->x;
        mine->y = ce.y < mine->y ? ce.y : mine->y;
    }
    range.x = (maxe->x - mine->x) + 1;
    range.y = (maxe->y - mine->y) + 1;
    return range;
}
void update_data(vector<vector<bool>> *vb, vector<elf_t> *nelfs, elf_t *range, elf_t *mine)
{
    vb->clear();
    vb->resize(range->y);
    for (auto it = vb->begin(); it != vb->end(); it++)
        it->resize(range->x, false);
    for (auto ne : *nelfs)
        vb->at(ne.y - mine->y)[ne.x - mine->x] = true;
    return;
}
void print_data(vector<bool> vb)
{
    for_each(vb.begin(), vb.end(), [](bool e)
             { cout << (e ? '#' : '.'); });
    cout << endl;
}
void swap_forward_if_equal(vector<elf_t> *to, vector<elf_t> *from, int32_t offset)
{
    bool swapped = false;
    for (int32_t i = offset + 1; i < (int32_t)to->size(); i++)
    {
        if (to->at(offset).x == to->at(i).x && to->at(offset).y == to->at(i).y)
        {
            to->at(i) = from->at(i);
            swapped = true;
        }
    }
    if (swapped)
        to->at(offset) = from->at(offset);
}
int32_t process_task1(void)
{
    cout << "original data:\n";
    for_each(data->begin(), data->end(), print_data);
    cout << endl;
    vector<elf_t> *n_elfs = new vector<elf_t>(elfs->size());
    vector<elf_t> *p_elfs = new vector<elf_t>(elfs->size());
    elf_t min_corner = {0, 0};
    elf_t max_corner = {0, 0};
    elf_t map_range = {0, 0};

    list<dir_t> order;
    order.push_back(dir_t::NORTH);
    order.push_back(dir_t::SOUTH);
    order.push_back(dir_t::WEST);
    order.push_back(dir_t::EAST);

    p_elfs->assign(elfs->begin(), elfs->end());
    int32_t ctr = 0;
    int32_t ictr = 0;
    // while (ctr < (int32_t)elfs->size() && ictr < 10)
    while (ictr < 10)
    {
        ictr++;
        ctr = 0;
        for (int32_t i = 0; i < (int32_t)p_elfs->size(); i++)
            ctr += update_pos(p_elfs, n_elfs, i, order) ? 0 : 1;

        order.push_back(order.front());
        order.pop_front();

        for (int32_t i = 0; i < (int32_t)p_elfs->size(); i++)
            swap_forward_if_equal(n_elfs, p_elfs, i);
        map_range = minmax_range_elf(n_elfs, &min_corner, &max_corner);
        p_elfs->assign(n_elfs->begin(), n_elfs->end());
    }
    update_data(data, n_elfs, &map_range, &min_corner);

    cout << "10th iteration of data:\n";
    for_each(data->begin(), data->end(), print_data);
    cout << endl;

    delete n_elfs;
    delete p_elfs;
    return map_range.x * map_range.y - (int32_t)elfs->size();
}
int32_t process_task2(void)
{
    vector<elf_t> *n_elfs = new vector<elf_t>(elfs->size());
    vector<elf_t> *p_elfs = new vector<elf_t>(elfs->size());
    elf_t min_corner = {0, 0};
    elf_t max_corner = {0, 0};
    elf_t map_range = {0, 0};

    list<dir_t> order;
    order.push_back(dir_t::NORTH);
    order.push_back(dir_t::SOUTH);
    order.push_back(dir_t::WEST);
    order.push_back(dir_t::EAST);

    p_elfs->assign(elfs->begin(), elfs->end());
    int32_t ctr = 0;
    int32_t ictr = 0;
    while (ctr < (int32_t)elfs->size() && ictr < 1000)
    {
        ictr++;
        ctr = 0;
        for (int32_t i = 0; i < (int32_t)p_elfs->size(); i++)
            ctr += update_pos(p_elfs, n_elfs, i, order) ? 0 : 1;

        order.push_back(order.front());
        order.pop_front();

        for (int32_t i = 0; i < (int32_t)p_elfs->size(); i++)
            swap_forward_if_equal(n_elfs, p_elfs, i);
        map_range = minmax_range_elf(n_elfs, &min_corner, &max_corner);
        p_elfs->assign(n_elfs->begin(), n_elfs->end());
    }
    update_data(data, n_elfs, &map_range, &min_corner);

    cout << ictr << "th iteration of data:\n";
    for_each(data->begin(), data->end(), print_data);
    cout << endl;

    delete n_elfs;
    delete p_elfs;
    return ictr;
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

    std::cout << "Task 1 result: " << task1 << "\n";
    std::cout << "Task 2 result: " << task2 << "\n";

    delete data;
    delete elfs;
    /** add delete to all data here */
    return 0;
}