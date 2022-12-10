#include <iostream>
#include <fstream>
#include <algorithm>
#include <map>
#include <cassert>
using namespace std;

#define BUFSIZE 1024

/** used in processing */
int32_t *data;
size_t rows = 0;
size_t cols = 0;
size_t length = 0;
size_t cnt = 0;
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

    data = new int32_t[length];

    char *indata = new char[BUFSIZE];
    int32_t ctr = 0;
    while (!ifs.eof())
    {
        int next = ifs.peek();
        if (next == '\n')
        {
            ifs.getline(indata, BUFSIZE);
        }
        else
        {
            ifs.getline(indata, BUFSIZE);
            if (cols == 0)
                cols = ifs.gcount() - 1;
            for (int32_t i = 0; i < ifs.gcount(); i++)
            {
                if (indata[i] >= '0' && indata[i] <= '9')
                    data[ctr++] = indata[i] - '0';
            }
            rows++;
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

inline bool is_edge(size_t row, size_t col)
{
    return ((row == 0 ||
             col == 0) ||
            (row == rows - 1 ||
             col == cols - 1));
}
inline size_t cindex(size_t row, size_t col)
{
    return row * cols + col;
}
bool walk(size_t row, size_t col, bool follow_row, size_t step, int32_t *dist)
{
    assert(step != 0);
    int32_t start = data[cindex(row, col)];
    size_t ncol = col;
    size_t nrow = row;
    int32_t ctr = 0;
    bool ret = true;

    while (!is_edge(nrow, ncol))
    {
        if (follow_row)
            ncol += step;
        else
            nrow += step;
        ctr++;
        if (start <= data[cindex(nrow, ncol)])
        {
            ret = false;
            break;
        }
    }
    if (dist != NULL)
        *dist = ctr;
    return ret;
}
inline bool is_visible(size_t row, size_t col)
{
    if (is_edge(row, col))
        return true;
    else
    {
        // from left
        if (walk(row, col, true, -1, NULL))
            return true;
        // from right
        if (walk(row, col, true, 1, NULL))
            return true;
        // from top
        if (walk(row, col, false, -1, NULL))
            return true;
        // from bottom
        if (walk(row, col, false, 1, NULL))
            return true;
    }
    return false;
}
int32_t process_task1(void)
{
    int32_t vcnt = 0;
    for (size_t row = 0; row < rows; row++)
    {
        for (size_t col = 0; col < cols; col++)
        {
            vcnt += is_visible(row, col) ? 1 : 0;
        }
    }
    return vcnt;
}
int32_t process_task2(void)
{
    int32_t score;
    int32_t dist = 0;
    int32_t max_score = 0;
    for (size_t row = 0; row < rows; row++)
    {
        for (size_t col = 0; col < cols; col++)
        {
            score = 1;
            walk(row, col, false, -1, &dist);
            score *= dist;
            walk(row, col, false, 1, &dist);
            score *= dist;
            walk(row, col, true, -1, &dist);
            score *= dist;
            walk(row, col, true, 1, &dist);
            score *= dist;
            max_score = max_score < score ? score : max_score;
        }
    }
    return max_score;
}
int main()
{
    /** use this for testing examples */
    // read_input("test.txt");

    /** use this to run input */
    read_input("input.txt");

    int32_t task1 = process_task1();
    int32_t task2 = process_task2();

    /** store output to file */
    write_output("output.txt", task1, task2);

    cout << "Task 1 result: " << task1 << "\n";
    cout << "Task 2 result: " << task2 << "\n";

    delete[] data;
    /** add delete to all data here */
    return 0;
}