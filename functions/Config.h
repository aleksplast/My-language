#ifndef CONFIG_H
#define CONFIG_H

struct Name
{
    char* name;
    int index;
};

struct NamesTable
{
    Name* names;
    int size;
};

const unsigned long long CANARY = 0xBADC0FFEE;
const unsigned int Seed = 33;
typedef NamesTable elem_t;

#endif //CONFIG_H
