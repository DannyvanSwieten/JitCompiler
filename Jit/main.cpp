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
    vm.pop(rbp);
    vm.move(<#Register op1#>, <#Register op2#>)
    vm.returnToMain();
    vm.print();
    
    auto f = vm.compile();
    std::cout << f() << std::endl;
    
    return 0;
}
