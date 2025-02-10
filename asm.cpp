/*
Name: Advait Deshmukh

Decleration of Authorship: I hereby confirm that the work presented in this project is my own.
*/

#include <bits/stdc++.h>
using namespace std;
#define vi vector<int>

typedef struct codeLine
{
    bool emptyLine = false;
    string label;
    bool labelPresent = false;
    string mnemonic;
    string operand;           // operand/offset
    int operandValueType = 0; // if hex, binary, octal
} codeLine;

//<Mnemonic, opcode(in hexadecimal), operand type
// operand type = 0: no operands
// operand type = 1: operand
// operand type = 2: offset
map<string, pair<string, int>> instructionTable;

string assemblyFileName;
string listingFileName;
string objectFileName;
string logFileName;
// stores the assembly code
vector<string> assemblyCode;
map<string, int> symbolTable; // Label, lineNo
set<string> symbolsFound;
map<int, string> lineWiseSymbolTable; // lineNo, label
vector<codeLine> codeTable;
vector<int> lineWisePC;
map<string, int> setLabels;

// final machine code:
vector<pair<string, string>> machineCode;

// errors
vector<pair<int, string>> errors;

void debugPrintCode()
{
    cout << "----------code--------------" << endl;
    for (auto i : assemblyCode)
    {
        cout << i << endl;
    }
    cout << endl;
}

void debugPrintSymbols()
{
    cout << "-----------symbols------------" << endl;
    for (auto i : symbolTable)
    {
        cout << i.first << " " << i.second << endl;
    }
    cout << endl;
}

void debugPrintCodeTable()
{
    cout << "--------codeTable---------" << endl;
    cout << "label, mnemonic, operand, opValueType" << endl;
    for (auto i : codeTable)
    {
        cout << i.label << ", " << i.mnemonic << ", " << i.operand << ", " << i.operandValueType << endl;
    }
}

