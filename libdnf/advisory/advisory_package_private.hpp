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

#ifndef LIBDNF_ADVISORY_ADVISORY_PACKAGE_PRIVATE_HPP
#define LIBDNF_ADVISORY_ADVISORY_PACKAGE_PRIVATE_HPP

#include "rpm/package_sack_impl.hpp"
#include "solv/pool.hpp"

#include "libdnf/advisory/advisory_package.hpp"

#include <solv/pooltypes.h>
#include <solv/solvable.h>

namespace libdnf::advisory {

//TODO(amatej): We might want to remove the Impl idiom to speed up iterating over
//              AdvisoryPackages (less classes, less overhead), but we would end
//              up with libsolv ints (Ids) in a public header.
class AdvisoryPackage::Impl {
public:
    std::string get_name() const;
    std::string get_version() const;
    std::string get_evr() const;
    std::string get_arch() const;
    Id get_name_id() const { return name; };
    Id get_evr_id() const { return evr; };
    Id get_arch_id() const { return arch; };
    AdvisoryId get_advisory_id() const { return advisory; };

    //TODO(amatej): Is this the correct name?
    /// Compare NEVRAs of two AdvisoryPackages
    ///
    /// @param first        First AdvisoryPackage to compare.
    /// @param second       Secondf AdvisoryPackage to compare.
    /// @return True if first AdvisoryPackage has smaller NEVRA, False otherwise.
    static bool nevra_compare_lower_id(const AdvisoryPackage & first, const AdvisoryPackage & second) {
        if (first.p_impl->name != second.p_impl->name)
            return first.p_impl->name < second.p_impl->name;
        if (first.p_impl->arch != second.p_impl->arch)
            return first.p_impl->arch < second.p_impl->arch;
        return first.p_impl->evr < second.p_impl->evr;
    }

    //TODO(amatej): Is this the correct name?
    /// Compare Name and Architecture of AdvisoryPackage and libdnf::rpm::Package
    ///
    /// @param adv_pkg      AdvisoryPackage to compare.
    /// @param pkg          libdnf::rpm::Package to compare.
    /// @return True if AdvisoryPackage has smaller name or architecture than libdnf::rpm::package, False otherwise.
    static bool name_arch_compare_lower_id(const AdvisoryPackage & adv_pkg, const rpm::Package & pkg) {
        const auto & pool = get_pool(adv_pkg.p_impl->base);
        Solvable * s = pool.id2solvable(pkg.get_id().id);

        if (adv_pkg.p_impl->name != s->name)
            return adv_pkg.p_impl->name < s->name;
        return adv_pkg.p_impl->arch < s->arch;
    }

    //TODO(amatej): Is this the correct name?
    /// Compare Name and Architecture of Solvable and AdvisoryPackage::Impl
    ///
    /// @param solvable     Solvable to compare
    /// @param adv_pkg      AdvisoryPackage::Impl to compare.
    /// @return True if Solvable has smaller name or architecture than AdvisoryPackage::Impl, False otherwise.
    static bool name_arch_compare_lower_solvable(const Solvable * solvable, const AdvisoryPackage::Impl & adv_pkg) {
        if (solvable->name != adv_pkg.name) {
            return solvable->name < adv_pkg.name;
        }
        return solvable->arch < adv_pkg.arch;
    }

    //TODO(amatej): Is this the correct name?
    /// Compare NEVRAs of Solvable and AdvisoryPackage::Impl
    ///
    /// @param solvable     Solvable to compare
    /// @param adv_pkg      AdvisoryPackage::Impl to compare.
    /// @return True if Solvable has smaller nevra than AdvisoryPackage::Impl, False otherwise.
    static bool nevra_compare_lower_solvable(const Solvable * solvable, const AdvisoryPackage::Impl & adv_pkg) {
        if (solvable->name != adv_pkg.name) {
            return solvable->name < adv_pkg.name;
        }
        if (solvable->arch != adv_pkg.arch) {
            return solvable->arch < adv_pkg.arch;
        }
        return solvable->evr < adv_pkg.evr;
    }

private:
    friend class AdvisoryCollection;
    friend AdvisoryPackage;

    Impl(
        const libdnf::BaseWeakPtr & base,
        AdvisoryId advisory,
        int owner_collection_index,
        Id name,
        Id evr,
        Id arch,
        const char * filename);

    AdvisoryId advisory;
    int owner_collection_index;

    Id name;
    Id evr;
    Id arch;
    const char * filename;
    libdnf::BaseWeakPtr base;
};

}  // namespace libdnf::advisory


#endif  // LIBDNF_ADVISORY_ADVISORY_PACKAGE_PRIVATE_HPP
