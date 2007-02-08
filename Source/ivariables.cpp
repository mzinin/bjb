#include <cctype>
#include "ivariables.h"

/*!
\english
  \file "util/ivariables.cpp"
  \brief Contains implemntation of classes for variables of polinomial.

  Contains implemntation of class IVariables.
\endenglish
\russian
  \file "util/ivariables.cpp"
  \brief Содержит реализацию классов для переменных многочлена.

  Содержит реализацию класса IVariables.
\endrussian
*/

IVariables::~IVariables() {}

void IVariables::add(const char *var) {
  int n = strlen(var);
  char *name = new char[n+1];
  int k = 0;
  while (k < n && isspace(var[k])) {
    ++k;
  }
  int i = 0;
  while (k < n && !isspace(var[k])) {
    name[i] = var[k];
    ++i;
    ++k;
  }
  name[i] = '\0';
  if (find(name) >= 0)
    IERROR("find(name) < 0");
  mList.push_back(name);
}

int IVariables::find(const char *var) const {
  int r=0;
  ConstIterator i(mList.begin());
  while (i!=mList.end() && strcmp(*i, var) != 0) {
    ++i;
    ++r;
  }
  if (i==mList.end())
    r = -1;
  return r;
}

static int readVariable(std::istream& in, const char *var) {
  in >> std::ws;
  int r=0;
  int i = 0;
  while (var[i] && var[i] == in.get()) {
    ++i;
    ++r;
  }
  if (var[i])
    r = 0;
  return r;
}

int IVariables::read(std::istream& in) const {
  std::streampos posbeg = in.tellg(), posend;
  int varCurrent=0, var=-1;
  int lenCurrent, len;
  ConstIterator i=mList.begin();
  while (i!=mList.end()){
    in.seekg(posbeg);
    lenCurrent = readVariable(in, *i);
    if (lenCurrent > 0) {
      var = varCurrent;
      len = lenCurrent;
      posend = in.tellg();
      break;
    }
    ++varCurrent;
    ++i;
  }

  while (i!=mList.end()){
    in.seekg(posbeg);
    lenCurrent = readVariable(in, *i);
    if (lenCurrent > len) {
      var = varCurrent;
      len = lenCurrent;
      posend = in.tellg();
    }
    ++varCurrent;
    ++i;
  }

  if (var >= 0)
    in.seekg(posend);
  else
    in.seekg(posbeg);

  return var;
}

