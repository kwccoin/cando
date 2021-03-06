/*
    File: energyComponent.h
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
#define	DEBUG_LEVEL_FULL

//
// (C) 2004 Christian E. Schafmeister
//

/*
 *	energyComponent.h
 *
 *	Maintains a database of stretch types
 */

#ifndef EnergyComponent_H 
#define	EnergyComponent_H

#include <stdio.h>
#include <string>
#include <vector>
#include <set>
#include <clasp/core/common.h>
#include <cando/geom/vector3.h>
//#include "atom.h"
//#include "bond.h"
//#include "largeSquareMatrix.h"
//#include "ffBaseDb.h"
//#include "ffTypesDb.h"
//#include "ffStretchDb.h"
//#include "ffAngleDb.h"
#include <cando/adapt/quickDom.h>
#include <cando/chem/energyAtomTable.h>
#include <cando/chem/nVector.h>
#include <cando/chem/residue.h>

		//
		// SCALE FACTOR FOR ELECTROSTIC INTERACTIONS???
		//
const double ELECTROSTATIC_MODIFIER = (93865.10/3.53*93865.10/93958.78);

#define	DECLARE_FLOAT(x) double x = 0.0

	// If we declare mathematica generated term variables
	// on the fly then set ON_THE_FLY_DECLARES
	// and variable declarations won't be included
	//
	// This should avoid a lot of variables being declared but
	// not used and eliminate a lot of compiler warnings
	//
#define	USE_EXPLICIT_DECLARES 1



#ifndef	delta2
#define		delta2	0.00001
#endif
#include <cando/adapt/quickDom.fwd.h>// energyComponent.h wants QDomNode needs quickDom.fwd.h

namespace       chem {


SMART(QDomNode);
SMART(AbstractLargeSquareMatrix);


#if TURN_ENERGY_FUNCTION_DEBUG_ON == 1
#define	LOG_ENERGY_CLEAR() {this->_DebugLog.str("");};
#define LOG_ENERGY(s) {this->_DebugLog << __FILE__ << ":" << __LINE__ << " " << s;}
#else
#define	LOG_ENERGY_CLEAR() {}
#define LOG_ENERGY(s) {}
#endif


class EnergyTerm 
{
 private:
 public:
  virtual string	className() {THROW_HARD_ERROR(BF("Subclass must implement"));};

  EnergyTerm() {};
};


class BeyondThresholdEnergyTerm
{
 public:
};



inline string	atomLabel(Atom_sp a)
{
  Residue_sp	res;
  stringstream	sstr;
  core::T_sp maybeResidue = a->getResidueContainedBy();
  if (gc::IsA<Residue_sp>(maybeResidue)) {
    res= gc::As_unsafe<Residue_sp>(maybeResidue);
    sstr <<":"<<res->getId()<<"@"<<_rep_(a->getName());
  } else {
    sstr << "@"<<_rep_(a->getName());
  }
  return sstr.str();
}






#define	REAL	double
#define	INT	int



#ifndef VERYSMALL
#define VERYSMALL       0.000000000001
#endif
#define VERYSMALLSQUARED       (VERYSMALL*VERYSMALL)
#define	TENM3		0.001


#ifndef	SIGN
#define	SIGN(x)	((x)<0.0?-1.0:1.0)
#endif
#define	myPow(x,y)	(pow(x,y))
#define	ArcCos(x)	(acos(x))
#define	Cos(x)		(cos(x))
#define	Sin(x)		(sin(x))

#define	TWOPI		(3.14159265*2.0)
#define	reciprocal(x)	(1.0/(x))
#define	mysqrt(x)	sqrt(x)
#define	power1(x)	(x)
#define	power2(x)	((x)*(x))
#define	c_two		2.0
#define	c_six		6.0
#define	oneHalf		0.5
#define	c_twelve	12.0
#define	c_True		true
#define	c_False		false

