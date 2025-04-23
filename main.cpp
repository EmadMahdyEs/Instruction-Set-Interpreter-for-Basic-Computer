#include <iostream>
#include <fstream>
#include <sstream>
#include <bitset>
#include <unordered_map>
#include <string>
using namespace std;

const int MEMORY_SIZE = 4096;
int memory[MEMORY_SIZE] = {1,1100,1200,1300,1400,1500,1600};


int AC = 0, E = 0, PC = 0;
bool inputFlag = true, outputFlag = true, interruptEnable = false, halt = false;

void executeRegisterInstruction(int instr) 
{
    switch (instr) 
    {
        case 0x7800: 
            AC = 0; 
            cout << "CLA  // AC = 0\n"; 
            break;
        case 0x7400: 
            E = 0; 
            cout << "CLE  // E = 0\n"; 
            break;
        case 0x7200: 
            AC = ~AC; 
            cout << "CMA  // AC = ~AC = " << "0x" << hex << uppercase<< AC << endl; 
            break;
        case 0x7100: 
            E = !E; 
            cout << "CME  // E = ~E = " << E << endl; 
            break;
        case 0x7080: 
            E = AC & 1; 
            AC = (E << 15) | (AC >> 1); 
            cout << "CIR  // AC = "<< "0x" << hex << uppercase<<AC<<", "<<"E = "<<E<<endl;
            break;
        case 0x7040: 
            AC = (AC << 1) | E; 
            E = (AC & 0x10000) >> 16; 
            AC &= 0xFFFF; 
            cout << "CIL  // AC = "<< "0x" << hex << uppercase<<AC<<", "<<"E = "<<E<<endl; 
            break;
        case 0x7020: 
            AC = (AC + 1) & 0xFFFF; 
            cout << "INC  // AC = " << "0x" << hex << uppercase<< AC << endl; 
            break;
        case 0x7010: 
            if ((AC & 0x8000) == 0)
            {
                PC++;
            } 
            cout << "SPA  // PC = "<< "0x" << hex << uppercase <<PC<<endl; 
            break;
        case 0x7008: 
            if ((AC & 0x8000) != 0)
            {
                PC++;
            } 
            cout << "SNA  // PC = "<< "0x" << hex << uppercase<<PC<<endl; 
            break;
        case 0x7004: 
            if (AC == 0)
            {
                PC++;
            } 
            cout << "SZA  // PC = "<< "0x" << hex << uppercase<<PC<<endl; 
            break;
        case 0x7002: 
            if (E == 0)
            {
                PC++;
            } 
            cout << "SZE  // PC = "<< "0x" << hex << uppercase<<PC<<endl; 
            break;
        case 0x7001: 
            halt = true; 
            cout << "HLT\n"; 
            break;
        default: 
            cout << "Unknown Register Instruction: 0x" << hex << instr << endl; 
            break;
    }
}

void executeIOInstruction(int instr) 
{
    switch (instr) 
    {
        case 0xF800: 
            cout << "INP: Enter input: "; 
            cin >> AC; 
            inputFlag = false; 
            break;
        case 0xF400: 
            cout << "OUT: " << AC << endl; 
            outputFlag = false; 
            break;
        case 0xF200: 
            if (inputFlag)
            {
                PC++;
            } 
            cout << "SKI // PC = "<< "0x" << hex << uppercase<<PC<<endl; 
            break;
        case 0xF100: 
            if (outputFlag)
            {
                PC++;
            } 
            cout << "SKO  // PC = "<< "0x" << hex << uppercase<<PC<<endl; 
            break;
        case 0xF080: 
            interruptEnable = true; 
            cout << "ION // Interrupt On"<<endl; 
            break;
        case 0xF040: 
            interruptEnable = false; 
            cout << "IOF  // Interrupt Off"<<endl; 
            break;
        default: 
            cout << "Unknown I/O Instruction: 0x" << hex << instr << endl; 
            break;
    }
}

