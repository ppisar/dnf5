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


#ifndef LIBDNF_CLI_OUTPUT_TRANSACTION_TABLE_HPP
#define LIBDNF_CLI_OUTPUT_TRANSACTION_TABLE_HPP

#include "libdnf-cli/tty.hpp"
#include "libdnf-cli/utils/units.hpp"

#include "libdnf/common/exception.hpp"
#include "libdnf/utils/to_underlying.hpp"

#include <fmt/format.h>
#include <libdnf/rpm/nevra.hpp>
#include <libsmartcols/libsmartcols.h>

#include <iostream>
#include <vector>


namespace libdnf::cli::output {


enum { COL_NAME, COL_ARCH, COL_EVR, COL_REPO, COL_SIZE };


static const char * action_color(libdnf::transaction::TransactionItemAction action) {
    switch (action) {
        case libdnf::transaction::TransactionItemAction::INSTALL:
        case libdnf::transaction::TransactionItemAction::UPGRADE:
        case libdnf::transaction::TransactionItemAction::REINSTALL:
            return "green";
        case libdnf::transaction::TransactionItemAction::DOWNGRADE:
            return "magenta";
        case libdnf::transaction::TransactionItemAction::REMOVE:
            return "red";
        case libdnf::transaction::TransactionItemAction::REPLACED:
            return "halfbright";
        case libdnf::transaction::TransactionItemAction::REASON_CHANGE:
            break;
    }

    libdnf_throw_assertion("Unexpected action in print_transaction_table: {}", libdnf::utils::to_underlying(action));
}


class ActionHeaderPrinter {
public:
    ActionHeaderPrinter(struct libscols_table * table) : table(table) {}

    // TODO(lukash) to bring more sanity into the templated functions here, it
    // would be better if the Transaction template type of
    // print_transaction_table() was required to have a TransactionItem type
    // defined inside, so that the ActionHeaderPrinter class could be templated
    // instad of this method, and we could do (in print_transaction_table(),
    // where this class is instantiated):
    // ActionHeaderPrinter<Transaction::TransactionItem> action_header_printer(...);
    template <class T>
    struct libscols_line * print(const T & tspkg) {
        if (!current_action || *current_action != tspkg.get_action() || !current_reason ||
            *current_reason != tspkg.get_reason()) {
            current_header_line = scols_table_new_line(table, NULL);
            std::string text;

            switch (tspkg.get_action()) {
                case libdnf::transaction::TransactionItemAction::INSTALL:
                    text = "Installing";
                    if (tspkg.get_reason() == libdnf::transaction::TransactionItemReason::DEPENDENCY) {
                        text += " dependencies";
                    } else if (tspkg.get_reason() == libdnf::transaction::TransactionItemReason::WEAK_DEPENDENCY) {
                        text += " weak dependencies";
                    }
                    break;
                case libdnf::transaction::TransactionItemAction::UPGRADE:
                    text = "Upgrading";
                    break;
                case libdnf::transaction::TransactionItemAction::DOWNGRADE:
                    text = "Downgrading";
                    break;
                case libdnf::transaction::TransactionItemAction::REINSTALL:
                    text = "Reinstalling";
                    break;
                case libdnf::transaction::TransactionItemAction::REMOVE:
                    text = "Removing";
                    if (tspkg.get_reason() == libdnf::transaction::TransactionItemReason::DEPENDENCY) {
                        text += " dependent packages";
                    } else if (tspkg.get_reason() == libdnf::transaction::TransactionItemReason::CLEAN) {
                        text += " unused dependencies";
                    }
                    break;
                case libdnf::transaction::TransactionItemAction::REPLACED:
                case libdnf::transaction::TransactionItemAction::REASON_CHANGE:
                    libdnf_throw_assertion(
                        "Unexpected action in print_transaction_table: {}",
                        libdnf::utils::to_underlying(tspkg.get_action()));
            }

            text += ":";

            scols_line_set_data(current_header_line, COL_NAME, text.c_str());

            current_action = tspkg.get_action();
            current_reason = tspkg.get_reason();
        }

