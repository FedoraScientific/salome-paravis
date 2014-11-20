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

#ifndef __MEDFILEFIELDREPRESENTATIONTREE_HXX__
#define __MEDFILEFIELDREPRESENTATIONTREE_HXX__

#include "MEDFileMesh.hxx"
#include "MEDFileField.hxx"
#include "MEDLoaderForPV.h"

#include "vtkType.h"

#include <vector>

class vtkQuadratureSchemeDefinition;
class vtkMutableDirectedGraph;
class vtkUnstructuredGrid;
class vtkRectilinearGrid;
class vtkStructuredGrid;
class vtkVariantArray;
class vtkIdTypeArray;
class vtkDoubleArray;
class vtkDataSet;

class TimeKeeper;
class MEDTimeReq;

class ELGACmp
{
public:
  vtkIdTypeArray *findOrCreate(const ParaMEDMEM::MEDFileFieldGlobsReal *globs, const std::vector<std::string>& locsReallyUsed, vtkDoubleArray *vtkd, vtkDataSet *ds, bool& isNew) const;
  void appendELGAIfAny(vtkDataSet *ds) const;
  ~ELGACmp();
private:
  vtkIdTypeArray *isExisting(const std::vector<std::string>& locsReallyUsed, vtkDoubleArray *vtkd) const;
  vtkIdTypeArray *createNew(const ParaMEDMEM::MEDFileFieldGlobsReal *globs, const std::vector<std::string>& locsReallyUsed, vtkDoubleArray *vtkd, vtkDataSet *ds) const;
private:
  //! size of _loc_names is equal to _elgas.
  mutable std::vector< std::vector<std::string> > _loc_names;
  //! size of _elgas is equal to _loc_names. All instances in _elgas are \b not null.
  mutable std::vector<vtkIdTypeArray *> _elgas;
  //! same size than _loc_names and _elgas.
  mutable std::vector< std::vector< std::pair< vtkQuadratureSchemeDefinition *, unsigned char > > > _defs;
};

class MEDLOADERFORPV_EXPORT MEDFileFieldRepresentationLeavesArrays : public ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDFileAnyTypeFieldMultiTS>
{
public:
  MEDFileFieldRepresentationLeavesArrays();
  MEDFileFieldRepresentationLeavesArrays(const ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDFileAnyTypeFieldMultiTS>& arr);
  MEDFileFieldRepresentationLeavesArrays& operator=(const MEDFileFieldRepresentationLeavesArrays& other);
  int getId() const;
  void setId(int& id) const;
  void feedSIL(vtkMutableDirectedGraph* sil, vtkIdType root, vtkVariantArray *edge, std::vector<std::string>& names) const;
  void computeFullNameInLeaves(const std::string& tsName, const std::string& meshName, const std::string& comSupStr) const;
  bool getStatus() const;
  bool setStatus(bool status) const;
  std::string getZeName() const;
  const char *getZeNameC() const;
  void appendFields(const MEDTimeReq *tr, const ParaMEDMEM::MEDFileFieldGlobsReal *globs, const ParaMEDMEM::MEDMeshMultiLev *mml, const ParaMEDMEM::MEDFileMeshStruct *mst, vtkDataSet *ds) const;
  void appendELGAIfAny(vtkDataSet *ds) const;
public:
  static const char ZE_SEP[];
  static const char TS_STR[];
  static const char COM_SUP_STR[];
  static const char FAMILY_ID_CELL_NAME[];
  static const char NUM_ID_CELL_NAME[];
  static const char FAMILY_ID_NODE_NAME[];
  static const char NUM_ID_NODE_NAME[];
private:
  mutable bool _activated;
  mutable int _id;
  mutable std::string _ze_name;
  mutable std::string _ze_full_name;
  ELGACmp _elga_cmp;
};

class MEDLOADERFORPV_EXPORT MEDFileFieldRepresentationLeaves
{
public:
  MEDFileFieldRepresentationLeaves();
  MEDFileFieldRepresentationLeaves(const std::vector< ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDFileAnyTypeFieldMultiTS> >& arr,
                                   const ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDFileFastCellSupportComparator>& fsp);
  ~MEDFileFieldRepresentationLeaves();
  bool empty() const;
  void setId(int& id) const;
  std::string getMeshName() const;
  int getNumberOfArrays() const;
  int getNumberOfTS() const;
  void feedSIL(const ParaMEDMEM::MEDFileMeshes *ms, const std::string& meshName, vtkMutableDirectedGraph* sil, vtkIdType root, vtkVariantArray *edge, std::vector<std::string>& names) const;
  void computeFullNameInLeaves(const std::string& tsName, const std::string& meshName, const std::string& comSupStr) const;
  bool containId(int id) const;
  bool containZeName(const char *name, int& id) const;
  bool isActivated() const;
  void printMySelf(std::ostream& os) const;
  void activateAllArrays() const;
  const MEDFileFieldRepresentationLeavesArrays& getLeafArr(int id) const;
  std::vector<double> getTimeSteps(const TimeKeeper& tk) const;
  std::vector< std::pair<int,int> > getTimeStepsInCoarseMEDFileFormat(std::vector<double>& ts) const;
  std::string getHumanReadableOverviewOfTS() const;
  vtkDataSet *buildVTKInstanceNoTimeInterpolation(const MEDTimeReq *tr, const ParaMEDMEM::MEDFileFieldGlobsReal *globs, const ParaMEDMEM::MEDFileMeshes *meshes) const;
private:
  vtkUnstructuredGrid *buildVTKInstanceNoTimeInterpolationUnstructured(ParaMEDMEM::MEDUMeshMultiLev *mm) const;
  vtkRectilinearGrid *buildVTKInstanceNoTimeInterpolationCartesian(ParaMEDMEM::MEDCMeshMultiLev *mm) const;
  vtkStructuredGrid *buildVTKInstanceNoTimeInterpolationCurveLinear(ParaMEDMEM::MEDCurveLinearMeshMultiLev *mm) const;
  void appendFields(const MEDTimeReq *tr, const ParaMEDMEM::MEDFileFieldGlobsReal *globs, const ParaMEDMEM::MEDMeshMultiLev *mml, const ParaMEDMEM::MEDFileMeshes *meshes, vtkDataSet *ds) const;
private:
  std::vector<MEDFileFieldRepresentationLeavesArrays> _arrays;
  ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDFileFastCellSupportComparator> _fsp;
  mutable vtkDataSet *_cached_ds;
};

