# $Id$
"""
.. hlt:type:: bitset

    The ``bitset`` data type groups a set of individual bits together. In each
    instance, any of the bits is either set or not set. The individual bits are
    identified by labels, which are declared in the bitsets type declaration::

        bitset MyBits { BitA, BitB, BitC, BitD }


    The individual labels can be accessed via their namespace, e.g.,
    ``MyBits::BitC``, and used with the ``bitset.*`` instructions::

        # Initialized to all bits cleared.
        local mybits: MyBits

        # Set a bit in the bit set.
        mybits = bitset.set mybits MyBits::BitB

    Normally, bits are numbered in the order they are given within the
    ``bitset`` type definition. In the example above, ``BitA`` is the least
    signficiant bit, ``BitB`` the next, etc. One can however also assign
    numbers to bits to enforce a particular mapping from labels to bit positions::

            bitset MyBits { BitA=2, BitB=3, BitC=5, BitD=7 }

    Such specific mappings are relevant when (1) printing their numerical
    value (see below); and (2) accessing an instance from C, as then one needs
    to know what bit to test. 

    If a ``bitmap`` instance is not explicitly initialized, all bits are
    initially cleared.

    A ``bitmap`` can be printed as either a string (which will display the
    labels of all set bits), or as an integer (which will be the numerical
    value corresponding to all bits sets according to their positions). 

    Note: For efficiency reasons, HILTI for supports only up to 64 bits per
    type; and one can only assign positions from 0 to 63. 

    Todo: We can't create constants with multiple bit set yet (e.g.,
    ``MyBits::BitA | MyBits::BitB`` should work, but doesn't.)
"""

import llvm.core

from hilti.constraints import *
from hilti.instructions.operators import *

import string

@hlt.type("bitset", 19, c="uint64_t", hdr="bitset.h")
class Bitset(type.ValueType, type.Constable):
    def __init__(self, labels, location=None):
        """The ``bitset`` type.

        Returns: dictonary string -> int - The labels mappend to their values.
        If a value is given as None, it will be chosen uniqule automatically.
        """
        super(Bitset, self).__init__(location=location)

        self._labels = {}
        next = 0
        for (label, bit) in labels:
            if bit == None:
                bit = next

            next = max(next, bit + 1)
            self._labels[label] = bit

        self._labels_sorted = labels

    def labels(self):
        """Returns the bit labels with their corresponding bit numbers.

        Returns: dictonary string -> int - The labels mappend to their values.
        """
        return self._labels

    ### Overridden from Type.

    def name(self):
        return "bitset { %s }" % ", ".join(["%s=%d" % (l, self._labels[l]) for (l, v) in self._labels_sorted])

    ### Overridden from HiltiType.

    def typeInfo(self, cg):
        """An ``bitset``'s type information keeps additional information in
        the ``aux`` field: ``aux`` points to a concatenation of entries ``{
        uint8, hlt_string }``, one per label. The integer corresponds to the
        bit number, and the string to the bit's label. The end of the array is
        marked by an string of null. 
        """

        typeinfo = cg.TypeInfo(self)
        typeinfo.to_string = "hlt::bitset_to_string";
        typeinfo.to_int64 = "hlt::bitset_to_int64";

        # Build ASCIIZ strings for the labels.
        aux = []
        zero = [cg.llvmConstInt(0, 8)]
        for (label, value) in sorted(self.labels().items(), key=lambda x: x[1]):
            label_glob = string._makeLLVMString(cg, label)
            aux += [llvm.core.Constant.struct([cg.llvmConstInt(value, 8), label_glob])]

        null = llvm.core.Constant.null(string._llvmStringTypePtr());
        aux += [llvm.core.Constant.struct([cg.llvmConstInt(0, 8), null])]

        name = cg.nameTypeInfo(self) + "_labels"
        const = llvm.core.Constant.array(aux[0].type, aux)
        glob = cg.llvmNewGlobalConst(name, const)
        glob.linkage = llvm.core.LINKAGE_LINKONCE_ANY

        typeinfo.aux = glob

        return typeinfo

    def llvmType(self, cg):
        return llvm.core.Type.int(64)

    def _validate(self, vld):
        for bit in self._labels.values():
            if bit >= 64:
                vld.error(self, "bitset can only store bits in the range 0..63")

    def cmpWithSameType(self, other):
        return self._labels == other._labels

    ### Overridden from ValueType.

    def llvmDefault(self, cg):
        return cg.llvmConstInt(0, 64)

    ### Overridden from Constable.

    def validateConstant(self, vld, const):
        """``bitset`` constants are strings naming one of the labels."""

        label = const.value()

        if not isinstance(label, str):
            util.internal_error("bitset label must be a Python string")

        if not label in self._labels:
            vld.error(self, "unknown bitset label %s" % label)

    def llvmConstant(self, cg, const):
        label = const.value()

        bit = self._labels[label]
        return cg.llvmConstInt(1, 64).shl(cg.llvmConstInt(bit, 64))

    def outputConstant(self, printer, const):
        printer.output(const.value())

@hlt.overload(Equal, op1=cBitset, op2=cSameTypeAsOp(1), target=cBool)
class Equal(Operator):
    """Returns True if *op1* equals *op2*. Both operands must be of the *same*
    ``bitset`` type.
    """
    def _codegen(self, cg):
        op1 = cg.llvmOp(self.op1())
        op2 = cg.llvmOp(self.op2())
        result = cg.builder().icmp(llvm.core.IPRED_EQ, op1, op2)
        cg.llvmStoreInTarget(self, result)

@hlt.instruction("bitset.set", op1=cBitset, op2=cSameTypeAsOp(1), target=cSameTypeAsOp(1))
class Set(Instruction):
    """Adds the bits set in *op2* to those set in *op1* and returns the
    result. Both operands must be of the *same* ``bitset`` type.
    """
    def _codegen(self, cg):
        op1 = cg.llvmOp(self.op1())
        op2 = cg.llvmOp(self.op2())
        result = cg.builder().or_(op1, op2)
        cg.llvmStoreInTarget(self, result)

@hlt.instruction("bitset.clear", op1=cBitset, op2=cSameTypeAsOp(1), target=cSameTypeAsOp(1))
class Clear(Instruction):
    """Clears the bits set in *op2* from those set in *op1* and returns the
    result. Both operands must be of the *same* ``bitset`` type.
    """
    def _codegen(self, cg):
        op1 = cg.llvmOp(self.op1())
        op2 = cg.llvmOp(self.op2())
        op2 = cg.builder().xor(op2, cg.llvmConstInt(-1, 64))
        result = cg.builder().and_(op1, op2)
        cg.llvmStoreInTarget(self, result)

@hlt.instruction("bitset.has", op1=cBitset, op2=cSameTypeAsOp(1), target=cBool)
class Has(Instruction):
    """ Returns True if all bits in *op2* are set in *op1*. Both operands must
    be of the *same* ``bitset`` type.
    """
    def _codegen(self, cg):
        op1 = cg.llvmOp(self.op1())
        op2 = cg.llvmOp(self.op2())
        tmp = cg.builder().and_(op1, op2)
        result = cg.builder().icmp(llvm.core.IPRED_EQ, tmp, op2)
        cg.llvmStoreInTarget(self, result)