void debugPrintMachineCode()
{
    cout << "-------Machine Code-------" << endl;
    for (auto i : machineCode)
    {
        cout << i.first << " " << i.second << endl;
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
}

bool isValidSymbolName(string s)
{
    if (s.empty() || isdigit(s[0]))
        return false;

    for (auto ch : s)
    {
        if (!isalnum(ch) && ch != '_')
            return false;
    }

    return true;
}

bool isDecimal(string s)
{
    if (s.empty())
        return 0;
    
    if(s.size() > 1 && s[0] == '0')
        return 0;

    for (auto i : s)
        if (!(i >= '0' && i <= '9'))
            return 0;
    return 1;
}

bool isOctal(string s)
{
    if (s.size() < 2 || s[0] != '0')
        return 0;

    for (auto i : s)
        if (!(i >= '0' && i <= '7'))
            return 0;

    return 1;
}

bool isHex(string s)
{
    if (s.size() < 3 || s[0] != '0' || s[1] != 'x' && s[1] != 'X')
        return 0;

    s = s.substr(2, string::npos);

    for (auto i : s)
        if (!isdigit(i) && !(i >= 'a' && i <= 'f' || i >= 'A' && i <= 'F'))
            return 0;

    return 1;
}

int getOperandType(string s)
{
    if (!s.empty())
    {
        // check if only '+' or '-' without number
        if (s[0] == '+' || s[0] == '-')
        {
            if (s.size() == 1)
                return -1;
            else
                s = s.substr(1, string::npos);
        }

        if (isOctal(s))
            return 8;
        else if (isDecimal(s))
            return 10;
        else if (isHex(s))
            return 16;
        else if (isValidSymbolName(s))
            return 1; // returning 1 means a symbol is the operand
    }
    else
        return 0; // returning 0 means no operand present

    // returning -1 means syntax error(handled in createCodeTable function)
    return -1;
}

bool readAssemblyCode()
{
    ifstream assemblyFile;

    cout << "Enter file name: ";
    cin >> assemblyFileName;

    assemblyFile.open(assemblyFileName);

    if (assemblyFile.fail())
    {
        errors.push_back({-1, "Error while reading file"});
        return false;
    }

    string currLine;
    while (getline(assemblyFile, currLine))
    {
        assemblyCode.push_back(currLine);
    }

    // storing names of list file, object file and log file
    listingFileName = assemblyFileName;
    objectFileName = assemblyFileName;
    logFileName = assemblyFileName;

    // to remove the .asm
    for (int i = 0; i < 4; i++)
    {
        listingFileName.pop_back();
        objectFileName.pop_back();
        logFileName.pop_back();
    }

    listingFileName += ".lst";
    objectFileName += ".o";
    logFileName += ".log";

    return true;
}

// refactors and cleans the code
// removes comments and takes care of spaces
void cleanCode()
{
    vector<string> cleanedAssemblyCode;
    for (int lineNo = 0; lineNo < assemblyCode.size(); lineNo++)
    {
        string line = assemblyCode[lineNo];
        string formattedLine;

        for (int i = 0; i < line.size(); i++)
        {
            if (line[i] == ';')
                break; // removing comments
            else if (line[i] != ' ' && line[i] != '\t')
            {
                formattedLine += line[i];
                if (line[i] == ':')
                    formattedLine += ' ';
            }
            else if (formattedLine.size() && formattedLine.back() != ' ')
                formattedLine += ' ';
        }

        if (!formattedLine.empty() && formattedLine.back() != ' ')
            formattedLine += ' ';
        cleanedAssemblyCode.push_back(formattedLine);
    }

    assemblyCode = cleanedAssemblyCode;
}

void createSymbolTable()
{
    int pc = 0;

    for (int lineNo = 0; lineNo < assemblyCode.size(); lineNo++)
    {
        string line = assemblyCode[lineNo];

        // storing the pc per line
        lineWisePC.push_back(pc);

        string symbol;
        bool labelFound = false;
        bool mnemonicFound = false;
        bool codeFound = false;
        for (auto ch : line)
        {
            if (ch == ':')
            {
                if (symbolsFound.find(symbol) != symbolsFound.end())
                {
                    errors.push_back({lineNo + 1, "Duplicate label definition"});
                    break;
                }
                else if (!isValidSymbolName(symbol))
                {
                    errors.push_back({lineNo + 1, "Not allowed label name"});
                    break;
                }
                symbolTable[symbol] = lineNo;
                symbolsFound.insert(symbol);
                labelFound = true;
                symbol.clear();
            }
            else if (ch == ' ' && symbol.size()) // at ending of a mnemonic or literal
            {
                // if it is mnemonic
                if (instructionTable.find(symbol) != instructionTable.end())
                {
                    mnemonicFound = true;
                    codeFound = true;
                }
                // if it is a label or operand
                else if (isValidSymbolName(symbol) && mnemonicFound)
                {
                    if (symbolTable.find(symbol) == symbolTable.end())
                        symbolTable[symbol] = -1;
                    codeFound = true;
                }

                symbol.clear();
            }
            else if (ch != ' ')
                symbol += ch;
        }

        // if instruction found then only increment the pc
        if (codeFound)
            pc++;
    }

    // making line wise symbol table
    for (auto i : symbolTable)
    {
        lineWiseSymbolTable[i.second] = i.first;
    }
}

void createCodeTable()
{
    for (int lineNo = 0; lineNo < assemblyCode.size(); lineNo++)
    {
        string line = assemblyCode[lineNo];
        codeLine _codeLine;

        if (line.empty())
        {
            _codeLine.emptyLine = true;
        }

        string _label = lineWiseSymbolTable[lineNo];
        if (_label.size())
        {
            _codeLine.labelPresent = true;
            _codeLine.label = _label;
        }

        string curr;
        int ind = 0; // 0:mnemonic, 1:operand
        int i = _codeLine.labelPresent ? _label.size() + 2 : 0;

        bool errorFound = false;

        for (; i < line.size(); i++) //+2 for the ": "
        {
            char ch = line[i];
            if (ch != ' ')
                curr += ch;
            else if (ind == 0)
            {
                if (instructionTable.find(curr) != instructionTable.end())
                {
                    _codeLine.mnemonic = curr;
                    ind++;
                }
                // checking if it isnt duplicate label in which case error has already been called before
                else if (curr.size() && curr.find(':') == string::npos)
                {
                    errors.push_back({lineNo + 1, "Invalid mnemonic"});
                    errorFound = true;
                    break;
                }

                curr.clear();
            }
            else if (ind == 1)
            {
                _codeLine.operand = curr;
                int opType = getOperandType(curr);

                if (curr.find(',') != string::npos)
                {
                    errors.push_back({lineNo + 1, "Invalid syntax"});
                    errorFound = true;
                    break;
                }
                else if (opType == -1)
                {
                    errors.push_back({lineNo + 1, "Invalid operand"});
                    errorFound = true;
                    break;
                }
                else if (instructionTable[_codeLine.mnemonic].second == 0 && opType != 0)
                {
                    errors.push_back({lineNo + 1, "Unexpected operand"});
                    errorFound = true;
                    break;
                }
                else if (opType == 1 && symbolTable[curr] == -1)
                {
                    if (_codeLine.mnemonic == "br")
                        errors.push_back({lineNo + 1, "No such label defined"});
                    else
                        errors.push_back({lineNo + 1, "No such variable defined"});
                }

                _codeLine.operandValueType = opType;
            }
        }

        if (!errorFound && instructionTable[_codeLine.mnemonic].second != 0 && _codeLine.operandValueType == 0)
        {
            errors.push_back({lineNo + 1, "Missing operand"});
        }

        /*
        If the mnemonic is set then setting the symbol table value of the label in the symbolTable to the operand
        instead of the line number the label is present on (the operand is converted to decimal)
        */
        if (_codeLine.mnemonic == "SET")
        {
            setLabels[_codeLine.label] = stoi(_codeLine.operand, 0, _codeLine.operandValueType);
        }

        codeTable.push_back(_codeLine);
    }
}

void writeListingFile(ofstream &listFile)
{
    listFile.open(listingFileName);

    // writing to list file
    for (int i = 0; i < codeTable.size(); i++)
    {
        // if line is empty no need to list it
        if (codeTable[i].emptyLine)
            continue;

        string mac = machineCode[lineWisePC[i]].second;

        if (i + 1 < lineWisePC.size() && lineWisePC[i] == lineWisePC[i + 1])
            mac = "        ";

        listFile << decimalToHex(lineWisePC[i], 8) << " " << mac << " " << assemblyCode[i] << endl;
    }

    listFile.close();
}

void writeObjectFile(ofstream &objectFile)
{
    objectFile.open(objectFileName, ios::out | ios::binary);

    // writing to object file
    for (auto line : machineCode)
    {
        unsigned int val;

        stringstream ss;
        ss << hex << line.second;
        ss >> val;
        static_cast<int>(val);

        objectFile.write((const char *)&val, sizeof(unsigned int));
    }

    objectFile.close();
}

void writeFiles()
{
    // opening listing file
    ofstream listFile;
    writeListingFile(listFile);

    // opening object file
    ofstream objectFile;
    writeObjectFile(objectFile);
}

void firstPass()
{
    // reads assembly code from file and if errors stop
    if (!readAssemblyCode())
        return;
    cleanCode();

    createSymbolTable();
    createCodeTable();
}

// converts to machine code
void secondPass()
{
    for (int lineNo = 0; lineNo < codeTable.size(); lineNo++)
    {
        string lineLocation = decimalToHex(lineWisePC[lineNo], 8);
        codeLine curr = codeTable[lineNo];

        if (curr.emptyLine)
            continue;

        string currMacCode;

        // if only label present
        if (curr.mnemonic.empty())
        {
            continue;
        }
        // if no operand present
        else if (curr.operandValueType == 0)
        {
            currMacCode += "000000";
        }
        // if operand value type is a symbol
        else if (curr.operandValueType == 1)
        {
            int address;

            if (instructionTable[curr.mnemonic].second == 2)                              // if operand takes an offset
                address = lineWisePC[symbolTable[curr.operand]] - lineWisePC[lineNo] - 1; // extra -1 as in emulater we do +1 to pc every time
            else if (setLabels.find(curr.operand) != setLabels.end())                     // checking if label hadx been set
                address = setLabels[curr.operand];                                        // here its not actually an address but a value
            else
                address = lineWisePC[symbolTable[curr.operand]];

            currMacCode = decimalToHex(address, 6);
        }
        // if operand value type is any number type
        else
        {
            // converting to base 10
            int value = stoi(curr.operand, 0, curr.operandValueType);

            if (curr.mnemonic == "data" || curr.mnemonic == "SET")
                currMacCode = decimalToHex(value, 8);
            else
                currMacCode = decimalToHex(value, 6);
        }

        currMacCode += instructionTable[curr.mnemonic].first;
        machineCode.push_back({lineLocation, currMacCode});
    }
}

// return false if no errors found
bool logErrors()
{
    // not returning first to clear any previous contents of file
    sort(errors.begin(), errors.end());

    ofstream logFile;
    logFile.open(logFileName, ofstream::trunc);

    for (auto i : errors)
    {
        if (i.first != -1)
            logFile << "Error at line " << i.first << ": " << i.second << endl;
        else
            logFile << i.second << endl;
    }

    if (errors.empty())
    {
        logFile << "No errors found" << endl;
        logFile << "Listing file generated as: " << listingFileName << endl;
        logFile << "Object file generated as: " << objectFileName << endl;
        logFile.close();
        return false;
    }

    logFile.close();

    return true;
}

int main()
{
    initializeInstructionTable();

    firstPass();

    if (!logErrors())
    {
        secondPass();
        writeFiles();
        cout << "Assembled succesfully" << endl;
    }
    else
        cout << "Errors found" << endl;
}