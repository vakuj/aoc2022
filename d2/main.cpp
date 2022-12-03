#include <iostream>
#include <fstream>

#define BUFSIZE 1024
/** used in processing */

char *data1;
char *data2;
int32_t cnt;
size_t length;
/** add additional */

int32_t map_input(char input)
{
    int32_t mapped = 0;
    switch (input)
    {
    case 'A':
        mapped = 1;
        break;
    case 'B':
        mapped = 2;
        break;
    case 'C':
        mapped = 3;
        break;
    case 'X':
        mapped = 1;
        break;
    case 'Y':
        mapped = 2;
        break;
    case 'Z':
        mapped = 3;
        break;

    default:
        break;
    }
    return mapped;
}
int32_t map_rule1(char playerA, char playerB)
{
    if (playerA == 'A') // player a is rock
    {
        if (playerB == 'Y') // player B is platerAper
            return 6;       // B win
        else if (playerB == 'X')
            return 3; // draw
        else
            return 0; // B lose
    }
    if (playerA == 'B') // player a is platerAper
    {
        if (playerB == 'Z') // player B is scissors
            return 6;       // B win
        else if (playerB == 'Y')
            return 3; // draw
        else
            return 0; // B lose
    }
    if (playerA == 'C') // player a is scissors
    {
        if (playerB == 'X') // player B is rock
            return 6;       // B win
        else if (playerB == 'Z')
            return 3; // draw
        else
            return 0; // B lose
    }
    return 0;
}
int32_t map_rule2(char playerA, char playRes)
{
    /**
     * x,y,z = lose,draw,win
     * a = c,a,b
     * b = a,b,c
     * c = b,c,a
     */

    if (playerA == 'A') // player a is rock
    {
        if (playRes == 'X')
            return 3 + 0;
        else if (playRes == 'Y')
            return 1 + 3;
        else
            return 2 + 6;
    }
    if (playerA == 'B') // player a is paper
    {
        if (playRes == 'X')
            return 1 + 0;
        else if (playRes == 'Y')
            return 2 + 3;
        else
            return 3 + 6;
    }
    if (playerA == 'C') // player a is scissors
    {
        if (playRes == 'X')
            return 2 + 0;
        else if (playRes == 'Y')
            return 3 + 3;
        else
            return 1 + 6;
    }
    return 0;
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

    data1 = new char[length];
    data2 = new char[length];

    char *indata = new char[BUFSIZE];
    int32_t ctr = 0;
    while (!ifs.eof())
    {
        ifs.getline(indata, BUFSIZE);
        /** store for later processing */
        data1[ctr] = indata[0];
        data2[ctr] = indata[2];
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
}
int32_t process_task1(void)
{
    int32_t total_score = 0;
    for (int32_t i = 0; i < cnt; i++)
    {
        total_score += map_input(data2[i]) + map_rule1(data1[i], data2[i]);
    }
    return total_score;
}
int32_t process_task2(void)
{
    int32_t total_score = 0;
    for (int32_t i = 0; i < cnt; i++)
    {
        total_score += map_rule2(data1[i], data2[i]);
    }
    return total_score;
}
int main()
{
    /** use this for testing examples */
    // read_input("test.txt");

    /** use this to run input */
    read_input("input.txt");

    // process_task();

    int32_t task1 = process_task1();
    int32_t task2 = process_task2();

    /** store output to file */
    write_output("output.txt", task1, task2);

    std::cout << "Task 1 result: " << task1 << "\n";
    std::cout << "Task 2 result: " << task2 << "\n";

    delete[] data1;
    delete[] data2;
    /** add delete to all data here */
    return 0;
}