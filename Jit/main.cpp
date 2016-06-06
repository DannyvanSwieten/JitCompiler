//
//  main.cpp
//  Jit
//
//  Created by Danny van Swieten on 5/6/16.
//  Copyright © 2016 Danny van Swieten. All rights reserved.
//

#include <iostream>
#include "VirtualMachine.hpp"

int main(int argc, const char * argv[])
{
    VirtualMachine vm;
    
    vm.push();
//    vm.addImmediate(rcx, 10);
    vm.add(rax, rcx);
    vm.print();
//    vm.move(rcx, rax);
    
    return 0;
}