	// ChiralOffset is a term added in the chiral restraint energy function
	// to try and prevent the four atoms in a chiral restraint from
	// settling into the same plane
#define	ChiralOffset	0.0



#define	TEST_FORCE( func, delta, argLow, argHigh, term, idx ) {	\
    double eLow = func argLow;\
    double eHigh = func argHigh;\
    double numForce = -(eHigh-eLow)/(delta);\
    LOG(BF("eHigh = %le eLow = %le delta = %le")% (eHigh) % (eLow) % (delta) ); \
    LOG(BF("numForce = %le")% numForce );			\
    if ( !_areValuesClose( numForce, term, #func, #term, idx ) ) { fails++; };\
  }

#define	TEST_DIAGONAL_HESSIAN( func, delta, argLow, argMiddle, argHigh, term, idx ) {\
    double eLow = func argLow;\
    double eMiddle = func argMiddle;\
    double eHigh = func argHigh;\
    double numHessian = (eHigh+eLow-2.0*eMiddle)/(((delta)/2.0)*((delta)/2.0));\
    if ( !_areValuesClose( numHessian, term, #func, #term, idx ) ) { fails++; };\
  }

#define	TEST_OFF_DIAGONAL_HESSIAN( func, delta, argMM, argPM, argMP, argPP, term, idx ) {\
    double eMM = func argMM;\
    double eMP = func argMP;\
    double ePM = func argPM;\
    double ePP = func argPP;\
    double numHessian = ((ePP-ePM)-(eMP-eMM))/(delta*delta);\
    if ( !_areValuesClose( numHessian, term, #func, #term, idx ) ) { fails++; };\
  }


#define	ForceAcc(i,o,v) {\
      if ( hasForce ) {\
          force->setElement((i)+(o),(v)+force->getElement((i)+(o)));\
      }\
  }

//
// Accumulate an off diagonal Hessian element
//
#define	OffDiagHessAcc(i1,o1,i2,o2,v) {\
      if ( hasHessian ) {\
          hessian->addToElement((i1)+(o1),(i2)+(o2),v);\
      }\
      if ( hasHdAndD ) {\
          hdvec->addToElement((i1)+(o1),v*dvec->element((i2)+(o2)));\
          hdvec->addToElement((i2)+(o2),v*dvec->element((i1)+(o1)));\
      }\
  }

//
// Accumulate a diagonal Hessian element
//
#define	DiagHessAcc(i1,o1,i2,o2,v) {\
      if ( hasHessian ) {\
          hessian->addToElement((i1)+(o1),(i2)+(o2),v);\
      }\
      if ( hasHdAndD ) {\
          hdvec->addToElement((i1)+(o1),v*dvec->element((i1)+(o1)));\
      }\
  }



#define	ALL_ENERGY_COMPONENTS(msg) {		\
    this->_Stretch->msg; 			\
    this->_Angle->msg; 			\
    this->_Dihedral->msg; 			\
    this->_Nonbond->msg; 			\
    this->_ImproperRestraint->msg; 		\
    this->_ChiralRestraint->msg; 		\
    this->_AnchorRestraint->msg; 		\
    this->_FixedNonbondRestraint->msg; 	\
  }

template <class ComponentType, class EntryType>
  string	component_beyondThresholdInteractionsAsString(ComponentType& component) 
{
  int	bt;
  stringstream	ss;
  bt = component._BeyondThresholdTerms.end()-
    component._BeyondThresholdTerms.begin();
  ss << component.className() << "(#"<< bt <<") ";
  return ss.str();
};


#ifdef XML_ARCHIVE
template <class ComponentType, class EntryType>
  void	archiveEnergyComponentTerms(core::ArchiveP node,ComponentType& component)
{_G();
  if ( node->loading() )
  {
    core::VectorNodes::iterator	vi;
    EntryType term(node->lisp());
    component._Terms.clear();
    component._BeyondThresholdTerms.clear();
    for ( vi=node->begin_Children(); vi!=node->end_Children(); vi++ )
    {
      if ( (*vi)->isNamed(component.className()+"Term") )
      {
        term.archive(*vi);
        component._Terms.push_back(term);
      } else if ( (*vi)->isNamed(component.className()+"BeyondThresholdTerm") )
      {
        term.archive(*vi);
        component._BeyondThresholdTerms.push_back(term);
      } else
      {
        ARCHIVE_ERROR(BF("Illegal node"), *vi);
      }
    }
  } else
  {
    typename vector<EntryType>::iterator ti;
    core::ArchiveP	child;
    for (ti=component._Terms.begin(); ti!=component._Terms.end(); ti++ )
    {
      child = node->createChildNode(component.className()+"Term");
      ti->archive(child);
    }
    for (ti=component._BeyondThresholdTerms.begin(); ti!=component._BeyondThresholdTerms.end(); ti++ )
    {
      child = node->createChildNode(component.className()+"BeyondThresholdTerm");
      ti->archive(child);
    }
  }
}
#endif