void executeMemoryInstruction(int instr) 
{
    int opcode = (instr >> 12) & 0x7;
    int addr = instr & 0x0FFF;
    bool indirect = instr & 0x8000;
    int effAddr = 0;
    if (indirect)
    {
        effAddr = memory[addr];
    }
    else
    {
        effAddr = addr;
    }
    switch (opcode) 
    {
        case 0: 
            AC &= memory[effAddr]; 
            cout << "AND  // AC = "<< "0x" << hex << uppercase << AC << endl; 
            break;
        case 1: 
            AC += memory[effAddr]; 
            cout << "ADD  // AC = " << "0x" << hex << uppercase<< AC << endl; 
            break;
        case 2: 
            AC = memory[effAddr]; 
            cout << "LDA  // AC = "<< "0x" << hex << uppercase << AC << endl; 
            break;
        case 3: 
            memory[effAddr] = AC; 
            cout << "STA  // M[" << "0x" << hex << uppercase<< effAddr << "] = " << AC << endl; 
            break;
        case 4: 
            PC = effAddr; 
            cout << "BUN  // PC = " << "0x" << hex << uppercase<< PC << endl; 
            break;
        case 5: 
            memory[effAddr] = PC; 
            PC = effAddr + 1; 
            cout << "BSA  // PC = " << "0x" << hex << uppercase<< PC << endl; 
            break;
        case 6:
            memory[effAddr]++;
            if (memory[effAddr] == 0)
            {
                PC++;
            } 
            cout << "ISZ  // M[" << "0x" << hex << uppercase<< effAddr << "] = " << "0x" << hex << uppercase<< memory[effAddr] << ", PC = " << "0x" << hex << uppercase<< PC << endl;
            break;
        default: 
            cout << "Unknown Memory Instruction\n"; 
            break;
    }
}

void executeInstruction(string hex) 
{
    int instr = stoi(hex, nullptr, 16);
    if ((instr & 0xF000) == 0x7000)
    {
        executeRegisterInstruction(instr);
    }
    else if ((instr & 0xF000) == 0xF000)
    {
        executeIOInstruction(instr);
    }   
    else
    {
        executeMemoryInstruction(instr);
    }
}

void readFromFile(const string& filename) 
{
    ifstream file(filename);
    string line;
    while (getline(file, line) && !halt) 
    {
        if (line.empty())
        {
            continue;
        }
        executeInstruction(line);
    }
}

void readHexFromScreen() 
{
    string input;
    while (!halt) 
    {
        cout << "Enter HEX instruction (or 'done'): ";
        cin >> input;
        if (input == "done")
        {
            break;
        }
        executeInstruction(input);
    }
}

void readMnemonicFromScreen() 
{
    unordered_map<string, int> mnemonicHex = {
        // Register
        {"CLA", 0x7800}, {"CLE", 0x7400}, {"CMA", 0x7200}, {"CME", 0x7100},
        {"CIR", 0x7080}, {"CIL", 0x7040}, {"INC", 0x7020}, {"SPA", 0x7010},
        {"SNA", 0x7008}, {"SZA", 0x7004}, {"SZE", 0x7002}, {"HLT", 0x7001},
        // I/O
        {"INP", 0xF800}, {"OUT", 0xF400}, {"SKI", 0xF200}, {"SKO", 0xF100},
        {"ION", 0xF080}, {"IOF", 0xF040},
        // Memory (Direct = 0xxx; Indirect = 8xxx, etc.)
        {"AND", 0x0000}, {"ADD", 0x1000}, {"LDA", 0x2000}, {"STA", 0x3000},
        {"BUN", 0x4000}, {"BSA", 0x5000}, {"ISZ", 0x6000},
        {"IAND", 0x8000}, {"IADD", 0x9000}, {"ILDA", 0xA000}, {"ISTA", 0xB000},
        {"IBUN", 0xC000}, {"IBSA", 0xD000}, {"IISZ", 0xE000}
    };

    cin.ignore();
    while (!halt) 
    {
        cout << "Enter instruction (exammle: LDA 02F): ";
        string mnemonic, operand;
        getline(cin, operand);

        if (operand == "done")
        {
            break;
        } 
            
        istringstream ss(operand);
        ss >> mnemonic >> operand;

        if (mnemonicHex.count(mnemonic)) 
        {
            int base = mnemonicHex[mnemonic];
            if (base < 0x7000) 
            {
                int address = stoi(operand, nullptr, 16);
                base += address;
            }
            stringstream hexStream;
            hexStream << hex << uppercase << base;
            executeInstruction(hexStream.str());
        } 
        else 
        {
            cout << "Unknown mnemonic.\n";
        }
    }
}

int main() 
{
    cout << "===== Basic Computer Interpreter =====\n";

    cout << "\n[1] Reading from file...\n";
    readFromFile("instructions.txt");

    cout << "\n[2] Enter HEX instructions from screen...\n";
    readHexFromScreen();

    cout << "\n[3] Enter Assembly Mnemonics from screen...\n";
    readMnemonicFromScreen();

    cout << "\nExecution completed.\n";
    return 0;
}
