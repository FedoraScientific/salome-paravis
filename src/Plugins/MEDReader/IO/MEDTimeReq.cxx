// Copyright (C) 2010-2014  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
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

#include "MEDTimeReq.hxx"

#include <sstream>

MEDTimeReq::~MEDTimeReq()
{
}

///////////

MEDStdTimeReq::~MEDStdTimeReq()
{
}

MEDStdTimeReq::MEDStdTimeReq(int timeReq):_time_req(timeReq)
{
}

/*!
 * Does nothing ! It is not a bug
 */
void MEDStdTimeReq::setNumberOfTS(int nbOfTS) const
{
}

int MEDStdTimeReq::size() const
{
  return 1;
}

int MEDStdTimeReq::getCurrent() const
{
  return _time_req;
}

/*!
 * Does nothing ! It is not a bug
 */
void MEDStdTimeReq::initIterator() const
{
}

std::string MEDStdTimeReq::buildName(const std::string& name) const
{
  return std::string(name);
}

/*!
 * Does nothing ! It is not a bug
 */
void MEDStdTimeReq::operator++() const
{
}

///////////

MEDModeTimeReq::MEDModeTimeReq(const std::vector<bool>& v):_v(v),_it(0),_sz(0)
{
}

MEDModeTimeReq::~MEDModeTimeReq()
{
}

int MEDModeTimeReq::size() const
{
  int ret(0);
  for(int i=0;i<_sz;i++)
    {
      if(_v[i])
        ret++;
    }
  return ret;
}

int MEDModeTimeReq::getCurrent() const
{
  return _it;
}

void MEDModeTimeReq::setNumberOfTS(int nbOfTS) const
{
  _sz=nbOfTS;
}

void MEDModeTimeReq::initIterator() const
{
  for(_it=0;_it<_sz;_it++)
    if(_v[_it])
      return;
}

std::string MEDModeTimeReq::buildName(const std::string& name) const
{
  std::ostringstream oss,oss2,oss3;
  oss << name << " [";
  //
  oss3 << _sz-1;
  std::size_t len(oss3.str().length());
  oss2.width(len);
  oss2.fill('0'); oss2 << _it;
  //
  oss << oss2.str() << "]"; 
  return oss.str();
}

void MEDModeTimeReq::operator++() const
{
  _it++;
  for(;_it<_sz;_it++)
    if(_v[_it])
      return;
}
