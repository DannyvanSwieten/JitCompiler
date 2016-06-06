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
    
    vm.push(rbp);
    vm.move(1);
    vm.pop(rbp);
    vm.returnToMain();
    
    /*
     Print the complete machine code in hex. Compare this to assembly: (should match)
     push   rbp
     mov    DWORD PTR [rbp-0x4],0x1
     pop    rbp
     ret
     */
    vm.printHex();
    
    auto f = vm.compile();
    std::cout << f() << std::endl;
    
    return 0;
}
