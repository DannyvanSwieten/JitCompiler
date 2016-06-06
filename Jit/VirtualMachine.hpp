//
//  VirtualMachine.hpp
//  Jit
//
//  Created by Danny van Swieten on 5/11/16.
//  Copyright © 2016 Danny van Swieten. All rights reserved.
//

#ifndef VirtualMachine_hpp
#define VirtualMachine_hpp

#define _BSD_SOURCE  // MAP_ANONYMOUS
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include <iostream>
#include <bitset>
#include <vector>

using Opcode    = std::bitset<8>;
using ModRegMem = std::bitset<8>;
using Register  = std::bitset<3>;

using OneByteDisplacement   = uint8_t;
using FourByteDisplacement  = uint32_t;

static const Register rax = Register("000");
static const Register rcx = Register("001");
static const Register rdx = Register("010");
static const Register rbx = Register("011");
static const Register rbp = Register("101"); // This is the basepointer register. Used to adress memory.


// Instructions. Instructions are encoded
static const Opcode addinstr    = Opcode("00000000");
static const Opcode subinstr    = Opcode("00101000");
static const Opcode moveinstr   = Opcode("10001000");

static const Opcode pushinstr   = Opcode("01010000");

class VirtualMachine
{
public:
    
    class Page
    {
    public:
        using Memory = void*;
        Page();
        ~Page();
        
        void map(std::vector<unsigned char>& buffer);
        
    private:
        
        Memory raw;
        size_t pageSize = 0;
        int prot = PROT_READ | PROT_WRITE;
        int flags = MAP_ANONYMOUS | MAP_PRIVATE;
    };
    
    void startEncoding();
    
    // Push
    void push(Register reg = rbp);
    
    // Add
    void add(Register op1, Register op2);
    void addImmediate(Register op1, int32_t op2);
    void add(Register op1, OneByteDisplacement op2);
    void add(Register op1, FourByteDisplacement op2);
    
    // Move
    void move(Register op1, Register op2);
    void run();
    
    void print();
    
private:
    
    Page page;
    size_t pageSize = 0;
    int32_t stackPointer;
    
    std::vector<unsigned char> codeBuffer;
};

#endif /* VirtualMachine_hpp */