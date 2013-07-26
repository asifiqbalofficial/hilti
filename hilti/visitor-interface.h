
#ifndef HILTI_VISITOR_INTERFACE_H
#define HILTI_VISITOR_INTERFACE_H

namespace hilti {

class Constant;
class Ctor;
class Declaration;
class Expression;
class Function;
class Hook;
class ID;
class Instruction;
class Module;
class Scope;
class Statement;
class Type;
class Variable;

namespace type {
   class Address;
   class Any;
   class Bitset;
   class Block;
   class Bool;
   class Bytes;
   class CAddr;
   class Callable;
   class Channel;
   class Classifier;
   class Double;
   class Enum;
   class Exception;
   class File;
   class Function;
   class Hook;
   class IOSource;
   class Integer;
   class Interval;
   class Iterator;
   class Label;
   class List;
   class Map;
   class MatchTokenState;
   class Module;
   class Network;
   class Overlay;
   class OptionalArgument;
   class Port;
   class Reference;
   class RegExp;
   class Set;
   class String;
   class Struct;
   class Time;
   class Timer;
   class TimerMgr;
   class Tuple;
   class TypeType;
   class Unknown;
   class Unset;
   class Vector;
   class Void;
   class Context;
   class Scope;

   namespace trait {
      namespace parameter {
          class Attribute;
          class Base;
          class Enum;
          class Integer;
          class Type;
      }
   }

   namespace function {
      class Parameter;
      class Result;
   }

   namespace iterator {
      class Bytes;
      class IOSource;
      class List;
      class Map;
      class Set;
      class Vector;
   }

   namespace struct_ {
      class Field;
   }

   namespace overlay {
      class Field;
   }
}

namespace constant {
   class Address;
   class Bitset;
   class Bool;
   class CAddr;
   class Double;
   class Enum;
   class Integer;
   class Interval;
   class Label;
   class Network;
   class Port;
   class Reference;
   class String;
   class Time;
   class Tuple;
   class Unset;
}

namespace ctor {
   class Bytes;
   class List;
   class Map;
   class RegExp;
   class Set;
   class Vector;
}

namespace expression {
   class Block;
   class CodeGen;
   class Coerced;
   class Constant;
   class Constant;
   class Ctor;
   class Default;
   class Function;
   class ID;
   class List;
   class Module;
   class Name;
   class Parameter;
   class Type;
   class Variable;
   class Void;
}

namespace declaration {
   class Constant;
   class Function;
   class Hook;
   class Type;
   class Variable;
}

namespace variable {
   class Local;
   class Global;
}

namespace statement {
   class Block;
   class Instruction;
   class Noop;
   class Try;
   class ForEach;

   namespace try_ {
      class Catch;
   }

   namespace instruction {
      class Resolved;
      class Unresolved;
   }

}

namespace instruction {
   namespace internal {
       class AmbigiousInstruction;
       class UnknownInstruction;
   }
}

// This is autogenerated and declares all the statement::instruction::*::*
// classes.
#include <hilti/autogen/instructions-declare.h>

}

#include "common.h"

namespace hilti {

/// Visitor interface. This class defines one visit method for each type of
/// HILTI AST node that we have. Visitor implementation then override the
/// ones they need.
class VisitorInterface
{
public:
   virtual ~VisitorInterface();

   virtual void visit(ID* s)                {}
   virtual void visit(Module* m)            {}

   // Functions.
   virtual void visit(Function* f)          {}
   virtual void visit(Hook* f)              {}
   virtual void visit(type::function::Parameter* p) {}
   virtual void visit(type::function::Result* p) {}

   // Types.
   virtual void visit(Type* t)              {}
   virtual void visit(type::Address* t)     {}
   virtual void visit(type::Any* a)         {}
   virtual void visit(type::Bitset* t)      {}
   virtual void visit(type::Bool* b)        {}
   virtual void visit(type::Bytes* t)       {}
   virtual void visit(type::CAddr* t)       {}
   virtual void visit(type::Callable* t)    {}
   virtual void visit(type::Channel* t)     {}
   virtual void visit(type::Classifier* t)  {}
   virtual void visit(type::Double* t)      {}
   virtual void visit(type::Enum* t)        {}
   virtual void visit(type::Exception* t)   {}
   virtual void visit(type::File* t)        {}
   virtual void visit(type::Function* t)    {}
   virtual void visit(type::Hook* t)        {}
   virtual void visit(type::IOSource* t)    {}
   virtual void visit(type::Integer* i)     {}
   virtual void visit(type::Interval* t)    {}
   virtual void visit(type::Iterator* t)    {}
   virtual void visit(type::Label* t)       {}
   virtual void visit(type::List* t)        {}
   virtual void visit(type::Map* t)         {}
   virtual void visit(type::MatchTokenState* t) {}
   virtual void visit(type::Network* t)     {}
   virtual void visit(type::Overlay* t)     {}
   virtual void visit(type::OptionalArgument* t) {}
   virtual void visit(type::Port* t)        {}
   virtual void visit(type::Reference* t)   {}
   virtual void visit(type::RegExp* t)      {}
   virtual void visit(type::Set* t)         {}
   virtual void visit(type::String* s)      {}
   virtual void visit(type::Struct* t)      {}
   virtual void visit(type::Time* t)        {}
   virtual void visit(type::Timer* t)       {}
   virtual void visit(type::TimerMgr* t)    {}
   virtual void visit(type::Tuple* t)       {}
   virtual void visit(type::TypeType* t)    {}
   virtual void visit(type::Unknown* t)     {}
   virtual void visit(type::Unset* t)       {}
   virtual void visit(type::Vector* t)      {}
   virtual void visit(type::Void* v)        {}
   virtual void visit(type::Context* v)     {}
   virtual void visit(type::Scope* v)     {}

