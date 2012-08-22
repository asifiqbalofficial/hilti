
#include "attribute.h"
#include "type.h"
#include "constant.h"
#include "declaration.h"
#include "expression.h"
#include "scope.h"
#include "passes/printer.h"

using namespace binpac;
using namespace type;

shared_ptr<binpac::Type> trait::Parseable::fieldType()
{
    auto t = dynamic_cast<binpac::Type*>(this);
    assert(t);
    return t->sharedPtr<binpac::Type>();
}

std::list<trait::Parseable::ParseAttribute> trait::Parseable::parseAttributes() const
{
    return {};
}

string binpac::Type::render()
{
    std::ostringstream s;
    passes::Printer(s, true).run(sharedPtr<Node>());
    return s.str();
}

binpac::Type::Type(const Location& l) : ast::Type<AstInfo>(l)
{
}

shared_ptr<Scope> binpac::Type::typeScope()
{
    return nullptr;
}

OptionalArgument::OptionalArgument(shared_ptr<Type> arg)
{
    _arg = arg;
}

shared_ptr<binpac::Type> OptionalArgument::argType() const
{
    return _arg;
}

bool OptionalArgument::equal(shared_ptr<binpac::Type> other) const
{
    return other ? _arg->equal(other) : true;
}

string OptionalArgument::render()
{
    return string("[ " + _arg->render() + " ]");
}

string binpac::Type::docRender()
{
    return render();
}

PacType::PacType(const Location& l) : binpac::Type(l)
{
    _attrs = std::make_shared<AttributeSet>(l);
}

PacType::PacType(const attribute_list& attrs,  const Location& l) : binpac::Type(l)
{
    _attrs = std::make_shared<AttributeSet>(attrs, l);
}

#if 0
shared_ptr<AttributeSet> PacType::attributes() const
{
    return _attrs;
}
#endif

TypedPacType::TypedPacType(const Location& l) : PacType(l)
{
    setWildcard(true);
}

TypedPacType::TypedPacType(shared_ptr<Type> rtype, const Location& l) : PacType(l)
{
    _argtype = rtype;
    addChild(_argtype);
}

shared_ptr<Type> TypedPacType::argType() const
{
    return _argtype;
}

bool TypedPacType::_equal(shared_ptr<binpac::Type> other) const
{
    return trait::Parameterized::equal(other);
}

trait::Parameterized::type_parameter_list TypedPacType::parameters() const
{
    type_parameter_list params;

    if ( ! _argtype )
        return params;

    auto p = std::make_shared<trait::parameter::Type>(_argtype);
    params.push_back(p);
    return params;
}

Iterator::Iterator(const Location& l) : TypedPacType(l)
{
}

Iterator::Iterator(shared_ptr<Type> ttype, const Location& l) : TypedPacType(ttype, l)
{
}

Any::Any(const Location& l) : binpac::Type(l)
{
    setMatchesAny(true);
}


Unknown::Unknown(const Location& l) : binpac::Type(l)
{
}

Unknown::Unknown(shared_ptr<ID> id, const Location& l)
{
}

shared_ptr<ID> Unknown::id() const
{
    return _id;
}

TypeByName::TypeByName(shared_ptr<ID> id, const Location& l) : binpac::Type(l)
{
    _id = id;
}

const shared_ptr<ID> TypeByName::id() const
{
    return _id;
}

bool TypeByName::_equal(shared_ptr<binpac::Type> other) const
{
    return std::dynamic_pointer_cast<TypeByName>(other)->_id->name() == _id->name();
}

Unset::Unset(const Location& l) : PacType(l)
{
}

MemberAttribute::MemberAttribute(shared_ptr<ID> attr, const Location& l) : binpac::Type(l)
{
    _attribute = attr;
    addChild(_attribute);
}

shared_ptr<ID> MemberAttribute::attribute() const
{
    return _attribute;
}

string MemberAttribute::render()
{
    return _attribute ? _attribute->name() : "<id>";
}

bool MemberAttribute::equal(shared_ptr<binpac::Type> other) const
{
    auto mother = std::dynamic_pointer_cast<MemberAttribute>(other);
    assert(mother);

    return _attribute ? _attribute == mother->_attribute : true;
}

type::Module::Module(const Location& l) : binpac::Type(l)
{
}

Void::Void(const Location& l) : binpac::Type(l)
{
}


String::String(const Location& l) : PacType(l)
{
}


Address::Address(const Location& l) : PacType(l)
{
}

Network::Network(const Location& l) : PacType(l)
{
}

Port::Port(const Location& l) : PacType(l)
{
}

Bitset::Bitset(const label_list& labels, const Location& l) : PacType(l)
{
    int next = 0;
    for ( auto label : labels ) {
        auto bit = label.second;

        if ( bit < 0 )
            bit = next;

        next = std::max(next, bit + 1);

        _labels.push_back(make_pair(label.first, bit));
    }

    _labels.sort([] (const Label& lhs, const Label& rhs) { return lhs.first->name().compare(rhs.first->name()) < 0; });
}

Bitset::Bitset(const Location& l) : PacType(l)
{
    setWildcard(true);
}

const Bitset::label_list& Bitset::labels() const
{
    return _labels;
}

int Bitset::labelBit(shared_ptr<ID> label) const
{
    for ( auto l : _labels ) {
        if ( *l.first == *label )
            return l.second;
    }

    throw ast::InternalError(util::fmt("unknown bitset label %s", label->pathAsString().c_str()), this);
}

shared_ptr<Scope> Bitset::typeScope()
{
    if ( _scope )
        return _scope;

    _scope = shared_ptr<Scope>(new Scope());

    for ( auto label : _labels ) {
        auto p = shared_from_this();
        auto p2 = std::dynamic_pointer_cast<binpac::Type>(p);
        constant::Bitset::bit_list bl;
        bl.push_back(label.first);
        auto val = shared_ptr<Constant>(new constant::Bitset(bl, p2, location()));
        auto expr = shared_ptr<expression::Constant>(new expression::Constant(val, location()));
        _scope->insert(label.first, expr);
    }

    return _scope;
}

bool Bitset::_equal(shared_ptr<Type> other) const
{
    auto bother = std::dynamic_pointer_cast<Bitset>(other);
    assert(bother);

    if ( _labels.size() != bother->_labels.size() )
        return false;

    auto i1 = _labels.begin();
    auto i2 = bother->_labels.begin();

    for ( ; i1 != _labels.end(); ++i1, ++i2 ) {
        if ( i1->first != i2->first || i1->second != i2->second )
            return false;
    }

    return true;
}

Enum::Enum(const label_list& labels, const Location& l) : PacType(l)
{
    int next = 1;
    for ( auto label : labels ) {
        if ( *label.first == "Undef" )
            throw ast::RuntimeError("enum label 'Undef' is already predefined", this);

        auto i = label.second;

        if ( i < 0 )
            i = next;

        next = std::max(next, i + 1);

        _labels.push_back(make_pair(label.first, i));
    }

    _labels.push_back(make_pair(std::make_shared<ID>("Undef"), -1));
    _labels.sort([] (const Label& lhs, const Label& rhs) { return lhs.first->name().compare(rhs.first->name()) < 0; });
}

Enum::Enum(const Location& l) : PacType(l)
{
    setWildcard(true);
}

shared_ptr<Scope> Enum::typeScope()
{
    if ( _scope )
        return _scope;

    _scope = shared_ptr<Scope>(new Scope());

    for ( auto label : _labels ) {
        auto p = shared_from_this();
        auto p2 = std::dynamic_pointer_cast<binpac::Type>(p);
        auto val = shared_ptr<Constant>(new constant::Enum(label.first, p2, location()));
        auto expr = shared_ptr<expression::Constant>(new expression::Constant(val, location()));
        _scope->insert(label.first, expr);
    }

    return _scope;
}

int Enum::labelValue(shared_ptr<ID> label) const
{
    for ( auto l : _labels ) {
        if ( *l.first == *label )
            return l.second;
    }

    throw ast::InternalError(util::fmt("unknown enum label %s", label->pathAsString().c_str()), this);
    return -1;
}

bool Enum::_equal(shared_ptr<Type> other) const
{
    auto eother = std::dynamic_pointer_cast<Enum>(other);
    assert(eother);

    if ( _labels.size() != eother->_labels.size() )
        return false;

    auto i1 = _labels.begin();
    auto i2 = eother->_labels.begin();

    for ( ; i1 != _labels.end(); ++i1, ++i2 ) {
        if ( i1->first != i2->first || i1->second != i2->second )
            return false;
    }

    return true;
}

const Enum::label_list& Enum::labels() const
{
    return _labels;
}

CAddr::CAddr(const Location& l) : PacType(l)
{
}

Double::Double(const Location& l) : PacType(l)
{
}

Bool::Bool(const Location& l) : PacType(l)
{
}

Interval::Interval(const Location& l) : PacType(l)
{
}

Time::Time(const Location& l) : PacType(l)
{
}

Integer::Integer(int width, bool sign, const Location& l) : PacType(l)
{
    _width = width;
    _signed = sign;
}

Integer::Integer(const Location& l) : PacType(l)
{
    _width = 0;
    setWildcard(true);
}

int Integer::width() const
{
    return _width;
}

bool Integer::signed_() const
{
    return _signed;
}

bool Integer::_equal(shared_ptr<binpac::Type> other) const
{
    return trait::Parameterized::equal(other);
}

trait::Parameterized::type_parameter_list Integer::parameters() const
{
    type_parameter_list params;
    auto p = std::make_shared<trait::parameter::Integer>(_width);
    params.push_back(p);
    return params;
}

Tuple::Tuple(const Location& l) : PacType(l)
{
    setWildcard(true);
}

Tuple::Tuple(const type_list& types, const Location& l) : PacType(l)
{
    for ( auto t : types )
        _types.push_back(t);

    for ( auto t : _types )
        addChild(t);
}

trait::Parameterized::type_parameter_list Tuple::parameters() const
{
    type_parameter_list params;

    for ( auto t : _types ) {
        auto p = std::make_shared<trait::parameter::Type>(t);
        params.push_back(p);
    }

    return params;
}

const trait::TypeList::type_list Tuple::typeList() const
{
    trait::TypeList::type_list types;

    for ( auto t : _types )
        types.push_back(t);

    return types;
}

bool Tuple::_equal(shared_ptr<binpac::Type> other) const
{
    return trait::Parameterized::equal(other);
}

TypeType::TypeType(shared_ptr<binpac::Type> type, const Location& l) : binpac::Type(l)
{
    _rtype = type; addChild(_rtype);
}

TypeType::TypeType(const Location& l) : binpac::Type()
{
    setWildcard(true);
}

shared_ptr<binpac::Type> TypeType::typeType() const
{
    return _rtype;
}

bool TypeType::_equal(shared_ptr<binpac::Type> other) const
{
    return _rtype->equal(std::dynamic_pointer_cast<TypeType>(other)->_rtype);
}

Exception::Exception(shared_ptr<Type> base, shared_ptr<Type> arg, const Location& l) : TypedPacType(arg, l)
{
    _base = base;
    addChild(_base);
}

Exception::Exception(const Location& l) : TypedPacType(l)
{
}

shared_ptr<Type> Exception::baseType() const
{
    return _base;
}

void Exception::setBaseType(shared_ptr<Type> base)
{
    _base = base;
}

bool Exception::isRootType() const
{
    return _libtype == "Hilti::UnspecifiedException";
}

void Exception::setLibraryType(const string& name)
{
    _libtype = name;
}

string Exception::libraryType() const
{
    return _libtype;
}

