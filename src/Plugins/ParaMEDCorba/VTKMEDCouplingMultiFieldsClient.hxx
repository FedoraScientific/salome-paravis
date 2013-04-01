// Copyright (C) 2010-2013  CEA/DEN, EDF R&D
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

#ifndef __VTKMEDCOUPLINGMULTIFIELDSCLIENT_HXX__
#define __VTKMEDCOUPLINGMULTIFIELDSCLIENT_HXX__

#include "ParaMEDMEM2VTK_defines.hxx"
#include "ParaMEDCouplingCorbaServant.hh"

#include <vector>
#include <string>

class vtkDataSet;
class vtkDoubleArray;

namespace ParaMEDMEM2VTK
{
  /*!
   * Stores all info on field without consideration of time
   */
  class TinyInfoOnField
  {
  public:
    int _type;//ON_CELLS = 0, ON_NODES = 1, ON_GAUSS_PT = 2, ON_GAUSS_NE = 3
    std::string _name;
  };

  class ParaMEDMEM2VTK_EXPORT MEDCouplingMultiFieldsFetcher
  {
  public:
    MEDCouplingMultiFieldsFetcher(int bufferingPolicy, SALOME_MED::MEDCouplingMultiFieldsCorbaInterface_ptr mfieldsPtr);
    ~MEDCouplingMultiFieldsFetcher();
    std::vector<double> getTimeStepsForPV();
    void fetchRegardingPolicy();
    vtkDataSet *buildDataSetOnTime(double time);
  private:
    void appendFieldValueOnAlreadyFetchedData(vtkDataSet *ds, int fieldId);
    int getPosGivenTimeLabel(double t);
  private://CORBA Fetching
    void retrievesMainTinyInfo();
    void fetchAll();
    void fetchMeshes();
    void fetchDataIfNeeded(int fieldId);
    void unregisterRemoteServantIfAllFetched();
    void applyBufferingPolicy();
  private:
    int _effective_pol;
    SALOME_MED::MEDCouplingMultiFieldsCorbaInterface_var _mfields_ptr;
    bool _mfields_ptr_released;
  private:
    std::vector<TinyInfoOnField> _info_per_field;
    std::vector<double> _time_label_per_field;
    std::vector<int> _time_discr_per_field;
    std::vector<int> _mesh_id_per_field;
    std::vector< std::vector<int> > _array_ids_per_field;
    std::vector< std::vector<double> > _time_def_per_field;
  private:
    int _field_id_previous;
    int _arr_pos_previous;
    double _time_field_id_previous;
    int _arr_id_previous;
    int _mesh_id_previous;
  private:
    std::vector<vtkDataSet *> _meshes;
    std::vector<bool> _is_meshes_polyhedron;//to correct bug of polyhedrons in VTK on DeepCopy. BUG id :0011860
    std::vector<vtkDoubleArray *> _arrays;
  private:
    static const double EPS_TIME;
  };
}

#endif
