/*
Copyright Contributors to the libdnf project.

This file is part of libdnf: https://github.com/rpm-software-management/libdnf/

Libdnf is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 2.1 of the License, or
(at your option) any later version.

Libdnf is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with libdnf.  If not, see <https://www.gnu.org/licenses/>.
*/


#ifndef LIBDNF_RPM_TRANSACTION_CALLBACKS_HPP
#define LIBDNF_RPM_TRANSACTION_CALLBACKS_HPP

#include "libdnf/base/transaction_package.hpp"

#include <cstdint>

namespace libdnf::rpm {


/// Class represents one item in transaction set.
using TransactionItem = base::TransactionPackage;


// suppress "unused-parameter" warnings because TransactionCallbacks is a virtual class
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

/// Base class for Transaction callbacks
/// User implements Transaction callbacks by inheriting this class and overriding its methods.
class TransactionCallbacks {
public:
    /// Scriptlet type
    // TODO(jrohel): Are all scriptlets types present and correct?
    enum class ScriptType {
        UNKNOWN,
        PRE_INSTALL,            // "%pre"
        POST_INSTALL,           // "%post"
        PRE_UNINSTALL,          // "%preun"
        POST_UNINSTALL,         // "%postun"
        PRE_TRANSACTION,        // "%pretrans"
        POST_TRANSACTION,       // "%posttrans"
        TRIGGER_PRE_INSTALL,    // "%triggerprein"
        TRIGGER_INSTALL,        // "%triggerin"
        TRIGGER_UNINSTALL,      // "%triggerun"
        TRIGGER_POST_UNINSTALL  // "%triggerpostun"
    };

    virtual ~TransactionCallbacks() = default;

    virtual void install_progress(const TransactionItem & item, uint64_t amount, uint64_t total) {}
    virtual void install_start(const TransactionItem & item, uint64_t total) {}
    virtual void install_stop(const TransactionItem & item, uint64_t amount, uint64_t total) {}
    virtual void transaction_progress(uint64_t amount, uint64_t total) {}
    virtual void transaction_start(uint64_t total) {}
    virtual void transaction_stop(uint64_t total) {}
    virtual void uninstall_progress(const TransactionItem & item, uint64_t amount, uint64_t total) {}
    virtual void uninstall_start(const TransactionItem & item, uint64_t total) {}
    virtual void uninstall_stop(const TransactionItem & item, uint64_t amount, uint64_t total) {}
    virtual void unpack_error(const TransactionItem & item) {}
    virtual void cpio_error(const TransactionItem & item) {}
    virtual void script_error(const TransactionItem * item, Nevra nevra, ScriptType type, uint64_t return_code) {}
    virtual void script_start(const TransactionItem * item, Nevra nevra, ScriptType type) {}
    virtual void script_stop(const TransactionItem * item, Nevra nevra, ScriptType type, uint64_t return_code) {}
    virtual void elem_progress(const TransactionItem & item, uint64_t amount, uint64_t total) {}
    virtual void verify_progress(uint64_t amount, uint64_t total) {}
    virtual void verify_start(uint64_t total) {}
    virtual void verify_stop(uint64_t total) {}
};

#pragma GCC diagnostic pop


}  // namespace libdnf::rpm

#endif  // LIBDNF_RPM_TRANSACTION_CALLBACKS_HPP
