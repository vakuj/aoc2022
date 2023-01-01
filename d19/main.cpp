#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>

using namespace std;
#define BUFSIZE 1024

typedef struct cost_t
{
    int32_t ore;
    int32_t clay;
    int32_t obsidian;
    int32_t geode;
    bool hasFunds(cost_t cost)
    {
        if (cost.ore > 0)
            if (this->ore < cost.ore)
                return false;
        if (cost.clay > 0)
            if (this->clay < cost.clay)
                return false;
        if (cost.obsidian > 0)
            if (this->obsidian < cost.obsidian)
                return false;
        if (cost.geode > 0)
            if (this->geode < cost.geode)
                return false;
        return true;
    }
    void withdraw(cost_t cost)
    {
        this->ore -= cost.ore;
        this->clay -= cost.clay;
        this->obsidian -= cost.obsidian;
        this->geode -= cost.obsidian;
    }
    void deposit(cost_t cost)
    {
        this->ore += cost.ore;
        this->clay += cost.clay;
        this->obsidian += cost.obsidian;
        this->geode += cost.obsidian;
    }
    friend ostream &operator<<(ostream &os, cost_t cost)
    {
        os << cost.ore << ", " << cost.clay << ", ";
        os << cost.obsidian << ", " << cost.geode;
        return os;
    }
} cost_t;
struct BLUEPRINT
{
public:
    enum blue_t
    {
        UNDEF,
        ORE,
        CLAY,
        OBSIDIAN,
        GEODE,
    };

private:
    cost_t ore_robot = {0, 0, 0, 0};
    cost_t clay_robot = {0, 0, 0, 0};
    cost_t obsidian_robot = {0, 0, 0, 0};
    cost_t geode_robot = {0, 0, 0, 0};
    int32_t id_nbr;

public:
    BLUEPRINT() {}
    BLUEPRINT(int32_t __id) : id_nbr(__id) {}
    ~BLUEPRINT() {}
    void set_id(int32_t __id) { id_nbr = __id; }
    int32_t get_id(void) { return id_nbr; }
    void set_cost(blue_t bt, cost_t cost)
    {
        if (bt == blue_t::ORE)
            ore_robot = cost;
        if (bt == blue_t::CLAY)
            clay_robot = cost;
        if (bt == blue_t::OBSIDIAN)
            obsidian_robot = cost;
        if (bt == blue_t::GEODE)
            geode_robot = cost;
    }
    cost_t get_cost(blue_t bt)
    {
        if (bt == blue_t::ORE)
            return ore_robot;
        if (bt == blue_t::CLAY)
            return clay_robot;
        if (bt == blue_t::OBSIDIAN)
            return obsidian_robot;
        if (bt == blue_t::GEODE)
            return geode_robot;
        return ore_robot;
    }
};
struct BANK
{
private:
    cost_t funds = {0, 0, 0, 0};

public:
    BANK() {}
    ~BANK() {}
    bool try_withdraw(cost_t cost)
    {
        if (funds.hasFunds(cost))
        {
            funds.withdraw(cost);
            return true;
        }
        return false;
    }
    void deposit(cost_t cost) { funds.deposit(cost); }
    cost_t get_balance(void) { return funds; }
};
struct MINE
{
private:
    BLUEPRINT bp;
    BANK bank;
    cost_t workers = {1, 0, 0, 0};

public:
    MINE() {}
    MINE(BLUEPRINT bl) : bp(bl) {}
    ~MINE() {}

    BLUEPRINT::blue_t try_upgrade(void)
    {
        if (bank.try_withdraw(bp.get_cost(BLUEPRINT::blue_t::GEODE)))
            return BLUEPRINT::blue_t::GEODE;
        if (bank.try_withdraw(bp.get_cost(BLUEPRINT::blue_t::OBSIDIAN)))
            return BLUEPRINT::blue_t::OBSIDIAN;
        if (bank.try_withdraw(bp.get_cost(BLUEPRINT::blue_t::CLAY)))
            return BLUEPRINT::blue_t::CLAY;
        if (bank.try_withdraw(bp.get_cost(BLUEPRINT::blue_t::ORE)))
            return BLUEPRINT::blue_t::ORE;
        return BLUEPRINT::blue_t::UNDEF;
    }
    void upgrade(BLUEPRINT::blue_t worker)
    {
        switch (worker)
        {
        case BLUEPRINT::blue_t::ORE:
            workers.ore += 1;
            break;
        case BLUEPRINT::blue_t::CLAY:
            workers.clay += 1;
            break;
        case BLUEPRINT::blue_t::OBSIDIAN:
            workers.obsidian += 1;
            break;
        case BLUEPRINT::blue_t::GEODE:
            workers.geode += 1;
            break;
        default:
            break;
        }
    }
    int32_t evolve(int32_t end_time)
    {
        int32_t time = 0;
        BLUEPRINT::blue_t next_worker;
        while (time < end_time)
        {
            next_worker = try_upgrade();
            cout << next_worker << " -> ";
            bank.deposit(workers);
            cout << bank.get_balance() << " + ";
            upgrade(next_worker);
            cout << workers << endl;
            time++;
        }
        return bp.get_id() * bank.get_balance().geode;
    }
};

/** used in processing */
vector<MINE> *data;
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

    data = new vector<MINE>();
    MINE mine;
    BLUEPRINT bp;
    int32_t id_nbr = 0;
    cost_t ore = {0, 0, 0, 0};
    cost_t clay = {0, 0, 0, 0};
    cost_t obsi = {0, 0, 0, 0};
    cost_t geod = {0, 0, 0, 0};

    char *indata = new char[BUFSIZE];
    int32_t ctr = 0;
    while (!ifs.eof())
    {
        int next = ifs.peek();
        if (next == '\n')
        {
            /** forward without storing */
        }
        else
        {
            /** store for later processing */
            ifs.getline(indata, BUFSIZE);
            if (7 == sscanf(indata, "Blueprint %d: Each ore robot costs %d ore. Each clay robot costs %d ore. Each obsidian robot costs %d ore and %d clay. Each geode robot costs %d ore and %d obsidian.",
                            &id_nbr, &ore.ore, &clay.ore, &obsi.ore, &obsi.clay, &geod.ore, &geod.obsidian))
            {
                bp.set_id(id_nbr);
                bp.set_cost(BLUEPRINT::blue_t::ORE, ore);
                bp.set_cost(BLUEPRINT::blue_t::CLAY, clay);
                bp.set_cost(BLUEPRINT::blue_t::OBSIDIAN, obsi);
                bp.set_cost(BLUEPRINT::blue_t::GEODE, geod);
                mine = MINE(bp);
                data->push_back(mine);
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
    std::ofstream ofs(filename, std::ofstream::out);
    ofs << "Task 1 result: " << task1 << "\n";
    ofs << "Task 2 result: " << task2 << "\n";
    ofs.close();
}
void process_task()
{
    /** Do something to process the task in general */
}
int32_t process_task1(void)
{
    /** Do something to process the task specific to 1 */
    return data->at(0).evolve(24);
}
int32_t process_task2(void)
{
    /** Do something to process the task specific to 2 */
    return 0;
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

    std::cout << "Task 1 result: " << task1 << "\n";
    std::cout << "Task 2 result: " << task2 << "\n";

    delete data;
    /** add delete to all data here */
    return 0;
}