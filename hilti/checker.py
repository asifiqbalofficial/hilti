# $Id$
#
# Visitor to checks the validity of the visited code.

import block
import module
import function
import instruction
import scope
import id
import type
import visitor
import util

class Checker(visitor.Visitor):
    def __init__(self):
        super(Checker, self).__init__(all=True)
        self._infunction = False
        self._have_module = False

    def error(self, obj, str):
        util.error(str, context=obj)

checker = Checker()

### Overall control structures.

@checker.when(module.Module)
def _(self, m):
    if self._have_module:
        util.error("more than one module declaration in input file", context=m)
    
    self._have_module = True

### 

### Global ID definitions. 

@checker.when(id.ID, type.StructDeclType)
def _(self, id):
    if not self._have_module:
        util.error("input file must start with module declaration", context=id)

    if self._infunction:
        util.error("structs cannot be declared inside functions", context=id)
        
@checker.when(id.ID, type.StorageType)
def _(self, id):
    if not self._have_module:
        util.error("input file must start with module declaration", context=id)

### Function definitions.

@checker.pre(function.Function)
def _(self, f):
    assert not self._infunction
    self._infunction = True
    
    if not self._have_module:
        util.error("input file must start with module declaration", context=f)
        
@checker.post(function.Function)
def _(self, f):
    assert self._infunction
    self._infunction = False
    
    if not self._have_module:
        util.error("input file must start with module declaration", context=f)
        
### Instructions.

@checker.when(instruction.Instruction)
def _(self, i):
    
    # Check that signature maches the actual operands. 
    def typeError(actual, expected, tag):
        self.error(i, "type of %s does not match signature (expected %s but is %s) " % (tag, str(expected), str(actual)))
        
    def checkOp(op, sig, tag):
        if op and not sig:
            typeError(type.name(op.type()), "undefined", tag)
            
        if not op and sig:
            typeError("undefined", type.name(sig.type), tag)
        
        if op and sig and not op.type() == sig:
            typeError(type.name(op.type()), type.name(sig), tag)
                
    checkOp(i.op1(), i.signature().op1(), "operand 1")
    checkOp(i.op2(), i.signature().op2(), "operand 2")
    checkOp(i.target(), i.signature().target(),"target")


