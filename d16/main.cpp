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
map<id_t, map<id_t, int32_t>> jump_table;
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
        cout << " with rate = ";
        cout.width(3);
        cout << it.second.rate << " -> [";
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
int32_t chain_score(id_t current, set<id_t> none_open, map<id_t, bool> visited, int32_t ctr)
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
            auto jc = jump_table[current][it];
            current_score = estimate_valve_score(current, ctr - 1);
            current_score += chain_score(it, none_open, visited, ctr - jc);
        }
        if (current_score > max_score)
            max_score = current_score;
    }
    if (erased)
        visited[current] = false;
    // none_open.insert(current);
    return max_score;
}
id_t open_next(id_t current, set<id_t> &none_open, map<id_t, bool> &visited, int32_t ctr)
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
        auto jc = jump_table[current][it];
        current_score = chain_score(it, none_open, visited, ctr - jc);
        if (current_score >= max_score)
        {
            max_score = current_score;
            next_id = it;
        }
        // print_id(current);
        // cout << " -> ";
        // print_id(it);
        // cout << " : " << current_score << "\n";
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
void construct_jump_table(set<id_t> valves_to_open, int32_t look_ahead = 10)
{
    cout << ">> Building Jump table\n";
    cout << "[";
    for (auto it : valves_to_open)
    {
        for (auto ait : *data)
            jump_table[ait.second.id][it] = 0;
        cout << "=";
    }
    for (auto it : *data)
    {
        if (valves_to_open.count(it.first) != 0)
            for (auto cit : it.second.connections)
                jump_table[cit][it.first] = 2;

        for (auto cit : it.second.connections)
            if (valves_to_open.count(cit) != 0)
                jump_table[it.first][cit] = 2;
    }
    for (auto it : valves_to_open)
    {
        for (auto sit : *data)
        {
            if (it == sit.first)
                jump_table[sit.first][it] = 0;
            else
            {
                if (jump_table[sit.first][it] == 0)
                {
                    auto time = look_ahead - find_time_id(it, sit.first, look_ahead);
                    jump_table[sit.first][it] = time;
                }
            }
            // print_id(sit.first);
            // cout << " -> ";
            // print_id(it);
            // cout << " with " << jump_table[sit.first][it] << "\n";
        }
        cout << "=";
    }
    cout << "]\n";
    cout << ">> Jump table done\n\n";
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

    for (auto it : *data)
    {
        if (it.second.rate != 0)
        {
            visited[it.first] = false;
            valves_to_open.insert(it.first);
        }
    }
    construct_jump_table(valves_to_open);

    id_t next_id = open_next(valve.id, valves_to_open, visited, look_ahead);
    while (ctr > 0)
    {
        auto dp = pressure_increase(&open_valves);
        cout.width(3);
        cout << 30 - ctr << ": ";
        cout.width(3);
        cout << dp << " | ";
        accumulated_pressure += dp;
        if (valve.id == next_id)
        {
            cout << "At ";
            print_id(next_id);
            cout << " opening ";
            print_id(next_id);
            cout << "\n";
            open_valves.insert(next_id);
            next_id = open_next(valve.id, valves_to_open, visited, look_ahead);
        }
        else
        {
            cout << "At ";
            print_id(valve.id);
            cout << " towards ";
            print_id(next_id);
            cout << "\n";
            path_next(&valve, next_id);
        }

        ctr--;
    }
    print_id_set(open_valves, "Open valves");
    cout << "\n====\n";
    return accumulated_pressure;
}
inline int32_t distance_between(id_t id0, id_t id1)
{
    return jump_table[id0][id1];
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

    for (auto it : *data)
    {
        if (it.second.rate != 0)
        {
            visited[it.first] = false;
            valves_to_open.insert(it.first);
        }
    }

    /** Plan ahead approach:
     * 1. Let each one pick two consecutive destinations
     * 2. Iterate through remaining adding one for each.
     */
    list<id_t> elephant_list;
    list<id_t> self_list;
    id_t next_id_elephant = open_next(valve_elephant.id, valves_to_open, visited, look_ahead);
    elephant_list.push_back(next_id_elephant);
    next_id_elephant = open_next(elephant_list.back(), valves_to_open, visited, look_ahead);
    elephant_list.push_back(next_id_elephant);
    id_t next_id_self = open_next(valve_self.id, valves_to_open, visited, look_ahead);
    self_list.push_back(next_id_self);
    next_id_self = open_next(self_list.back(), valves_to_open, visited, look_ahead);
    self_list.push_back(next_id_self);

    while (!valves_to_open.empty())
    {
        next_id_elephant = open_next(elephant_list.back(), valves_to_open, visited, look_ahead);
        elephant_list.push_back(next_id_elephant);
        if (!valves_to_open.empty())
        {
            next_id_self = open_next(self_list.back(), valves_to_open, visited, look_ahead);
            self_list.push_back(next_id_self);
        }
    }
    cout << "Before swapping\nElephants list(distance): ";
    int32_t total_dist = 0;

    auto it = elephant_list.begin();
    while (it != elephant_list.end())
    {
        print_id(*it);
        cout << "(" << total_dist << "), ";
        total_dist += distance_between(*it, *(it++));
    }
    cout << "\n     your list(distance): ";
    total_dist = 0;
    it = self_list.begin();
    while (it != self_list.end())
    {
        print_id(*it);
        cout << "(" << total_dist << "), ";
        total_dist += distance_between(*it, *(it++));
    }
    cout << "\n\n";

    /**
     * Since the elephant got to pick first
     * you can choose to swap some valves
     * with the elephant in the from the
     * from the movement plan, given that
     * you have shorter distance to that
     * valve. Firt valve is not cannot
     * be changed.
     */
    auto eit = elephant_list.begin();
    auto sit = self_list.begin();
    next_id_elephant = *eit;
    next_id_self = *sit;
    eit++;
    sit++;
    while (eit != elephant_list.end() && sit != self_list.end())
    {
        if (distance_between(next_id_self, *eit) < distance_between(next_id_self, *sit))
        {
            iter_swap(sit, eit);
        }
        next_id_elephant = *eit;
        next_id_self = *sit;
        sit++;
        eit++;
    }
    cout << "After swapping\nElephants list(distance): ";
    total_dist = 0;
    it = elephant_list.begin();
    while (it != elephant_list.end())
    {
        print_id(*it);
        cout << "(" << total_dist << "), ";
        total_dist += distance_between(*it, *(it++));
    }
    cout << "\n     your list(distance): ";
    total_dist = 0;
    it = self_list.begin();
    while (it != self_list.end())
    {
        print_id(*it);
        cout << "(" << total_dist << "), ";
        total_dist += distance_between(*it, *(it++));
    }
    cout << "\n\n";

    next_id_self = self_list.front();
    self_list.pop_front();
    next_id_elephant = elephant_list.front();
    elephant_list.pop_front();
    while (ctr > 0)
    {
        auto dp = pressure_increase(&open_valves);
        cout.width(3);
        cout << 26 - ctr << ": ";
        cout.width(3);
        cout << dp << " | ";
        accumulated_pressure += dp;
        if (valve_self.id == next_id_self)
        {
            cout << "You at ";
            print_id(next_id_self);
            cout << " opening ";
            print_id(next_id_self);
            cout << " | ";
            open_valves.insert(next_id_self);
            if (!self_list.empty())
            {
                next_id_self = self_list.front();
                self_list.pop_front();
            }
        }
        else
        {
            cout << "You at ";
            print_id(valve_self.id);
            cout << " towards ";
            print_id(next_id_self);
            cout << " | ";
            path_next(&valve_self, next_id_self);
        }
        if (valve_elephant.id == next_id_elephant)
        {

            cout << "Elephant at ";
            print_id(next_id_elephant);
            cout << " opening ";
            print_id(next_id_elephant);
            cout << "\n";
            open_valves.insert(next_id_elephant);
            // next_id_elephant = open_next(valve_elephant.id, valves_to_open, visited, look_ahead);
            if (!elephant_list.empty())
            {
                next_id_elephant = elephant_list.front();
                elephant_list.pop_front();
            }
        }
        else
        {
            cout << "Elephant at ";
            print_id(valve_elephant.id);
            cout << " towards ";
            print_id(next_id_elephant);
            cout << "\n";
            path_next(&valve_elephant, next_id_elephant);
        }
        ctr--;
    }
    cout << "\n";
    print_id_set(open_valves, "Open valves");
    print_id_set(valves_to_open, "None opened");
    return accumulated_pressure;
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
    /** add delete to all data here */
    return 0;
}