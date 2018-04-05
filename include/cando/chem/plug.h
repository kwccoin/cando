/*
    File: plug.h
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
       
       
#ifndef	Plug_H //[
#define Plug_H



#include <clasp/core/common.h>
#include <cando/chem/entityNameSet.h>
#include <cando/chem/chemPackage.h>


namespace chem
{
  typedef	uint	PlugId;

  class	CandoDatabase_O;

  SMART(Mate);
  class Mate_O : public EntityNameSetWithCap_O
  {
    LISP_CLASS(chem,ChemPkg,Mate_O,"Mate",EntityNameSetWithCap_O);
  public:
  Mate_O() : _Cap(_Unbound<core::Symbol_O>()) {};
  Mate_O(core::Symbol_sp cap) : _Cap(cap) {};
  public:
    CL_LISPIFY_NAME(make_mate);
    CL_LAMBDA(cap);
    CL_DEF_CLASS_METHOD static Mate_sp make(core::Symbol_sp cap) {
      return gctools::GC<Mate_O>::allocate(cap);
    }
    virtual bool fieldsp() const { return true; };
    virtual void fields(core::Record_sp node);
  private:
    /*! The cap is the name of a monomer that will cap
     * the current plug.  It should be a member of
     * one of the Mates but if OverrideCapMateRequirement
     * then it can be anything.  Make sure its chemically
     * reasonable
     */
    core::Symbol_sp	_Cap;

  public:
