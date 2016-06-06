//
//  VirtualMachine.cpp
//  Jit
//
//  Created by Danny van Swieten on 5/11/16.
//  Copyright © 2016 Danny van Swieten. All rights reserved.
//

#include "VirtualMachine.hpp"

unsigned char nibble(int32_t value, int byteNumber)
{
    return (value >> (8*byteNumber)) & 0x0FF;
}

void VirtualMachine::print()
{
    for(auto& code: codeBuffer)
    {
        Opcode binary = code;
        std::cout << binary.to_string() << std::endl;
    }
}

VirtualMachine::Page::Page():
pageSize(sysconf(_SC_PAGESIZE))
{
    raw = mmap(NULL, pageSize, prot, flags, -1, 0);
}

VirtualMachine::Page::~Page()
{
    munmap(raw, pageSize);
}

std::function<int(void)> VirtualMachine::compile()
{
    return page.map(codeBuffer);
}

std::function<int(void)> VirtualMachine::Page::map(std::vector<unsigned char>& buffer)
{
    memcpy(raw, buffer.data(), buffer.size());
    
    int prot = PROT_READ | PROT_EXEC;
    mprotect(raw, pageSize, prot);
    
    typedef int (*fptr)();
    
    fptr my_fptr = reinterpret_cast<fptr>(raw) ;
    std::function<int(void)> f = my_fptr;
    return f;
}

void VirtualMachine::returnToMain()
{
    codeBuffer.emplace_back(0xc3);
}

void VirtualMachine::push(Register reg)
{
    Opcode instruction = pushinstr;
    instruction[0] = reg[0];
    instruction[1] = reg[1];
    instruction[2] = reg[2];
    
    // Single byte instruction.
    
    unsigned char hex   = instruction.to_ulong();
    codeBuffer.emplace_back(hex);
}

void VirtualMachine::pop(Register reg)
{
    Opcode instruction = popinstr;
    instruction[0] = reg[0];
    instruction[1] = reg[1];
    instruction[2] = reg[2];
    
    unsigned char hex = instruction.to_ulong();
    codeBuffer.emplace_back(hex);
}

void VirtualMachine::add(Register op1, Register op2)
{
    Opcode instruction = addinstr;
    instruction[7]  = 1; // S bit. 0 for 1 byte, 1 for > 1 byte
    instruction[6]  = 1; // D bit. 0 for R/M, 1 for REG
    
    std::cout << addinstr.to_string() << std::endl;
    
    // Register addressing mode (Register -> Register)
    ModRegMem encode;
    encode[0]   = 1;
    encode[1]   = 1;
    
    // Reg bits. (Register in this case)
    encode[2]   = op1[0];
    encode[3]   = op1[1];
    encode[4]   = op1[2];
    
    // R/M bits. (Register in this case)
    encode[5]   = op2[0];
    encode[6]   = op2[1];
    encode[7]   = op2[2];
    
    unsigned char firstByte     = instruction.to_ulong();
    unsigned char secondByte    = encode.to_ulong();
    
    codeBuffer.insert(codeBuffer.end(), {firstByte, secondByte});
}

void VirtualMachine::addImmediate(Register op1, int32_t op2)
{
    Opcode add;
    add[6]  = 0; // Immediate value has same size as operand.
    add[7]  = 1; // Destination is Register.
    add[0]  = 1; // Immediate instruction.
    
    // Adressing mode: Indirect addressing
    ModRegMem encode;
    encode[0]   = 0;
    encode[1]   = 0;
    
    // REG is used for opcode extension. 000 = immediate add.
    encode[2]   = 0;
    encode[3]   = 0;
    encode[4]   = 0;
    
    unsigned char firstByte     = add.to_ulong();
    unsigned char secondByte    = encode.to_ulong();
    unsigned char thirdByte     = nibble(op2, 0);
    unsigned char fourthByte    = nibble(op2, 1);
    unsigned char fifthByte     = nibble(op2, 2);
    unsigned char sixthByte     = nibble(op2, 3);
    
    codeBuffer.insert(codeBuffer.end(),
    {
        firstByte,
        secondByte,
        thirdByte,
        fourthByte,
        fifthByte,
        sixthByte
    });
}

void VirtualMachine::add(Register op1, OneByteDisplacement op2)
{
    Opcode instruction = addinstr;
    instruction[7]  = 1; // S bit. 0 for 1 byte, 1 for > 1 byte
    instruction[6]  = 1; // D bit. 0 for R/M, 1 for REG
    
    unsigned char byte1 = instruction.to_ulong();
    
    // Adressing mode: Indirect addressing
    ModRegMem encode;
    encode[0]   = 0;
    encode[1]   = 1;
    
    encode[2]   = op1[0];
    encode[3]   = op1[1];
    encode[4]   = op1[2];
    
    // M/R bits. (M in this case)
    encode[5]   = rbp[0];
    encode[6]   = rbp[1];
    encode[7]   = rbp[2];
    
    unsigned char byte2 = encode.to_ulong();
    unsigned char byte3 = nibble(op2, 0);
    
    codeBuffer.insert(codeBuffer.end(), {byte1, byte2, byte3});
}

void VirtualMachine::move(Register op1, Register op2)
{
    
    // Adressing mode: Reg bits represent register.
    ModRegMem encode;
    encode[0]   = 1;
    encode[1]   = 1;
    
    // First register address
    encode[2]   = op1[0];
    encode[3]   = op1[1];
    encode[4]   = op1[2];
    
    // Second register address
    encode[5]   = op2[0];
    encode[6]   = op2[1];
    encode[7]   = op2[2];
    
    // Move instruction
    unsigned char byte1 = 0x8b;
    unsigned char byte2 = (unsigned char)encode.to_ulong();

    codeBuffer.insert(codeBuffer.end(),
                      {
                          byte1,
                          byte2
                      });
}