#include <stdio.h>

#include "frontend\frontend.h"
#include "backend\backend.h"
#include "middleend\middleend.h"

int main(int argc, char* argv[])
{
    const char* input = GetComArg(argc, argv);

    setlocale(LC_ALL, "Rus");
    Source datasrc = InputHandler(input);

    FILE* asmprog = fopen("asmprog.txt", "w");

    Tree tree = {};
    char log[STRSIZE] = "pictures\\graphlog.htm";

    char** namestable = (char**) calloc(datasrc.size, sizeof(char*));

    printf("HERE %d\n", __LINE__);

    char* datastr = TranslitIntoEnglish(datasrc.ptr, datasrc.size);

    printf("datastr = %s\n", datastr);

    Node** nodes = LexicAnalizer(datastr, &namestable);

    printf("HERE %d\n", __LINE__);

    Node* node = GetGrammar(nodes);

    printf("HERE %d\n", __LINE__);

    DataPrint(node);
    node = ReadFromStandart("data.txt");

    TreeCtor(&tree, NUM_TYPE, 0, OP_UNKNOWN, NULL, log);
    tree.anchor = node;
    TreeGraphDump(&tree, 0, __LINE__, __func__, __FILE__);

    CreateAncestor(tree.anchor, NULL, &tree);
    SimplifyCode(&tree);

    CreateAsmProgramm(tree.anchor, asmprog);

    system("asm.exe asmprog.txt");
    system("cpu.exe");

    return 0;
}