//	static  Mate_sp create(core::Lisp_sp e,CandoDatabase_sp db);
  public:

    core::Symbol_sp getName() const;
    CL_LISPIFY_NAME("setCap");
    CL_DEFMETHOD     void	setCap(core::Symbol_sp cc ) {this->_Cap = cc; };
    CL_LISPIFY_NAME("getCap");
    CL_DEFMETHOD     core::Symbol_sp	getCap() { return this->_Cap; };
  };

  SMART(RingClosingMate);
  class RingClosingMate_O : public EntityNameSetWithCap_O
  {
    LISP_CLASS(chem,ChemPkg,RingClosingMate_O,"RingClosingMate",EntityNameSetWithCap_O);
  public:
    core::Symbol_sp getName() const;
  };

  SMART(Plug);
  class Plug_O : public core::CxxObject_O
  {
    LISP_CLASS(chem,ChemPkg,Plug_O,"Plug",core::CxxObject_O);
  public:
  Plug_O(core::Symbol_sp name) : _Name(name) {};
  public:
    CL_LISPIFY_NAME("make-plug");
    CL_DEF_CLASS_METHOD static Plug_sp make(core::Symbol_sp name) {
      return gctools::GC<Plug_O>::allocate(name);
    }
  public:
    virtual void fields(core::Record_sp node);
  public:
    typedef	gctools::Vec0<Mate_sp>	Mates;
  private:
    /*! Name of plug, prefix of '-' means its an incoming plug
     * prefix of '+' means its an outgoing plug
     */
    core::Symbol_sp			_Name;
  public:
    CL_DEFMETHOD     void setName(core::Symbol_sp s) { this->_Name = s; };
    CL_DEFMETHOD     core::Symbol_sp getName() const { return this->_Name; };
    CL_DEFMETHOD     virtual bool getIsIn() {_OF(); SUBCLASS_MUST_IMPLEMENT();};
    CL_DEFMETHOD     virtual bool getIsRingClosing() { return false; };

    virtual core::Symbol_sp otherSidePlugName();
    virtual bool hasMates() { return false;};

    CL_DEFMETHOD virtual core::Symbol_sp getB0() const	{ SUBIMP(); };
    CL_DEFMETHOD virtual core::Symbol_sp getB1() const	{ SUBIMP(); };
    CL_DEFMETHOD virtual BondOrder getBondOrder0() const { SUBIMP(); };
    CL_DEFMETHOD virtual BondOrder getBondOrder1() const { SUBIMP(); };

    virtual void	addMate(Mate_sp z ) {_OF();SUBCLASS_MUST_IMPLEMENT();};

    virtual Mates::iterator begin_Mates();
    virtual Mates::iterator end_Mates();

    virtual int		numberOfMates() { return 0; };

    CL_DEFMETHOD     virtual core::List_sp	matesAsList() { return _Nil<core::T_O>(); };
  };

  SMART(PlugWithMates);
  class PlugWithMates_O : public Plug_O
  {
    LISP_CLASS(chem,ChemPkg,PlugWithMates_O,"PlugWithMates",Plug_O);
  public:
  PlugWithMates_O(core::Symbol_sp name, core::List_sp mates, core::Symbol_sp bond0, BondOrder bondOrder0, core::Symbol_sp bond1, BondOrder bondOrder1 ) : Plug_O(name), _B0(bond0), _B1(bond1), _BondOrder0(bondOrder0), _BondOrder1(bondOrder1) {};
  public:
    CL_LISPIFY_NAME("make-plug-with-mates");
    CL_LAMBDA("name mates bond0 bond_order0 &optional bond1 (bondorder1 :single-bond)");
    CL_DEF_CLASS_METHOD static PlugWithMates_sp make( core::Symbol_sp name, core::List_sp mates, core::Symbol_sp bond0, BondOrder bondOrder0, core::Symbol_sp bond1, BondOrder bondOrder1 ) {
      PlugWithMates_sp me = gctools::GC<PlugWithMates_O>::allocate(name,mates,bond0,bondOrder0,bond1,bondOrder1);
      core::fillVec0(mates,me->_Mates);
      return me;
    }
  public:
    virtual void fields(core::Record_sp node);
  protected:
    //! Name of atom for first bond
      core::Symbol_sp			_B0;
    //! Name of atom for second bond
    core::Symbol_sp			_B1;
    BondOrder                           _BondOrder0;
    BondOrder                           _BondOrder1;
    //! RepresentedEntityNameSets that this plug can plug into
    Mates		_Mates;
  public:
    bool getIsIn() {_OF(); SUBCLASS_MUST_IMPLEMENT();};

    PlugWithMates_sp getPlugWithMates() { return this->sharedThis<PlugWithMates_O>();};

    core::Symbol_sp getB0() const	{ return this->_B0; };
    core::Symbol_sp getB1() const	{ return this->_B1; };
    BondOrder getBondOrder0() const { return this->_BondOrder0; };
    BondOrder getBondOrder1() const { return this->_BondOrder1; };
    
    virtual bool hasMates() { return true;};

    void	addMate(Mate_sp z ) {this->_Mates.push_back(z);};

    Mates::iterator begin_Mates() { return this->_Mates.begin(); };
    Mates::iterator end_Mates() { return this->_Mates.end(); };

    int		numberOfMates() { return this->_Mates.size(); };
    bool	recognizesMateNameOrPdb(core::Symbol_sp name);

    core::List_sp	matesAsList() { return core::Cons_O::createFromVec0(this->_Mates); };
  };



  SMART(OutPlug);
  class OutPlug_O : public PlugWithMates_O
  {
    LISP_CLASS(chem,ChemPkg,OutPlug_O,"OutPlug",PlugWithMates_O);
  public:
  OutPlug_O(core::Symbol_sp name, core::List_sp mates, MatterName stubPivotAtom, core::Symbol_sp bond0, BondOrder bondOrder0, core::Symbol_sp bond1, BondOrder bondOrder1) : PlugWithMates_O(name,mates,bond0,bondOrder0,bond1,bondOrder1), _StubPivotAtom(stubPivotAtom) {};
  public:
    CL_LISPIFY_NAME("make-out-plug");
    CL_LAMBDA("name mates stub_pivot_atom bond0 bondorder0 &optional bond1 (bondorder1 :single-bond)");
    CL_DEF_CLASS_METHOD static OutPlug_sp make(core::Symbol_sp name, core::List_sp mates, MatterName stubPivotAtom, core::Symbol_sp bond0, BondOrder bondOrder0, core::Symbol_sp bond1, BondOrder bondOrder1) {
      GC_ALLOCATE_VARIADIC(OutPlug_O, me, name, mates, stubPivotAtom, bond0, bondOrder0, bond1, bondOrder1);
      return me;
    };

  public:
    virtual void fields(core::Record_sp node);
  private:
	/*! This contains the name of the atom that we will force to be the third atom
	  that defines the Stub of the Bond0 atom */
    MatterName	_StubPivotAtom;
  public:

    CL_LISPIFY_NAME("hasStubPivotAtom");
    CL_DEFMETHOD     bool hasStubPivotAtom() { return this->_StubPivotAtom.notnilp(); };
    CL_LISPIFY_NAME("getStubPivotAtom");
    CL_DEFMETHOD 	MatterName getStubPivotAtom() { return this->_StubPivotAtom;};

    bool getIsIn() { return false;};

  };

  SMART(InPlug);
  class InPlug_O : public PlugWithMates_O
  {
    LISP_CLASS(chem,ChemPkg,InPlug_O,"InPlug",PlugWithMates_O);
  public:
  InPlug_O( core::Symbol_sp name, core::List_sp mates, core::Symbol_sp bond0, BondOrder bondOrder0, core::Symbol_sp bond1, BondOrder bondOrder1 ) : PlugWithMates_O(name,mates,bond0,bondOrder0,bond1,bondOrder1) {};
  public:
    CL_LISPIFY_NAME("make-in-plug");
    CL_LAMBDA("name mates bond0 bondorder0 &optional bond1 (bondorder1 :single-bond)");
    CL_DEF_CLASS_METHOD static InPlug_sp make(core::Symbol_sp name, core::List_sp mates, core::Symbol_sp bond0, BondOrder bondOrder0, core::Symbol_sp bond1, BondOrder bondOrder1)  {
      GC_ALLOCATE_VARIADIC(InPlug_O, me, name, mates, bond0, bondOrder0, bond1, bondOrder1 );
      return me;
    };
  public:
    virtual void fields(core::Record_sp node);
  public:
  
	/*! Regular InPlugs use the first bonded atom as the root */
    CL_LISPIFY_NAME("rootAtomName");
    CL_DEFMETHOD     virtual MatterName rootAtomName() const {return this->_B0;};

    bool getIsIn() { return true;};
  };





  SMART(JumpPlug);

