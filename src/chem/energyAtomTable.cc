//#define DEBUG_LEVEL_FULL
/*
    File: energyAtomTable.cc
*/
/*
Open Source License
Copyright (c) 2016, Christian E. Schafmeister
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
 
This is an open source license for the CANDO software from Temple University, but it is not the only one. Contact Temple University at mailto:techtransfer@temple.edu if you would like a different license.
*/
/* -^- */
       
//#include "core/archiveNode.h"
//#include "core/archive.h"
#include <clasp/core/foundation.h>
#include <clasp/core/symbolTable.h>
#include <clasp/core/nativeVector.h>
#include <clasp/core/ql.h>
#include <clasp/core/bformat.h>
#include <clasp/core/sort.h>
#include <clasp/core/evaluator.h>
#include <clasp/core/array.h>
#include <clasp/core/hashTableEq.h>
#include <clasp/core/symbolTable.h>
#include <cando/chem/atom.h>
#include <cando/chem/virtualAtom.h>
#include <cando/chem/residue.h>
#include <cando/chem/energyAtomTable.h>
#include <cando/chem/energyComponent.h>
#include <cando/chem/energyFunction.h>
#include <cando/chem/forceField.h>
#include <cando/units/unitsPackage.h>
#include <cando/units/quantity.h>
#include <cando/chem/loop.h>
#include <clasp/core/lispStream.h>
#include <cando/chem/spanningLoop.h>
#include <cando/chem/ffNonbondDb.h>
#include <clasp/core/translators.h>
#include <clasp/core/wrappers.h>




namespace chem 
{
EnergyAtom::EnergyAtom() {};
EnergyAtom::EnergyAtom(Atom_sp atom,uint coordinateIndex) : Base(atom,coordinateIndex) {} ;
EnergyAtom::EnergyAtom(ForceField_sp forceField, Atom_sp atom, uint coordinateIndex ) : Base(atom,coordinateIndex)
{
  this->defineForAtom(forceField,atom,coordinateIndex);
}


void EnergyAtom::defineForAtom(ForceField_sp forceField, Atom_sp a1, uint coordinateIndex)
{
  this->setupBase(a1,coordinateIndex);
  this->_TypeIndex = forceField->getNonbondDb()->findTypeIndex(a1->getType());
  this->_Charge = a1->getCharge();
  this->_AtomicNumber = a1->getAtomicNumber();
}

#ifdef XML_ARCHIVE
void	EnergyAtom::archive(core::ArchiveP node)
{
  this->Base::archive(node);
}
#endif


#if 0 //[
adapt::QDomNode_sp	EnergyAtom::asXmlRelativeToContainer(chem::Matter_sp parent)
{
  adapt::QDomNode_sp	node;
  string	path;
  node = chem::new_adapt::QDomNode_sp("EnergyAtom");
  path = this->_Atom->getIdPath(parent);
  node->addAttributeString("storagePath",path);
  node->addAttributeString("atomName",this->_Atom->getName());
  node->addAttributeInt("coordinateIndex",this->_CoordinateIndex);
  node->addAttributeDoubleScientific("x",this->_Atom->getPosition().getX());
  node->addAttributeDoubleScientific("y",this->_Atom->getPosition().getY());
  node->addAttributeDoubleScientific("z",this->_Atom->getPosition().getZ());
  node->addAttributeDoubleScientific("fx",this->_Atom->getForce().getX());
  node->addAttributeDoubleScientific("fy",this->_Atom->getForce().getY());
  node->addAttributeDoubleScientific("fz",this->_Atom->getForce().getZ());
  return node;
}


void	EnergyAtom::parseFromXmlRelativeToContainer(adapt::QDomNode_sp xml,
                                                    chem::Matter_sp parent)
{
  string	path;
  double	x,y,z,fx,fy,fz;
  Vector3	v;
  LOG(BF("parsing xml with localName=%s") % xml->getLocalName().c_str() );
  path = xml->getAttributeString("storagePath");
  this->_Atom = (parent->getContentForIdPath(path)).as<chem::Atom_O>();
  ASSERTNOTNULL(this->_Atom);
  this->_CoordinateIndex = xml->getAttributeInt("coordinateIndex");
  x = xml->getAttributeDouble("x");
  y = xml->getAttributeDouble("y");
  z = xml->getAttributeDouble("z");
  v.set(x,y,z);
  this->_Atom->setPosition(v);
  fx = xml->getAttributeDouble("fx");
  fy = xml->getAttributeDouble("fy");
  fz = xml->getAttributeDouble("fz");
  v.set(fx,fy,fz);
  this->_Atom->setForce(v);
}
#endif //]

string		EnergyAtom::getResidueAndName()
{
  chem::Residue_sp	res;
  stringstream	ss;
  res = this->atom()->getResidueContainedBy().as<Residue_O>();
  ss.str("");
  ss << ":"<<res->getId()<<"@"<<this->atom()->getName();
  return ss.str();
}


void AtomTable_O::initialize()
{
  this->_AtomTableIndices = core::HashTableEq_O::create_default();
  core::MDArray_int32_t_sp residues = core::MDArray_int32_t_O::make_vector_with_fill_pointer(32,0,0);
  core::MDArrayT_sp residue_names = core::MDArrayT_O::make(32,_Nil<core::T_O>(),core::make_fixnum(0));
  core::MDArray_int32_t_sp atoms_per_molecule = core::MDArray_int32_t_O::make_vector_with_fill_pointer(32,0,0);
//  atoms_per_molecule->vectorPushExtend(0);
  this->_Residues = residues;
  this->_ResidueNames = residue_names;
  this->_AtomsPerMolecule = atoms_per_molecule;
}


EnergyAtom* AtomTable_O::getEnergyAtomPointer(Atom_sp a)
{_OF();
  core::T_mv it = this->_AtomTableIndices->gethash(a);
  if ( it.second().nilp() ) // it == this->_AtomTableIndices.end() )
  {
    SIMPLE_ERROR(BF("Could not find atom[%s] in AtomTable") % _rep_(a) );
  }
  return &this->_Atoms[core::clasp_to_fixnum(it)];
}

CL_LISPIFY_NAME("addAtomInfo");
CL_DEFMETHOD     int AtomTable_O::addAtomInfo(Atom_sp atom, units::Quantity_sp charge, units::Quantity_sp mass, int typeIndex, uint atomicNumber )
{_OF();
  int coordinateIndex = this->getNumberOfAtoms()*3;
  EnergyAtom ea(atom,coordinateIndex);
  ea._Charge = charge->value_in_unit_asReal(units::_sym_elementaryCharge->symbolValue().as<units::Unit_O>());
  ea._Mass = mass->value_in_unit_asReal(units::_sym_daltons->symbolValue().as<units::Unit_O>());
  ea._TypeIndex = typeIndex;
  ea._AtomicNumber = atomicNumber;
  this->add(ea);
  return coordinateIndex;
}


#ifdef XML_ARCHIVE
void	AtomTable_O::archiveBase(core::ArchiveP node)
{
  if ( node->loading() )
  {
    this->_Atoms.clear();
    core::VectorNodes::iterator	vi;
    for (vi=node->begin_Children(); vi!=node->end_Children(); vi++ )
    {
      EnergyAtom ea(_lisp);
      ea.archive(*vi);
      this->_Atoms.push_back(ea);
      (*vi)->setRecognized(true);
    }
  } else
  {
    core::ArchiveP child;
    gctools::Vec0<EnergyAtom>::iterator	eai;
    for ( eai=this->_Atoms.begin(); eai!=this->_Atoms.end(); eai++ )
    {
      child = node->createChildNode("EnergyAtom");
      eai->archive(child);
    }
  }
}
#endif








CL_DEFMETHOD void	AtomTable_O::dumpTerms()
{
  gctools::Vec0<EnergyAtom>::iterator	eai;
  string				as1,as2,as3,as4;
  core::Symbol_sp				str1, str2, str3, str4;
  int index = 0;
  for ( eai=this->_Atoms.begin(); eai!=this->_Atoms.end(); eai++ ) {
    as1 = atomLabel(eai->atom());
    str1 = eai->atom()->getType();
    core::write_bf_stream(BF("(TERM %d ATOM %-9s %-9s :charge %8.5lf :mass %8.5lf :typeIndex %d)\n")
              % index
              % as1
              % _rep_(str1)
              % eai->_Charge
              % eai->_Mass
              % eai->_TypeIndex );
    index++;
  }
}

CL_DEFMETHOD core::MDArray_int32_t_sp AtomTable_O::atom_table_residues() const {
  printf("%s:%d In :atom_table_residues\n", __FILE__, __LINE__ );

  if (this->_Residues.unboundp()) {
    SIMPLE_ERROR(BF("Residues table is not bound"));
  }
  return core::eval::funcall(cl::_sym_copySeq,this->_Residues);
}

CL_DEFMETHOD core::MDArrayT_sp AtomTable_O::atom_table_residue_names() const {
  printf("%s:%d In :atom_table_residue_names\n", __FILE__, __LINE__ );
  if (this->_ResidueNames.unboundp()) {
    SIMPLE_ERROR(BF("Residue names table is not bound"));
  }
  return core::eval::funcall(cl::_sym_copySeq,this->_ResidueNames);
}

CL_DEFMETHOD core::MDArray_int32_t_sp AtomTable_O::atom_table_atoms_per_molecule() const {
  printf("%s:%d In :atom_table_per_molecule\n", __FILE__, __LINE__ );
  if (this->_AtomsPerMolecule.unboundp()) {
    SIMPLE_ERROR(BF("atoms per molecule table is not bound"));
  }
  return core::eval::funcall(cl::_sym_copySeq,this->_AtomsPerMolecule);
}


DONT_OPTIMIZE_WHEN_DEBUG_RELEASE void AtomTable_O::constructFromMatter(Matter_sp mol, ForceField_sp forceField, core::T_sp activeAtoms )
{
  uint idx = this->_Atoms.size();
  uint coordinateIndex = idx*3;
  
  Loop residue_loop;
  residue_loop.loopTopGoal(mol,RESIDUES);
  while (residue_loop.advanceLoopAndProcess()) {
    Residue_sp res = residue_loop.getResidue();
    _Residues->vectorPushExtend(idx);
    _ResidueNames->vectorPushExtend(res->getName());
    {_BLOCK_TRACE("Defining ATOMS");
      Loop loop;
      Atom_sp a1;
      loop.loopTopGoal(res,ATOMS);
      while ( loop.advanceLoopAndProcess() ) 
      {
        a1 = loop.getAtom();
        if ( activeAtoms.notnilp() && !inAtomSet(activeAtoms,a1) ) continue;
        if ( a1.isA<VirtualAtom_O>() ) 
        {
          LOG(BF("Skipping virtual atom[%s]") % _rep_(a1) );
          continue; // skip virtuals
        }
        LOG(BF("Setting atom[%s] in AtomTable[%d]") % _rep_(a1) % idx );
        this->_AtomTableIndices->setf_gethash(a1,core::clasp_make_fixnum(idx));
        EnergyAtom ea(forceField,a1,coordinateIndex);
        ea._AtomName = a1->getName();
        {_BLOCK_TRACE("Building spanning tree for atom");
          LOG(BF("Spanning tree for atom: %s\n") % _rep_(a1->getName()));
          SpanningLoop_sp span = SpanningLoop_O::create(a1);
          Atom_sp bonded;
          while ( span->advance() ) {
            bonded = span->getAtom();
            if ( bonded == a1 ) continue;
            int backCount = bonded->getBackCount();
            LOG(BF("Looking at atom[%s] at remove[%d]") % _rep_(bonded) % backCount );
			// Once we crawl out 4 bonds we have gone as far as we need
            if ( backCount >= 4 ) {
              LOG(BF("Hit remove of 4 - terminating spanning loop"));
              break;
            }
            ASSERT(backCount>0 && backCount<=3);
            LOG(BF("Adding atom at remove %d --> %s\n") % (backCount-1) % _rep_(bonded->getName()));
            ea._AtomsAtRemoveBondAngle14[backCount-1].insert(bonded);
          }
        }
        this->_Atoms.push_back(ea);
        idx++;
        coordinateIndex += 3;
#ifdef DEBUG_ON
        stringstream ss;
        gctools::SmallOrderedSet<Atom_sp>::iterator si;
        for ( int zr = 0; zr<=EnergyAtom::max_remove; zr++ ) {
          ss.str("");
          for ( si = ea._AtomsAtRemoveBondAngle14[zr].begin(); si!=ea._AtomsAtRemoveBondAngle14[zr].end(); si++ ) {
            ss << " " << _rep_((*si));
          }
          LOG(BF("Atoms at remove %d = %s") % zr % ss.str() );
        }
#endif
      }
    }
  }
  this->_Residues->vectorPushExtend(idx);
  this->_AtomsPerMolecule->vectorPushExtend(idx);
}

/*! Fill excludedAtomIndices with the excluded atom list.
Amber starts counting atoms at 1 so add 1 to every index.
The atomIndex passed is index0.*/
CL_DEFMETHOD size_t AtomTable_O::push_back_excluded_atom_indices_and_sort( core::MDArray_int32_t_sp excludedAtomIndices, size_t atomIndex)
{
  size_t start_size = excludedAtomIndices->length();
  EnergyAtom* ea = &(this->_Atoms[atomIndex]);
  uint otherIndex;
  for ( int ri = 0; ri<=EnergyAtom::max_remove; ++ri ) {
    for (auto bi = ea->_AtomsAtRemoveBondAngle14[ri].begin(); bi!=ea->_AtomsAtRemoveBondAngle14[ri].end(); ++bi ) {
      otherIndex = this->_AtomTableIndices->gethash(*bi).unsafe_fixnum();
      // Amber starts counting atom indices from 1 but Clasp starts with 0 
      if (otherIndex > atomIndex) excludedAtomIndices->vectorPushExtend(otherIndex);
    }
  }
  size_t end_size = excludedAtomIndices->length();
  if (end_size == start_size ) {
    // Amber rules are that if there are no excluded atoms then put -1 in the
    // excluded atom list
    excludedAtomIndices->vectorPushExtend(-1);
    ++end_size;
  }
  // sort the indices in increasing order
  sort::quickSortMemory((int32_t*)excludedAtomIndices->rowMajorAddressOfElement_(0),start_size,end_size);
  return (end_size - start_size);
}

SYMBOL_EXPORT_SC_(ClPkg,copy_seq);

/*! Calculate the AMBER excluded atom list and return two vectors, one containing the number of 
excluded atoms for each atom and the second containing the sorted excluded atom list */
CL_DEFMETHOD core::T_mv AtomTable_O::calculate_excluded_atom_list()
{
//  printf("%s:%d In calculate_excludec_atom_list\n", __FILE__, __LINE__ );

  core::MDArray_int32_t_sp number_excluded_atoms = core::MDArray_int32_t_O::make_vector_with_fill_pointer(32,0,0);
  core::MDArray_int32_t_sp excluded_atoms_list = core::MDArray_int32_t_O::make_vector_with_fill_pointer(32,0,0);
  size_t num_atoms = this->getNumberOfAtoms();
  for ( size_t i1=0; i1<num_atoms; ++i1) {
    size_t num = this->push_back_excluded_atom_indices_and_sort(excluded_atoms_list,i1);
    number_excluded_atoms->vectorPushExtend(num);
  }
  core::T_sp t_number_excluded_atoms = core::eval::funcall(cl::_sym_copy_seq,number_excluded_atoms);
  core::T_sp t_excluded_atoms_list = core::eval::funcall(cl::_sym_copy_seq,excluded_atoms_list);
  return Values(t_number_excluded_atoms, t_excluded_atoms_list);
}

CL_DEFMETHOD core::Symbol_sp AtomTable_O::elt_atom_type(int index) {
  return this->_Atoms[index]._SharedAtom->getType();
};
SYMBOL_EXPORT_SC_(KeywordPkg,atom_name_vector);
SYMBOL_EXPORT_SC_(KeywordPkg,atom_type_vector);
SYMBOL_EXPORT_SC_(KeywordPkg,charge_vector);
SYMBOL_EXPORT_SC_(KeywordPkg,mass_vector);
SYMBOL_EXPORT_SC_(KeywordPkg,atomic_number_vector);
SYMBOL_EXPORT_SC_(KeywordPkg,residues);
SYMBOL_EXPORT_SC_(KeywordPkg,residue_names);
SYMBOL_EXPORT_SC_(KeywordPkg,atoms_per_molecule);

CL_DEFMETHOD void  AtomTable_O::fill_atom_table_from_vectors(core::List_sp vectors)
{
  core::Array_sp atom_name_vec = (safe_alist_lookup<core::Array_sp>(vectors,kw::_sym_atom_name_vector));
  core::Array_sp atom_type_vec = (safe_alist_lookup<core::Array_sp>(vectors,kw::_sym_atom_type_vector));
  core::Array_sp charge_vec = (safe_alist_lookup<core::Array_sp>(vectors,kw::_sym_charge_vector));
  core::Array_sp mass_vec = (safe_alist_lookup<core::Array_sp>(vectors,kw::_sym_mass_vector));
  core::Array_sp atomic_number_vec = (safe_alist_lookup<core::Array_sp>(vectors,kw::_sym_atomic_number_vector));
//  this->_Residues = (safe_alist_lookup<core::SimpleVector_int32_t_sp>(vectors,kw::_sym_residues));
//  this->_ResidueNames = (safe_alist_lookup<core::SimpleVector_sp>(vectors,kw::_sym_residue_names));
  this->_Atoms.resize(atom_name_vec->length());

  for (size_t i = 0, iEnd(atom_name_vec->length()); i<iEnd ;++i)
  {
    printf("%s:%d About to set _AtomName with %s\n", __FILE__, __LINE__, _rep_(atom_name_vec->rowMajorAref(i)).c_str());
    this->_Atoms[i]._AtomName     =  gc::As<core::Symbol_sp>(atom_name_vec->rowMajorAref(i));  // atom-name-vector
    //    The _TypeIndex is going to go away once we have ensured that the new Common Lisp code
    // that calculates nonbond terms works.
//    this->_Atoms[i]._TypeIndex    =  forceField->getNonbondDb()->findTypeIndex((*atom_type_vec)[i]);
    this->_Atoms[i]._TypeIndex = UNDEF_UINT;
    this->_Atoms[i]._Charge       =  translate::from_object<double>(charge_vec->rowMajorAref(i))._v;   // charge-vector
    this->_Atoms[i]._Mass         =  translate::from_object<double>(mass_vec->rowMajorAref(i))._v;                // masses
    this->_Atoms[i]._AtomicNumber =  translate::from_object<int>(atomic_number_vec->rowMajorAref(i))._v;       // vec
  }
}


};
