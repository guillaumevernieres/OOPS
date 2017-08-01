/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   July 2015

#include "eckit/config/LocalConfiguration.h"

#include "eckit/config/Configuration.h"
#include "eckit/parser/Tokenizer.h"
#include "eckit/exception/Exceptions.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class ConfigurationNotFound : public Exception {

  public:
    ConfigurationNotFound(const std::string& name) {
        std::ostringstream s;
        s << "ConfigurationNotFound: [" << name << "]";
        reason(s.str());
    }
};

//----------------------------------------------------------------------------------------------------------------------

Configuration::Configuration(const Configuration& other, const std::string& path) :
    root_(other.root_),
    separator_(other.separator_) {

    bool found = false;
    root_ = lookUp(path, found);
    if(!found) throw ConfigurationNotFound(path);
}

Configuration::Configuration(const Configuration &other):
    root_(other.root_),
    separator_(other.separator_) {
}

Configuration::Configuration(const eckit::Value& root, char separator) :
    root_(root),
    separator_(separator) {
}

Configuration& Configuration::operator=(const Configuration &other) {
    root_ = other.root_;
    separator_ = other.separator_;
    return *this;
}

Configuration::~Configuration() {
}


char Configuration::separator() const {
    return separator_;
}

eckit::Value Configuration::lookUp(const std::string &s, bool &found) const {

    eckit::Tokenizer parse(separator_);
    std::vector<std::string> path;
    parse(s, path);

    eckit::Value result = root_;

    // std::cout << "Configuration::lookUp root=" << result << std::endl;
    for (size_t i = 0; i < path.size(); i++) {
        const std::string &key = path[i];
        if (!result.contains(key)) {
            found = false;
            return result;
        }
        // std::cout << "Configuration::lookUp key=" << key  << std::endl;

        result = result[key];
        // std::cout << "Configuration::lookUp key=" << result  << std::endl;

    }

    found = true;
    return result;
}


eckit::Value Configuration::lookUp(const std::string &name) const {
    bool found = false;
    eckit::Value v = lookUp(name, found);
    if(!found) throw ConfigurationNotFound(name);
    return v;
}


bool Configuration::has(const std::string &name) const {
    bool found = false;
    lookUp(name, found);
    return found;
}

bool Configuration::get(const std::string &name, std::string &value) const {
    bool found = false;
    eckit::Value v = lookUp(name, found);
    if (found) {
        value = std::string(v);
    }
    return found;
}

bool Configuration::get(const std::string &name, bool &value) const {
    bool found = false;
    eckit::Value v = lookUp(name, found);
    if (found) {
        value = v;
    }
    return found;
}

bool Configuration::get(const std::string &name, long &value) const {
    bool found = false;
    eckit::Value v = lookUp(name, found);
    if (found) {
        value = long(v);
    }
    return found;
}

bool Configuration::get(const std::string &name, double &value) const {
    bool found = false;
    eckit::Value v = lookUp(name, found);
    if (found) {
        value = v;
    }
    return found;
}

bool Configuration::get(const std::string &name, LocalConfiguration& value) const {
    bool found = has(name);
    if(found) {
        value = LocalConfiguration(*this, name);
    }
    return found;
}

bool Configuration::get(const std::string &name, std::vector<long> &value) const {
    bool found = false;
    eckit::Value v = lookUp(name, found);
    if (found) {
        ASSERT(v.isList());
        value.clear();
        int i = 0;
        while (v.contains(i)) {
            value.push_back(v[i]);
            i++;
        }
    }
    return found;
}


bool Configuration::get(const std::string &name, std::vector<LocalConfiguration> &value) const {
    bool found = false;
    eckit::Value v = lookUp(name, found);
    if (found) {
        ASSERT(v.isList());
        value.clear();
        int i = 0;
        while (v.contains(i)) {
            value.push_back(LocalConfiguration(v[i], separator_));
            i++;
        }
    }
    return found;
}

bool Configuration::get(const std::string &name, std::vector<double> &value) const {
    bool found = false;
    eckit::Value v = lookUp(name, found);
    if (found) {
        ASSERT(v.isList());
        value.clear();
        int i = 0;
        while (v.contains(i)) {
            value.push_back(v[i]);
            i++;
        }
    }
    return found;
}

bool Configuration::get(const std::string &name, std::vector<std::string> &value) const {
    bool found = false;
    eckit::Value v = lookUp(name, found);
    if (found) {
        ASSERT(v.isList());
        value.clear();
        int i = 0;
        while (v.contains(i)) {
            value.push_back(v[i]);
            i++;
        }
    }
    return found;
}

