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

    char* datastr = TranslitIntoEnglish(datasrc.ptr, datasrc.size);

    Node** nodes = LexicAnalizer(datastr, &namestable);

    Node* node = GetGrammar(nodes);

    TreeCtor(&tree, NUM_TYPE, 0, OP_UNKNOWN, NULL, log);
    tree.anchor = node;

    CreateAncestor(node, NULL, &tree);
    ChangeCoreFunctions(node);
    TreeGraphDump(&tree, 0, __LINE__, __func__, __FILE__);
    SimplifyCode(&tree);

    DataPrint(node);
    node = ReadFromStandart("data.txt");

    tree.anchor = node;
    TreeGraphDump(&tree, 0, __LINE__, __func__, __FILE__);

    CreateAncestor(tree.anchor, NULL, &tree);

    CreateAsmProgramm(tree.anchor, asmprog);

    system("asm.exe asmprog.txt");
    system("cpu.exe");

    return 0;
}