        return current_header_line;
    }

private:
    struct libscols_table * table = nullptr;
    struct libscols_line * current_header_line = nullptr;
    std::optional<libdnf::transaction::TransactionItemAction> current_action;
    std::optional<libdnf::transaction::TransactionItemReason> current_reason;
};


class TransactionSummary {
public:
    void add(const libdnf::transaction::TransactionItemAction & action) {
        switch (action) {
            case libdnf::transaction::TransactionItemAction::INSTALL:
                installs++;
                break;
            case libdnf::transaction::TransactionItemAction::UPGRADE:
                upgrades++;
                break;
            case libdnf::transaction::TransactionItemAction::DOWNGRADE:
                downgrades++;
                break;
            case libdnf::transaction::TransactionItemAction::REINSTALL:
                reinstalls++;
                break;
            case libdnf::transaction::TransactionItemAction::REMOVE:
                removes++;
                break;
            case libdnf::transaction::TransactionItemAction::REPLACED:
                replaced++;
                break;
            case libdnf::transaction::TransactionItemAction::REASON_CHANGE:
                libdnf_throw_assertion(
                    "Unexpected action in print_transaction_table: {}", libdnf::utils::to_underlying(action));
        }
    }

    void print() {
        std::cout << "\nTransaction Summary:\n";
        if (installs != 0) {
            std::cout << fmt::format(" {:15} {:4} packages\n", "Installing:", installs);
        }
        if (reinstalls != 0) {
            std::cout << fmt::format(" {:15} {:4} packages\n", "Reinstalling:", reinstalls);
        }
        if (upgrades != 0) {
            std::cout << fmt::format(" {:15} {:4} packages\n", "Upgrading:", upgrades);
        }
        if (replaced != 0) {
            std::cout << fmt::format(" {:15} {:4} packages\n", "Replacing:", replaced);
        }
        if (removes != 0) {
            std::cout << fmt::format(" {:15} {:4} packages\n", "Removing:", removes);
        }
        if (downgrades != 0) {
            std::cout << fmt::format(" {:15} {:4} packages\n", "Downgrading:", downgrades);
        }
        std::cout << std::endl;
    }

private:
    int installs = 0;
    int reinstalls = 0;
    int upgrades = 0;
    int downgrades = 0;
    int removes = 0;
    int replaced = 0;
};


template <class TransactionPackage>
static bool transaction_package_cmp(const TransactionPackage & tspkg1, const TransactionPackage & tspkg2) {
    if (tspkg1.get_action() != tspkg2.get_action()) {
        return tspkg1.get_action() > tspkg2.get_action();
    }

    if (tspkg1.get_reason() != tspkg2.get_reason()) {
        return tspkg1.get_reason() > tspkg2.get_reason();
    }

    return libdnf::rpm::cmp_naevr(tspkg1.get_package(), tspkg2.get_package());
}


/// Prints all transaction problems
template <class Transaction>
void print_resolve_logs(Transaction transaction) {
    const std::vector<std::string> logs = transaction.get_resolve_logs_as_strings();
    for (const auto & log : logs) {
        std::cout << log << std::endl;
    }
    if (logs.size() > 0) {
        std::cout << std::endl;
    }
}

template <class Transaction>
bool print_transaction_table(Transaction & transaction) {
    // even correctly resolved transaction can contain some warnings / hints / infos
    // in resolve logs (e.g. the package user wanted to install is already installed).
    // Present them to the user.
    print_resolve_logs(transaction);

    // TODO (nsella) split function into create/print if possible
    //static struct libscols_table * create_transaction_table(bool with_status) {}
    auto tspkgs = transaction.get_transaction_packages();

    if (tspkgs.empty()) {
        std::cout << "Nothing to do." << std::endl;
        return false;
    }

    struct libscols_table * tb = scols_new_table();

    auto column = scols_table_new_column(tb, "Package", 0.3, SCOLS_FL_TREE);
    auto header = scols_column_get_header(column);
    scols_cell_set_color(header, "bold");

    column = scols_table_new_column(tb, "Arch", 6, 0);
    header = scols_column_get_header(column);
    scols_cell_set_color(header, "bold");

    column = scols_table_new_column(tb, "Version", 0.3, SCOLS_FL_TRUNC);
    header = scols_column_get_header(column);
    scols_cell_set_color(header, "bold");

    column = scols_table_new_column(tb, "Repository", 0.1, SCOLS_FL_TRUNC);
    header = scols_column_get_header(column);
    scols_cell_set_color(header, "bold");

    column = scols_table_new_column(tb, "Size", 9, SCOLS_FL_RIGHT);
    header = scols_column_get_header(column);
    scols_cell_set_color(header, "bold");

    scols_table_enable_maxout(tb, 1);
    scols_table_enable_colors(tb, libdnf::cli::tty::is_interactive());

    struct libscols_symbols * sb = scols_new_symbols();
    scols_symbols_set_branch(sb, " ");
    scols_symbols_set_right(sb, " ");
    scols_symbols_set_vertical(sb, " ");
    scols_table_set_symbols(tb, sb);

    // TODO(dmach): use colors from config
    // TODO(dmach): highlight version changes (rebases)
    // TODO(dmach): consider reordering so the major changes (installs, obsoletes, removals) are at the bottom next to the confirmation question
    // TODO(jrohel): Print relations with obsoleted packages

    std::sort(tspkgs.begin(), tspkgs.end(), transaction_package_cmp<decltype(*tspkgs.begin())>);

    struct libscols_line * header_ln = nullptr;
    TransactionSummary ts_summary;
    ActionHeaderPrinter action_header_printer(tb);

    for (auto & tspkg : tspkgs) {
        // TODO(lukash) maybe this shouldn't come here at all
        // TODO(lukash) handle OBSOLETED correctly throught the transaction table output
        if (tspkg.get_action() == libdnf::transaction::TransactionItemAction::REASON_CHANGE) {
            continue;
        }

        if (tspkg.get_action() == libdnf::transaction::TransactionItemAction::REPLACED) {
            ts_summary.add(tspkg.get_action());
            continue;
        }

        auto pkg = tspkg.get_package();

        header_ln = action_header_printer.print(tspkg);

        struct libscols_line * ln = scols_table_new_line(tb, header_ln);
        scols_line_set_data(ln, COL_NAME, pkg.get_name().c_str());
        scols_line_set_data(ln, COL_ARCH, pkg.get_arch().c_str());
        scols_line_set_data(ln, COL_EVR, pkg.get_evr().c_str());
        if (tspkg.get_action() == libdnf::transaction::TransactionItemAction::REMOVE) {
            scols_line_set_data(ln, COL_REPO, pkg.get_from_repo_id().c_str());
        } else {
            scols_line_set_data(ln, COL_REPO, pkg.get_repo_id().c_str());
        }
        auto tspkg_size = static_cast<int64_t>(pkg.get_install_size());
        scols_line_set_data(ln, COL_SIZE, libdnf::cli::utils::units::format_size(tspkg_size).c_str());
        auto ce = scols_line_get_cell(ln, COL_NAME);
        scols_cell_set_color(ce, action_color(tspkg.get_action()));

        ts_summary.add(tspkg.get_action());
        for (auto & replaced : tspkg.get_replaces()) {
            // highlight incoming packages with epoch/version change
            if (tspkg.get_package().get_epoch() != replaced.get_epoch() ||
                tspkg.get_package().get_version() != replaced.get_version()) {
                auto cl_evr = scols_line_get_cell(ln, COL_EVR);
                scols_cell_set_color(cl_evr, "bold");
            }

            struct libscols_line * ln_replaced = scols_table_new_line(tb, ln);
            // TODO(jmracek) Translate it
            std::string name("replacing ");
            name.append(replaced.get_name());
            scols_line_set_data(ln_replaced, COL_NAME, name.c_str());
            scols_line_set_data(ln_replaced, COL_ARCH, replaced.get_arch().c_str());
            scols_line_set_data(ln_replaced, COL_EVR, replaced.get_evr().c_str());
            scols_line_set_data(ln_replaced, COL_REPO, replaced.get_from_repo_id().c_str());

            auto replaced_size = static_cast<int64_t>(replaced.get_install_size());
            scols_line_set_data(ln_replaced, COL_SIZE, libdnf::cli::utils::units::format_size(replaced_size).c_str());
            auto replaced_color = action_color(libdnf::transaction::TransactionItemAction::REPLACED);
            auto obsoleted_color = "brown";

            scols_cell_set_color(scols_line_get_cell(ln_replaced, COL_EVR), replaced_color);
            if (pkg.get_arch() == replaced.get_arch()) {
                scols_cell_set_color(scols_line_get_cell(ln_replaced, COL_ARCH), replaced_color);
            } else {
                scols_cell_set_color(scols_line_get_cell(ln_replaced, COL_ARCH), obsoleted_color);
            }
            if (pkg.get_name() == replaced.get_name()) {
                scols_cell_set_color(scols_line_get_cell(ln_replaced, COL_NAME), replaced_color);
            } else {
                scols_cell_set_color(scols_line_get_cell(ln_replaced, COL_NAME), obsoleted_color);
            }
            scols_cell_set_color(scols_line_get_cell(ln_replaced, COL_REPO), replaced_color);
            scols_cell_set_color(scols_line_get_cell(ln_replaced, COL_SIZE), replaced_color);
        }
    }

    scols_print_table(tb);
    scols_unref_symbols(sb);
    scols_unref_table(tb);

    ts_summary.print();

    return true;
}

}  // namespace libdnf::cli::output

#endif  // LIBDNF_CLI_OUTPUT_TRANSACTION_TABLE_HPP
