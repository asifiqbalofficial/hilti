
#ifndef HILTI_OPTIONS_H
#define HILTI_OPTIONS_H

#include <list>
#include <set>
#include <string>

#include <util/file-cache.h>

using std::string;


namespace hilti {

/// A set of options controlling the compilation and link process.
class Options
{
public:
    typedef std::list<string> string_list;
    typedef std::set<string> string_set;

    Options();
    Options(const Options& other) = default;
    virtual ~Options() {};

    /// If true, produce code with debugging support. Enabling this has a
    /// significant performance impact on the generated code.
    bool debug = false;

    /// If true, optimize the produced code for performance. This activates
    /// both HILTI and LLVM level optimization. However, the HILTI-level
    /// passes can be disabled individually my modifying optimization_passes.
    bool optimize = false;

    /// If >0, include instrumentation for runtime profiling into generated
    /// code. For larger values, more fine-granular profiling may be
    /// included. Enabling profiling has a significant performance impact.
    unsigned int profile = 0;

    /// If true, all generated code is verified for correctness. Disabling
    /// this is primarily for debugging purposes.
    bool verify = true;

    /// If true, prepare code for JITing. This must be set if the code will
    /// be run through JIT. This will be checked for by jitModule(), which
    /// aborts if it's not set.
    bool jit = false;

    /// List of directories to search for imports and other \c *.hlt library
    /// files. The current directory will always be tried first. By default,
    /// this set is set to the current directory plus the installation-wide
    /// HILTI library directories (in that order).
    string_list libdirs_hlt;

    /// A set of labels specifying HILTI optimization passes to run when \a
    /// optimize is true. optimizationLabels() returns a list of valid
    /// labels. By default, this set contains all of them.
    string_set optimizations;

    /// A set of labels specifying parts of the code generator that will
    /// produce debugging output during operation. cgDebugLabels() returns a
    /// list of valid labels. By default, this set is empty.
    string_set cg_debug;

    /// A directory where to cache compiled modules. If left unset, caching
    /// is disabled.
    string module_cache;

    /// Returns true if the given label is enabled in \a optimization. This
    /// is just a convinience method.
    bool optimizing(const string& label) const;

    /// Returns all available HILTI optimization passes.
    virtual string_set optimizationLabels() const;

    /// Returns true if the given label is enabled in cg_debug. This is just
    /// a convinience method.
    bool cgDebugging(const string& label) const;

    /// Returns all available debugging options for the code generator.
    virtual string_set cgDebugLabels() const;

    /// Initialized the cache key with option-specific values.
    virtual void toCacheKey(::util::cache::FileCache::Key* key) const;

};

}

#endif
