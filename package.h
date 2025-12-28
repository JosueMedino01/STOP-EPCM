// package.h
// Inclui dependências comuns do projeto para evitar includes repetidos em vários arquivos.
// Nota: este header põe alguns nomes do std no namespace global por conveniência.
//       Em projetos maiores pode ser preferível evitar 'using' em headers para não poluir o namespace.

#pragma once

// Cabeçalhos padrão frequentemente usados
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <map>
#include <unordered_map>
#include <set>
#include <tuple>
#include <utility>
#include <functional>
#include <cassert>
#include <cstdint>
#include <cmath>
#include <limits>

// Conveniências: traz tipos/funcs usados com frequência para o namespace global
// (reduz a necessidade de escrever std:: em cada arquivo)
using std::string;
using std::wstring;
using std::vector;
using std::pair;
using std::tuple;
using std::make_pair;
using std::make_tuple;
using std::map;
using std::unordered_map;
using std::set;
using std::shared_ptr;
using std::unique_ptr;
using std::make_shared;
using std::make_unique;
using std::size_t;
using std::move;
using std::forward;
using std::swap;
using std::cout;
using std::cerr;
using std::cin;
using std::endl;

// Pequenas utilities úteis em muitos arquivos (opcional)
inline void log_info(const string &msg) { std::cout << msg << std::endl; }
inline void log_error(const string &msg) { std::cerr << "ERROR: " << msg << std::endl; }