bool Configuration::get(const std::string &name, size_t &value) const {
    bool found = false;
    eckit::Value v = lookUp(name, found);
    if (found) {
        value = long(v);
    }
    return found;
}

//----------------------------------------------------------------------------------------------------------------------

template<class T>
void Configuration::_get(const std::string &name, T& value) const {
    if(!get(name, value)) {
        throw ConfigurationNotFound(name);
    }
}

bool Configuration::getBool(const std::string &name) const {
    bool result;
    _get(name, result);
    return result;
}

int Configuration::getInt(const std::string &name) const {
    long result;
    _get(name, result);
    ASSERT(int(result) == result);
    return result;
}

long Configuration::getLong(const std::string &name) const {
    long result;
    _get(name, result);
    return result;
}

size_t Configuration::getUnsigned(const std::string &name) const {
    size_t result;
    _get(name, result);
    return result;
}

float Configuration::getFloat(const std::string &name) const {
    double result;
    _get(name, result);
    ASSERT(float(result) == result);
    return result;
}

double Configuration::getDouble(const std::string &name) const {
    double result;
    _get(name, result);
    return result;
}

std::string Configuration::getString(const std::string &name) const {
    std::string result;
    _get(name, result);
    return result;
}


std::vector<int> Configuration::getIntVector(const std::string &name) const {
    std::vector<int> result;
    std::vector<long> tmp;
    _get(name, tmp);
    result.reserve(tmp.size());
    for(size_t i = 0; i < tmp.size(); i++) {
        ASSERT(int(tmp[i]) == tmp[i]);
        result.push_back(tmp[i]);
    }
    return result;
}

std::vector<long> Configuration::getLongVector(const std::string &name) const {
    std::vector<long> result;
    _get(name, result);
    return result;
}

std::vector<size_t> Configuration::getUnsignedVector(const std::string &name) const {
    std::vector<size_t> result;
    std::vector<long> tmp;
    _get(name, tmp);
    result.reserve(tmp.size());
    for(size_t i = 0; i < tmp.size(); i++) {
        ASSERT(tmp[i] >= 0);
        result.push_back(tmp[i]);
    }
    return result;
}

std::vector<float> Configuration::getFloatVector(const std::string &name) const {
     std::vector<float> result;
    std::vector<long> tmp;
    _get(name, tmp);
    result.reserve(tmp.size());
    for(size_t i = 0; i < tmp.size(); i++) {
        ASSERT(float(tmp[i]) == tmp[i]);
        result.push_back(tmp[i]);
    }
    return result;
}

std::vector<double> Configuration::getDoubleVector(const std::string &name) const {
    std::vector<double> result;
    _get(name, result);
    return result;
}

std::vector<std::string> Configuration::getStringVector(const std::string &name) const {
    std::vector<std::string> result;
    _get(name, result);
    return result;
}

std::vector<LocalConfiguration> Configuration::getSubConfigurations(const std::string &name) const {
    std::vector<LocalConfiguration> result;
    _get(name, result);
    return result;
}

LocalConfiguration Configuration::getSubConfiguration(const std::string &name) const {
    LocalConfiguration result;
    if (has(name)) _get(name, result);
    return result;
}

template<class T>
void Configuration::_getWithDefault(const std::string &name, T& value, const T& defaultVal) const {
    if(!get(name, value)) {
        value = defaultVal;
    }
}

bool Configuration::getBool(const std::string &name, const bool& defaultVal) const {
    bool result;
    _getWithDefault(name, result, defaultVal);
    return result;
}

int Configuration::getInt (const std::string &name, const int& defaultVal) const {
    long result;
    _getWithDefault(name, result, long(defaultVal));
    ASSERT(int(result) == result);
    return result;
}

long Configuration::getLong(const std::string &name, const long& defaultVal) const {
    long result;
    _getWithDefault(name, result, defaultVal);
    return result;
}

size_t Configuration::getUnsigned(const std::string &name, const size_t& defaultVal) const {
    size_t result;
    _getWithDefault(name, result, defaultVal);
    return result;
}

float Configuration::getFloat(const std::string &name, const float& defaultVal) const {
    double result;
    _getWithDefault(name, result, double(defaultVal));
    ASSERT(float(result) == result);
    return result;
}

double Configuration::getDouble(const std::string &name, const double& defaultVal) const {
    double result;
    _getWithDefault(name, result, defaultVal);
    return result;
}

std::string Configuration::getString(const std::string &name, const std::string& defaultVal) const {
    std::string result;
    _getWithDefault(name, result, defaultVal);
    return result;
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit
