///
/// Base class for functionality joined by all the builders.
///

#ifndef BRO_PLUGIN_HILTI_COMPILER_BUILDER_BASE_H
#define BRO_PLUGIN_HILTI_COMPILER_BUILDER_BASE_H

#include <stdexcept>
#include <string>
#include <memory>

class BroObj;
class BroType;
class Expr;
class Func;
class ID;
class Val;

namespace hilti {
	class Type;
	class Expression;

	namespace declaration {
		class Function;
	}

	namespace builder {
		class BlockBuilder;
		class ModuleBuilder;
	}
}

namespace plugin {
    namespace Bro_Hilti {
        class Plugin;
    }
}

extern plugin::Bro_Hilti::Plugin HiltiPlugin;

namespace bro {
namespace hilti {

class ValueConverter;

namespace compiler {

#ifdef DEBUG
#define DBG_LOG_COMPILER(args...) debug_logger.Log((::plugin::Plugin&)HiltiPlugin, args)
#else
#define DBG_LOG_COMPILER(args...)
#endif

class BuilderException : public std::runtime_error {
public:
	BuilderException(const std::string& what) : std::runtime_error(what)	{}
};

class ModuleBuilder;

class BuilderBase  {
public:
	/**
	 * Returns the module builder currently in use.
	 *
	 * This is a short-cut to using the compiler's corresponding method.
	 */
	::hilti::builder::ModuleBuilder* ModuleBuilder();

	/**
	 * Returns the current block builder.
	 *
	 * This is a short-cut to using the compiler's corresponding method.
	 *
	 * @return The block builder.
	 */
	std::shared_ptr<::hilti::builder::BlockBuilder> Builder() const;

	/**
	 * Returns the module builder for the glue code module.
	 *
	 * This is a short-cut to using the compiler's corresponding method.
	 */
	::hilti::builder::ModuleBuilder* GlueModuleBuilder() const;

	/**
	 * Throws a \a BuilderException, terminating the current compilation.
	 *
	 * msg: The error message to carry upstream with the exception
	 */
	void Error(const std::string& msg, const BroObj* obj = 0);

	/**
	 * Logs a warning.
	 *
	 * msg: The warning message to log.
	 */
	void Warning(const std::string& msg, const BroObj* obj = 0);

	/**
	 * Reports an internal error and aborts execution.
	 *
	 * msg: The error message to log.
	 */
	void InternalError(const std::string& msg, const BroObj* obj = 0);

	/**
	 * Returns the compiler in use. This forwards to the corresponding
	 * method of the ModuleBuilder.
	 */
	class Compiler* Compiler() const;

	/**
	 * Returns the expression builder to use. This forwards to the
	 * corresponding method of the ModuleBuilder.
	 */
	class ExpressionBuilder* ExpressionBuilder() const;

	/**
	 * Returns the statement builder to use. This forwards to the
	 * corresponding method of the ModuleBuilder.
	 */
	class StatementBuilder* StatementBuilder() const;

	/**
	 * Returns the type builder to use. This forwards to the
	 * corresponding method of the ModuleBuilder.
	 */
	class TypeBuilder* TypeBuilder() const;

	/**
	 * Returns the value builder to use. This forwards to the
	 * corresponding method of the ModuleBuilder.
	 */
	class ValueBuilder* ValueBuilder() const;

	/**
	 * Returns the conversion builder to use. This forwards to the
	 * corresponding method of the ModuleBuilder.
	 */
	class ValueConverter* ValueConverter() const;

	/**
	 * Returns the internal HILTI-level symbol for a Bro Function.
	 *
	 * This is a short-cut to using the compiler's corresponding method.
	 *
	 * @param func The function to return the symbol for.
	 */
	std::string HiltiSymbol(const ::Func* func, bool include_module = false);

	/**
	 * Returns the internal HILTI-level symbol for the stub of a Bro
	 * Function.
	 *
	 * This is a short-cut to using the compiler's corresponding method.
	 *
	 * @param func The function to return the symbol for.
	 *
	 * @param include_module If true, the returned name will include the
	 * module name and hence represent the symbol as visibile at the LLVM
	 * level after linking.
	 */
	std::string HiltiStubSymbol(const ::Func* func, bool include_module);

	/**
	 * Returns the internal HILTI-level symbol for a Bro ID.
	 *
	 * This is a short-cut to using the compiler's corresponding method.
	 */
	std::string HiltiSymbol(const ::ID* id);

	/**
	 * Renders a \a BroObj via its \c Describe() method and turns the
	 * result into a valid HILTI identifier string.
	 *
	 * This is a short-cut to using the compiler's corresponding method.
	 *
	 * @param obj The object to describe.
	 *
	 * @return A string with a valid HILTI identifier.
	 */
	std::string HiltiODescSymbol(const ::BroObj* obj);

	/**
	 * Converts a Bro type into its HILTI equivalent. This is a short-cut
	 * to using the type builder's corresponding Compile() method.
	 *
	 * @param type The Bro type to converts.
	 *
	 * @return The corresponding HILTI type.
	 */
	std::shared_ptr<::hilti::Type> HiltiType(const ::BroType* type);

	/**
	 * Compiles a Bro value into its HILTI expression equivalent. This is
	 * a short-cut to using the value builder's corresponding Compile()
	 * method.
	 *
	 * @param val The Bro value to compile.
	 *
	 * @param target_type The target type that the compiled value should
	 * have. This acts primarily as a hint in case the expression's type
	 * isn't unambigious (e.g., with untyped constructors).
	 *
	 * @return The corresponding HILTI expression.
	 */
	std::shared_ptr<::hilti::Expression> HiltiValue(const ::Val* val,
							shared_ptr<::hilti::Type> target_type = nullptr);

	/**
	 * Compiles a Bro expression into its HILTI equivalent. This is a
	 * short-cut to using the expression's builder's corresponding
	 * Compile() method.
	 *
	 * @param expr The Bro expression to compile.
	 *
	 * @param target_type The target type that the compiled expression
	 * should have. This acts primarily as a hint in case the
	 * expression's type isn't unambigious (e.g., with untyped
	 * constructors).
	 *
	 * @return The corresponding HILTI expression.
	 */
	std::shared_ptr<::hilti::Expression> HiltiExpression(const ::Expr* expr,
							     shared_ptr<::hilti::Type> target_type = nullptr);

	/**
	 * Conerts a Bro \a Val into a HILTI expression dynamically at
	 * runtime.  This is a short-cut to using the conversion builder's
	 * corresponding Convert() method.
	 *
	 * @param val The value to convert.
	 *
	 * @return An expression referencing the converted value.
	 */
	std::shared_ptr<::hilti::Expression> RuntimeValToHilti(std::shared_ptr<::hilti::Expression> val, const ::BroType* type);

	/**
	 * Conerts a HILTI expression into a Bro \a Val dynamically at
	 * runtime.  This is a short-cut to using the conversion builder's
	 * corresponding Convert() method.
	 *
	 * @param val The value to convert.
	 *
	 * @param type The target Bro type to convert into.
	 *
	 * @return An expression referencing the converted value, which
	 * correspond to a pointer to a Bro Val.
	 */
	std::shared_ptr<::hilti::Expression> RuntimeHiltiToVal(std::shared_ptr<::hilti::Expression> val, const ::BroType* type);

	/**
	 * Declares the prototype for single function. This branches out into
	 * the other Declare*() methods for the various function flavors.
	 * 
	 * If the declaration already exists, that one is returned.
	 *
	 * The method forwards to ModuleBuilder::DeclareFunction.
	 *
	 * @param func The function to compile.
	 *
	 * @return An expression referencing the declared function.
	 */
	std::shared_ptr<::hilti::Expression> DeclareFunction(const ::Func* func);

	/**
	 * Returns a string with location information for a BroObj.
	 */
	std::string Location(const ::BroObj *obj);

protected:
	/**
	 * Constructor.
	 *
	 * mbuilder: The module builder to use.
	 */
	BuilderBase(class ModuleBuilder* mbuilder);

private:
	class ModuleBuilder* mbuilder;
};

}
}
}

#endif


