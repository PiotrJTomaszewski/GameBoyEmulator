#include <cstring>
#include <cstdio>
#include "disassembler.h"

Disassembler::Disassembler() {

}

Disassembler::~Disassembler() {

}

void Disassembler::disassemble_code(ReadWriteInterface &src, uint16_t start_addr, uint16_t end_addr) {
    uint16_t address = start_addr;
    while (address < end_addr) {
        disassembled_t tmp_dis;
        tmp_dis.address = address;
        address += disassemble_instr(src, address, tmp_dis.text);
        disassembled.push_back(tmp_dis);
    }
}

std::vector<Disassembler::disassembled_t> &Disassembler::get_disassembled_code() {
    return disassembled;
}

int Disassembler::disassemble_instr(ReadWriteInterface &src, uint16_t mem_addr, char *buffer) {
    int op;
    int instr_len = 1;
    switch (src.read(mem_addr)) {
        case 0x00: // NOP; 1 byte; 4 cycles NOP
            strcpy(buffer, "NOP");
            break;
        case 0x01: // LD BC,nn; 3 bytes; 12 cycles
            sprintf(buffer, "LD BC,0x%02X%02X", src.read(mem_addr+2), src.read(mem_addr+1));
            instr_len = 3;
            break;
        case 0x02: // LD (BC),A; 1 byte; 8 cycles
            strcpy(buffer, "LD (BC),A");
            break;
        case 0x03: // INC BC; 1 byte; 8 cycles
            strcpy(buffer, "INC BC");
            break;
        case 0x04: // INC B; 1 byte; 4 cycles; Z,N,H flags
            strcpy(buffer, "INC B");
            break;
        case 0x05: // DEC B; 1 byte; 4 cycles; Z,N,H flags
            strcpy(buffer, "DEC B");
            break;
        case 0x06: // LD B,n; 2 bytes; 8 cycles
            sprintf(buffer, "LD B,0x%02X", src.read(mem_addr+1));
            instr_len = 2;
            break;
        case 0x07: // RLCA; 1 byte; 4 cycles; Z,N,H,C flags
            strcpy(buffer, "RLCA");
            break;
        case 0x08: // LD (nn),SP; 3 bytes; 20 cycles
            sprintf(buffer, "LD (0x%02X%02X),SP", src.read(mem_addr+2), src.read(mem_addr+1));
            instr_len = 3;
            break;
        case 0x09: // ADD HL,BC; 1 byte; 8 cycles; N,H,C flags
            strcpy(buffer, "ADD HL,BC");
            break;
        case 0x0A: // LD A,(BC); 1 byte; 8 cycles
            strcpy(buffer, "LD A,(BC)");
            break;
        case 0x0B: // DEC BC; 1 byte; 8 cycles
            strcpy(buffer, "DEC BC");
            break;
        case 0x0C: // INC C; 1 byte; 4 cycles; Z,N,H flags
            strcpy(buffer, "INC C");
            break;
        case 0x0D: // DEC C; 1 byte; 4 cycles; Z,N,H flags
            strcpy(buffer, "DEC C");
            break;
        case 0x0E: // LD C,n; 2 bytes; 8 cycles
            sprintf(buffer, "LD C,0x%02X", src.read(mem_addr+1));
            instr_len = 2;
            break;
        case 0x0F: // RRCA; 1 byte; 4 cycles; Z,N,H,C flags
            strcpy(buffer, "RRCA");
            break;
        case 0x10: // Extended instructions 0x00: STOP; 2 bytes; 4 cycles
            strcpy(buffer, "STOP");
            instr_len = 2;
            break;
        case 0x11: // LD DE,nn; 3 bytes; 12 cycles
            sprintf(buffer, "LD DE,0x%02X%02X", src.read(mem_addr+2), src.read(mem_addr+1));
            instr_len = 3;
            break;
        case 0x12: // LD (DE),A; 1 byte; 8 cycles
            strcpy(buffer, "LD (DE),A");
            break;
        case 0x13: // INC DE; 1 byte; 8 cycles
            strcpy(buffer, "INC DE");
            break;
        case 0x14: // INC D; 1 byte; 4 cycles; Z,N,H flags
            strcpy(buffer, "INC D");
            break;
        case 0x15: // DEC D; 1 byte; 4 cycles; Z,N,H flags
            strcpy(buffer, "DEC D");
            break;
        case 0x16: // LD D,n; 2 bytes; 8 cycles
            sprintf(buffer, "LD D,0x%02X", src.read(mem_addr+1));
            instr_len = 2;
            break;
        case 0x17: // RLA; 1 byte; 4 cycles; Z,N,H,C flags
            strcpy(buffer, "RLA");
            break;
        case 0x18: // JR n; 2 bytes; 8 cycles
            sprintf(buffer, "JR 0x%02X", src.read(mem_addr+1));
            instr_len = 2;
            break;
        case 0x19: // ADD HL,DE; 1 byte; 8 cycles; N,H,C flags
            strcpy(buffer, "ADD HL,DE");
            break;
        case 0x1A: // LD A,(DE); 1 byte; 8 cycles
            strcpy(buffer, "LD A,(DE)");
            break;
        case 0x1B: // DEC DE; 1 byte; 8 cycles
            strcpy(buffer, "DEC DE");
            break;
        case 0x1C: // INC E; 1 byte; 4 cycles; Z,N,H flags
            strcpy(buffer, "INC E");
            break;
        case 0x1D: // DEC E; 1 byte; 4 cycles; Z,N,H flags
            strcpy(buffer, "DEC E");
            break;
        case 0x1E: // LD E,n; 2 bytes; 8 cycles
            sprintf(buffer, "LD E,0x%02X", src.read(mem_addr+1));
            instr_len = 2;
            break;
        case 0x1F: // RRA; 1 byte; 4 cycles; Z,N,H,C flags
            strcpy(buffer, "RRA");
            break;
        case 0x20: // JR NZ,n; 2 bytes; 8 cycles
            sprintf(buffer, "JR NZ,0x%02X", src.read(mem_addr+1));
            instr_len = 2;
            break;
        case 0x21: // LD HL,nn; 3 bytes; 12 cycles
            sprintf(buffer, "LD HL,0x%02X%02X", src.read(mem_addr+2), src.read(mem_addr+1));
            instr_len = 3;
            break;
        case 0x22: // LD (HL+),A; 1 byte; 8 cycles
            strcpy(buffer, "LD (HL+),A");
            break;
        case 0x23: // INC HL; 1 byte; 8 cycles
            strcpy(buffer, "INC HL");
            break;
        case 0x24: // INC H; 1 byte; 4 cycles; Z,N,H flags
            strcpy(buffer, "INC H");
            break;
        case 0x25: // DEC H; 1 byte; 4 cycles; Z,N,H flags
            strcpy(buffer, "DEC H");
            break;
        case 0x26: // LD H,n; 2 bytes; 8 cycles
            sprintf(buffer, "LD H,0x%02X", src.read(mem_addr+1));
            instr_len = 2;
            break;
        case 0x27: // ???
            break;
        case 0x28: // JR Z,n; 2 bytes; 8 cycles
            sprintf(buffer, "JR Z,0x%02X", src.read(mem_addr+1));
            instr_len = 2;
            break;
        case 0x29: // ADD HL,HL; 1 byte; 8 cycles; N,H,C flags
            strcpy(buffer, "ADD HL,HL");
            break;
        case 0x2A: // LD A,(HL+); 1 byte; 8 cycles
            strcpy(buffer, "LD A,(HL+)");
            break;
        case 0x2B: // DEC HL; 1 byte; 8 cycles
            strcpy(buffer, "DEC HL");
            break;
        case 0x2C: // INC L; 1 byte; 4 cycles; Z,N,H flags
            strcpy(buffer, "INC L");
            break;
        case 0x2D: // DEC L; 1 byte; 4 cycles; Z,N,H flags
            strcpy(buffer, "DEC L");
            break;
        case 0x2E: // LD L,n; 2 bytes; 8 cycles
            sprintf(buffer, "LD L,0x%02X", src.read(mem_addr+1));
            instr_len = 2;
            break;
        case 0x2F: // CPL; 1 byte; 4 cycles; N,H flags
            strcpy(buffer, "CPL");
            break;
        case 0x30: // JR NC,n; 2 bytes; 8 cycles
            sprintf(buffer, "JR NC,0x%02X", src.read(mem_addr+1));
            instr_len = 2;
            break;
        case 0x31: // LD SP,nn; 3 bytes; 12 cycles
            sprintf(buffer, "LD SP,0x%02X%02X", src.read(mem_addr+2), src.read(mem_addr+1));
            instr_len = 3;
            break;
        case 0x32: // LD (HL-),A; 1 byte; 8 cycles
            strcpy(buffer, "LD (HL-)");
            break;
        case 0x33: // INC SP; 1 byte; 8 cycles
            strcpy(buffer, "INC SP");
            break;
        case 0x34: // INC (HL); 1 byte; 12 cycles; Z,N,H flags
            strcpy(buffer, "INC (HL)");
            break;
        case 0x35: // DEC (HL); 1 byte; 12 cycles; Z,N,H flags
            strcpy(buffer, "DEC (HL)");
            break;
        case 0x36: // LD (HL),n; 2 bytes; 12 cycles
            sprintf(buffer, "LD (HL),0x%02X", src.read(mem_addr+1));
            instr_len = 2;
            break;
        case 0x37: // STC; 1 byte; 4 cycles; N,H,C flag
            strcpy(buffer, "STC");
            break;
        case 0x38: // JR C,n; 2 bytes; 8 cycles
            sprintf(buffer, "JR C,0x%02X", src.read(mem_addr+1));
            instr_len = 2;
            break;
        case 0x39: // ADD HL,SP; 1 byte; 8 cycles; N,H,C flags
            strcpy(buffer, "ADD HL,SP");
            break;
        case 0x3A: // LD A,(HL-); 1 byte; 8 cycles
            strcpy(buffer, "LD A,(HL-)");
            break;
        case 0x3B: // DEC SP; 1 byte; 8 cycles
            strcpy(buffer, "DEC SP");
            break;
        case 0x3C: // INC A; 1 byte; 4 cycles; Z,N,H flags
            strcpy(buffer, "INC A");
            break;
        case 0x3D: // DEC A; 1 byte; 4 cycles; Z,N,H flags
            strcpy(buffer, "DEC A");
            break;
        case 0x3E: // LD A,n; 2 bytes; 8 cycles
            sprintf(buffer, "LD A,0x%02X", src.read(mem_addr+1));
            instr_len = 2;
            break;
        case 0x3F: // CCF; 1 byte; 4 cycles; N, H, C flags
            strcpy(buffer, "CCF");
            break;
        case 0x40: // LD B,B; 1 byte; 4 cycles
            strcpy(buffer, "LD B,B");
            break;
        case 0x41: // LD B,C; 1 byte; 4 cycles
            strcpy(buffer, "LD B,C");
            break;
        case 0x42: // LD B,D; 1 byte; 4 cycles
            strcpy(buffer, "LD B,D");
            break;
        case 0x43: // LD B,E; 1 byte; 4 cycles
            strcpy(buffer, "LD B,E");
            break;
        case 0x44: // LD B,H; 1 byte; 4 cycles
            strcpy(buffer, "LD B,H");
            break;
        case 0x45: // LD B,L; 1 byte; 4 cycles
            strcpy(buffer, "LD B,L");
            break;
        case 0x46: // LD B,(HL); 1 byte; 8 cycles
            strcpy(buffer, "LD B,(HL)");
            break;
        case 0x47: // LD B,A; 1 byte; 4 cycles
            strcpy(buffer, "LD B,A");
            break;
        case 0x48: // LD C,B; 1 byte; 4 cycles
            strcpy(buffer, "LD C,B");
            break;
        case 0x49: // LD C,C; 1 byte; 4 cycles
            strcpy(buffer, "LD C,C");
            break;
        case 0x4A: // LD C,D; 1 byte; 4 cycles
            strcpy(buffer, "LD C,D");
            break;
        case 0x4B: // LD C,E; 1 byte; 4 cycles
            strcpy(buffer, "LD C,E");
            break;
        case 0x4C: // LD C,H; 1 byte; 4 cycles
            strcpy(buffer, "LD C,H");
            break;
        case 0x4D: // LD C,L; 1 byte; 4 cycles
            strcpy(buffer, "LD C,L");
            break;
        case 0x4E: // LD C,(HL); 1 byte; 8 cycles
            strcpy(buffer, "LD C,(HL)");
            break;
        case 0x4F: // LD C,A; 1 byte; 4 cycles
            strcpy(buffer, "LD C,A");
            break;
        case 0x50: // LD D,B; 1 byte; 4 cycles
            strcpy(buffer, "LD D,B");
            break;
        case 0x51: // LD D,C; 1 byte; 4 cycles
            strcpy(buffer, "LD D,C");
            break;
        case 0x52: // LD D,D; 1 byte; 4 cycles
            strcpy(buffer, "LD D,D");
            break;
        case 0x53: // LD D,E; 1 byte; 4 cycles
            strcpy(buffer, "LD D,E");
            break;
        case 0x54: // LD D,H; 1 byte; 4 cycles
            strcpy(buffer, "LD D,H");
            break;
        case 0x55: // LD D,L; 1 byte; 4 cycles
            strcpy(buffer, "LD D,L");
            break;
        case 0x56: // LD D,(HL); 1 byte; 8 cycles
            strcpy(buffer, "LD D,(HL)");
            break;
        case 0x57: // LD D,A; 1 byte; 4 cycles
            strcpy(buffer, "LD D,A");
            break;
        case 0x58: // LD E,B; 1 byte; 4 cycles
            strcpy(buffer, "LD E,B");
            break;
        case 0x59: // LD E,C; 1 byte; 4 cycles
            strcpy(buffer, "LD E,C");
            break;
        case 0x5A: // LD E,D; 1 byte; 4 cycles
            strcpy(buffer, "LD E,D");
            break;
        case 0x5B: // LD E,E; 1 byte; 4 cycles
            strcpy(buffer, "LD E,E");
            break;
        case 0x5C: // LD E,H; 1 byte; 4 cycles
            strcpy(buffer, "LD E,H");
            break;
        case 0x5D: // LD E,L; 1 byte; 4 cycles
            strcpy(buffer, "LD E,L");
            break;
        case 0x5E: // LD E,(HL); 1 byte; 8 cycles
            strcpy(buffer, "LD E,(HL)");
            break;
        case 0x5F: // LD E,A; 1 byte; 4 cycles
            strcpy(buffer, "LD E,A");
            break;
        case 0x60: // LD H,B; 1 byte; 4 cycles
            strcpy(buffer, "LD H,B");
            break;
        case 0x61: // LD H,C; 1 byte; 4 cycles
            strcpy(buffer, "LD H,C");
            break;
        case 0x62: // LD H,D; 1 byte; 4 cycles
            strcpy(buffer, "LD H,D");
            break;
        case 0x63: // LD H,E; 1 byte; 4 cycles
            strcpy(buffer, "LD H,E");
            break;
        case 0x64: // LD H,H; 1 byte; 4 cycles
            strcpy(buffer, "LD H,H");
            break;
        case 0x65: // LD H,L; 1 byte; 4 cycles
            strcpy(buffer, "LD H,L");
            break;
        case 0x66: // LD H,(HL); 1 byte; 8 cycles
            strcpy(buffer, "LD H,(HL)");
            break;
        case 0x67: // LD H,A; 1 byte; 4 cycles
            strcpy(buffer, "LD H,A");
            break;
        case 0x68: // LD L,B; 1 byte; 4 cycles
            strcpy(buffer, "LD L,B");
            break;
        case 0x69: // LD L,C; 1 byte; 4 cycles
            strcpy(buffer, "LD L,C");
            break;
        case 0x6A: // LD L,D; 1 byte; 4 cycles
            strcpy(buffer, "LD L,D");
            break;
        case 0x6B: // LD L,E; 1 byte; 4 cycles
            strcpy(buffer, "LD L,E");
            break;
        case 0x6C: // LD L,H; 1 byte; 4 cycles
            strcpy(buffer, "LD L,H");
            break;
        case 0x6D: // LD L,L; 1 byte; 4 cycles
            strcpy(buffer, "LD L,L");
            break;
        case 0x6E: // LD L,(HL); 1 byte; 8 cycles
            strcpy(buffer, "LD L,(HL)");
            break;
        case 0x6F: // LD L,A; 1 byte; 4 cycles
            strcpy(buffer, "LD L,A");
            break;
        case 0x70: // LD (HL),B; 1 byte; 8 cycles
            strcpy(buffer, "LD (HL),B");
            break;
        case 0x71: // LD (HL),C; 1 byte; 8 cycles
            strcpy(buffer, "LD (HL),C");
            break;
        case 0x72: // LD (HL),D; 1 byte; 8 cycles
            strcpy(buffer, "LD (HL),D");
            break;
        case 0x73: // LD (HL),E; 1 byte; 8 cycles
            strcpy(buffer, "LD (HL),E");
            break;
        case 0x74: // LD (HL),H; 1 byte; 8 cycles
            strcpy(buffer, "LD (HL),H");
            break;
        case 0x75: // LD (HL),L; 1 byte; 8 cycles
            strcpy(buffer, "LD (HL),L");
            break;
        case 0x76: // HALT; 1 byte; 4 cycles
            strcpy(buffer, "HALT");
            break;
        case 0x77: // LD (HL),A; 1 byte; 8 cycles
            strcpy(buffer, "LD (HL),A");
            break;
        case 0x78: // LD A,B; 1 byte; 4 cycles
            strcpy(buffer, "LD A,B");
            break;
        case 0x79: // LD A,C; 1 byte; 4 cycles
            strcpy(buffer, "LD A,C");
            break;
        case 0x7A: // LD A,D; 1 byte; 4 cycles
            strcpy(buffer, "LD A,D");
            break;
        case 0x7B: // LD A,E; 1 byte; 4 cycles
            strcpy(buffer, "LD A,E");
            break;
        case 0x7C: // LD A,H; 1 byte; 4 cycles
            strcpy(buffer, "LD A,H");
            break;
        case 0x7D: // LD A,L; 1 byte; 4 cycles
            strcpy(buffer, "LD A,L");
            break;
        case 0x7E: // LD A,(HL); 1 byte; 8 cycles
            strcpy(buffer, "LD A,(HL)");
            break;
        case 0x7F: // LD A,A; 1 byte; 4 cycles
            strcpy(buffer, "LD A,A");
            break;
        case 0x80: // ADD A,B; 1 byte; 4 cycles; Z,N,H,C flags
            strcpy(buffer, "ADD A,B");
            break;
        case 0x81: // ADD A,C; 1 byte; 4 cycles; Z,N,H,C flags
            strcpy(buffer, "ADD A,C");
            break;
        case 0x82: // ADD A,D; 1 byte; 4 cycles; Z,N,H,C flags
            strcpy(buffer, "ADD A,D");
            break;
        case 0x83: // ADD A,E; 1 byte; 4 cycles; Z,N,H,C flags
            strcpy(buffer, "ADD A,E");
            break;
        case 0x84: // ADD A,H; 1 byte; 4 cycles; Z,N,H,C flags
            strcpy(buffer, "ADD A,H");
            break;
        case 0x85: // ADD A,L; 1 byte; 4 cycles; Z,N,H,C flags
            strcpy(buffer, "ADD A,L");
            break;
        case 0x86: // ADD A,(HL); 1 byte; 8 cycles; Z,N,H,C flags
            strcpy(buffer, "ADD A,(HL)");
            break;
        case 0x87: // ADD A,A; 1 byte; 4 cycles; Z,N,H,C flags
            strcpy(buffer, "ADD A,A");
            break;
        case 0x88: // ADC A,B; 1 byte; 4 cycles; Z,N,H,C flags
            strcpy(buffer, "ADC A,B");
            break;
        case 0x89: // ADC A.C; 1 byte; 4 cycles; Z,N,H,C flags
            strcpy(buffer, "ADC A,C");
            break;
        case 0x8A: // ADC A,D; 1 byte; 4 cycles; Z,N,H,C flags
            strcpy(buffer, "ADC A,D");
            break;
        case 0x8B: // ADC A,E; 1 byte; 4 cycles; Z,N,H,C flags
            strcpy(buffer, "ADC A,E");
            break;
        case 0x8C: // ADC A,H; 1 byte; 4 cycles; Z,N,H,C flags
            strcpy(buffer, "ADC A,H");
            break;
        case 0x8D: // ADC A,L; 1 byte; 4 cycles; Z,N,H,C flags
            strcpy(buffer, "ADC A,L");
            break;
        case 0x8E: // ADC A,(HL); 1 byte; 8 cycles; Z,N,H,C flags
            strcpy(buffer, "ADC A,(HL)");
            break;
        case 0x8F: // ADC A,A; 1 byte; 4 cycles; Z,N,H,C flags
            strcpy(buffer, "ADC A,A");
            break;
        case 0x90: // SUB B; 1 byte; 4 cycles; Z,N,H,C flags
            strcpy(buffer, "SUB B");
            break;
        case 0x91: // SUB C; 1 byte; 4 cycles; Z,N,H,C flags
            strcpy(buffer, "SUB C");
            break;
        case 0x92: // SUB D; 1 byte; 4 cycles; Z,N,H,C flags
            strcpy(buffer, "SUB D");
            break;
        case 0x93: // SUB E; 1 byte; 4 cycles; Z,N,H,C flags
            strcpy(buffer, "SUB E");
            break;
        case 0x94: // SUB H; 1 byte; 4 cycles; Z,N,H,C flags
            strcpy(buffer, "SUB H");
            break;
        case 0x95: // SUB L; 1 byte; 4 cycles; Z,N,H,C flags
            strcpy(buffer, "SUB L");
            break;
        case 0x96: // SUB (HL); 1 byte; 8 cycles; Z,N,H,C flags
            strcpy(buffer, "SUB (HL)");
            break;
        case 0x97: // SUB A; 1 byte; 4 cycles; Z,N,H,C flags
            strcpy(buffer, "SUB A");
            break;
        case 0x98: // SBC A,B; 1 byte; 4 cycles; Z,N,H,C flags
            strcpy(buffer, "SBC A,B");
            break;
        case 0x99: // SBC A,C; 1 byte; 4 cycles; Z,N,H,C flags
            strcpy(buffer, "ABC A,C");
            break;
        case 0x9A: // SBC A,D; 1 byte; 4 cycles; Z,N,H,C flags
            strcpy(buffer, "SBC A,D");
            break;
        case 0x9B: // SBC A,E; 1 byte; 4 cycles; Z,N,H,C flags
            strcpy(buffer, "SBC A,E");
            break;
        case 0x9C: // SBC A,H; 1 byte; 4 cycles; Z,N,H,C flags
            strcpy(buffer, "SBC A,H");
            break;
        case 0x9D: // SBC A,L; 1 byte; 4 cycles; Z,N,H,C flags
            strcpy(buffer, "SBC A,L");
            break;
        case 0x9E: // SBC A,(HL); 1 byte; 8 cycles; Z,N,H,C flags
            strcpy(buffer, "SBC A,(HL)");
            break;
        case 0x9F: // SBC A,A; 1 byte; 4 cycles; Z,N,H,C flags
            strcpy(buffer, "SBC A,A");
            break;
        case 0xA0: // AND B; 1 byte; 4 cycles; Z,N,H,C flags
            strcpy(buffer, "AND B");
            break;
        case 0xA1: // AND C; 1 byte; 4 cycles; Z,N,H,C flags
            strcpy(buffer, "AND C");
            break;
        case 0xA2: // AND D; 1 byte; 4 cycles; Z,N,H,C flags
            strcpy(buffer, "AND D");
            break;
        case 0xA3: // AND E; 1 byte; 4 cycles; Z,N,H,C flags
            strcpy(buffer, "AND E");
            break;
        case 0xA4: // AND H; 1 byte; 4 cycles; Z,N,H,C flags
            strcpy(buffer, "AND H");
            break;
        case 0xA5: // AND L; 1 byte; 4 cycles; Z,N,H,C flags
            strcpy(buffer, "AND L");
            break;
        case 0xA6: // AND (HL); 1 byte; 8 cycles; Z,N,H,C flags
            strcpy(buffer, "AND (HL)");
            break;
        case 0xA7: // AND A; 1 byte; 4 cycles; Z,N,H,C flags
            strcpy(buffer, "AND A");
            break;
        case 0xA8: // XOR B; 1 byte; 4 cycles; Z,N,H,C flags
            strcpy(buffer, "XOR B");
            break;
        case 0xA9: // XOR C; 1 byte; 4 cycles; Z,N,H,C flags
            strcpy(buffer, "XOR C");
            break;
        case 0xAA: // XOR D; 1 byte; 4 cycles; Z,N,H,C flags
            strcpy(buffer, "XOR D");
            break;
        case 0xAB: // XOR E; 1 byte; 4 cycles; Z,N,H,C flags
            strcpy(buffer, "XOR E");
            break;
        case 0xAC: // XOR H; 1 byte; 4 cycles; Z,N,H,C flags
            strcpy(buffer, "XOR H");
            break;
        case 0xAD: // XOR L; 1 byte; 4 cycles; Z,N,H,C flags
            strcpy(buffer, "XOR L");
            break;
        case 0xAE: // XOR M; 1 byte; 8 cycles; Z,N,H,C flags
            strcpy(buffer, "XOR M");
            break;
        case 0xAF: // XOR A; 1 byte; 4 cycles; Z,N,H,C flags
            strcpy(buffer, "XOR A");
            break;
        case 0xB0: // OR B; 1 byte; 4 cycles; Z,N,H,C flags
            strcpy(buffer, "OR B");
            break;
        case 0xB1: // OR C; 1 byte; 4 cycles; Z,N,H,C flags
            strcpy(buffer, "OR C");
            break;
        case 0xB2: // OR D; 1 byte; 4 cycles; Z,N,H,C flags
            strcpy(buffer, "OR D");
            break;
        case 0xB3: // OR E; 1 byte; 4 cycles; Z,N,H,C flags
            strcpy(buffer, "OR E");
            break;
        case 0xB4: // OR H; 1 byte; 4 cycles; Z,N,H,C flags
            strcpy(buffer, "OR H");
            break;
        case 0xB5: // OR L; 1 byte; 4 cycles; Z,N,H,C flags
            strcpy(buffer, "OR L");
            break;
        case 0xB6: // OR (HL); 1 byte; 8 cycles; Z,N,H,C flags
            strcpy(buffer, "OR (HL)");
            break;
        case 0xB7: // OR A; 1 byte; 4 cycles; Z,N,H,C flags
            strcpy(buffer, "OR A");
            break;
        case 0xB8: // CMP B; 1 byte; 4 cycles; Z,N,H,C flags
            strcpy(buffer, "CMP B");
            break;
        case 0xB9: // CMP C; 1 byte; 4 cycles; Z,N,H,C flags
            strcpy(buffer, "CMP C");
            break;
        case 0xBA: // CMP D; 1 byte; 4 cycles; Z,N,H,C flags
            strcpy(buffer, "CMP D");
            break;
        case 0xBB: // CMP E; 1 byte; 4 cycles; Z,N,H,C flags
            strcpy(buffer, "CMP E");
            break;
        case 0xBC: // CMP H; 1 byte; 4 cycles; Z,N,H,C flags
            strcpy(buffer, "CMP H");
            break;
        case 0xBD: // CMP L; 1 byte; 4 cycles; Z,N,H,C flags
            strcpy(buffer, "CMP L");
            break;
        case 0xBE: // CMP (HL); 1 byte; 8 cycles; Z,N,H,C flags
            strcpy(buffer, "CMP (HL)");
            break;
        case 0xBF: // CMP A; 1 byte; 4 cycles; Z,N,H,C flags
            strcpy(buffer, "CMP A");
            break;
        case 0xC0: // RET NZ; 1 byte; 8 cycles
            strcpy(buffer, "RET NZ");
            break;
        case 0xC1: // POP BC; 1 byte; 12 cycles
            strcpy(buffer, "POP BC");
            break;
        case 0xC2: // JP NZ,adr; 3 bytes; 12 cycles
            sprintf(buffer, "JP NZ,0x%02X%02X", src.read(mem_addr+2), src.read(mem_addr+1));
            instr_len = 3;
            break;
        case 0xC3: // JP adr; 3 bytes; 12 cycles
            sprintf(buffer, "JP 0x%02X%02X", src.read(mem_addr+2), src.read(mem_addr+1));
            instr_len = 3;
            break;
        case 0xC4: // CALL NZ,adr; 3 bytes; 12 cycles
            sprintf(buffer, "CALL NZ,0x%02X%02X", src.read(mem_addr+2), src.read(mem_addr+1));
            instr_len = 3;
            break;
        case 0xC5: // PUSH BC; 1 byte; 16 cycles
            strcpy(buffer, "PUSH BC");
            break;
        case 0xC6: // ADD A,n; 2 bytes; 8 cycles; Z,N,H,C flags
            sprintf(buffer, "ADD A,0x%02X", src.read(mem_addr+1));
            instr_len = 2;
            break;
        case 0xC7: // RST 00H; 1 byte; 32 cycles
            strcpy(buffer, "RST 0x0000");
            break;
        case 0xC8: // RET Z; 1 byte; 8 cycles
            strcpy(buffer, "RET Z");
            break;
        case 0xC9: // RET; 1 byte; 8 cycles
            strcpy(buffer, "RET");
            break;
        case 0xCA: // JP Z,adr; 3 bytes; 12 cycles
            sprintf(buffer, "JP Z,0x%02X%02X", src.read(mem_addr+2), src.read(mem_addr+1));
            instr_len = 3;
            break;
        case 0xCB: // Extended instructions
            op = src.read(mem_addr+1);
            instr_len = 2;
            switch (op) {
                case 0x00: // RLC B; 2 bytes; 8 cycles; Z,N,H,C flags
                    strcpy(buffer, "RLC B");
                    break;
                case 0x01: // RLC C; 2 bytes; 8 cycles; Z,N,H,C flags
                    strcpy(buffer, "RLC C");
                    break;
                case 0x02: // RLC D; 2 bytes; 8 cycles; Z,N,H,C flags
                    strcpy(buffer, "RLC D");
                    break;
                case 0x03: // RLC E; 2 bytes; 8 cycles; Z,N,H,C flags
                    strcpy(buffer, "RLC E");
                    break;
                case 0x04: // RLC H; 2 bytes; 8 cycles; Z,N,H,C flags
                    strcpy(buffer, "RLC H");
                    break;
                case 0x05: // RLC L; 2 bytes; 8 cycles; Z,N,H,C flags
                    strcpy(buffer, "RLC L");
                    break;
                case 0x06: // RLC (HL); 2 bytes; 16 cycles; Z,N,H,C flags
                    strcpy(buffer, "RLC (HL)");
                    break;
                case 0x07: // RLC A; 2 bytes; 8 cycles; Z,N,H,C flags
                    strcpy(buffer, "RLC A");
                    break;
                case 0x08: // RRC B; 2 bytes; 8 cycles; Z,N,H,C flags
                    strcpy(buffer, "RRC B");
                    break;
                case 0x09: // RRC C; 2 bytes; 8 cycles; Z,N,H,C flags
                    strcpy(buffer, "RRC C");
                    break;
                case 0x0A: // RRC D; 2 bytes; 8 cycles; Z,N,H,C flags
                    strcpy(buffer, "RRC D");
                    break;
                case 0x0B: // RRC E; 2 bytes; 8 cycles; Z,N,H,C flags
                    strcpy(buffer, "RRC E");
                    break;
                case 0x0C: // RRC H; 2 bytes; 8 cycles; Z,N,H,C flags
                    strcpy(buffer, "RRC H");
                    break;
                case 0x0D: // RRC L; 2 bytes; 8 cycles; Z,N,H,C flags
                    strcpy(buffer, "RRC L");
                    break;
                case 0x0E: // RRC (HL); 2 bytes; 16 cycles; Z,N,H,C flags
                    strcpy(buffer, "RRC (HL)");
                    break;
                case 0x0F: // RRC A; 2 bytes; 8 cycles; Z,N,H,C flags
                    strcpy(buffer, "RRC A");
                    break;
                case 0x10: // RL B; 2 bytes; 8 cycles; Z,N,H,C flags
                    strcpy(buffer, "RL B");
                    break;
                case 0x11: // RL C; 2 bytes; 8 cycles; Z,N,H,C flags
                    strcpy(buffer, "RL C");
                    break;
                case 0x12: // RL D; 2 bytes; 8 cycles; Z,N,H,C flags
                    strcpy(buffer, "RL D");
                    break;
                case 0x13: // RL E; 2 bytes; 8 cycles; Z,N,H,C flags
                    strcpy(buffer, "RL E");
                    break;
                case 0x14: // RL H; 2 bytes; 8 cycles; Z,N,H,C flags
                    strcpy(buffer, "RL H");
                    break;
                case 0x15: // RL L; 2 bytes; 8 cycles; Z,N,H,C flags
                    strcpy(buffer, "RL L");
                    break;
                case 0x16: // RL (HL); 2 bytes; 16 cycles; Z,N,H,C flags
                    strcpy(buffer, "RL (HL)");
                    break;
                case 0x17: // RL A; 2 bytes; 8 cycles; Z,N,H,C flags
                    strcpy(buffer, "RL A");
                    break;
                case 0x18: // RR B; 2 bytes; 8 cycles; Z,N,H,C flags
                    strcpy(buffer, "RR B");
                    break;
                case 0x19: // RR C; 2 bytes; 8 cycles; Z,N,H,C flags
                    strcpy(buffer, "RR C");
                    break;
                case 0x1A: // RR D; 2 bytes; 8 cycles; Z,N,H,C flags
                    strcpy(buffer, "RR D");
                    break;
                case 0x1B: // RR E; 2 bytes; 8 cycles; Z,N,H,C flags
                    strcpy(buffer, "RR E");
                    break;
                case 0x1C: // RR H; 2 bytes; 8 cycles; Z,N,H,C flags
                    strcpy(buffer, "RR H");
                    break;
                case 0x1D: // RR L; 2 bytes; 8 cycles; Z,N,H,C flags
                    strcpy(buffer, "RR L");
                    break;
                case 0x1E: // RR (HL); 2 bytes; 16 cycles; Z,N,H,C flags
                    strcpy(buffer, "RR (HL)");
                    break;
                case 0x1F: // RR A; 2 bytes; 8 cycles; Z,N,H,C flags
                    strcpy(buffer, "RR A");
                    break;
                case 0x20: // SLA B; 2 bytes; 8 cycles; Z,N,H,C flags
                    strcpy(buffer, "SLA B");
                    break;
                case 0x21: // SLA C; 2 bytes; 8 cycles; Z,N,H,C flags
                    strcpy(buffer, "SLA C");
                    break;
                case 0x22: // SLA D; 2 bytes; 8 cycles; Z,N,H,C flags
                    strcpy(buffer, "SLA D");
                    break;
                case 0x23: // SLA E; 2 bytes; 8 cycles; Z,N,H,C flags
                    strcpy(buffer, "SLA E");
                    break;
                case 0x24: // SLA H; 2 bytes; 8 cycles; Z,N,H,C flags
                    strcpy(buffer, "SLA H");
                    break;
                case 0x25: // SLA L; 2 bytes; 8 cycles; Z,N,H,C flags
                    strcpy(buffer, "SLA L");
                    break;
                case 0x26: // SLA (HL); 2 bytes; 16 cycles; Z,N,H,C flags
                    strcpy(buffer, "SLA (HL)");
                    break;
                case 0x27: // SLA A; 2 bytes; 8 cycles; Z,N,H,C flags
                    strcpy(buffer, "SLA A");
                    break;
                case 0x28: // SRA B; 2 bytes; 8 cycles; Z,N,H,C flags
                    strcpy(buffer, "SRA B");
                    break;
                case 0x29: // SRA C; 2 bytes; 8 cycles; Z,N,H,C flags
                    strcpy(buffer, "SRA C");
                    break;
                case 0x2A: // SRA D; 2 bytes; 8 cycles; Z,N,H,C flags
                    strcpy(buffer, "SRA D");
                    break;
                case 0x2B: // SRA E; 2 bytes; 8 cycles; Z,N,H,C flags
                    strcpy(buffer, "SRA E");
                    break;
                case 0x2C: // SRA H; 2 bytes; 8 cycles; Z,N,H,C flags
                    strcpy(buffer, "SRA H");
                    break;
                case 0x2D: // SRA L; 2 bytes; 8 cycles; Z,N,H,C flags
                    strcpy(buffer, "SRA L");
                    break;
                case 0x2E: // SRA (HL); 2 bytes; 16 cycles; Z,N,H,C flags
                    strcpy(buffer, "SRA (HL)");
                    break;
                case 0x2F: // SRA A; 2 bytes; 8 cycles; Z,N,H,C flags
                    strcpy(buffer, "SRA A");
                    break;
                case 0x30: // SWAP B; 2 bytes; 8 cycles; Z,N,H,C flags
                    strcpy(buffer, "SWAP B");
                    break;
                case 0x31: // SWAP C; 2 bytes; 8 cycles; Z,N,H,C flags
                    strcpy(buffer, "SWAP C");
                    break;
                case 0x32: // SWAP D; 2 bytes; 8 cycles; Z,N,H,C flags
                    strcpy(buffer, "SWAP D");
                    break;
                case 0x33: // SWAP E; 2 bytes; 8 cycles; Z,N,H,C flags
                    strcpy(buffer, "SWAP E");
                    break;
                case 0x34: // SWAP H; 2 bytes; 8 cycles; Z,N,H,C flags
                    strcpy(buffer, "SWAP H");
                    break;
                case 0x35: // SWAP L; 2 bytes; 8 cycles; Z,N,H,C flags
                    strcpy(buffer, "SWAP L");
                    break;
                case 0x36: // SWAP (HL); 2 bytes; 16 cycles; Z,N,H,C flags
                    strcpy(buffer, "SWAP (HL)");
                    break;
                case 0x37: // SWAP A; 2 bytes; 8 cycles; Z,N,H,C flags
                    strcpy(buffer, "SWAP A");
                    break;
                case 0x38: // SRL B; 2 bytes; 8 cycles; Z,N,H,C flags
                    strcpy(buffer, "SRL B");
                    break;
                case 0x39: // SRL C; 2 bytes; 8 cycles; Z,N,H,C flags
                    strcpy(buffer, "SRL C");
                    break;
                case 0x3A: // SRL D; 2 bytes; 8 cycles; Z,N,H,C flags
                    strcpy(buffer, "SRL D");
                    break;
                case 0x3B: // SRL E; 2 bytes; 8 cycles; Z,N,H,C flags
                    strcpy(buffer, "SRL E");
                    break;
                case 0x3C: // SRL H; 2 bytes; 8 cycles; Z,N,H,C flags
                    strcpy(buffer, "SRL H");
                    break;
                case 0x3D: // SRL L; 2 bytes; 8 cycles; Z,N,H,C flags
                    strcpy(buffer, "SRL L");
                    break;
                case 0x3E: // SRL (HL); 2 bytes; 16 cycles; Z,N,H,C flags
                    strcpy(buffer, "SRL (HL)");
                    break;
                case 0x3F: // SRL A; 2 bytes; 8 cycles; Z,N,H,C flags
                    strcpy(buffer, "SRL A");
                    break;
                default: // BIT, SET, RES operations // TODO: Cleanup
                {
                    int bit_no = (op & 0b00111000) >> 3;
                    int reg_id = op & 0b00000111;
                    char reg_lookup[] = {'B', 'C', 'D', 'E', 'H', 'L'};
                    switch((op & 0b11000000) >> 6) {
                        case 0x01: // BIT b,r; 2 bytes; 8/16 cycles; Z,N,H flags
                            if (reg_id == 7) {
                                sprintf(buffer, "BIT %d,A", bit_no);
                            } else if (reg_id == 6) {
                                sprintf(buffer, "BIT %d,(HL)", bit_no);
                            } else {
                                sprintf(buffer, "BIT %d,%c", bit_no, reg_lookup[reg_id]);
                            }
                            break;
                        case 0x10: // RES b,r; 2 bytes; 8/16 cycles
                            if (reg_id == 7) {
                                sprintf(buffer, "RES %d,A", bit_no);
                            } else if (reg_id == 6) {
                                sprintf(buffer, "RES %d,(HL)", bit_no);
                            } else {
                                sprintf(buffer, "RES %d,%c", bit_no, reg_lookup[reg_id]);
                            }
                            break;
                        case 0x11: // SET b,r; 2 bytes; 8/16 cycles
                            if (reg_id == 7) {
                                sprintf(buffer, "SET %d,A", bit_no);
                            } else if (reg_id == 6) {
                                sprintf(buffer, "SET %d,(HL)", bit_no);
                            } else {
                                sprintf(buffer, "SET %d,%c", bit_no, reg_lookup[reg_id]);
                            }
                            break;
                    }
                }
            }
            break;
        case 0xCC: // CALL Z,adr; 3 bytes; 12 cycles
            sprintf(buffer, "CALL Z,0x%02X%02X", src.read(mem_addr+2), src.read(mem_addr+1));
            instr_len = 3;
            break;
        case 0xCD: // CALL adr; 3 bytes; 12 cycles
            sprintf(buffer, "CALL 0x%02X%02X", src.read(mem_addr+2), src.read(mem_addr+1));
            instr_len = 3;
            break;
        case 0xCE: // ADC A,n; 2 bytes; 8 cycles; Z,N,H,C flags
            sprintf(buffer, "ADC A,0x%02X", src.read(mem_addr+1));
            instr_len = 2;
            break;
        case 0xCF: // RST 08H; 1 byte; 32 cycles
            strcpy(buffer, "RST 0x0008");
            break;
        case 0xD0: // RET NC; 1 byte; 8 cycles
            strcpy(buffer, "RET NC");
            break;
        case 0xD1: // POP DE; 1 byte; 12 cycles
            strcpy(buffer, "POP DE");
            break;
        case 0xD2: // JP NC,adr; 3 bytes; 12 cycles
            sprintf(buffer, "JP NC,0x%02X%02X", src.read(mem_addr+2), src.read(mem_addr+1));
            instr_len = 3;
            break;
        case 0xD3: // ???
            break;
        case 0xD4: // CALL NC,adr; 3 bytes; 12 cycles
            sprintf(buffer, "CALL NC,0x%02X%02X", src.read(mem_addr+2), src.read(mem_addr+1));
            instr_len = 3;
            break;
        case 0xD5: // PUSH DE; 1 byte; 16 cycles
            strcpy(buffer, "PUSH DE");
            break;
        case 0xD6: // SUB n; 2 bytes; 8 cycles; Z,N,H,C flags
            sprintf(buffer, "SUB 0x%02X", src.read(mem_addr+1));
            instr_len = 2;
            break;
        case 0xD7: // RST 10H; 1 byte; 32 cycles
            strcpy(buffer, "RST 0x0010");
            break;
        case 0xD8: // RET C; 1 byte; 8 cycles
            strcpy(buffer, "RET C");
            break;
        case 0xD9: // RETI; 1 byte; 8 cycles
            strcpy(buffer, "RETI");
            break;
        case 0xDA: // JP C,adr; 3 bytes; 12 cycles
            sprintf(buffer, "JP C,0x%02X%02X", src.read(mem_addr+2), src.read(mem_addr+1));
            instr_len = 3;
            break;
        case 0xDB: // ???
            break;
        case 0xDC: // CALL C,adr; 3 bytes; 12 cycles
            sprintf(buffer, "CALL C,0x%02X%02X", src.read(mem_addr+2), src.read(mem_addr+1));
            instr_len = 3;
            break;
        case 0xDD: // ???
            break;
        case 0xDE: // ???
            break;
        case 0xDF: // RST 18H; 1 byte; 32 cycles
            strcpy(buffer, "RST 0x0018");
            break;
        case 0xE0: // LD ($FF00 + n),A; 2 bytes; 12 cycles
            sprintf(buffer, "LD (0xFF00 + 0x%02X),A", src.read(mem_addr+1));
            instr_len = 2;
            break;
        case 0xE1: // POP HL; 1 byte; 12 cycles
            strcpy(buffer, "POP HL");
            break;
        case 0xE2: // LD ($FF00 + C),A; 1 byte; 8 cycles
            sprintf(buffer, "LD (0xFF00 + C),A");
            break;
        case 0xE3: // ???
            break;
        case 0xE4: // ???
            break;
        case 0xE5: // PUSH HL; 1 byte; 16 cycles
            strcpy(buffer, "PUSH HL");
            break;
        case 0xE6: // AND n; 2 bytes; 8 cycles; Z,N,H,C flags
            sprintf(buffer, "AND 0x%02X", src.read(mem_addr+1));
            instr_len = 2;
            break;
        case 0xE7: // RST 20H; 1 byte; 32 cycles
            strcpy(buffer, "RST 0x0020");
            break;
        case 0xE8: // ADD SP,n; 2 bytes; 16 cycles; Z,N,H,C flags
            sprintf(buffer, "ADD SP,0x%02X", src.read(mem_addr+1));
            instr_len = 2;
            break;
        case 0xE9: // JP (HL); 1 byte; 4 cycles
            strcpy(buffer, "JP (HL)");
            break;
        case 0xEA: // LD (nn),A; 3 bytes; 16 cycles
            sprintf(buffer, "LD (0x%02X%02X),A", src.read(mem_addr+2), src.read(mem_addr+1));
            instr_len = 3;
            break;
        case 0xEB: // ???
            break;
        case 0xEC: // ???
            break;
        case 0xED: // ???
            break;
        case 0xEE: // XOR n; 2 bytes; 8 cycles; Z,N,H,C flags
            sprintf(buffer, "XOR 0x%02X", src.read(mem_addr+1));
            instr_len = 2;
            break;
        case 0xEF: // RST 28H; 1 byte; 32 cycles
            strcpy(buffer, "RST 0x0028");
            break;
        case 0xF0: // LD A,($FF00 + n); 2 bytes; 12 cycles
            sprintf(buffer, "LD A,(0xFF00 + 0x%02X)", src.read(mem_addr+1));
            instr_len = 2;
            break;
        case 0xF1: // POP AF; 1 byte; 12 cycles
            strcpy(buffer, "POP AF");
            break;
        case 0xF2: // LD A,($FF00 + C); 1 byte; 8 cycles
            strcpy(buffer, "LD A,(0xFF00 + C)");
            break;
        case 0xF3: // DI; 1 byte; 4 cycles
            strcpy(buffer, "DI");
            break;
        case 0xF4: // ???
            break;
        case 0xF5: // PUSH AF; 1 byte; 16 cycles
            strcpy(buffer, "PUSH AF");
            break;
        case 0xF6: // OR n; 2 bytes; 8 cycles; Z,N,H,C flags
            sprintf(buffer, "OR 0x%02X", src.read(mem_addr+1));
            instr_len = 2;
            break;
        case 0xF7: // RST 30H; 1 byte; 32 cycles
            strcpy(buffer, "RST 0x0030");
            break;
        case 0xF8: // LDHL SP+n; 2 bytes; 12 cycles; Z,N,H,C flags
            sprintf(buffer, "LDHL SP+0x%02X", src.read(mem_addr+1));
            instr_len = 2;
            break;
        case 0xF9: // LD SP,HL; 1 byte; 8 cycles
            strcpy(buffer, "LD SP,HL");
            break;
        case 0xFA: // LD A,(nn); 3 bytes; 16 cycles
            sprintf(buffer, "LD A,(0x%02X%02X)", src.read(mem_addr+2), src.read(mem_addr+1));
            instr_len = 3;
            break;
        case 0xFB: // EI; 1 byte; 4 cycles
            strcpy(buffer, "EI");
            break;
        case 0xFC: // ???
            break;
        case 0xFD: // ???
            break;
        case 0xFE: // CMP n; 2 bytes; 8 cycles; Z,N,H,C flags
            sprintf(buffer, "CMP 0x%02X", src.read(mem_addr+1));
            instr_len = 2;
            break;
        case 0xFF: // RST 38H; 1 byte; 32 cycles
            strcpy(buffer, "RST 0x0038");
            break;
    }
    return instr_len;
 }