class MEDLOADERFORPV_EXPORT MEDFileFieldRepresentationTree
{
public:
  MEDFileFieldRepresentationTree();
  int getNumberOfLeavesArrays() const;
  void assignIds() const;
  void activateTheFirst() const;
  void computeFullNameInLeaves() const;
  void feedSIL(vtkMutableDirectedGraph* sil, vtkIdType root, vtkVariantArray *edge, std::vector<std::string>& names) const;
  std::string feedSILForFamsAndGrps(vtkMutableDirectedGraph* sil, vtkIdType root, vtkVariantArray *edge, std::vector<std::string>& names) const;
  std::string getNameOf(int id) const;
  const char *getNameOfC(int id) const;
  bool getStatusOf(int id) const;
  int getIdHavingZeName(const char *name) const;
  bool changeStatusOfAndUpdateToHaveCoherentVTKDataSet(int id, bool status) const;
  int getMaxNumberOfTimeSteps() const;
  //
  std::string getDftMeshName() const;
  std::vector<double> getTimeSteps(int& lev0, const TimeKeeper& tk) const;
  vtkDataSet *buildVTKInstance(bool isStdOrMode, double timeReq, std::string& meshName, const TimeKeeper& tk) const;
  void printMySelf(std::ostream& os) const;
  //non const methods
  void loadMainStructureOfFile(const char *fileName, bool isMEDOrSauv, int iPart, int nbOfParts);
  void removeEmptyLeaves();
  // static methods
  static bool IsFieldMeshRegardingInfo(const std::vector<std::string>& compInfos);
  static std::string PostProcessFieldName(const std::string& fullFieldName);
public:
  static const char ROOT_OF_GRPS_IN_TREE[];
  static const char ROOT_OF_FAM_IDS_IN_TREE[];
  static const char COMPO_STR_TO_LOCATE_MESH_DA[];
private:
  const MEDFileFieldRepresentationLeavesArrays& getLeafArr(int id) const;
  const MEDFileFieldRepresentationLeaves& getTheSingleActivated(int& lev0, int& lev1, int& lev2) const;
  static ParaMEDMEM::MEDFileFields *BuildFieldFromMeshes(const ParaMEDMEM::MEDFileMeshes *ms);
  static void AppendFieldFromMeshes(const ParaMEDMEM::MEDFileMeshes *ms, ParaMEDMEM::MEDFileFields *ret);
  static std::string BuildAUniqueArrayNameForMesh(const std::string& meshName, const ParaMEDMEM::MEDFileFields *ret);
  static std::vector<std::string> SplitFieldNameIntoParts(const std::string& fullFieldName, char sep);
private:
  // 1st : timesteps, 2nd : meshName, 3rd : common support
  std::vector< std::vector< std::vector< MEDFileFieldRepresentationLeaves > > > _data_structure;
  ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDFileMeshes> _ms;
  ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDFileFields> _fields;
};

class MEDLOADERFORPV_EXPORT TimeKeeper
{
public:
  TimeKeeper(int policy);
  int getPolicy() const { return _policy; }
  void setPolicy(int policy) { _policy=policy; }
  std::vector<double> getTimeStepsRegardingPolicy(const std::vector< std::pair<int,int> >& tsPairs, const std::vector<double>& ts) const;
  int getTimeStepIdFrom(double timeReq) const;
  std::vector<double> getPostProcessedTime() const { return _postprocessed_time; }
  void printSelf(std::ostream& oss) const;
  std::vector<bool> getTheVectOfBool() const;
  std::vector< std::pair<bool,std::string> >& getTimesFlagArray() { return _activated_ts; }
  void setMaxNumberOfTimeSteps(int maxNumberOfTS);
private:
  std::vector<double> getTimeStepsRegardingPolicy0(const std::vector< std::pair<int,int> >& tsPairs, const std::vector<double>& ts) const;
  std::vector<double> getTimeStepsRegardingPolicy1(const std::vector< std::pair<int,int> >& tsPairs, const std::vector<double>& ts) const;
  std::vector<double> processedUsingPairOfIds(const std::vector< std::pair<int,int> >& tsPairs) const;
private:
  int _policy;
  mutable std::vector<double> _postprocessed_time;
  std::vector< std::pair<bool,std::string> > _activated_ts;
};

#endif
