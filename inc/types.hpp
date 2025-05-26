#ifndef TYPES_HPP
#define TYPES_HPP

#include <string>
#include <vector>

class Client;

typedef std::pair<std::string, std::vector< Client * > > message;

#endif