/*! @class Define a Plug that is connected to the AtomTree through a Jump */

  class JumpPlug_O : public Plug_O
  {
    LISP_CLASS(chem,ChemPkg,JumpPlug_O,"JumpPlug",Plug_O);
  public: // ctors
  JumpPlug_O(core::Symbol_sp name, MatterName jumpAtomName ) : Plug_O(name), _JumpAtomName(jumpAtomName) {};
  public:
    CL_LISPIFY_NAME("make-jump-plug");
    CL_LAMBDA("name");
    CL_DEF_CLASS_METHOD static JumpPlug_sp make(core::Symbol_sp name, MatterName jumpAtomName) {
      return gctools::GC<JumpPlug_O>::allocate(name,jumpAtomName);
    }
  public:
    virtual void fields(core::Record_sp node);
  private:
    MatterName		_JumpAtomName;
  public:

    /*! Return the name of the root atom */
    CL_LISPIFY_NAME("rootAtomName");
    CL_DEFMETHOD     virtual MatterName rootAtomName() const { return this->_JumpAtomName;};

		//! JumpPlugs are a kind of InPlugs
    bool getIsIn() { return true;};

  };




  SMART(RingClosingPlug);
  class RingClosingPlug_O : public OutPlug_O
  {
    LISP_CLASS(chem,ChemPkg,RingClosingPlug_O,"RingClosingPlug",OutPlug_O);
  public:
  RingClosingPlug_O( core::Symbol_sp name, core::List_sp mates, MatterName stubPivotAtom, core::Symbol_sp bond0, BondOrder bondOrder0, core::Symbol_sp bond1, BondOrder bondOrder1 ) : OutPlug_O(name,mates,stubPivotAtom,bond0,bondOrder0,bond1,bondOrder1) {};
  public:
    CL_LISPIFY_NAME("make-ring-closing-plug");
    CL_LAMBDA("name mates stub_pivot_atom bond0 &optional bond1");
    CL_DEF_CLASS_METHOD static RingClosingPlug_sp make(core::Symbol_sp name, core::List_sp mates, MatterName stubPivotAtom, core::Symbol_sp bond0, BondOrder bondOrder0, core::Symbol_sp bond1, BondOrder bondOrder1) {
      GC_ALLOCATE_VARIADIC(RingClosingPlug_O, me, name, mates, stubPivotAtom, bond0, bondOrder0, bond1, bondOrder1 );
      core::fillVec0(gc::As<core::List_sp>(mates),me->_Mates);
      return me;
    };

  public:
    virtual void fields(core::Record_sp node);
  public:
    bool getIsIn() { return false;};
    virtual bool getIsRingClosing() { return true; };
    bool recognizesRingClosingMate(core::Symbol_sp mateName);
    core::List_sp ringClosingMatesAsList();
  };


};

#endif //]
