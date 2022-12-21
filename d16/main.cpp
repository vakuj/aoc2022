#include <iostream>
#include <fstream>
#include <algorithm>

#include <list>
#include <map>
#include <set>

#include <cstring>
#include <cassert>
using namespace std;
#define BUFSIZE 1024
/** used in processing */
/** valve data type */
#define id_t uint16_t

typedef struct valve_t
{
    id_t id;                // this valve's id number
    int32_t rate;           // flow rate of valve
    list<id_t> connections; // valve id numbers to connected valves
} valve_t;
map<id_t, valve_t> *data;
int32_t cnt;
size_t length;
/** add additional */

inline id_t chars_to_id(const char *cpair)
{
    return (id_t)((((id_t)0 | cpair[0]) << 8) | cpair[1]);
}
inline id_t chars_to_id(char *cpair)
{
    return (id_t)((((id_t)0 | cpair[0]) << 8) | cpair[1]);
}
void parse_valve_line(char *cstr, valve_t *valve)
{
    valve->id = 0;
    valve->rate = 0;
    valve->connections.clear();
    char *cpair1 = new char[2];
    char *cpair2 = new char[2];
    if (5 == sscanf(cstr, "Valve %c%c has flow rate=%d; tunnel leads to valve %c%c", cpair1, cpair1 + 1, &(valve->rate), cpair2, cpair2 + 1))
    {
        valve->id = chars_to_id(cpair1);
        // only one single connection
        valve->connections.push_back(chars_to_id(cpair2));
    }
    else
    {
        if (3 != sscanf(cstr, "Valve %c%c has flow rate=%d; tunnels lead to valves", cpair1, cpair1 + 1, &(valve->rate)))
            assert(0 && "Undefined behavior");

        const char *tokens = ", ";
        valve->id = chars_to_id(cpair1);
        char *ptr = strtok(cstr, tokens);
        while (ptr != NULL)
        {
            // move forward until "valves" is found (just before connection list starts)
            ptr = strtok(NULL, tokens);
            if (strcmp(ptr, "valves") == 0)
            {
                ptr = strtok(NULL, tokens); // skip one more (the "valves")
                break;
            }
        }
        while (ptr != NULL)
        {
            // grab every connection and store for later use.
            valve->connections.push_back(chars_to_id(ptr));
            ptr = strtok(NULL, tokens);
        }
    }
    delete[] cpair1;
    delete[] cpair2;
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

    data = new map<id_t, valve_t>();
    valve_t valve;
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
            parse_valve_line(indata, &valve);
            data->insert(pair<id_t, valve_t>(valve.id, valve));
            ctr++;
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
void print_id(id_t id)
{
    cout << (char)((0xff00 & id) >> 8) << (char)(0x00ff & id);
}
void process_task()
{
    /** Do something to process the task in general */
    for (auto it : *data)
    {
        print_id(it.second.id);
        cout << " with rate = " << it.second.rate << " -> [";
        for (auto cit : it.second.connections)
        {
            print_id(cit);
            cout << ", ";
        }
        cout << "]\n";
    }
}
int32_t pressure_increase(set<id_t> *open_valves)
{
    int32_t dp = 0;
    if (open_valves->empty())
        return dp;
    for (auto it : *open_valves)
        dp += data->at(it).rate;
    // cout << dp << "\n";
    return dp;
}
void print_id_set(set<id_t> ids, const char *comment)
{
    cout << comment << ": ";
    for (auto it : ids)
    {
        print_id(it);
        cout << ", ";
    }
    cout << "\n";
}
inline int32_t estimate_valve_score(id_t id, int32_t time_remaining) { return data->at(id).rate * time_remaining; }
int32_t find_time_id(id_t start_id, id_t search_id, int32_t time_remaining)
{
    if (time_remaining <= 0)
        return 0;
    if (start_id == search_id)
        return time_remaining - 1;
    int32_t max_time = 0;
    int32_t current_time = 0;
    for (auto it : data->at(start_id).connections)
    {
        current_time = find_time_id(it, search_id, time_remaining - 1);
        max_time = current_time < max_time ? max_time : current_time;
    }
    return max_time;
}
int32_t chain_score(id_t current, set<id_t> none_open, map<id_t, bool> visited, map<id_t, map<id_t, int32_t>> jump_count, int32_t ctr)
{
    if (ctr <= 0)
        return 0;
    if (none_open.empty())
        return estimate_valve_score(current, ctr);
    int32_t max_score = 0;
    int32_t current_score = 0;
    ;

    bool erased = false;
    // if (none_open.count(current) != 0)
    if (!visited[current])
    {
        visited[current] = true;
        // none_open.erase(current);
        erased = true;
    }
    for (auto it : none_open)
    {
        if (!visited[it])
        {
            auto jc = jump_count[current][it];
            current_score = estimate_valve_score(current, ctr - 1);
            current_score += chain_score(it, none_open, visited, jump_count, ctr - jc);
        }
        if (current_score > max_score)
            max_score = current_score;
    }
    if (erased)
        visited[current] = false;
    // none_open.insert(current);
    return max_score;
}
id_t open_next(id_t current, set<id_t> &none_open, map<id_t, bool> &visited, map<id_t, map<id_t, int32_t>> jump_count, int32_t ctr)
{
    if (none_open.empty())
        return current;
    if (ctr <= 0)
        return current;
    int32_t max_score = 0;
    int32_t current_score;
    id_t next_id = current;
    for (auto it : none_open)
    {
        auto jc = jump_count[current][it];
        current_score = chain_score(it, none_open, visited, jump_count, ctr - jc);
        if (current_score >= max_score)
        {
            max_score = current_score;
            next_id = it;
        }
        print_id(current);
        cout << " -> ";
        print_id(it);
        cout << " : " << current_score << "\n";
    }
    none_open.erase(next_id);
    visited[next_id] = true;
    // cout << " of [";
    // for (auto it : none_open)
    // {
    //     print_id(it);
    //     cout << ", ";
    // }
    // cout << "]\n";
    return next_id;
}
void path_next(valve_t *current, id_t next)
{
    int32_t min_steps = INT32_MAX;
    id_t child = current->id;
    for (auto it : current->connections)
    {
        auto steps = 15 - find_time_id(it, next, 15);
        if (steps < min_steps)
        {
            min_steps = steps;
            child = it;
        }
    }
    *current = data->at(child);
}
void construct_jump_list(map<id_t, map<id_t, int32_t>> &jump_count, set<id_t> valves_to_open, int32_t look_ahead = 10)
{
    cout << "Building Jump List\n";
    for (auto ait : *data)
        for (auto it : valves_to_open)
            jump_count[ait.second.id][it] = 0;
    // for (auto it : *data)
    // {
    //     if (valves_to_open.count(it.first) != 0)
    //         for (auto cit : it.second.connections)
    //             jump_count[cit][it.first] = 2;

    //     for (auto cit : it.second.connections)
    //         if (valves_to_open.count(cit) != 0)
    //             jump_count[it.first][cit] = 2;
    // }
    for (auto it : valves_to_open)
    {
        for (auto sit : *data)
        {
            if (it == sit.first)
                jump_count[sit.first][it] = 0;
            else
            {
                if (jump_count[sit.first][it] == 0)
                {
                    auto time = look_ahead - find_time_id(it, sit.first, look_ahead);
                    jump_count[sit.first][it] = time;
                }
            }
            // print_id(sit.first);
            // cout << " -> ";
            // print_id(it);
            // cout << " with " << jump_count[sit.first][it] << "\n";
        }
    }
    cout << "Jump list done\n";
}
int32_t process_task1(void)
{
    valve_t valve = data->at(chars_to_id("AA"));
    int32_t look_ahead = 10; // time steps to look into future.
    int32_t ctr = 30;        // max time
    int32_t accumulated_pressure = 0;

    set<id_t> open_valves;
    set<id_t> valves_to_open;

    map<id_t, bool> visited;
    map<id_t, map<id_t, int32_t>> jump_count;

    for (auto it : *data)
    {
        if (it.second.rate != 0)
        {
            visited[it.first] = false;
            valves_to_open.insert(it.first);
        }
    }
    construct_jump_list(jump_count, valves_to_open);

    id_t next_id = open_next(valve.id, valves_to_open, visited, jump_count, look_ahead);
    while (ctr > 0)
    {
        auto dp = pressure_increase(&open_valves);
        cout << ctr << ": " << dp << "\n";
        accumulated_pressure += dp;
        if (valve.id == next_id)
        {
            open_valves.insert(next_id);
            next_id = open_next(valve.id, valves_to_open, visited, jump_count, look_ahead);
        }
        else
            path_next(&valve, next_id);

        ctr--;
    }
    cout << "\n";
    print_id_set(open_valves, "Open valves");
    return accumulated_pressure;
}
int32_t process_task2(void)
{
    valve_t valve_self = data->at(chars_to_id("AA"));
    valve_t valve_elephant = data->at(chars_to_id("AA"));
    int32_t look_ahead = 10; // time steps to look into future.
    int32_t ctr = 26;        // max time
    int32_t accumulated_pressure = 0;

    set<id_t> open_valves;
    set<id_t> valves_to_open;

    map<id_t, bool> visited;
    map<id_t, map<id_t, int32_t>> jump_count;

    for (auto it : *data)
    {
        if (it.second.rate != 0)
        {
            visited[it.first] = false;
            valves_to_open.insert(it.first);
        }
    }
    construct_jump_list(jump_count, valves_to_open);

    id_t next_id_elephant = open_next(valve_elephant.id, valves_to_open, visited, jump_count, look_ahead);
    id_t next_id_self = open_next(valve_self.id, valves_to_open, visited, jump_count, look_ahead);
    while (ctr > 0)
    {
        auto dp = pressure_increase(&open_valves);
        cout << ctr << ": " << dp << "\n";
        accumulated_pressure += dp;
        if (valve_self.id == next_id_self)
        {
            cout << "You open: ";
            print_id(next_id_self);
            cout << "\n";
            open_valves.insert(next_id_self);
            next_id_self = open_next(valve_self.id, valves_to_open, visited, jump_count, look_ahead);
        }
        else
        {
            path_next(&valve_self, next_id_self);
            cout << "You path to: ";
            print_id(valve_self.id);
            cout << "\n";
        }
        if (valve_elephant.id == next_id_elephant)
        {
            cout << "Elephant opens: ";
            print_id(next_id_elephant);
            cout << "\n";
            open_valves.insert(next_id_elephant);
            next_id_elephant = open_next(valve_elephant.id, valves_to_open, visited, jump_count, look_ahead);
        }
        else
        {
            path_next(&valve_elephant, next_id_elephant);
            cout << "Elephant paths to: ";
            print_id(valve_elephant.id);
            cout << "\n";
        }
        ctr--;
    }
    cout << "\n";
    print_id_set(open_valves, "Open valves");
    return accumulated_pressure;
}
int main()
{
    /** use this for testing examples */
    read_input("test.txt");

    /** use this to run input */
    // read_input("input.txt");

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