   // Iterator types.
   virtual void visit(type::iterator::Bytes* i)    {}
   virtual void visit(type::iterator::IOSource* i) {}
   virtual void visit(type::iterator::List* i)     {}
   virtual void visit(type::iterator::Map* i)      {}
   virtual void visit(type::iterator::Set* i)      {}
   virtual void visit(type::iterator::Vector* i)   {}

   // Other type-related classes.
   virtual void visit(type::struct_::Field* f) {}
   virtual void visit(type::overlay::Field* f) {}

   // Type parameters.
   virtual void visit(type::trait::parameter::Attribute* p) {}
   virtual void visit(type::trait::parameter::Base* p)      {}
   virtual void visit(type::trait::parameter::Enum* p)      {}
   virtual void visit(type::trait::parameter::Integer* p)   {}
   virtual void visit(type::trait::parameter::Type* p)      {}

   // Constants.
   virtual void visit(Constant* s)            {}
   virtual void visit(constant::Address* c)   {}
   virtual void visit(constant::Bitset* c)    {}
   virtual void visit(constant::Bool* i)      {}
   virtual void visit(constant::CAddr* c)     {}
   virtual void visit(constant::Double* c)    {}
   virtual void visit(constant::Enum* c)      {}
   virtual void visit(constant::Integer* c)   {}
   virtual void visit(constant::Interval* c)  {}
   virtual void visit(constant::Label* c)     {}
   virtual void visit(constant::Network* c)   {}
   virtual void visit(constant::Port* c)      {}
   virtual void visit(constant::Reference* c) {}
   virtual void visit(constant::String* i)    {}
   virtual void visit(constant::Time* c)      {}
   virtual void visit(constant::Tuple* c)     {}
   virtual void visit(constant::Unset* c)     {}

   // Ctors.
   virtual void visit(Ctor* c)         {}
   virtual void visit(ctor::Bytes* c)  {}
   virtual void visit(ctor::List* c)   {}
   virtual void visit(ctor::Map* c)    {}
   virtual void visit(ctor::RegExp* c) {}
   virtual void visit(ctor::Set* c)    {}
   virtual void visit(ctor::Vector* c) {}

   // Expressions.
   virtual void visit(Expression* e)            {}
   virtual void visit(expression::Block* e)     {}
   virtual void visit(expression::CodeGen* e)   {}
   virtual void visit(expression::Coerced* e)   {}
   virtual void visit(expression::Constant* e)  {}
   virtual void visit(expression::Ctor* e)      {}
   virtual void visit(expression::Default* e)   {}
   virtual void visit(expression::Function* e)  {}
   virtual void visit(expression::ID* e)        {}
   virtual void visit(expression::List* e)      {}
   virtual void visit(expression::Module* e)    {}
   virtual void visit(expression::Parameter* e) {}
   virtual void visit(expression::Type* e)      {}
   virtual void visit(expression::Variable* e)  {}
   virtual void visit(expression::Void* e)  {}

   // Declarations.
   virtual void visit(Declaration* d)           {}
   virtual void visit(declaration::Constant* d) {}
   virtual void visit(declaration::Function* d) {}
   virtual void visit(declaration::Hook* d) {}
   virtual void visit(declaration::Type* d)     {}
   virtual void visit(declaration::Variable* d) {}

   // Variables
   virtual void visit(Variable* v)         {}
   virtual void visit(variable::Local* l)  {}
   virtual void visit(variable::Global* g) {}

   // Statements.
   virtual void visit(Statement* s)              {}
   virtual void visit(statement::Block* s)       {}
   virtual void visit(statement::Instruction* s) {}
   virtual void visit(statement::Noop* s)        {}
   virtual void visit(statement::Try* s)         {}
   virtual void visit(statement::try_::Catch* s) {}
   virtual void visit(statement::ForEach* s)     {}

   virtual void visit(statement::instruction::Resolved* s)   {}
   virtual void visit(statement::instruction::Unresolved* s) {}

   // Instructions.
   virtual void visit(Instruction* s)       {}

       // This is autogenerated and has the visits() for all the
       // statement::instruction::*::* classes.
       #include <hilti/autogen/instructions-visits.h>

protected:
   /// Internal function. This carries out the visitor callback.
   virtual void callAccept(shared_ptr<ast::NodeBase> node);
};

}

#endif
