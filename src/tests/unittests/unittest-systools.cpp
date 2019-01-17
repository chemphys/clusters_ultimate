#include "testutils.h"

#include "bblock/sys_tools.h"
#include "setupbromidewater5.h"

#include <vector>
#include <iostream>
#include <iomanip>
#include <random>
#include <cmath>

constexpr double TOL = 1E-6;

TEST_CASE("Test the system tools functions (no PBC).") {
    // Create the bromide -- water system
    SETUP_BROMIDE_WATER_5

    // Prepare variables for SetUpMonomers
    std::vector<size_t> sites_out;
    std::vector<size_t> nat_out;
    std::vector<size_t> first_index_out;
    // Run SetUpMonomers
    try {
        systools::SetUpMonomers(monomer_names, sites_out, nat_out, first_index_out);
    } catch (CUException &e) {
        std::cerr << e.what();
    }

    // Prepare variables for OrderMonomers
    std::vector<std::pair<std::string, size_t>> mon_type_count;
    std::vector<size_t> original2current;
    std::vector<std::pair<size_t, size_t>> orginal_order;
    std::vector<std::pair<size_t, size_t>> orginal_order_realSites;
    std::vector<std::string> mon_names = monomer_names;
    // Run OrderMonomers
    try {
        mon_type_count = systools::OrderMonomers(mon_names, sites_out, nat_out, original2current, orginal_order,
                                                 orginal_order_realSites);
    } catch (CUException &e) {
        std::cerr << e.what();
    }

    SECTION("SetupMonomers") {
        SECTION("Sites Vector") { REQUIRE(VectorsAreEqual(sites_out, n_sites_vector)); }

        SECTION("Number of Atoms Vector") { REQUIRE(VectorsAreEqual(nat_out, n_atoms_vector)); }

        SECTION("First Index Vector") { REQUIRE(VectorsAreEqual(first_index_out, first_index_realSites)); }

        SECTION("Set up an empty monomer list") {
            std::vector<std::string> monomers_empty;
            bool not_possible_to_setup_monomers = false;
            try {
                systools::SetUpMonomers(monomers_empty, sites_out, nat_out, first_index_out);
            } catch (CUException &e) {
                not_possible_to_setup_monomers = true;
            }
            REQUIRE(not_possible_to_setup_monomers);
        }

        SECTION("Add a monomer not in the database") {
            std::vector<std::string> monomers_not_in_db = {"not", "in", "data", "base", "at", "all"};
            bool monomer_not_in_database = false;
            try {
                systools::SetUpMonomers(monomers_not_in_db, sites_out, nat_out, first_index_out);
            } catch (CUException &e) {
                monomer_not_in_database = true;
            }
            REQUIRE(monomer_not_in_database);
        }
    }

    SECTION("OrderMonomers") {
        SECTION("Monomer type count") { REQUIRE(VectorsAreEqual(internal_mon_type_count, mon_type_count)); }

        SECTION("Internal Order monomer names") { REQUIRE(VectorsAreEqual(mon_names, internal_monomer_names)); }

        SECTION("Original to current vector") {
            REQUIRE(VectorsAreEqual(original2current, internal_original_to_current_order));
        }

        SECTION("Original Order Vector") { REQUIRE(VectorsAreEqual(orginal_order, internal_orginal_order)); }

        SECTION("Original Order for Real Sites Vector") {
            REQUIRE(VectorsAreEqual(orginal_order_realSites, internal_original_order_realSites));
        }

        SECTION("Monomer vector is empty") {
            std::vector<std::string> monomers_empty;
            std::vector<std::pair<std::string, size_t>> mon_type_count2;
            std::vector<size_t> original2current2;
            std::vector<std::pair<size_t, size_t>> orginal_order2;
            std::vector<std::pair<size_t, size_t>> orginal_order_realSites2;
            bool not_possible_to_order_monomers = false;
            try {
                mon_type_count2 = systools::OrderMonomers(monomers_empty, sites_out, nat_out, original2current2,
                                                          orginal_order2, orginal_order_realSites2);
            } catch (CUException &e) {
                not_possible_to_order_monomers = true;
            }
            REQUIRE(not_possible_to_order_monomers);
        }

        SECTION("Monomer vector size not matching atom or sites vector") {
            std::vector<std::string> mon_names2 = monomer_names;
            std::vector<size_t> nats2(nat_out.size() + 1, 3);
            std::vector<size_t> sites2(nat_out.size() - 1, 3);
            std::vector<std::pair<std::string, size_t>> mon_type_count2;
            std::vector<size_t> original2current2;
            std::vector<std::pair<size_t, size_t>> orginal_order2;
            std::vector<std::pair<size_t, size_t>> orginal_order_realSites2;
            bool sites_vector_not_matching_monomer_size = false;
            try {
                mon_type_count2 = systools::OrderMonomers(monomer_names, sites2, nat_out, original2current2,
                                                          orginal_order2, orginal_order_realSites2);
            } catch (CUException &e) {
                sites_vector_not_matching_monomer_size = true;
            }
            REQUIRE(sites_vector_not_matching_monomer_size);

            bool atoms_vector_not_matching_monomer_size = false;
            try {
                mon_type_count2 = systools::OrderMonomers(monomer_names, sites_out, nats2, original2current2,
                                                          orginal_order2, orginal_order_realSites2);
            } catch (CUException &e) {
                atoms_vector_not_matching_monomer_size = true;
            }
            REQUIRE(atoms_vector_not_matching_monomer_size);
        }
    }

    // SECTION("Energy without gradients") { REQUIRE(energy_nograd == Approx(one_body_energy).margin(TOL)); }
}