#if defined(SWIGPYTHON)
%module(package="libdnf5") advisory
#elif defined(SWIGPERL)
%module "libdnf5::advisory"
#elif defined(SWIGRUBY)
%module "libdnf5/advisory"
#endif

%include <exception.i>
%include <std_vector.i>

%include <shared.i>

%import "common.i"

%exception {
    try {
        $action
    } catch (const libdnf5::UserAssertionError & e) {
        SWIG_exception(SWIG_RuntimeError, e.what());
    } catch (const libdnf5::Error & e) {
        SWIG_exception(SWIG_RuntimeError, e.what());
    } catch (const std::runtime_error & e) {
        SWIG_exception(SWIG_RuntimeError, e.what());
    }
}

// TODO(jkolarik): advisory modules skipped for now

%{
    #include "libdnf5/advisory/advisory.hpp"
    #include "libdnf5/advisory/advisory_package.hpp"
    #include "libdnf5/advisory/advisory_set.hpp"
    #include "libdnf5/advisory/advisory_set_iterator.hpp"
    #include "libdnf5/advisory/advisory_collection.hpp"
    #include "libdnf5/advisory/advisory_query.hpp"
    #include "libdnf5/advisory/advisory_reference.hpp"
%}

#define CV __perl_CV

%include "libdnf5/advisory/advisory.hpp"
%include "libdnf5/advisory/advisory_package.hpp"
%include "libdnf5/advisory/advisory_set.hpp"

%rename(next) libdnf5::advisory::AdvisorySetIterator::operator++();
%rename(value) libdnf5::advisory::AdvisorySetIterator::operator*();
%include "libdnf5/advisory/advisory_set_iterator.hpp"

%ignore libdnf5::advisory::AdvisoryCollection::get_modules();
%include "libdnf5/advisory/advisory_collection.hpp"
%include "libdnf5/advisory/advisory_query.hpp"
%include "libdnf5/advisory/advisory_reference.hpp"

%template(VectorAdvisoryCollection) std::vector<libdnf5::advisory::AdvisoryCollection>;
%template(VectorAdvisoryPackage) std::vector<libdnf5::advisory::AdvisoryPackage>;
%template(VectorAdvisoryReference) std::vector<libdnf5::advisory::AdvisoryReference>;

add_iterator(AdvisorySet)