SMART(EnergyComponent );
class EnergyComponent_O : public core::CxxObject_O
{
  LISP_CLASS(chem,ChemPkg,EnergyComponent_O,"EnergyComponent",core::CxxObject_O);
 public:
 public: // virtual functions inherited from Object
  void initialize();
#if XML_ARCHIVE
  void	archiveBase(core::ArchiveP node);
#endif

 protected: // instance variables
  bool		_Enabled;
  double		_Scale;
  double		_ErrorThreshold;
  bool		_DebugEnergy;
  double		_TotalEnergy;
  int		_Debug_NumberOfTermsToCalculate;
  stringstream	_DebugLog;
//protected:		// Define these in subclasses
//	vector<TermClass>	_Terms;
//	vector<TermClass>	_BeyondThresholdTerms;
 public:
  CL_DEFMETHOD virtual size_t numberOfTerms() {_OF(); SUBCLASS_MUST_IMPLEMENT();};
  void setScale(double s) {this->_Scale = s; };
  double getScale() { return this->_Scale ; };
  CL_LISPIFY_NAME("enable");
  CL_DEFMETHOD 	void enable() {this->_Enabled = true; };
  CL_LISPIFY_NAME("disable");
  CL_DEFMETHOD 	void disable() {this->_Enabled = false; };
  CL_LISPIFY_NAME("getDebugOn");
  CL_DEFMETHOD 	bool getDebugOn() {return this->_DebugEnergy; };
  CL_LISPIFY_NAME("enableDebug");
  CL_DEFMETHOD 	void enableDebug() {this->_DebugEnergy = true; };
  CL_LISPIFY_NAME("disableDebug");
  CL_DEFMETHOD 	void disableDebug() {this->_DebugEnergy = false; };
  CL_LISPIFY_NAME("setDebug_NumberOfTermsToCalculate");
  CL_DEFMETHOD 	void setDebug_NumberOfTermsToCalculate(int i) {this->_Debug_NumberOfTermsToCalculate=i;}
  bool isEnabled() { return this->_Enabled; };
  CL_LISPIFY_NAME("setErrorThreshold");
  CL_DEFMETHOD 	void	setErrorThreshold(double tr) { this->_ErrorThreshold = tr; };
  CL_LISPIFY_NAME("getErrorThreshold");
  CL_DEFMETHOD 	double	getErrorThreshold() { return this->_ErrorThreshold; };

  CL_DEFMETHOD virtual core::List_sp extract_vectors_as_alist() const { SUBCLASS_MUST_IMPLEMENT(); };
 
  string summarizeEnergyAsString();
  string enabledAsString();
  string debugLogAsString();

 public:	// Virtual methods
  CL_LISPIFY_NAME("getEnergy");
  CL_DEFMETHOD     virtual double getEnergy() { return this->_TotalEnergy; };

  virtual void zeroEnergy();
  CL_DEFMETHOD virtual void dumpTerms() {_OF();SUBCLASS_MUST_IMPLEMENT();};

  virtual	double evaluateAll( NVector_sp 	pos,
                                    bool 		calcForce,
                                    gc::Nilable<NVector_sp> 	force,
                                    bool		calcDiagonalHessian,
                                    bool		calcOffDiagonalHessian,
                                    gc::Nilable<AbstractLargeSquareMatrix_sp>	hessian,
                                    gc::Nilable<NVector_sp>	hdvec,
                                    gc::Nilable<NVector_sp> dvec) = 0;

  virtual	int	checkForBeyondThresholdInteractions( stringstream& info, NVector_sp pos ) {_OF();SUBCLASS_MUST_IMPLEMENT();};

  virtual string	beyondThresholdInteractionsAsString() {_OF();SUBCLASS_MUST_IMPLEMENT();};

  virtual	void	compareAnalyticalAndNumericalForceAndHessianTermByTerm(
                                                                               NVector_sp pos ) {_OF();SUBCLASS_MUST_IMPLEMENT();};
 public:
  EnergyComponent_O( const EnergyComponent_O& ss ); //!< Copy constructor

  DEFAULT_CTOR_DTOR(EnergyComponent_O);
};
template <typename SP>
SP safe_alist_lookup(core::List_sp list, core::T_sp key) {
  if (list.consp()) {
    core::T_sp entry = list.unsafe_cons()->assoc(key,_Nil<core::T_O>(),_Nil<core::T_O>(),_Nil<core::T_O>());
    if (entry.consp()) {
      return gctools::As<SP>(CONS_CDR(entry));
    }
  }
  SIMPLE_ERROR(BF("Could not find %s") % _rep_(key));
}
  
;
};


TRANSLATE(chem::EnergyComponent_O);
#endif
