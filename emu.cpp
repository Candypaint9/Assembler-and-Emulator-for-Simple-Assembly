/*
Name: Advait Deshmukh

Decleration of Authorship: I hereby confirm that the work presented in this project is my own.
*/

#include <bits/stdc++.h>
using namespace std;

// to store all opcodes, mnemonics and operand types
map<string, pair<string, int>> instructionTable;
map<string, string> opcodeWiseInstructions;

// option chosen
string option;

// registers
int A = 0;
int B = 0;
int SP = 0;
int PC = 0;

vector<int> machineCode;
vector<int> memory((1 << 24));

void initializeInstructionTable()
{
    instructionTable["data"] = {"", 1};
    instructionTable["ldc"] = {"00", 1};
    instructionTable["adc"] = {"01", 1};
    instructionTable["ldl"] = {"02", 2};
    instructionTable["stl"] = {"03", 2};
    instructionTable["ldnl"] = {"04", 2};
    instructionTable["stnl"] = {"05", 2};
    instructionTable["add"] = {"06", 0};
    instructionTable["sub"] = {"07", 0};
    instructionTable["shl"] = {"08", 0};
    instructionTable["shr"] = {"09", 0};
    instructionTable["adj"] = {"0a", 1};
    instructionTable["a2sp"] = {"0b", 0};
    instructionTable["sp2a"] = {"0c", 0};
    instructionTable["call"] = {"0d", 2};
    instructionTable["return"] = {"0e", 0};
    instructionTable["brz"] = {"0f", 2};
    instructionTable["brlz"] = {"10", 2};
    instructionTable["br"] = {"11", 2};
    instructionTable["HALT"] = {"12", 0};
    instructionTable["SET"] = {"", 1};

    for (auto i : instructionTable)
    {
        opcodeWiseInstructions[i.second.first] = i.first;
    }
}

string decimalToHex(int n, int size)
{
    if (n < 0)
        n += (1LL << size * 4); // to adjust for size when -ve

    stringstream ss;
    ss << hex << n;
    string ans = ss.str();

    reverse(ans.begin(), ans.end());
    while (ans.size() < size)
        ans += '0';
    reverse(ans.begin(), ans.end());

    return ans;
}

void printISA()
{
    cout << endl;
    cout << "mnemonic, opcode, operand type" << endl;
    for (auto i : instructionTable)
    {
        cout << i.first << ", " << i.second.first << ", ";

        if (i.second.second == 0)
            cout << " " << endl;
        else if (i.second.second == 1)
            cout << "value" << endl;
        else
            cout << "offset" << endl;
    }
}

bool readObjectFile(string objectFileName)
{
    cout << "-trace: shows instruction trace" << endl;
    cout << "-before: shows memory dump before execution" << endl;
    cout << "-after: shows memory dump after execution" << endl;
    cout << "-isa: shows the ISA" << endl;

    ifstream objectFile;
    objectFile.open(objectFileName, ios::binary | ios::in);

    if (objectFile.fail())
        return false;

    unsigned int currLine;
    int memoryInd = 0;
    while (objectFile.read((char *)&currLine, sizeof(int)))
    {
        machineCode.push_back(currLine);
        memory[memoryInd] = currLine;
        memoryInd++;
    }

    return true;
}

// returns updated pc after executing the operation
bool updateRegs(int opCode, int operand)
{
    bool halt = false;
    switch (opCode)
    {
    case 0: // ldc
        B = A;
        A = operand;
        break;

    case 1: // adc
        A += operand;
        break;

    case 2: // ldl
        B = A;
        A = memory[SP + operand];
        break;

    case 3: // stl
        memory[SP + operand] = A;
        A = B;
        break;

    case 4: // ldnl
        A = memory[A + operand];
        break;

    case 5: // stnl
        memory[A + operand] = B;
        break;

    case 6: // add
        A = B + A;
        break;

    case 7: // sub
        A = B - A;
        break;

    case 8: // shl
        A = (B << A);
        break;

    case 9: // shr
        A = (B >> A);
        break;

    case 10: // adj
        SP += operand;
        break;

    case 11: // a2sp
        SP = A;
        A = B;
        break;

    case 12: // sp2a
        B = A;
        A = SP;
        break;

    case 13: // call
        B = A;
        A = PC;
        PC += operand;
        break;

    case 14: // return
        PC = A;
        A = B;
        break;

    case 15: // brz
        if (A == 0)
            PC += operand;
        break;

    case 16: // brlz
        if (A < 0)
            PC += operand;
        break;

    case 17: // br
        PC += operand;
        break;

    case 18: // HALT
        halt = true;
        break;
    }

    if (halt)
        return 1;
    return 0;
}

void execute(bool trace)
{
    PC = 0;
    int counter = 0;
    while (PC < machineCode.size())
    {
        // seperating the opcode and the operands
        int curr = machineCode[PC];
        int opcode = 0;
        int operand = 0;

        // first 8 bits are opcode and next 24 bits are operand
        for (int i = 0; i < 8; i++)
        {
            if ((1 << i) & curr)
                opcode += (1 << i);
        }
        for (int i = 8; i < 31; i++)
        {
            if ((1 << i) & curr)
                operand += (1 << (i - 8));
        }

        // checking operand for -ve values (by checking if first bit is set)
        if ((1 << 31) & curr)
            operand -= (1 << 23);

        if (trace)
        {
            cout << "PC: " << decimalToHex(PC, 8) << "\t";
            cout << "SP: " << decimalToHex(SP, 8) << "\t";
            cout << "A: " << decimalToHex(A, 8) << "\t";
            cout << "B: " << decimalToHex(B, 8) << "\t";
            cout << opcodeWiseInstructions[decimalToHex(opcode, 2)] << " " << operand << endl
                 << endl;
        }

        // checking for seg fault
        if (PC < 0)
        {
            cout << "Segmentation Fault" << endl;
            exit(0);
        }
        if (counter > (1 << 24))
        {
            cout << "Infinite loop detected" << endl;
            exit(0);
        }

        // update registers according to operation
        bool halt = updateRegs(opcode, operand);

        // checking for halt
        if (halt)
        {
            break;
        }

        // incrementing the pc after each instruction
        PC++;
        counter++;
    }

    cout << counter << " number of instructions executed" << endl;
}

void memoryDump()
{
    for (int i = 0; i < machineCode.size(); i += 4)
    {
        // pc
        cout << decimalToHex(i, 8) << "\t";

        for (int j = i; j < min((int)machineCode.size(), i + 4); j++)
        {
            cout << decimalToHex(memory[j], 8) << " ";
        }
        cout << endl;
    }
}

int main(int argc, char *argv[])
{
    initializeInstructionTable();

    if (!readObjectFile(argv[2]))
    {
        cout << "Error while reading file" << endl;
        return 0;
    }

    string option = argv[1];

    if (option == "-isa")
        printISA();
    else if (option == "-trace")
        execute(true);
    else if (option == "-before")
    {
        cout << endl
             << "Memory dump before execution: " << endl;
        memoryDump();
    }
    else if (option == "-after")
    {
        cout << endl
             << "Memory dump after execution: " << endl;
        execute(false);
        memoryDump();
    }
    else
        cout << "Invalid option" << endl;
}