type::Function::Function(shared_ptr<type::function::Result> result, const parameter_list& args, const Location& l)
: binpac::Type(l), ast::type::mixin::Function<AstInfo>(this, result, args)
{
}

type::Function::Function(const Location& l)
    : binpac::Type(l),
    ast::type::mixin::Function<AstInfo>(this, std::make_shared<function::Result>(std::make_shared<Void>(), false, l), parameter_list())
{
    setWildcard(true);
}

bool type::Function::_equal(shared_ptr<binpac::Type> o) const
{
    auto other = ast::checkedCast<Function>(o);

    if ( ((*result()) != (*other->result())) )
        return false;

    auto p1 = parameters();
    auto p2 = other->parameters();

    if ( p1.size() != p2.size() )
        return false;

    auto i = p1.begin();
    auto j = p2.begin();

    while ( i != p1.end() ) {
        if ( *(*i++) != *(*j++) )
            return false;
    }

    return true;
}

function::Parameter::Parameter(shared_ptr<binpac::ID> id, shared_ptr<Type> type, bool constant, shared_ptr<Expression> default_value, Location l)
    : ast::type::mixin::function::Parameter<AstInfo>(id, type, constant, default_value, l)
{
}

function::Result::Result(shared_ptr<Type> type, bool constant, Location l)
    : ast::type::mixin::function::Result<AstInfo>(type, constant, l)
{
}

type::Hook::Hook(shared_ptr<binpac::type::function::Result> result, const parameter_list& args, const Location& l)
: Function(result, args, l)
{
}

type::Hook::Hook(const Location& l) : Function(l)
{
}

Bytes::Bytes(const Location& l) : PacType(l)
{
}

shared_ptr<Type> Bytes::iterType()
{
    return shared_ptr<Type>(new iterator::Bytes(location()));
}

shared_ptr<binpac::Type> Bytes::elementType()
{
    return shared_ptr<Type>(new iterator::Bytes(location()));
}

shared_ptr<Type> Bytes::fieldType()
{
    return std::make_shared<type::Bytes>(location());
}

std::list<trait::Parseable::ParseAttribute> Bytes::parseAttributes() const
{
    return {
        { "length", std::make_shared<type::Integer>(64, false), nullptr, false }
    };
}

iterator::Bytes::Bytes(const Location& l) : Iterator(shared_ptr<Type>(new type::Bytes(l)))
{
}

File::File(const Location& l) : PacType(l)
{
}

iterator::ContainerIterator::ContainerIterator(shared_ptr<Type> ctype, const Location& l) : Iterator(ctype)
{
}

iterator::Map::Map(shared_ptr<Type> ctype, const Location& l) : ContainerIterator(ctype, l)
{
}

iterator::Set::Set(shared_ptr<Type> ctype, const Location& l) : ContainerIterator(ctype, l)
{
}

iterator::Vector::Vector(shared_ptr<Type> ctype, const Location& l) : ContainerIterator(ctype, l)
{
}

iterator::List::List(shared_ptr<Type> ctype, const Location& l) : ContainerIterator(ctype, l)
{
}

List::List(shared_ptr<Type> etype, const Location& l)
: TypedPacType(etype, l)
{
}

List::List(const Location& l) : TypedPacType(l)
{
}

shared_ptr<binpac::Type> List::iterType()
{
    return std::make_shared<iterator::List>(this->sharedPtr<Type>(), location());
}

shared_ptr<binpac::Type> List::elementType()
{
    return argType();
}

shared_ptr<binpac::Type> List::fieldType()
{
    return argType();
}

Vector::Vector(shared_ptr<Type> etype, const Location& l)
: TypedPacType(etype, l)
{
}

Vector::Vector(const Location& l) : TypedPacType(l)
{
}

shared_ptr<binpac::Type> Vector::iterType()
{
    return std::make_shared<iterator::Vector>(this->sharedPtr<Type>(), location());
}

shared_ptr<binpac::Type> Vector::elementType()
{
    return argType();
}

