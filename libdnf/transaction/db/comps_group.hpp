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


#ifndef LIBDNF_TRANSACTION_DB_COMPS_GROUP_HPP
#define LIBDNF_TRANSACTION_DB_COMPS_GROUP_HPP


#include "utils/sqlite3/sqlite3.hpp"

#include <memory>


namespace libdnf::transaction {


class CompsGroup;
class Transaction;
class TransactionItem;


/// Return a vector of CompsGroup objects with comps groups in a transaction
std::vector<CompsGroup> get_transaction_comps_groups(libdnf::utils::SQLite3 & conn, Transaction & trans);


/// Create a query (statement) that inserts new records to the 'comps_group' table
std::unique_ptr<libdnf::utils::SQLite3::Statement> comps_group_insert_new_query(libdnf::utils::SQLite3 & conn);


/// Use a query to insert a new record to the 'comps_group' table
int64_t comps_group_insert(libdnf::utils::SQLite3::Statement & query, CompsGroup & grp);


/// Insert CompsGroup objects associated with a transaction into the database
void insert_transaction_comps_groups(libdnf::utils::SQLite3 & conn, Transaction & trans);


}  // namespace libdnf::transaction


#endif  // LIBDNF_TRANSACTION_DB_COMPS_GROUP_HPP
