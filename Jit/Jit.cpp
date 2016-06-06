//
//  Jit.cpp
//  Jit
//
//  Created by Danny van Swieten on 5/10/16.
//  Copyright © 2016 Danny van Swieten. All rights reserved.
//

#include "Jit.hpp"

unsigned char nibble(int32_t value, int byteNumber)
{
    return (value >> (8*byteNumber)) & 0x0FF;
}

Jit::Jit()
{
    buffer = std::make_unique<InstructionBuffer>();
    pageSize = sysconf(_SC_PAGESIZE);
    int prot = PROT_READ | PROT_WRITE;
    int flags = MAP_ANONYMOUS | MAP_PRIVATE;
    buffer->memory = mmap(NULL, pageSize, prot, flags, -1, 0);
}

Jit::~Jit()
{
    munmap(buffer->memory, pageSize);
}

bool Jit::finalize()
{
    memcpy(buffer->memory, buffer->codeBuffer.data(), buffer->codeBuffer.size());
    
    int prot = PROT_READ | PROT_EXEC;
    mprotect(buffer->memory, pageSize, prot);
    return  true;
}
int Jit::execute()
{
    typedef int (*fptr)();
    
    fptr my_fptr = reinterpret_cast<fptr>(buffer->memory) ;
    return my_fptr();
}

void Jit::pushBasePointer()
{
    buffer->codeBuffer.emplace_back(0x55);
}

void Jit::pushStackPointerToBasePointer()
{
    buffer->codeBuffer.insert(buffer->codeBuffer.end(), {0x48, 0x89, 0xe5});
}

void Jit::popBasePointer()
{
    buffer->codeBuffer.insert(buffer->codeBuffer.end(), {0x5d});
}

void Jit::returnToMain()
{
    buffer->codeBuffer.insert(buffer->codeBuffer.end(), {0xc3});
}

void Jit::setImmediateEAX(int value)
{
    buffer->codeBuffer.insert(buffer->codeBuffer.end(), {0xb8});
    
    for (auto i = 0; i < 4; i++)
    {
        buffer->codeBuffer.insert(buffer->codeBuffer.end(), nibble(value, i));
    }
}

int32_t Jit::immediate(int value)
{
    stackPointer += sizeof(value);
    auto adress = nibble(stackPointer, 0);
    buffer->codeBuffer.insert(buffer->codeBuffer.end(), {0xc7, 0x45, adress});
    
    for (auto i = 0; i < 4; i++)
    {
        buffer->codeBuffer.insert(buffer->codeBuffer.end(), nibble(value, i));
    }
    
    return stackPointer;
}

void Jit::add()
{
    opcode add("00000011");
}