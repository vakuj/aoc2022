#include <iostream>
#include <fstream>
#include <algorithm>
// #include <vector>
#include <map>

#define BUFSIZE 1024
/** used in processing */
int32_t *play;
int32_t *data1;
int32_t *data2;
int32_t cnt;
size_t length;
/** add additional */

bool map_input(int32_t *dst, int32_t offset, char input)
{
    bool entry = true;
    switch (input)
    {
    case 'A':
        dst[offset] = 1;
        break;
    case 'B':
        dst[offset] = 2;
        break;
    case 'C':
        dst[offset] = 3;
        break;
    case 'X':
        dst[offset] = 1;
        break;
    case 'Y':
        dst[offset] = 2;
        break;
    case 'Z':
        dst[offset] = 3;
        break;

    default:
        entry = false;
        break;
    }
    return entry;
}
void map_play(int32_t *dst, int32_t offset, char pA, char pB)
{
    if (pA == 'A') // player a is rock
    {
        if (pB == 'Y')       // player B is paper
            dst[offset] = 1; // B win
        else if (pB == 'X')
            dst[offset] = 0; // draw
        else
            dst[offset] = -1; // B lose
        return;
    }
    if (pA == 'B') // player a is paper
    {
        if (pB == 'Z')       // player B is scissors
            dst[offset] = 1; // B win
        else if (pB == 'Y')
            dst[offset] = 0; // draw
        else
            dst[offset] = -1; // B lose

        return;
    }
    if (pA == 'C') // player a is scissors
    {
        if (pB == 'X')       // player B is rock
            dst[offset] = 1; // B win
        else if (pB == 'Z')
            dst[offset] = 0; // draw
        else
            dst[offset] = -1; // B lose
        return;
    }
}

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

    data1 = new int32_t[length];
    data2 = new int32_t[length];
    play = new int32_t[length];

    char *indata = new char[BUFSIZE];
    int32_t ctr = 0;
    while (!ifs.eof())
    {
        ifs.getline(indata, BUFSIZE);
        /** store for later processing */
        bool entry = map_input(data1, ctr, indata[0]);
        entry &= map_input(data2, ctr, indata[2]);
        if (!entry)
        {
            std::cerr << "failed to insert, aborting...\n";
            exit(1);
        }
        map_play(play, ctr, indata[0], indata[2]);
        ctr++;
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
    std::cout << "cnt: " << cnt << "\n";
    int32_t total_score = 0;
    for (int32_t i = 0; i < cnt; i++)
    {
        total_score += data2[i]; // score of selection

        if (play[i] > 0)
            total_score += 6; // you won
        if (play[i] == 0)
            total_score += 3; // draw
        if (play[i] < 0)
            total_score += 0; // you lost
    }
    std::cout << "total score: " << total_score << "\n";
}
int32_t process_task1(void)
{
    /** Do something to process the task specific to 1 */
    return 0;
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
    // write_output("output.txt", task1, task2);

    std::cout << "Task 1 result: " << task1 << "\n";
    std::cout << "Task 2 result: " << task2 << "\n";

    delete[] data1;
    delete[] data2;
    delete[] play;
    /** add delete to all data here */
    return 0;
}