#include <iostream>
#include <fstream>
#include <algorithm>

#include <list>
#include <map>
#include <stack>

#include <string>
#include <cstring>
#define BUFSIZE 1024

using namespace std;
/** used in processing */

/**
 * @brief directory item
 *
 * @param level depth of item location
 * @param size size of item
 * @param isfile true if file
 */
typedef struct item_t
{
    size_t level;
    size_t size;
    bool isfile;
} item_t;
/**
 * @brief map of directory
 *
 * @param string full directory string used as as key
 * @param item_t description of directory item.
 */
map<string, item_t> items;
int32_t cnt;
size_t length;
/** add additional */

/**
 * @brief Take a list of strings and append to string.
 *        Each element is appended with a /.
 *        string self is appended to end.
 * @param trace list of string read front-to-back
 * @param self to be appended at end.
 * @return string
 */
string trace_string(list<string> *trace, string self)
{
    auto it = trace->begin();
    string tstr = "";
    while (it != trace->end())
    {
        tstr.append(*it);
        tstr.append("/");
        it++;
    }
    tstr.append(self);
    return tstr;
}
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

    char *indata = new char[BUFSIZE];
    char *parent = new char[BUFSIZE];
    char *self = new char[BUFSIZE];

    list<string> trace;
    size_t level = 0;
    size_t self_size = 0;
    int32_t ctr = 0;
    while (!ifs.eof())
    {
        ifs.getline(indata, BUFSIZE);

        if (indata[0] == '$')
        {
            if (strncmp(indata, "$ cd", 4) == 0)
            {
                if (strcmp(indata, "$ cd ..") == 0)
                {
                    level--;
                    trace.pop_back();
                }
                else
                {
                    if (1 == sscanf(indata, "$ cd %s", parent))
                    {
                        items.insert(pair<string, item_t>(trace_string(&trace, parent), {level, 0, false}));
                        trace.push_back(parent);
                        level++;
                    }
                }
            }
        }
        else if (strncmp(indata, "dir", 3) == 0)
        {
            if (1 == sscanf(indata, "dir %s", self))
            {
                items.insert(pair<string, item_t>(trace_string(&trace, self), {level, 0, false}));
                ctr++;
            }
        }
        else
        {
            if (2 == sscanf(indata, "%zu %s", &self_size, self))

            {
                items.insert(pair<string, item_t>(trace_string(&trace, self), {level, self_size, true}));
                ctr++;
            }
        }
    }
    cnt = ctr;
    ifs.close();
    delete[] indata;
    delete[] parent;
    delete[] self;
}

void write_output(const char *filename, size_t task1, size_t task2)
{
    ofstream ofs(filename, ofstream::out);
    ofs << "Task 1 result: " << task1 << "\n";
    ofs << "Task 2 result: " << task2 << "\n";
    ofs.close();
}

void print_items(void)
{
    for (auto it = items.begin(); it != items.end(); it++)
    {
        cout << it->first << " ";
        cout << (it->second.isfile ? "(file, " : "(dir, ");
        cout << "size = " << it->second.size;
        cout << ", level = " << it->second.level << ")\n";
    }
    cout << "----\n";
    cout << items.size() << " of " << cnt << "\n";
    cout << "----\n";
}
/** Update the sizes of the dirs which where intialized 0 */
void process_task(void)
{
    stack<pair<string, item_t>> dirs;
    // Put all dirs to a stack to process later
    for (auto it = items.begin(); it != items.end(); it++)
    {
        if (!it->second.isfile)
            dirs.push(pair<string, item_t>(it->first, it->second));
    }

    pair<string, item_t> dtop;
    /** Go through the stack of dirs and update
     *  the sizes by the content of their subdirs
     *  and files.
     */
    while (!dirs.empty())
    {
        dtop = dirs.top();
        for (auto it = items.begin(); it != items.end(); it++)
        {
            if (it->second.isfile)
            {
                if (it->first.find(dtop.first) != string::npos)
                    items[dtop.first].size += it->second.size;
            }
        }
        dirs.pop();
    }
}

size_t process_task1(void)
{
    size_t total_size = 0;
    for (auto it = items.begin(); it != items.end(); it++)
    {
        if (!it->second.isfile)
            total_size += it->second.size <= 100000 ? it->second.size : 0;
    }
    return total_size;
}
size_t process_task2(void)
{
    size_t smallest = 0;
    size_t to_free = items["/"].size - 70000000 + 30000000;
    size_t *sizes = new size_t[cnt];
    size_t ctr = 0;
    for (auto it = items.begin(); it != items.end(); it++)
    {
        if (!it->second.isfile)
        {
            if (it->second.size >= to_free)
            {
                sizes[ctr++] = it->second.size;
            }
        }
    }

    sort(sizes, sizes + ctr);
    smallest = sizes[0];
    delete[] sizes;
    return smallest;
}
int main()
{
    /** use this for testing examples */
    // read_input("test.txt");

    /** use this to run input */
    read_input("input.txt");

    process_task();
    // print_items();

    size_t task1 = process_task1();
    size_t task2 = process_task2();

    /** store output to file */
    write_output("output.txt", task1, task2);

    cout << "Task 1 result: " << task1 << "\n";
    cout << "Task 2 result: " << task2 << "\n";

    // delete[] data;
    /** add delete to all data here */
    return 0;
}