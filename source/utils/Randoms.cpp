//
// Created by david on 23/12/2019.
//

#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include "Randoms.h"

std::string Randoms::random_string() {
    boost::uuids::basic_random_generator<boost::mt19937> gen;
    boost::uuids::uuid u = gen(); //generate uuid

    std::string s1 = boost::lexical_cast<std::string>(u);
    return s1;
}