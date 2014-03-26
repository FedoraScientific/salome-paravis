// Copyright (C) 2010-2014  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
// Author : Anthony Geay

#ifndef __MEDTIMEREQ_HXX__
#define __MEDTIMEREQ_HXX__

#include <string>
#include <vector>

class MEDTimeReq
{
public:
  virtual int size() const = 0;
  virtual void setNumberOfTS(int nbOfTS) const = 0;
  virtual std::string buildName(const std::string& name) const = 0;
  virtual void initIterator() const = 0;
  virtual int getCurrent() const = 0;
  virtual void operator++() const = 0;
  virtual ~MEDTimeReq();
};

class MEDStdTimeReq : public MEDTimeReq
{
public:
  MEDStdTimeReq(int timeReq);
  int size() const;
  int getCurrent() const;
  void initIterator() const;
  void setNumberOfTS(int nbOfTS) const;
  std::string buildName(const std::string& name) const;
  void operator++() const;
public:
  ~MEDStdTimeReq();
private:
  int _time_req;
};

class MEDModeTimeReq : public MEDTimeReq
{
public:
  MEDModeTimeReq(const std::vector<bool>& v, const std::vector<double>& ts);
  ~MEDModeTimeReq();
  int size() const;
  int getCurrent() const;
  void initIterator() const;
  void setNumberOfTS(int nbOfTS) const;
  std::string buildName(const std::string& name) const;
  void operator++() const;
private:
  std::vector<bool> _v;
  std::vector<double> _ts;
  mutable int _it;
  mutable int _sz;
};

#endif