shared_ptr<binpac::Type> Vector::fieldType()
{
    return argType();
}

Set::Set(shared_ptr<Type> etype, const Location& l)
: TypedPacType(etype, l)
{
}

Set::Set(const Location& l) : TypedPacType(l)
{
}

shared_ptr<binpac::Type> Set::iterType()
{
    return std::make_shared<iterator::Set>(this->sharedPtr<Type>(), location());
}

shared_ptr<binpac::Type> Set::elementType()
{
    return argType();
}

shared_ptr<binpac::Type> Set::fieldType()
{
    return argType();
}

Map::Map(shared_ptr<Type> key, shared_ptr<Type> value, const Location& l)
: PacType(l)
{
    _key = key; _value = value; addChild(_key); addChild(_value);
}

shared_ptr<binpac::Type> Map::iterType()
{
    return std::make_shared<iterator::Map>(this->sharedPtr<Type>(), location());
}

shared_ptr<binpac::Type> Map::elementType()
{
    type_list types = { _key, _value };
    return std::make_shared<Tuple>(types, location());
}

bool Map::_equal(shared_ptr<binpac::Type> other) const
{
    return trait::Parameterized::equal(other);
}

trait::Parameterized::type_parameter_list Map::parameters() const
{
    type_parameter_list params;

    if ( wildcard() )
        return params;

    auto key = std::make_shared<trait::parameter::Type>(_key);
    auto value = std::make_shared<trait::parameter::Type>(_value);

    params.push_back(key);
    params.push_back(value);

    return params;
}

RegExp::RegExp(const attribute_list& attrs, const Location& l) : PacType(l)
{
    _attrs = std::make_shared<AttributeSet>(attrs, l);
}

RegExp::RegExp(const Location& l) : PacType(l)
{
    setWildcard(true);
}

shared_ptr<AttributeSet> RegExp::attributes() const
{
    return _attrs;
}

bool RegExp::_equal(shared_ptr<binpac::Type> other) const
{
    return trait::Parameterized::equal(other);
}

trait::Parameterized::type_parameter_list RegExp::parameters() const
{
    uint64_t flags = 0;

    if ( _attrs->has("&nosub") )
        flags = 1;

    type_parameter_list params = { std::make_shared<trait::parameter::Integer>(flags) };
    return params;
}


TimerMgr::TimerMgr(const Location& l) : PacType(l)
{
}


Timer::Timer(const Location& l) : PacType(l)
{
}

int unit::Item::Item::_id_counter = 0;

unit::Item::Item(shared_ptr<ID> id, const hook_list& hooks, const attribute_list& attrs, const Location& l) : Node(l)
{
    if ( ! id ) {
        id = std::make_shared<ID>(util::fmt("__anon%d", ++_id_counter), l);
        _anonymous = true;
    }

    _attrs = std::make_shared<AttributeSet>(attrs);

    addChild(_id);
    addChild(_attrs);

    for ( auto h : hooks )
        _hooks.push_back(h);

    for ( auto h : _hooks )
        addChild(h);
}

bool unit::Item::anonymous() const
{
    return _anonymous;
}

shared_ptr<ID> unit::Item::id() const
{
    return _id;
}

shared_ptr<Scope> unit::Item::scope() const
{
    return _scope;
}

hook_list unit::Item::hooks() const
{
    hook_list hooks;

    for ( auto h : _hooks )
        hooks.push_back(h);

    return hooks;
}

shared_ptr<AttributeSet> unit::Item::attributes() const
{
    return _attrs;
}

unit::item::Field::Field(shared_ptr<ID> id,
                         shared_ptr<binpac::Type> type,
                         shared_ptr<Expression> default_,
                         shared_ptr<Expression> cond,
                         const hook_list& hooks,
                         const attribute_list& attrs,
                         const expression_list& sinks,
                         const Location& l)
    : Item(id, hooks, attrs, l)
{
    _type = type;
    _default = default_;
    _cond = cond;

    addChild(_type);
    addChild(_default);
    addChild(_cond);

    for ( auto s : sinks )
        _sinks.push_back(s);

    for ( auto s : _sinks )
        addChild(s);
}

shared_ptr<binpac::Type> unit::item::Field::type() const
{
    return _type;
}

shared_ptr<Expression> unit::item::Field::default_() const
{
    return _default;
}

/// Returns the item's associated condition, or null if none.
shared_ptr<Expression> unit::item::Field::condition() const
{
    return _cond;
}

expression_list unit::item::Field::sinks() const
{
    expression_list sinks;

    for ( auto s : _sinks )
        sinks.push_back(s);

    return sinks;
}


unit::item::field::Constant::Constant(shared_ptr<Expression> val, shared_ptr<Expression> cond, const hook_list& hooks, const Location& l)
    : Field(nullptr, val->type(), val, cond, hooks, attribute_list(), expression_list(), l)
{
}

unit::item::field::Type::Type(shared_ptr<ID> id,
                       shared_ptr<binpac::Type> type,
                       shared_ptr<Expression> default_,
                       shared_ptr<Expression> cond,
                       const hook_list& hooks,
                       const attribute_list& attrs,
                       const expression_list& params,
                       const expression_list& sinks,
                       const Location& l)
    : Field(id, type, default_, cond, hooks, attrs, sinks, l)
{
    for ( auto p : params )
        _params.push_back(p);

    for ( auto p : _params )
        addChild(p);
}

expression_list unit::item::field::Type::parameters() const
{
    expression_list params;

    for ( auto p : _params )
        params.push_back(p);

    return params;
}

unit::item::field::RegExp::RegExp(const string& regexp,
                           shared_ptr<ID> id,
                           shared_ptr<Expression> default_,
                           shared_ptr<Expression> cond,
                           const hook_list& hooks,
                           const attribute_list& attrs,
                           const expression_list& sinks,
                           const Location& l)
    : Field(id, std::make_shared<type::RegExp>(l), default_, cond, hooks, attrs, sinks, l)
{
    _regexp = regexp;
}

const string& unit::item::field::RegExp::regexp() const
{
    return _regexp;
}

unit::item::field::switch_::Case::Case(const expression_list& exprs, shared_ptr<Item> item, const Location& l) : Node(l)
{
    for ( auto e : exprs )
        _exprs.push_back(e);

    for ( auto e : _exprs )
        addChild(e);

    _item = item;
    addChild(_item);
}

expression_list unit::item::field::switch_::Case::expressions() const
{
    expression_list exprs;

    for ( auto e : _exprs )
        exprs.push_back(e);

    return exprs;
}

shared_ptr<unit::Item> unit::item::field::switch_::Case::item() const
{
    return _item;
}

unit::item::field::Switch::Switch(shared_ptr<Expression> expr, const case_list& cases, const hook_list& hooks, const Location& l)
    : Field(nullptr, nullptr, nullptr, nullptr, hooks, attribute_list(), expression_list(), l)
{
    _expr = expr;
    addChild(_expr);

    for ( auto c : cases )
        _cases.push_back(c);

    for ( auto c : _cases )
        addChild(c);
}

shared_ptr<Expression> unit::item::field::Switch::expression() const
{
    return _expr;
}

unit::item::field::Switch::case_list unit::item::field::Switch::cases() const
{
    case_list cases;

    for ( auto c : _cases )
        cases.push_back(c);

    return cases;
}

unit::item::Variable::Variable(shared_ptr<binpac::ID> id, shared_ptr<binpac::Type> type, shared_ptr<Expression> default_, const hook_list& hooks, const Location& l)
    : Item(id, hooks, attribute_list(), l)
{
    _type = type;
    _default = default_;

    addChild(_type);
    addChild(_default);
}

shared_ptr<Expression> unit::item::Variable::default_() const
{
    return _default;
}

shared_ptr<binpac::Type> unit::item::Variable::type() const
{
    return _type;
}


unit::item::Property::Property(shared_ptr<binpac::ID> id, shared_ptr<binpac::Expression> value, const Location& l)
    : Item(id, hook_list(), attribute_list(), l)
{
    _value = value;
    addChild(value);
}


shared_ptr<Expression> unit::item::Property::value() const
{
    return _value;
}

unit::item::GlobalHook::GlobalHook(shared_ptr<ID> id, shared_ptr<binpac::Hook> hook, const Location& l)
    : Item(hook->id(), hook_list(), attribute_list(), l)
{
    _id = id;
    _hook = hook;
    addChild(_id);
    addChild(_hook);
}

shared_ptr<binpac::Hook> unit::item::GlobalHook::hook() const
{
    return _hook;
}

Unit::Unit(const parameter_list& params, const unit_item_list& items, const Location& l)
{
    for ( auto p : params )
        _params.push_back(p);

    for ( auto p : _params )
        addChild(p);

    for ( auto i : items )
        _items.push_back(i);

    for ( auto d : _items )
        addChild(d);
}


Unit::Unit(const Location& l) : PacType(l)
{
    setWildcard(true);
}

parameter_list Unit::parameters() const
{
    parameter_list params;

    for ( auto p : _params )
        params.push_back(p);

    return params;
}

unit_item_list Unit::items() const
{
    unit_item_list items;

    for ( auto i : _items )
        items.push_back(i);

    return items;
}

std::list<shared_ptr<unit::item::Field>> Unit::fields() const
{
    std::list<shared_ptr<unit::item::Field>> m;

    for ( auto i : _items ) {
        auto f = ast::tryCast<unit::item::Field>(i);

        if ( f )
            m.push_back(f);
    }

    return m;
}

std::list<shared_ptr<unit::item::Variable>> Unit::variables() const
{
    std::list<shared_ptr<unit::item::Variable>> m;

    for ( auto i : _items ) {
        auto f = ast::tryCast<unit::item::Variable>(i);

        if ( f )
            m.push_back(f);
    }

    return m;
}

std::list<shared_ptr<unit::item::GlobalHook>> Unit::hooks() const
{
    std::list<shared_ptr<unit::item::GlobalHook>> m;

    for ( auto i : _items ) {
        auto f = ast::tryCast<unit::item::GlobalHook>(i);

        if ( f )
            m.push_back(f);
    }

    return m;
}

std::list<shared_ptr<unit::item::Property>> Unit::properties() const
{
    std::list<shared_ptr<unit::item::Property>> m;

    for ( auto i : _items ) {
        auto f = ast::tryCast<unit::item::Property>(i);

        if ( f )
            m.push_back(f);
    }

    return m;
}


shared_ptr<unit::Item> Unit::item(shared_ptr<ID> id) const
{
    for ( auto i : _items ) {
        if ( i->id()->name() == id->name() )
            return i;
    }

    return nullptr;
}

shared_ptr<Scope> Unit::scope() const
{
    return _scope;
}

shared_ptr<Grammar> Unit::grammar() const
{
    return _grammar;
}

void Unit::setGrammar(shared_ptr<Grammar> grammar)
{
    _grammar = grammar;
}

bool Unit::_equal(shared_ptr<binpac::Type> ty) const
{
    auto other = ast::checkedCast<Unit>(ty);

    if ( _items.size() != other->_items.size() )
        return false;

    auto i1 = _items.begin();
    auto i2 = other->_items.begin();

    for ( ; i1 != _items.end(); ++i1, ++i2 ) {
        auto f1 = *i1;
        auto f2 = *i2;

        if ( f1->id()->name() != f2->id()->name() )
            return false;

        // Comparing the items by rendering them to avoid infinite recursion
        // for cycles.
        std::ostringstream t1;
        passes::Printer(t1, true).run(f1);

        std::ostringstream t2;
        passes::Printer(t2, true).run(f2);

        if ( t1.str() != t2.str() )
            return false;
    }

    return true;
}
